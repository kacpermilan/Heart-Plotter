#include "RPeaks.h"
#define PI 3.14159265358979323846

using namespace std;

//referencja do metody detect_using_Pan_Tompkins, klasy RPeaks, ktora zwraca wartosc typu OperationStatus
OperationStatus RPeaks::detect_using_Pan_Tompkins(std::vector<DataPoint> signal) {
    std::vector<float> electrocardiogram_signal;
    std::vector<int> r_peaks;
    for (const auto& data_point : signal) //autonumeracja
    {
        electrocardiogram_signal.push_back(static_cast<float>(data_point.y)); //zwracanie wartosci y sygnalu
    }

    std::shared_ptr<const std::vector<float>> shared_ecg_signal = std::make_shared<const std::vector<float>>(electrocardiogram_signal);
    RPeaks::r_peaks = RPeaks::GetPeaksPanTompkins(shared_ecg_signal);
    return OperationStatus::success;
}

//referencja do metody detect_using_Hilbert_transform, klasy RPeaks, ktora zwraca wartosc typu OperationStatus
OperationStatus RPeaks::detect_using_Hilbert_transform(std::vector<DataPoint> signal) {
    std::vector<float> electrocardiogram_signal;
    for (const auto& data_point : signal) //autonumeracja
    {
        electrocardiogram_signal.push_back(static_cast<float>(data_point.y)); //zwracanie wartosci y sygnalu
    }

    std::shared_ptr<const std::vector<float>> shared_ecg_signal = std::make_shared<const std::vector<float>>(electrocardiogram_signal);
    RPeaks::r_peaks = RPeaks::GetPeaksHilbert(shared_ecg_signal);
    return OperationStatus::success;
}




//Operacja konwolucji
template<typename T>
std::vector<T> RPeaks::Conv(std::vector<T> const& f, std::vector<T> const& g) const
{
    int const nf = f.size();
    int const ng = g.size();
    int const n = nf + ng - 1;
    std::vector<T> out(n, T());
    for (auto i(0); i < n; ++i)
    {
        int const jmn = (i >= ng - 1) ? i - (ng - 1) : 0;
        int const jmx = (i < nf - 1) ? i : nf - 1;
        for (auto j(jmn); j <= jmx; ++j)
            out[i] += (f[j] * g[i - j]);
    }
    return out;
}

//Filtracja pasmowo-przepustowa
vector<float> RPeaks::Filter(vector<float> signal, float fc1, float fc2) const
{

    const float M = 5.0;
    const float N = 2 * M + 1;

    vector<float> n1;
    for (int i = -M; i <= M; i++)
        n1.push_back(i);
        
    
    float fc = fc1 / (fs / 2);

    //Odpowiedz filtra dolno-przepustowego
    vector<float> y1;
    for (float value : n1)
    {
        float result;
        if ((PI * value) != 0)
            result = sin(2 * PI * fc * value) / (PI * value);
        else
            result = 2 * fc;
        y1.push_back(result);
    }

    //Odpowiedz filtra gorno-przepustowego
    vector<float> y2;
    fc = fc2 / (fs / 2);
    for (float value : n1)
    {
        float result;
        if ((PI * value) != 0)
            result = -sin(2 * PI * fc * value) / (PI * value);
        else
            result = 1 - (2 * fc);
        y2.push_back(result);
    }

    vector<float> n(N); //n = 0:N-1
    iota(n.begin(), n.end(), 0);

    vector<float> window(N);
    for (auto i = 0; i < n.size(); i++)
        window[i] = 0.54 - 0.46 * cos(2 * PI * n[i] / (N - 1));

    for (auto i = 0; i < y1.size(); i++)
        y1[i] = window[i] * y1[i];

    for (auto i = 0; i < y2.size(); i++)
        y2[i] = window[i] * y2[i];

    vector<float> c1 = Conv(y1, signal);
    vector<float> c2 = Conv(y2, c1);

    return c2;
}


//Obliczenie pierwszej pochodnej
vector<float> RPeaks::Derivative(vector<float>& signal) const
{
    vector<float> deriv;
    for (int i = 1; i < signal.size() - 1; i++)
    { 
        float acc = 1. / (2 * fs) * (signal[i + 1] - signal[i]);
        deriv.push_back(acc);
    }
    return deriv;
}


