#include "HeatrClass.h"

OperationStatus HeartClass::classify_QRSes(std::vector<DataPoint> signal,
                                           std::unique_ptr<iWaves> waves,
                                           std::unique_ptr<iRPeaks> rPeaks)
{
    const int numQrs = rPeaks->r_peaks.size();
    classified_QRSes.resize(numQrs, UNDETERMINED);

    for (int i = 0; i < numQrs; ++i)
    {
        const int QRS_onset = waves->QRS_onsets[i]; // jak bedzie wrzucony modul waves to dostosuje tu te nazwy, narazie sa robocze
        const int QRS_offset = waves->QRS_end[i];
        const int P_wave_onset = waves->P_onsets[i];
        const double RR_interval = waves->RR_intervals[i];
        //const int P_onset = rPeaks->r_peaks[i] - P_wave_onset; //????

        const int poleV = calculatePole(signal, QRS_onset, QRS_offset);
        const double dlugoscV = calculateDlugosc(signal, QRS_onset, QRS_offset);
        const double rm = calculateRM(poleV, dlugoscV);
        const double qrs_amplitude = calculateQRSAmplitude(signal, QRS_onset, QRS_offset);
        //const double pr_interval = calculatePRInterval(waves->P_onsets, QRS_onset);

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

    return classified_QRSes;
}

int HeartClass::calculatePole(const std::vector<DataPoint>& signal, int onset, int offset)
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