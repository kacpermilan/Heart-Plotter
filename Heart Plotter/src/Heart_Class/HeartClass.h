#include "iHeartClass.h"

#include <vector>
#include <memory>

class HeartClass : public iHeartClass
{
public:
    HeartClass() = default;

    OperationStatus classify_QRSes(std::vector<DataPoint> signal,
                                   std::unique_ptr<iWaves> waves,
                                   std::unique_ptr<iRPeaks> rPeaks);


    OperationStatus detect_P_onsets(std::vector<DataPoint> signal, std::unique_ptr<iWaves> waves);
    OperationStatus detect_P_ends(std::vector<DataPoint> signal, std::unique_ptr<iWaves> waves);
    OperationStatus detect_P_offsets(std::vector<DataPoint> signal, std::unique_ptr<iWaves> waves);
    OperationStatus calculate_RR_intervals(std::unique_ptr<iRPeaks> rPeaks);

private:
    double calculatePole(const std::vector<DataPoint>& signal, int onset, int offset);
    double calculateDlugosc(const std::vector<DataPoint>& signal, int onset, int offset);
    double calculateRM(double pole, double dlugosc);
    double calculateQRSAmplitude(const std::vector<DataPoint>& signal, int onset, int offset);
    double calculatePRInterval(const std::vector<double>& P_onsets, int QRS_onset);

    std::vector<SyndromeType> classified_QRSes;
    std::vector<double> P_onsets;
    std::vector<double> P_ends;
    std::vector<double> P_offsets;
    std::vector<double> RR_intervals;
};