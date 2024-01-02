#pragma once


/*
 * Sources:
 * J. Pan and W. J. Tompkins, "A Real-Time QRS Detection Algorithm," in IEEE Transactions on Biomedical Engineering, vol. BME-32, no. 3, pp. 230-236, March 1985, doi: 10.1109/TBME.1985.325532.
 * Lab1 qrs
 * https://en.wikipedia.org/wiki/Pan%E2%80%93Tompkins_algorithm
 */


#include "iRPeaks.h"
#include <vector>
#include <memory>


class RPeaks : public iRPeaks
{
public:
    RPeaks() = default;


    OperationStatus detect_using_Pan_Tompkins(std::vector<DataPoint> signal);
    OperationStatus detect_using_Hilbert_transform(std::vector<DataPoint> signal);
    std::vector<int> GetPeaks(std::shared_ptr<const std::vector<float>> electrocardiogram_signal, int fs = 360);

private:
    int m_fs; // electrocardiogram_signal sampling frequency

    std::vector<float> Filter(std::vector<float> signal, float fc1, float fc2) const;

    template<typename T>
    std::vector<T> Conv(std::vector<T> const& f, std::vector<T> const& g) const;

    void Normalize(std::vector<float>& v) const;
};



