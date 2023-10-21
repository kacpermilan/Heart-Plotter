/*
File - iWaves.h
Desc - definition of interface for Waves module
*/

#include "../Common_Types/CommonTypes.h"

#include <vector>

class iWaves
{
public:
	virtual ~iWaves() = default;

	// detect P_onsets in signal
	virtual OperationStatus detect_P_onsets(std::vector<DataPoint> signal) = 0;

	// detect P_ends in signal
	virtual OperationStatus detect_P_ends(std::vector<DataPoint> signal) = 0;

	// detect QRS_onsets in signal
	virtual OperationStatus detect_QRS_onsets(std::vector<DataPoint> signal) = 0;

	// detect QRS_ends in signal
	virtual OperationStatus detect_QRS_ends(std::vector<DataPoint> signal) = 0;

	// detect T_ends in signal
	virtual OperationStatus detect_T_ends(std::vector<DataPoint> signal) = 0;

	// detected P_onsets 
	std::vector<double> P_onsets;

	// detected P_ends
	std::vector<double> P_ends;

	// detected QRS_onsets
	std::vector<double> QRS_onsets;

	// detected QRS_ends
	std::vector<double> QRS_ends;
};