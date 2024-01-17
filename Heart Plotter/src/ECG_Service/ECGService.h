#pragma once

#include "iECGService.h"

class ECGService : public iECGService 
{
public:
	ECGService();

	// perform ECG analysis, main functionality
	OperationStatus perform_ecg_analysis() override;

	// getter method
	Parameters get_parameters() override;

	// get plot data, depending on used plot type
	std::vector<DataPoint> get_plot_data(PlotType plottype) override;

	// modules
	std::unique_ptr<iECGBaseline> ecgbaseline;
	std::unique_ptr<iHeartClass> heartclass;
	std::unique_ptr<iHRV1> hrv1;
	std::unique_ptr<iHRV2> hrv2;
	std::unique_ptr<iHRVDFA> hrvdfa;
	std::unique_ptr<iRPeaks> rpeaks;
	std::unique_ptr<iSTSegment> stsegment;
	std::unique_ptr<iTWaveAlt> twavealt;
	std::unique_ptr<iWaves> waves;
private:
	Parameters calculated_params;
	bool was_analysis_performed;
};

// This is bad, but no time
namespace temp_mock_signal
{
	static std::vector<DataPoint> mockSignal;
}