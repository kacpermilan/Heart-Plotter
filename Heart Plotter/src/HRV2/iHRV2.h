/*
File - iHRV2.h
Desc - definition of interface for HRV2 module
*/

#pragma once

#include "../Common_Types/CommonTypes.h"

#include <vector>


class iHRV2
{
public:
	virtual ~iHRV2() = default;

	// generate histogram of R-R intervals
	virtual OperationStatus generate_interval_histogram(std::vector<int> r_peaks, int fs, double HistogramBinLength) = 0;

	// calculate TiNN
	virtual OperationStatus calculate_TiNN(std::vector<double> intervals) = 0;

	// calculate triangular index
	virtual OperationStatus calculate_triang_index(std::vector<double> intervals) = 0;

	// generate Poincare plot (as x, y pairs)
	virtual OperationStatus generate_poincare(std::vector<double> intervals) = 0;

	// generate SDs
	virtual OperationStatus calculate_SDs(std::vector<double> intervals) = 0;


	// calculated parameters
	std::vector<DataPoint> interval_histogram;
	std::vector<DataPoint> poincare_plot;
	double TiNN;
	double triangular_index;
	double SD_1;
	double SD_2;
};
