#include "iHRV2.h"

class HRV2 : public iHRV2
{
public:
    OperationStatus generate_interval_histogram(std::vector<int> r_peaks, int fs, double HistogramBinLength = 0.008); //domyœlnie d³ugoœc histogramu to 0.008 s

    OperationStatus calculate_TiNN(std::vector<double> intervals);

    OperationStatus calculate_triang_index(std::vector<double> intervals);

    OperationStatus generate_poincare(std::vector<double> intervals);

    OperationStatus calculate_SDs(std::vector<double> intervals);

    HRV2();

    std::vector<double> getIntervals() const { return intervals; }
    double getTiNN() const { return TiNN; }
    double getTriangularIndex() const { return triangular_index; }
    double getSD_1() const { return SD_1; }
    double getSD_2() const { return SD_2; }
    std::vector<DataPoint> getpoincare_plot() const { return  poincare_plot; }
    std::vector<DataPoint> getinterval_histogram() const { return  interval_histogram; }


private:
    double Y;
    double X;
    double TiNN;
    double SD_1;
    double SD_2;
    double triangular_index;
    std::vector<double> intervals;
    std::vector<DataPoint> interval_histogram;
    std::vector<DataPoint> poincare_plot;

};