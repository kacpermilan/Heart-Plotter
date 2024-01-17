#pragma once

#include "../Heart Plotter/src/Common_Types/CommonTypes.h"
#include <string>
#include <vector>

class Input {
public:
    bool check_availability(std::string record);
    std::vector<DataPoint> get_preprocessed_data(std::string filename);
};
