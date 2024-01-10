#include <iostream>
#include <vector>
#include <cmath>

// Deklaracje funkcji
double pole(const std::vector<double>& signal, int QRS_onset, int QRS_offset);
double dlugosc(const std::vector<double>& signal, int QRS_onset, int QRS_offset);
std::vector<int> classifyEKG(const std::vector<double>& signal, const std::vector<int>& QRS_onset, const std::vector<int>& QRS_offset,
                             const std::vector<double>& P_wave, const std::vector<double>& RR_interval, const std::vector<int>& P_onset,
                             const std::vector<int>& R_peaks);

// Implementacja funkcji pole
double pole(const std::vector<double>& signal, int QRS_onset, int QRS_offset) {
    double result = 0;
    for (int i = QRS_onset; i <= QRS_offset; ++i) {
        result += std::abs(signal[i]);
    }
    return result;
}

// Implementacja funkcji dlugosc
double dlugosc(const std::vector<double>& signal, int QRS_onset, int QRS_offset) {
    double result = 0;
    for (int i = QRS_onset + 1; i <= QRS_offset; ++i) {
        double tmp1 = signal[i - 1];
        double tmp2 = signal[i];
        result += std::sqrt(1 + std::pow(tmp2 - tmp1, 2));
    }
    return result;
}

// Implementacja funkcji classifyEKG
std::vector<int> classifyEKG(const std::vector<double>& signal, const std::vector<int>& QRS_onset, const std::vector<int>& QRS_offset,
                             const std::vector<double>& P_wave, const std::vector<double>& RR_interval, const std::vector<int>& P_onset,
                             const std::vector<int>& R_peaks) {
    int numQrs = QRS_onset.size();
    std::vector<int> classifications(numQrs, 0);
    std::vector<int> classCounts(5, 0); // Licznik dla każdej klasy (5 klas)

    for (int i = 0; i < numQrs; ++i) {
        double poleV = pole(signal, QRS_onset[i], QRS_offset[i]);
        double dlugoscV = dlugosc(signal, QRS_onset[i], QRS_offset[i]);
        double rm = (dlugoscV / (2 * std::sqrt(M_PI * poleV))) - 1;
        double qrs_amplitude = *std::max_element(signal.begin() + QRS_onset[i], signal.begin() + QRS_offset[i] + 1);

        // Dodatkowe parametry do klasyfikacji
        double qrs_duration = dlugoscV;
        bool p_exists = !P_wave.empty() && P_wave[i] < QRS_onset[i];

        if (qrs_duration < 100 && rm > 65 && rm < 80 && qrs_amplitude >= 0.5 && qrs_amplitude <= 2.5) {
            classifications[i] = 1; // SV
        } else if (qrs_duration < 50 && RR_interval[i] > 0.2 && RR_interval[i] < 1.7) {
            classifications[i] = 4; // ARTEFAKT
        } else {
            if (!p_exists && qrs_duration > 120 && qrs_amplitude > 5) {
                classifications[i] = 2; // V
            } else {
                classifications[i] = 3; // INNY
            }
        }

        // Zliczanie wystąpień danej klasy
        classCounts[classifications[i] - 1]++;
    }

    // Wypisanie informacji o liczbie wystąpień dla każdej klasy
    std::cout << "Liczba wystąpień poszczególnych klas:" << std::endl;
    std::cout << "SV: " << classCounts[0] << std::endl;
    std::cout << "V: " << classCounts[1] << std::endl;
    std::cout << "INNY: " << classCounts[2] << std::endl;
    std::cout << "ARTEFAKT: " << classCounts[3] << std::endl;

    return classifications;
}

// Dodatkowe funkcje pomocnicze

// Funkcja calculatePRInterval nie została przepisana, ponieważ nie jest używana w głównej funkcji.
