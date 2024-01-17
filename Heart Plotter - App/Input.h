#pragma once

#include "../Heart Plotter/src/Common_Types/CommonTypes.h"
#include <string>
#include <vector>


class Input {
public:
    OperationStatus load_data(std::string record);
    std::vector<DataPoint> get_preprocessed_data(std::string filename);
};
