#include <iostream>
#include <vector>
#include <armadillo>
#include <algorithm>
#include <cmath>
#include "../R_Peaks/RPeaks.h"  // to use function CalculateHilbertTransform


class Waves : public iWaves{

public:
OperationStatus Waves::detect_QRS_onsets(std::vector<int> r_peaks, arma::vec derivative_zeros_idx);
OperationStatus Waves::detect_QRS_ends(std::vector<int> r_peaks, arma::vec derivative_zeros_idx);
OperationStatus Waves::detect_T_ends(std::vector<DataPoint> signal, std::vector<int> r_peaks);
OperationStatus Waves::detect_P_offsets(std::vector<DataPoint> signal, std::vector<int> r_peaks);
OperationStatus Waves::detect_P_onsets(std::vector<DataPoint> signal, std::vector<int> r_peaks);

private:
arma::vec Waves::SignalEnvelop(std::vector<DataPoint> signal);
arma::vec Waves::FivePointDerivative(arma::vec envelop, int fs);
arma::vec Waves::FindDerivativeMinima(arma::vec derivative_envelop);
arma::vec Waves::calculatePhase(std::vector<DataPoint> signal, double Rv);

std:vector<double> Waves::SearchForOnsetOffset(arma::vec derivative_zeros_idx, arma::imat windows, std::string method);
std::vector<int> Waves::calculateRRintervals(std::vector<int> r_peaks);
std::vector<double> Waves::detecting_T_P_points(std::vector<DataPoint> signal, arma::imat windows, double Rv, std::string method);
std::vector<double> Waves::finding_T_waves(std::vector<DataPoint> signal, std::vector<int> r_peaks);
std::vector<double> Waves::finding_P_waves(std::vector<DataPoint> signal, std::vector<int> r_peaks);
};

