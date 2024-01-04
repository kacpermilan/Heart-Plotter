#include "iInput.h"

class Input: public iInput
{

    OperationStatus load_data(), override;

    std::vector<DataPoint> get_preprocessed_data(), override;


};