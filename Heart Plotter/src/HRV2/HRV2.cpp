
#include "HRV2.h"
#include <algorithm>
#include <cmath>
#include <armadillo>

// Definicja konstruktora
HRV2::HRV2() : Y(0.0), X(0.0), TiNN(0.0), SD_1(0.0), SD_2(0.0), triangular_index(0.0) {
    // Inicjalizacja wektorów
    intervals = std::vector<double>();
    interval_histogram = std::vector<DataPoint>();
    poincare_plot = std::vector<DataPoint>();
}

// Funkcja do tworzenia histogramu
OperationStatus HRV2::generate_interval_histogram(std::vector<int> r_peaks, int fs, double HistogramBinLength) {

    if (r_peaks.empty()) {
        return ERROR;
    }
    intervals.clear();

    // Zamiana r_peaks na interwa³y i przeliczenie na sekundy
    for (size_t i = 1; i < r_peaks.size(); ++i) {
        double intervalInSeconds = static_cast<double>(r_peaks[i] - r_peaks[i - 1]) / static_cast<double>(fs);
        this->intervals.push_back(intervalInSeconds);
    }

    // Obliczanie minimaln¹ i maksymaln¹ d³ugoœæ interwa³u
    double RRmax = *max_element(intervals.begin(), intervals.end());
    double RRmin = *min_element(intervals.begin(), intervals.end());

    // Obliczaczanie d³ugoœci
    double length = RRmax - RRmin;

    // Obliczanie index_hist (liczba binów)
    int index_hist = static_cast<int>(length / HistogramBinLength);

    // Obliczanie histogramu
    std::vector<double> Histogram(index_hist, 0);
    for (double interval : intervals) {
        int bin = std::min(static_cast<int>((interval - RRmin) / static_cast<double>(HistogramBinLength)), index_hist - 1);
        // Sprawdzenie, czy bin mieœci siê w zakresie
        if (bin >= 0 && bin < index_hist) {
            Histogram[bin]++;
        }
    }

    // Przechowanie wyniku w atrybucie klasy
    interval_histogram.clear();
    for (int i = 0; i < Histogram.size(); ++i) {
        // Zmiana: reprezentacja binów w sekundach
        double binMaxInterval = RRmin + (i + 1) * HistogramBinLength;  // U¿ycie koñca przedzia³u jako reprezentacji binu
        interval_histogram.emplace_back(static_cast<double>(binMaxInterval), static_cast<double>(Histogram[i]));
    }

    // Znalezienie najliczniejszego binu
    auto maxElement = std::max_element(Histogram.begin(), Histogram.end());

    if (maxElement != Histogram.end()) {
        this->Y = *maxElement;
        this->X = static_cast<double>(std::distance(Histogram.begin(), maxElement));
    }
    return SUCCESS;
}

