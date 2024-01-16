#pragma once
/*
File - CommonTypes.h
Desc - Definitions for common datatypes and enums used in project
*/

#pragma once

/// DataPoint - basic struct for holding singular record, most signals will be in std::vector<DataPoint>
struct DataPoint 
{
	/// Basic C-tor
	DataPoint(double x_, double y_)
		: x(x_)
		, y(y_)
	{}
	double x;
	double y;
};

/// OperationStatus - enum holding return status for input/output operations
enum OperationStatus
{
	ERROR = 0,
	SUCCESS = 1,
	TEST_RETURN = 2
};

/// Parameters - class containing all main application parameters
struct Parameters
{
	// TODO, define and add parameters
	Parameters() = default;
};

/// PlotType - enum containing used types of plots
enum PlotType
{
	TEST_PLOT = 0,
	// TODO, define and add plot types
};

/// SegmentType - enum containing types of ST segments found
enum SegmentType
{
	HEALTHY = 0,
	MYOCARDIALSCHEMIA = 1,
	ACUTE_MYO_INFARCTION = 2,
	SEVERE_MYO_INFARCTION = 3,
	TEST_SEGMENTTYPE = 4,
};

/// SyndromeType - enum containing types of syndromes classified
enum SyndromeType
{
	UNDETERMINED = 0,
	SUPRAVENTRICULAR = 1,
	VENTRICULAR = 2,
	ARTIFACT = 3,
	TEST_SYNDROMETYPE = 4,
};
