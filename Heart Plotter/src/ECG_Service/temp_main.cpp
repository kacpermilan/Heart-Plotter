#include "../Common_Types/CommonTypes.h"
#include "ECGService.h"

#include <memory>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

// include your interface and dependencies here

// UNCOMMENTnij TO ZEBY WYPRINTOWAC MOCK SIGNAL
// #define PRINT_SIGNAL

// create your object here, if it requires other objects, create them
// EXAMPLE: const std::unique_ptr<iECGBaseline> ecgBaseline = std::make_unique<ECGBaseline>(*constructor params*)

//#define HILBERT_TRANSFORM

std::vector<DataPoint> load_mock_signal(const char* signalpath = "../../Heart Plotter/mock/100_MLII.dat")
{
	std::vector<DataPoint> outputVector;
	std::ifstream signalfile;
	signalfile.open(signalpath);
	std::string line;
	if (signalfile.is_open())
	{
		double i = 0;
		while (std::getline(signalfile, line))
		{
			std::istringstream sin(line);
			double measurement;
			sin >> measurement;
			outputVector.push_back(DataPoint(i, measurement));
			i++;
		}
	}
	return outputVector;
}


int main()
{
	// write any tests here

	temp_mock_signal::mockSignal = load_mock_signal();
# ifdef PRINT_SIGNAL
	for (auto elem : temp_mock_signal::mockSignal) {
		std::cout << elem.y << "\n";
	}
# endif
	ECGService service = ECGService();
	service.perform_ecg_analysis();


	std::getchar();
}