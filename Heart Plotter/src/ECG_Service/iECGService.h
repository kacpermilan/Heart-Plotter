/*
File - iECGService
Desc - definition of interface for ECGService module, the main driver for ECG analysis
*/

#pragma once

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

#include <vector>
#include <memory>


class iECGService
{
public:
	// virtual d-tor
	virtual ~iECGService() = default;

	// perform ECG analysis, main functionality
	virtual OperationStatus perform_ecg_analysis(std::vector<DataPoint> signal) = 0;

	// getter method
	virtual Parameters get_parameters() = 0;

	// get plot data, depending on used plot type
	virtual std::vector<DataPoint> get_plot_data(PlotType) = 0;
};
