#pragma once

#include "iRPeaks.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <math.h>
#include <gsl/gsl_fft_complex.h>
#include <cmath>
#include <gsl/gsl_complex_math.h>
#include <memory>


class RPeaks : public iRPeaks
{
public:
    RPeaks() = default;


    OperationStatus detect_using_Pan_Tompkins(std::vector<DataPoint> signal) override;
    OperationStatus detect_using_Hilbert_transform(std::vector<DataPoint> signal) override;
    std::vector<int> GetPeaksPanTompkins(std::shared_ptr<const std::vector<float>> electrocardiogram_signal, int my_fs = 360);
    std::vector<int> GetPeaksHilbert(std::shared_ptr<const std::vector<float>> electrocardiogram_signal, int my_fs = 360);


private:
    int fs = 360; 
    static constexpr int win_rect = 1024;

    std::vector<float> Filter(std::vector<float> signal, float fc1, float fc2) const;

    template<typename T>
    std::vector<T> Conv(std::vector<T> const& f, std::vector<T> const& g) const;

    std::vector<float> CalculateHilbertTransform(std::vector<float> signal, int first) const;

    std::vector<float> Derivative(std::vector<float>& signal) const;

    int CalcAverageDistance(std::vector<int>& peaks) const;

    float CalcRMSValue(std::vector<float>& signal) const;
    
};



