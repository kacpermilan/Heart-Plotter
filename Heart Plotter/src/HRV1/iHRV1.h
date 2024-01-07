/*
File - iHRV.h
Desc - definition of interface for HRV1 module
*/

#pragma once

#include "../Common_Types/CommonTypes.h"

#include <vector>

class iHRV1
{
public:

	virtual ~iHRV1() = default;
	
	// calculate time parameters of ECG signal
	virtual OperationStatus calculate_time_parameters(std::vector<DataPoint> signal, 
		std::vector<double> r_peaks) = 0;
	
	// calculate frequency parameters of ECG signal
	virtual OperationStatus calculate_frequency_parameters(std::vector<DataPoint> signal,
		std::vector<double> r_peaks) = 0;


	// RR_mean
	double RR_mean;

	// SDNN
	double SDNN;

	// RMSSD
	double RMSSD;

	// NN50
	double NN50;

	// pNN50
	double pNN50;
};