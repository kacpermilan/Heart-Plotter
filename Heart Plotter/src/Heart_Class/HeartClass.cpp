#include "HeartClass.h"

OperationStatus HeartClass::calculate_RR_intervals(std::unique_ptr<iRPeaks> rPeaks)
{
    const int numRPeaks = rPeaks->r_peaks.size();
    if (numRPeaks < 2) {
        // Cannot calculate RR_intervals with less than two R-peaks
        return OperationStatus::FAILURE;
    }

    RR_intervals.resize(numRPeaks - 1);

    for (int i = 1; i < numRPeaks; ++i) {
        RR_intervals[i - 1] = rPeaks->r_peaks[i] - rPeaks->r_peaks[i - 1];
    }

    return OperationStatus::SUCCESS;
}

OperationStatus HeartClass::classify_QRSes(std::vector<DataPoint> signal,
                                           std::unique_ptr<iWaves> waves,
                                           std::unique_ptr<iRPeaks> rPeaks)
{
    const int numQrs = rPeaks->r_peaks.size();
    classified_QRSes.resize(numQrs, UNDETERMINED);

    // Oblicz RR_intervals, jeśli jeszcze nie zostały obliczone
    if (RR_intervals.empty()) {
        calculate_RR_intervals(rPeaks);
    }

    for (int i = 0; i < numQrs; ++i)
    {
        const int QRS_onset = waves->QRS_onsets[i];
        const int QRS_offset = waves->QRS_ends[i];
        const int P_wave_onset = waves->P_onsets[i];
        const double RR_interval = RR_intervals[i];

        const int poleV = calculatePole(signal, QRS_onset, QRS_offset);
        const double dlugoscV = calculateDlugosc(signal, QRS_onset, QRS_offset);
        const double rm = calculateRM(poleV, dlugoscV);
        const double qrs_amplitude = calculateQRSAmplitude(signal, QRS_onset, QRS_offset);

        const double qrs_duration = dlugoscV;
        const bool p_exists = !waves->P_onsets.empty() && P_wave_onset < QRS_onset;

        if (qrs_duration < 100 && rm > 65 && rm < 80 && qrs_amplitude >= 0.5 && qrs_amplitude <= 2.5)
        {
            classified_QRSes[i] = SUPRAVENTRICULAR;
        }
        else if (qrs_duration < 50 && RR_interval > 0.2 && RR_interval < 1.7)
        {
            classified_QRSes[i] = ARTIFACT;
        }
        else
        {
            if (!p_exists && qrs_duration > 120 && qrs_amplitude > 5)
            {
                classified_QRSes[i] = VENTRICULAR;
            }
            else
            {
                classified_QRSes[i] = UNDETERMINED;
            }
        }
    }

    return OperationStatus::SUCCESS;
}

OperationStatus HeartClass::detect_P_onsets(std::vector<DataPoint> signal, std::unique_ptr<iWaves> waves)
{
    waves->P_onsets = waves->detect_P_onsets(signal);
    return OperationStatus::SUCCESS;
}

OperationStatus HeartClass::detect_P_ends(std::vector<DataPoint> signal, std::unique_ptr<iWaves> waves)
{
    waves->P_ends = waves->detect_P_ends(signal);
    return OperationStatus::SUCCESS;
}

OperationStatus HeartClass::detect_P_offsets(std::vector<DataPoint> signal, std::unique_ptr<iWaves> waves)
{
    waves->P_offsets = waves->detect_P_offsets(signal);
    return OperationStatus::SUCCESS;
}

double HeartClass::calculatePole(const std::vector<DataPoint>& signal, int onset, int offset)
{
    int pole = 0;
    for (int i = onset; i <= offset; ++i)
    {
        pole += abs(signal[i].y);
    }
    return pole;
}

double HeartClass::calculateDlugosc(const std::vector<DataPoint>& signal, int onset, int offset)
{
    double dlugosc = 0.0;
    for (int i = onset + 1; i <= offset; ++i)
    {
        dlugosc += sqrt(1 + pow(signal[i].y - signal[i - 1].y, 2));
    }
    return dlugosc;
}

double HeartClass::calculateRM(double pole, double dlugosc)
{
    return (dlugosc / (2 * sqrt(M_PI * pole))) - 1;
}

double HeartClass::calculateQRSAmplitude(const std::vector<DataPoint>& signal, int onset, int offset)
{
    double amplitude = 0.0;
    for (int i = onset; i <= offset; ++i)
    {
        amplitude = std::max(amplitude, abs(signal[i].y));
    }
    return amplitude;
}

double HeartClass::calculatePRInterval(const std::vector<double>& P_onsets, int QRS_onset)
{
    if (P_onsets.empty())
    {
        return 0.0;
    }
    else
    {
        return QRS_onset - P_onsets.back();
    }
}
