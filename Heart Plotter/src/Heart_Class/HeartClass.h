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

private:
    double calculatePole(const std::vector<DataPoint>& signal, int onset, int offset);
    double calculateDlugosc(const std::vector<DataPoint>& signal, int onset, int offset);
    double calculateRM(double pole, double dlugosc);
    double calculateQRSAmplitude(const std::vector<DataPoint>& signal, int onset, int offset);
    double calculatePRInterval(const std::vector<double>& P_onsets, int QRS_onset);

    std::vector<SyndromeType> classified_QRSes;
};