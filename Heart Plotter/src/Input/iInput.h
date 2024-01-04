/*
File - iInput.h
Desc - definition of interface for Input class
*/

#include "../Common_Types/CommonTypes.h"
#include <string>
#include <vector>

class iInput
{
public:
	virtual ~iInput() = default;

	/// Load data from <filepath>
	virtual OperationStatus load_data(std::string filename) = 0;

	/// Get loaded data as a vector of DataPoint struct
	virtual std::vector<DataPoint> get_preprocessed_data(std::string filename) = 0;
	/// Get loaded data as a vector of DataPoint struct
	
	
};