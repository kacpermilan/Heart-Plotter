/*
File - iSTSegment.h
Desc - definition of interface for STSegment module
*/

#pragma once

#include "../Common_Types/CommonTypes.h"
#include "../Waves/iWaves.h"

#include <vector>
#include <memory>

class iSTSegment
{
public:

	virtual ~iSTSegment() = default;
	// classify ST segments from signal
	virtual OperationStatus classify_ST_segments(std::vector<DataPoint> signal,
		std::unique_ptr<iWaves> waves) = 0;


	// segment classifications, in orded of them occuring in signal
	std::vector<SegmentType> classified_segments;
};