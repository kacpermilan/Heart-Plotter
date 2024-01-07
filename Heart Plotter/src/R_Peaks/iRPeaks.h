/*
File - iRPeaks.h
Desc - definition of interface for RPeaks module
*/

#include "../Common_Types/CommonTypes.h"

#include <vector>

class iRPeaks
{
public:
	virtual ~iRPeaks() = default;

	// detect R peaks of ECG signal using Pan Tompkins algorithm
	virtual OperationStatus detect_using_Pan_Tompkins(std::vector<DataPoint> signal) = 0;
	
	// detect R peaks of ECG signal using Hilbert transform
	virtual OperationStatus detect_using_Hilbert_transform(std::vector<DataPoint> signal) = 0;


	// timestamps (x value) of detected R peaks
	std::vector<int> r_peaks;
};