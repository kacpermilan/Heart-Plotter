/*
File - iTWaveAlt.h
Desc - definition of interface for TWaveAlt module
*/

#pragma once

#include "../Common_Types/CommonTypes.h"
#include "../Waves/iWaves.h"

#include <vector>
#include <memory>

class iTWaveAlt
{
public: 
	virtual ~iTWaveAlt() = default;

	// detect T Wave alternans from signal
	virtual OperationStatus detect_T_wave_alternan(std::vector<DataPoint> signal,
		std::unique_ptr<iWaves> waves) = 0;


	// detected alternan timestamps
	std::vector<double> T_wave_alternans;
};
