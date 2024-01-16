#include "iHRVDFA.h"
#include <armadillo>

class HRVDFA : public iHRVDFA
{
public:
    OperationStatus short_term(std::vector<DataPoint> signal, std::vector<double> r_peaks) override;
    OperationStatus long_term(std::vector<DataPoint> signal, std::vector<double> r_peaks) override;

    double getShortTermAlpha() const;
    double getLongTermAlpha() const;

    arma::vec st_fluctuations;
    arma::vec lt_fluctuations;
    arma::vec scales_st;
    arma::vec scales_lt;
    arma::vec applyPolyval(const arma::vec& scales, const arma::vec& fluctuations);

private:
    arma::vec prepare_data(std::vector<DataPoint> signal, std::vector<double> r_peaks, int minWindow, int maxWindow);
    arma::vec calculateWindows(const arma::vec& rr_intervals, int minWindow, int maxWindow);
    std::vector<DataPoint> dfa(const arma::vec& data, const arma::vec& scales);
    arma::vec integrate(const arma::vec& data);
    std::vector<double> fluctuations(const arma::vec& integrated, const arma::vec& scales);
    double calculate_rms(const arma::vec& integrated, double scale);
    arma::vec alpha(const arma::vec& scales, const std::vector<double>& flucts);

    double st_alpha;
    double lt_alpha;
};
