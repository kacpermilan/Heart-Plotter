#include "ECGService.h"
#include "../ECG_Baseline/ECGBaseline.h"
#include "../HRV1/HRV1.h"
#include "../HRV2/HRV2.h"
#include "../R_Peaks/Rpeaks.h"

# include <iostream>

// ideally parameters should be passed to constructor, but this is faster
ECGService::ECGService() 
	: ecgbaseline(std::make_unique<ECGBaseline>())
	, hrv1(std::make_unique<HRV1>())
	, hrv2(std::make_unique<HRV2>())
	, rpeaks(std::make_unique<RPeaks>())
{}

OperationStatus ECGService::perform_ecg_analysis()
{
	auto signalAnalyzed = temp_mock_signal::mockSignal; // TODO TODO TODO, load signal from GUI somehow
	auto filterType = iECGBaseline::FilterType::BUTTERWORTH_FILTER;
	auto filterParameters = iECGBaseline::FilterParameters();
	filterParameters.windowSize = 20;
	filterParameters.cutoff = 50;
	filterParameters.order = 3;
	filterParameters.samplingRate = 360;

	auto ret = ecgbaseline->set_filter_type(filterType);
	if (ret == OperationStatus::error)
	{
		std::cout << "Set filter type failed\n";
		return ret;
	}
	ret = ecgbaseline->set_filter_parameters(filterParameters);
	if (ret == OperationStatus::error)
	{
		std::cout << "Set filter params failed\n";
		return ret;
	}
	ret = ecgbaseline->filter_signal(signalAnalyzed);
	if (ret == OperationStatus::error)
	{
		std::cout << "Filter signal failed\n";
		return ret;
	}
	auto signalFiltered = ecgbaseline->filtered_signal;
#ifdef HILBERT_TRANSFORM
	ret = rpeaks->detect_using_Hilbert_transform(signalFiltered);
#else
	ret = rpeaks->detect_using_Pan_Tompkins(signalFiltered);
#endif
	if (ret == OperationStatus::error)
	{
		std::cout << "RPeaks detect failed\n";
		return ret;
	}
	auto peaksDetected = rpeaks->r_peaks;
	ret = hrv1->calculate_frequency_parameters(signalFiltered, peaksDetected);
	if (ret == OperationStatus::error)
	{
		std::cout << "hrv1 frequency failed\n";
		return ret;
	}
	ret = hrv1->calculate_time_parameters(signalAnalyzed, peaksDetected);
	if (ret == OperationStatus::error)
	{
		std::cout << "hrv2 frequency failed\n";
		return ret;
	}
	std::cout << "LF/HF = " << hrv1->LFHF << ", NN50 = " << hrv1->NN50 << "\n";
}

Parameters ECGService::get_parameters()
{
	return Parameters();
}

std::vector<DataPoint> ECGService::get_plot_data(PlotType plottype)
{
	return std::vector<DataPoint>{};
}
