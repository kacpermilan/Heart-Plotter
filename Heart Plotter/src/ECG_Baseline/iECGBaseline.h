/*
File - iECGBaseline.h
Desc - definition of interface for ECGBaseline
*/


#include "../Common_Types/CommonTypes.h"

#include <vector>

/// iECGBaseline - class for filtering input signal
class iECGBaseline
{
public:
	enum FilterType
	{
		TEST_FILTERTYPE = 0,
		// TODO define and add filter types
	};

	struct FilterParameters
	{
		// TODO define and add filter parameters
	};


	// Setter method
	OperationStatus set_filter_type(iECGBaseline::FilterType filterType) = 0;
	
	// Setter method
	OperationStatus set_filter_parameters(iECGBaseline::FilterParameters filterParams) = 0;

	// Filter input signal
	OperationStatus filter_signal(std::vector<DataPoint> inputSignal) = 0;


	// Type of filter used to filter signal
	iECGBaseline::FilterType filter_type;

	// Parameters of filter
	iECGBaseline::FilterParameters filter_parameters;

	// Filtered signal (after call to filter_signal())
	std::vector<DataPoint> filtered_signal;
};