#include "ECGService.h"
#include "../ECG_Baseline/ECGBaseline.h"
#include "../HRV1/HRV1.h"
#include "../HRV2/HRV2.h"
#include "../R_Peaks/Rpeaks.h"

# include <iostream>

#include "../HRV_DFA/HRVDFA.h"

#define HILBERT_TRANSFORM

// ideally parameters should be passed to constructor, but this is faster
ECGService::ECGService() 
	: ecgbaseline(std::make_unique<ECGBaseline>())
	, hrv1(std::make_unique<HRV1>())
	, hrv2(std::make_unique<HRV2>())
	, hrvdfa(std::make_unique<HRVDFA>())
	, rpeaks(std::make_unique<RPeaks>())
	, was_analysis_performed(false)
{}

OperationStatus ECGService::perform_ecg_analysis(std::vector<DataPoint> signal)
{
	auto signalAnalyzed = signal;
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

	// hrv1
	ret = hrv1->calculate_frequency_parameters(signalFiltered, peaksDetected);
	if (ret == OperationStatus::error)
	{
		std::cout << "hrv1 frequency failed\n";
		return ret;
	}
	ret = hrv1->calculate_time_parameters(signalAnalyzed, peaksDetected);
	if (ret == OperationStatus::error)
	{
		std::cout << "hrv1 time failed\n";
		return ret;
	}

	calculated_params.RR_mean = hrv1->RR_mean;
	calculated_params.SDNN = hrv1->SDNN;
	calculated_params.RMSSD = hrv1->RMSSD;
	calculated_params.NN50 = hrv1->NN50;
	calculated_params.pNN50 = hrv1->pNN50;
	calculated_params.ULF = hrv1->ULF;
	calculated_params.VLF = hrv1->VLF;
	calculated_params.LF = hrv1->LF;
	calculated_params.HF = hrv1->HF;
	calculated_params.LFHF = hrv1->LFHF;
	calculated_params.TP = hrv1->TP;

	// hrv2
	ret = hrv2->generate_interval_histogram(peaksDetected, 360, 0.008);
	if (ret == OperationStatus::error)
	{
		std::cout << "hrv2 histogram failed\n";
		return ret;
	}

	auto intervals = hrv2->getIntervals();

	ret = hrv2->calculate_TiNN(intervals);
	ret = hrv2->calculate_triang_index(intervals);
	ret = hrv2->calculate_SDs(intervals);
	
	calculated_params.TiNN = hrv2->getTiNN();
	calculated_params.triangular_index = hrv2->getTriangularIndex(); 
	calculated_params.SD_1 = hrv2->getSD_1();
	calculated_params.SD_2 = hrv2->getSD_2();

	//ret = hrvdfa->short_term(signalAnalyzed, intervals);
	ret = hrvdfa->long_term(signalAnalyzed, intervals);

	was_analysis_performed = true;
	return OperationStatus::success;
}

Parameters ECGService::get_parameters()
{
	if (was_analysis_performed)
	{
		return this->calculated_params;
	}
	else
	{
		return Parameters();
	}
}

std::vector<DataPoint> ECGService::get_plot_data(PlotType plottype)
{
	return std::vector<DataPoint>{};
}