// Funkcja do obliczania TiNN
OperationStatus HRV2::calculate_TiNN(std::vector<double> intervals) {

    // SprawdŸ, czy wektor wejœciowy przedzia³ów jest pusty
    if (intervals.empty()) {
        return ERROR;
    }

    if (interval_histogram.empty()) {
        return ERROR;
    }

    // Inicjalizacja zmiennych
    double accelerator = 0;
    size_t Histogram_size = interval_histogram.size();
    double globalminimum = INFINITY;
    bool setglobalminimum = false; // Flaga œledz¹ca, czy globalne minimum zosta³o ustawione
    double optimalN = 0;
    double optimalM = 0;

    // Pêtla przez mo¿liwe wartoœci index_N
    for (double index_N = 0; index_N < (this->X - 1); ++index_N) {
        for (double index_M = (this->X + 1); index_M < Histogram_size; ++index_M) {
            std::vector<double> x = { index_N, X, index_M }; // Utwórz wektor indeksów


            // Zresetuj akcelerator dla nowych danych
            if (accelerator != 0) {
                accelerator = 0;
            }

            // Zainicjowana interpolacja
            arma::vec x_values = { x[0], x[1], x[2] };
            arma::vec y_values = { 0, static_cast<double>(static_cast<int>(this->Y)), 0 };

            // Interpolacja liniowa
            arma::vec interpolation;
            arma::interp1(x_values, y_values, arma::linspace<arma::vec>(x_values[0], x_values[2], Histogram_size), interpolation, "linear");

            double minimum = 0;

            // Oblicz minimum dla pierwszej czêœci histogramu
            for (double i = 0; i <= index_N; ++i) {
                double HistogramValue = static_cast<double>(interval_histogram[i].y);
                minimum += std::pow(HistogramValue, 2);
            }

            // Oblicz minimum po zastosowaniu interpolacji histogramu
            for (arma::uword i = static_cast<arma::uword>(index_N + 1); i <= static_cast<arma::uword>(index_M - 1); ++i) {
                double LinearValue = interpolation.at(i);
                double HistogramValue = static_cast<double>(interval_histogram[i].y);
                minimum += std::pow((LinearValue - HistogramValue), 2);
            }

            // Oblicz minimum dla drugiej czêœci histogramu
            for (double i = index_M; i < Histogram_size; ++i) {
                double HistogramValue = static_cast<double>(interval_histogram[i].y);
                minimum += std::pow(HistogramValue, 2);
            }

            // Ustaw globalne minimum, jeœli nie zosta³o jeszcze ustawione
            if (!setglobalminimum) {
                globalminimum = minimum;
                setglobalminimum = true;
            }

            // Zaktualizuj optymalne wartoœci, jeœli bie¿¹ce minimum jest mniejsze lub równe globalnemu minimum
            if (minimum <= globalminimum) {
                globalminimum = minimum;
                optimalN = index_N;
                optimalM = index_M;
            }
        }
    }

    // Oblicz TiNN
    double time_N = interval_histogram[optimalN].x;
    double time_M = interval_histogram[optimalM].x;
    std::cout << "time_M" << time_M << std::endl;
    std::cout << "time_N" << time_N << std::endl;
    TiNN = (time_M - time_N) * 1000.0; //wyra¿any w ms

    return SUCCESS;
}

// Funkcja obliczaj¹ca indeks trójk¹tny
OperationStatus HRV2::calculate_triang_index(std::vector<double> intervals) {
    if (intervals.empty()) {
        return ERROR;
    }
    // Oblicz totalRRIntervals
    size_t totalRRIntervals = intervals.size();

    // Oblicz triangularIndex
    triangular_index = totalRRIntervals / Y;

    return SUCCESS;
}

// Funkcja tworz¹ca wykres Poincare
OperationStatus HRV2::generate_poincare(std::vector<double> intervals) {
    if (intervals.size() < 2) {
        return ERROR;
    }

    // Tworzenie wykresu Poincaré
    poincare_plot.clear();
    for (size_t i = 0; i < intervals.size() - 1; ++i) {
        poincare_plot.emplace_back(intervals[i], intervals[i + 1]);
    }

    return SUCCESS;
}

// Funkcja obliczaj¹ca SD_1 i SD_2
OperationStatus HRV2::calculate_SDs(std::vector<double> intervals) {
    if (intervals.size() < 2) {
        return ERROR;
    }

    // Obliczanie parametrów SD1 i SD2

    // Konwertowanie interwa³ów do wektora arma::vec
    arma::vec poincare_x_vec(intervals.data(), static_cast<arma::uword>(intervals.size()), false); // drugi parametr false, aby unikn¹æ kopiowania danych
    arma::vec poincare_y_vec(poincare_x_vec.memptr() + 1, static_cast<arma::uword>(intervals.size() - 1), false); // Pocz¹wszy od drugiego elementu

    // Obliczanie ró¿nic miêdzy kolejnymi punktami na wykresie Poincaré
    arma::vec diff = poincare_y_vec - poincare_x_vec.head(poincare_y_vec.size());

    // Obliczanie odchylenia standardowego SDSD
    double sdsd = arma::stddev(diff);

    // Obliczanie odchylenia standardowego NN (SDNN)
    double sdnn = arma::stddev(poincare_x_vec);

    SD_1 = std::sqrt(0.5) * sdsd;
    SD_2 = std::sqrt(2 * std::pow(sdnn, 2) - 0.5 * std::pow(sdsd, 2));

    return SUCCESS;
}



