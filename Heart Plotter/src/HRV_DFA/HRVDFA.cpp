#include "HRVDFA.h"

OperationStatus HRVDFA::short_term(std::vector<DataPoint> signal, std::vector<double> r_peaks)
{
    // Przygotowanie danych
    arma::vec rr_intervals = prepare_data(signal, r_peaks, 4, 16);

    // analyze short term dependencies from ECG signal
    scales_st = calculateWindows(rr_intervals, 4, 16);
    st_alpha = dfa(rr_intervals, scales_st)[0].x;

    // fluktuacja dla analizy krótkoterminowej
    st_fluctuations = fluctuations(integrate(rr_intervals), scales_st);

    return OperationStatus::SUCCESS;
}

OperationStatus HRVDFA::long_term(std::vector<DataPoint> signal, std::vector<double> r_peaks)
{
    // Przygotowanie danych
    arma::vec rr_intervals = prepare_data(signal, r_peaks, 16, 64);

    // analyze long term dependencies from ECG signal
    scales_lt = calculateWindows(rr_intervals, 16, 64);
    lt_alpha = dfa(rr_intervals, scales_lt)[0].x;

    // fluktuacja dla analizy długoterminowej
    lt_fluctuations = fluctuations(integrate(rr_intervals), scales_lt);

    return OperationStatus::SUCCESS;
}

arma::vec HRVDFA::prepare_data(std::vector<DataPoint> signal, std::vector<double> r_peaks, int minWindow, int maxWindow)
{
    // Sortowanie pików R
    std::sort(r_peaks.begin(), r_peaks.end());

    // Obliczenie odstępów między pikami R w [ms]
    arma::vec t = arma::zeros(r_peaks.size());
    for (int i = 0; i < r_peaks.size(); i++)
    {
        int index = static_cast<int>(r_peaks[i]); // Konwersja indeksu na liczbę całkowitą
        t(i) = signal[index].x;
    }

    arma::vec rr_intervals = arma::diff(t) * 1000; // Przeliczenie na [ms]

    return rr_intervals;
}

arma::vec HRVDFA::calculateWindows(const arma::vec& rr_intervals, int minWindow, int maxWindow)
{
    int dataSize = rr_intervals.size();
    maxWindow = std::min(maxWindow, dataSize); // Ograniczenie do rozmiaru danych

    arma::vec scales;

    if (minWindow <= 16 && maxWindow <= 64)
    {
        scales = arma::linspace(minWindow, 16, (16 - minWindow) / 2 + 1);
        arma::vec additionalScales = arma::linspace(16, maxWindow, (maxWindow - 16) / 4 + 1);
        scales = arma::join_cols(scales, additionalScales);
    }
    else if (minWindow <= 16 && maxWindow > 64)
    {
        scales = arma::linspace(minWindow, 16, (16 - minWindow) / 2 + 1);
        arma::vec additionalScales1 = arma::linspace(16, 64, (64 - 16) / 4 + 1);
        arma::vec additionalScales2 = arma::linspace(64, maxWindow, (maxWindow - 64) / 8 + 1);
        scales = arma::join_cols(scales, additionalScales1);
        scales = arma::join_cols(scales, additionalScales2);

    }
    else if (minWindow > 16 && maxWindow > 64)
    {
        scales = arma::linspace(minWindow, 64, (64 - minWindow) / 4 + 1);
        arma::vec additionalScales = arma::linspace(64, maxWindow, (maxWindow - 64) / 8 + 1);
        scales = arma::join_cols(scales, additionalScales);

    }
    else
    {
        scales = arma::linspace(minWindow, maxWindow, (maxWindow - minWindow) / 4 + 1);

    }
    return scales;
}

std::vector<DataPoint> HRVDFA::dfa(const arma::vec& data, const arma::vec& scales)
{
    // Algorytm DFA
    arma::vec integrated = integrate(data);
    std::vector<double> flucts = fluctuations(integrated, scales);
    arma::vec coeff = alpha(scales, flucts);

    // Wyniki
    std::vector<DataPoint> result;
    result.push_back(DataPoint(coeff(0), 0)); // Alpha

    for (size_t i = 0; i < flucts.size(); ++i)
    {
        result.push_back(DataPoint(scales[i], flucts[i])); // Skale i fluktuacje
    }

    return result;
}

arma::vec HRVDFA::integrate(const arma::vec& data)
{
    // Całkowanie sygnału
    return arma::cumsum(data - arma::mean(data));
}

std::vector<double> HRVDFA::fluctuations(const arma::vec& integrated, const arma::vec& scales)
{
    // Wyznaczanie fluktuacji
    std::vector<double> fluct;
    for (double sc : scales)
    {
        fluct.push_back(calculate_rms(integrated, sc));
    }

    return fluct;
}

double HRVDFA::calculate_rms(const arma::vec& integrated, double scale)
{
    // Pomocnicza funkcja do obliczania RMS
    int n = integrated.n_elem / scale;
    arma::mat moving_window = arma::reshape(integrated.head(n * scale), scale, n);


    arma::vec scale_ax = arma::linspace(0, scale - 1, scale);
    arma::vec rms(n);

    for (int e = 0; e < n; e++)
    {
        arma::vec xcut = moving_window.col(e);
        arma::vec coeff = arma::polyfit(scale_ax, xcut, 1);
        arma::vec xfit = arma::polyval(coeff, scale_ax);
        rms(e) = arma::norm(xcut - xfit, 2);
    }

    return arma::mean(arma::sqrt(rms));
}

arma::vec HRVDFA::alpha(const arma::vec& scales, const std::vector<double>& flucts)
{
    // Konwersja std::vector<double> na arma::vec
    arma::vec armaFlucts = arma::conv_to<arma::vec>::from(flucts);

    // Wyznaczenie współczynników alpha
    return arma::polyfit(arma::log2(scales), arma::log2(armaFlucts), 1);
}

double HRVDFA::getShortTermAlpha() const {
    return st_alpha;
}

double HRVDFA::getLongTermAlpha() const {
    return lt_alpha;
}
