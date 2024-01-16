#include "TWaveAlt.h"
#include <iostream>

OperationStatus TWaveAlt::detect_T_wave_alternan(std::vector<DataPoint> signal, std::unique_ptr<iWaves> waves)
{
    // Check if the provided pointer 'waves' is valid
    if (!waves)
    {
        std::cerr << "Error: Invalid pointer to waves." << std::endl;
        return OperationStatus::Failure;
    }

    // Check if the sizes of vectors 'signal' and 'window_ecg_values' are compatible
    const int window_size = 4; // Size of the moving window, consisting of four consecutive T waves.
    if (signal.size() < window_size)
    {
        std::cerr << "Error: Insufficient data points for T wave alternans analysis." << std::endl;
        return OperationStatus::Failure;
    }

    // Parameters for Moving four-beat window technique
    double threshold_percentage = 5.0; // Percentage threshold of fulfilled conditions.

    // Analysis of Moving four-beat window with conditions for TWA
    int twa_count = 0; // Counter that keeps track of the number of windows satisfying TWA conditions. Incremented when TWA conditions are met for a given window.

    for (size_t i = 0; i < (signal.size() - window_size + 1); ++i)
    {
        // Retrieving indices of four consecutive T waves
        std::vector<double> window_indices(signal.begin() + i, signal.begin() + i + window_size);

        // Retrieving ECG signal values for the analyzed window
        std::vector<double> window_ecg_values;
        for (const auto& index : window_indices)
        {
            window_ecg_values.push_back(waves->getECGValue(index));
        }

        // Calculating amplitude differences
        double AmpDiff12 = std::abs(window_ecg_values[0] - window_ecg_values[1]);
        double AmpDiff24 = std::abs(window_ecg_values[1] - window_ecg_values[3]);
        double AmpDiff13 = std::abs(window_ecg_values[0] - window_ecg_values[2]);
        double AmpDiff34 = std::abs(window_ecg_values[2] - window_ecg_values[3]);

        // Checking conditions for TWA
        bool condition1 = AmpDiff12 < AmpDiff13;
        bool condition2 = AmpDiff24 < AmpDiff34;
        bool condition3 = AmpDiff24 < AmpDiff13;
        bool condition4 = AmpDiff13 < AmpDiff34;

        // Checking if at least one condition is fulfilled
        if (condition1 && condition2 && condition3 && condition4)
        {
            twa_count++;
        }
    }

    // Checking if the condition for 5% of windows is fulfilled
    double percentage_fulfilled_twa = (static_cast<double>(twa_count) / (signal.size() - window_size + 1)) * 100.0;
    bool twa_detected = percentage_fulfilled_twa >= threshold_percentage;

    // Updating results in the iTWaveAlt class object
    T_wave_alternans.clear();
    if (twa_detected)
    {
        T_wave_alternans.push_back(1.0); // If T-Wave Alternans is detected, add the value 1.0 to the T_wave_alternans vector.
    }
    else
    {
        T_wave_alternans.push_back(0.0); // Otherwise, add the value 0.0.
    }

    // Returning the appropriate operation status
    return twa_detected ? OperationStatus::Success : OperationStatus::Failure;
}
