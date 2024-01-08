/*
File - ECGBaseline.h
Desc - definition of ECGBaseline class
*/

#pragma once

#include "iECGBaseline.h"

class ECGBaseline : public iECGBaseline {
public:
    ECGBaseline();
    virtual ~ECGBaseline();

    OperationStatus set_filter_type(iECGBaseline::FilterType filterType) override;
    OperationStatus set_filter_parameters(iECGBaseline::FilterParameters filterParams) override;
    OperationStatus filter_signal(std::vector<DataPoint> inputSignal) override;

private:
    // Input signal 
    std::vector<DataPoint> inputSignal;

    // Declarations of internal filter classes
    class MovingAverageFilter;
    class ButterworthFilter;
    class SavitzkyGolayFilter;
    class LMSFilter;
};