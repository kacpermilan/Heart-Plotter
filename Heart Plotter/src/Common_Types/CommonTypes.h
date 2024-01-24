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
	error = 0,
	success = 1,
	test_return = 2
};

/// Parameters - class containing all calculated signal parameters
struct Parameters
{
	// TODO, define and add parameters
	Parameters() = default;

	// HRV1
	double RR_mean;
	double SDNN;
	double RMSSD;
	double NN50;
	double pNN50;
	double ULF;
	double VLF;
	double LF;
	double HF;
	double LFHF;
	double TP;
	
	// HRV2
	double TiNN;
	double triangular_index;
	double SD_1;
	double SD_2;

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