//Transformata Hilberta
vector<float> RPeaks::CalculateHilbertTransform(vector<float> signal, int first) const
{
#define REAL(z, i) ((z)[2*(i)])
#define IMAG(z, i) ((z)[2*(i)+1])

    double data[2 * win_rect];

    for (int i = 0; i < win_rect; i++)
    {
        REAL(data, i) = signal[first + i];
        IMAG(data, i) = 0.0;
    }

    //Transformata Fouriera
    gsl_fft_complex_radix2_forward(data, 1, win_rect);

    //Ustawienie skladowych stalych na 0
    REAL(data, 0) = 0.0;
    IMAG(data, 0) = 0.0;

    //Przemnozenie skladowych harmonicznych przz -j oraz j
    for (int i = 0; i < win_rect / 2; i++)
    {
        gsl_complex a, b;
        GSL_REAL(a) = REAL(data, i);
        GSL_IMAG(a) = IMAG(data, i);
        GSL_REAL(b) = 0.0;
        GSL_IMAG(b) = -1.0;

        gsl_complex c = gsl_complex_mul(a, b);
        REAL(data, i) = GSL_REAL(c);
        IMAG(data, i) = GSL_IMAG(c);
    }


    for (int i = win_rect / 2; i < win_rect; i++)
    {
        gsl_complex a, b;
        GSL_REAL(a) = REAL(data, i);
        GSL_IMAG(a) = IMAG(data, i);
        GSL_REAL(b) = 0.0;
        GSL_IMAG(b) = 1.0;

        gsl_complex c = gsl_complex_mul(a, b);
        REAL(data, i) = GSL_REAL(c);
        IMAG(data, i) = GSL_IMAG(c);
    }

    //Odwrocona Transformata Fouriera
    gsl_fft_complex_radix2_inverse(data, 1, win_rect);

    vector<float> h;
    for (int i = 0; i < win_rect; i++)
        h.push_back(REAL(data, i));

#undef REAL
#undef IMAG
    return h;
}


//Obliczenie sredniokwadratowej sygnalu
float RPeaks::CalcRMSValue(vector<float>& signal) const
{
    float rms = 0.f;
    for_each(signal.begin(), signal.end(), [&rms](float x)
        {
            rms += x * x;
        });
    return sqrt(rms / signal.size());
}


//Obliczenie sredniej odleglosci miedzy kolejnymi wartosciami w wektorze peaks
int RPeaks::CalcAverageDistance(vector<int>& peaks) const
{
    float distance = 0.0;
    for (int i = 1; i < peaks.size(); i++)
        distance += (peaks[i] - peaks[i - 1]);
    return distance / (peaks.size() - 1);
}


