/*
File - iHRVDFA.h
Desc - definition of interface for HRVDFA module.
*/

#include "../Common_Types/CommonTypes.h"

#include <vector>

class iHRVDFA
{
public:
	virtual ~iHRVDFA() = default;

	// analyze short term dependencies from ECG signal
	virtual OperationStatus short_term(std::vector<DataPoint> signal, std::vector<double> r_peaks) = 0;

	// analyze long term dependencies from ECG signal
	virtual OperationStatus long_term(std::vector<DataPoint> signal, std::vector<double> r_peaks) = 0;
	

	// long term
	std::vector<DataPoint> lt_alpha;

	// short term
	std::vector<DataPoint> st_alpha;
};