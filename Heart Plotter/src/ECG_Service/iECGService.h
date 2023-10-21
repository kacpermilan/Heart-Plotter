/*
File - iECGService
Desc - definition of interface for ECGService module, the main driver for ECG analysis
*/


#include "../Common_Types/CommonTypes.h"
#include "../ECG_Baseline/iECGBaseline.h"
#include "../Heart_Class/iHeartClass.h"
#include "../HRV1/iHRV1.h"
#include "../HRV2/iHRV2.h"
#include "../HRV_DFA/iHRVDFA.h"
#include "../R_Peaks/iRPeaks.h"
#include "../ST_Segment/iSTSegment.h"
#include "../T_Wave_Alt/iTWaveAlt.h"
#include "../Waves/iWaves.h"
#include "../Input/iInput.h"
#include "../Output/iOutput.h"

#include <vector>
#include <memory>


class iECGService
{
public:
	// virtual d-tor
	virtual ~iECGService() = default;

	// perform ECG analysis, main functionality
	virtual OperationStatus perform_ecg_analysis() = 0;

	// getter method
	virtual Parameters get_parameters() = 0;

	// get plot data, depending on used plot type
	virtual std::vector<DataPoint> get_plot_data(PlotType) = 0;
	

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

	// input/output
	std::unique_ptr<iInput> input;
	std::unique_ptr<iOutput> output;
};