std::vector<int> RPeaks::GetPeaksPanTompkins(std::shared_ptr<const std::vector<float>> electrocardiogram_signal, int my_fs)
{
    fs = my_fs;

    //Filtracja
    vector<float> signal1 = Filter(*electrocardiogram_signal, 5, 15);

    //Rozniczkowanie y[n] =1/8(?x[n?2]?2x[n?1] + 2x[n+ 1] +x[n+ 2])
    vector<float> signal2(signal1.size());
    for (auto i = 2; i < signal1.size() - 2; i++)
    {
        float val = 1. / 8 * (-signal1[i - 2] - 2 * signal1[i - 1] + 2 * signal1[i + 1] + signal1[i + 2]);
        signal2[i - 2] = val;
    }

    //Potegowanie
    for_each(signal2.begin(), signal2.end(), [](float& i)
        {
            i = pow(i, 2);
        });

    //Calkowanie
    int C = 0.15 * fs;
    vector<float> window(C, 1. / C);
    vector<float> signal3 = Conv(window, signal2);


    //Progowanie
    const int fullWindow = 2 * fs + 1;
    vector<int> peaks;
    vector<int> false_peaks;
    float signal_level = *max_element(signal3.begin(), signal3.begin() + fullWindow);
    float noise_level = 0.5 * signal_level;
    int i = 0;
    int step = 0.2 * fs;
    float local_max = 0.0;
    float local_max_val = 0.0;
    while (i + step < signal3.size())
    {
        float threshold = noise_level + 0.25 * (signal_level - noise_level);

        local_max_val = 0;
        while (signal3[i + 1] > signal3[i])
        { 
            local_max = i;
            local_max_val = signal3[i];

            i += 1;
        }

        if (local_max_val >= threshold)
        { 
            peaks.push_back(local_max);
            signal_level = 0.125 * local_max_val + 0.875 * signal_level;
            i = local_max + step;

        }
        else if (local_max_val < threshold)
        { 
            false_peaks.push_back(local_max);
            noise_level = 0.125 * local_max_val + 0.875 * noise_level;
        }

        i += 1;

    }

    vector<int> r_peaks; //Nastepne maksimum nie moze wystapic w czasie 200ms od poprzedniego - okres refrakcji
    for_each(peaks.begin(), peaks.end(), [electrocardiogram_signal, &r_peaks, this](float peak)
        {
            auto local = max_element(electrocardiogram_signal->begin() + peak - int(0.2 * fs), electrocardiogram_signal->begin() + peak + int(0.2 * fs));
            int local_idx = distance(electrocardiogram_signal->begin(), local);
            r_peaks.push_back(local_idx);
        });

    return r_peaks;

}

vector<int> RPeaks::GetPeaksHilbert(std::shared_ptr<const std::vector<float>> electrocardiogram_signal, int my_fs)
{
    fs = my_fs;

    //Filtracja
    vector<float> signal = Filter(*electrocardiogram_signal, 5, 15);
    signal.erase(signal.begin(), signal.begin() + 9);
    //Rozniczkowanie 1/2fs[x(n+1)-x(n-1))]
    signal = this->Derivative(signal);

    int i = 0;
    float max_amplitude_old = 0.0;
    vector<int> peaks;
    vector<int> windows;
    while ((i + win_rect) < signal.size())
    {

        windows.push_back(i);
        //Transformata Hilberta
        vector<float> signal_hilb = CalculateHilbertTransform(signal, i);
        float rms_value = CalcRMSValue(signal_hilb);
        auto max_amplitude = max_element(signal_hilb.begin(), signal_hilb.end());
        float threshold;
        //Progowanie
        if (rms_value >= 0.18 * *max_amplitude)
        {
            threshold = 0.39 * *max_amplitude;
            if (*max_amplitude > 2 * max_amplitude_old)
                threshold = 0.0039 * max_amplitude_old;
        }
        else
        { 
            threshold = 1.6 * rms_value;
        }

        float maximum = 0.0;
        float maximum_val = 0.0;


        for (int j = 0; j < signal_hilb.size(); j++)
        {
            if (signal_hilb[j] > threshold && signal_hilb[j] > maximum_val)
            {
                maximum = j;
                maximum_val = signal_hilb[j];
            }
            if (signal_hilb[j] < maximum_val && maximum_val > 0)
                break;
        }

        int peak_current = i + maximum;

        //Je?li istnieja dwa piki blizej niz 200ms, oblicz srednia odleglosc miedzy pikami i wybierz ten blizszy wartosci sredniej
        if (peaks.size() >= 3 && peak_current - peaks.back() < 0.2 * fs)
        { 
            int peak_last = peaks.back();
            peaks.pop_back();

            int average_distance = CalcAverageDistance(peaks);
            int peak_second_to_last = peaks.back();

            int dist1 = abs(average_distance - (peak_second_to_last - peak_last));
            int dist2 = abs(average_distance - (peak_second_to_last - peak_current));
            if (dist1 <= dist2) 
                peaks.push_back(peak_last);
            else
                peaks.push_back(peak_current);

        }
        else
        {
            peaks.push_back(peak_current);
            
        }
        
        i = peak_current + 1; 
        max_amplitude_old = *max_amplitude;
        

    }

    for (int i = 0; i < peaks.size(); i++)
    {
        if (peaks.size() > 2) {
            peaks.erase(peaks.begin(), peaks.begin() + 2); 
        }

        return peaks;
    }

  


}