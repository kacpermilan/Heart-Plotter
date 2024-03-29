/*
File - iHeartClass.h
Desc - definition of interface for HeartClass module, 
	   which classifies QRS complexes
*/

#pragma once

#include "../Common_Types/CommonTypes.h"
#include "../Waves/iWaves.h"

#include <vector>
#include <memory>

class iHeartClass
{
public:
	virtual ~iHeartClass() = default;
	// given parameters of QRS complexes classify them into SyndromeTypes
	virtual OperationStatus classify_QRSes(std::vector<DataPoint> signal,
		std::unique_ptr<iWaves> waves) = 0;

	// classification of QRS complexes, in orded of them found in signal
	std::vector<SyndromeType> classified_QRSes;
};