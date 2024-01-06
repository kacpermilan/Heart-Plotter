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
		MOVING_AVERAGE_FILTER = 0,
		BUTTERWORTH_FILTER,
		SAVITZKY_GOLAY_FILTER,
		LMS_FILTER,
	};

	struct FilterParameters
	{
		int windowSize;
		int order;
		double delta;
		double cutoff;
		double samplingRate;

		FilterParameters() = default;
	};

	virtual ~iECGBaseline() = default;

	// Setter method
	virtual OperationStatus set_filter_type(iECGBaseline::FilterType filterType) = 0;
	
	// Setter method
	virtual OperationStatus set_filter_parameters(iECGBaseline::FilterParameters filterParams) = 0;

	// Filter input signal
	virtual OperationStatus filter_signal(std::vector<DataPoint> inputSignal) = 0;


	// Type of filter used to filter signal
	iECGBaseline::FilterType filter_type;

	// Parameters of filter
	iECGBaseline::FilterParameters filter_parameters;

	// Filtered signal (after call to filter_signal())
	std::vector<DataPoint> filtered_signal;
};