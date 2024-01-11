#include "Waves.h"


// calculating an envelop of the signal
arma::vec Waves::SignalEnvelop(std::vector<DataPoint> signal){
    std::vector<float> ecg  = signal.y;

    RPeaks rPeaksObject;
    // calculating hilbert transform using method from RPeaks
    std::vector<float> signal_hilbert = rPeaksObject.CalculateHilbertTransform(ecg, first); 
    
    //conversion std::vector<float> to arma::fev
    arma::fvec armaSignal_hilbert(signal_hilbert.data(), signal_hilbert.size(), false, false);
    arma::fvec armaSignal(ecg.data(), ecg.size(), false, false);
    
    // squaring data elemnt-wise
    arma::fvec  signal_hilbert_squared = arma::square(armaSignal_hilbert);
    arma::fvec  signal_squared = arma::square(armaSignal);

    //calculating an envelop  of the signal
    arma::fvec signal_envelop = arma::square(signal_squared + signal_hilbert_squared);

    //conversion to double
    arma::vec signal_envelop_double = arma::conv_to<arma::vec>::from(signal_envelop);

    // return values of the signal envelop
    return signal_envelop_double;
}


// fs - sampling frequency of the processed signal
//calculating derivative ; estimating with the parabola which best fits the five points of the ecg signal
arma::vec Waves::FivePointDerivative(arma::vec envelop, int fs = 360){

    //setting parameters
    int r = std::ceil(fs / 250.0);

    //inicializing derivative_envelop
    arma::vec derivative_envelop(envelop.size());

    // calculating derivative_envelop
    for (int i = 4; i < envelop.size() - 4; i++) {
        derivative_envelop(i) = (1.0 / 10.0) * (2.0 * (envelop(i + 2 * r) - envelop(i - 2 * r)) + envelop(i + r) - envelop(i - r));
    }
    // return values of the derivative of the signal
    return derivative_envelop;

}

//finding local minima on the derivative of the signal
arma::vec Waves::FindDerivativeMinima(arma::vec derivative_envelop){
    //vector with indicies where derivative change its sign
    std::vector<size_t> derivative_zeros_idx;

    //finding local minima
        for (size_t i = 1; i <= derivative_envelop.size() - 1; i++) {
            if ((derivative_envelop[i - 1] < 0.0) && (derivative_envelop[i] >= 0.0)) {
                derivative_zeros_idx.push_back(i);
            }
        }
    // conversion from std::vector to arma::vec
    arma::vec derivative_zeros_idx_arma(derivative_zeros_idx.begin(), derivative_zeros_idx.end());
    
    //returing idicies of local minima
    return derivative_zeros_idx_arma;
}



// seraching for indicies of offsets or onsets depending on the given method
std:vector<double> Waves::SearchForOnsetOffset(arma::vec derivative_zeros_idx, arma::imat windows, std::string method){
    // inicializing a vector for indicies of onsets or offstes
    std:vector<double> onset_offset_idx;

    for (int i = 0; i <= windows.row(0).size(), i++){
        std::vector<double> temp;
        // choosing idicies of potential onsets/offsets within a given window
        for (int j = 0; derivative_zeros_idx.size()){
            if ((windows_qrs_onset.row(0)(i) <= derivative_zeros_idx(j)) && (derivative_zeros_idx(j) <= windows_qrs_onset.row(1)(i))){
                onset_offset_idx.push_back(derivative_zeros_idx(j));
            }
        }

        // choosing one index of onset/offset per window depending of method specified 
        if (!temp.empty()){
            if (temp.size() >= 2){
                if (method == "onset"){
                    auto max_element = std::max_element(temp.begin(), temp.end());
                    onset_offset_idx.push_back(*max_element);
                } 
                else if (method == "offset"){
                    auto min_element = std::min_element(temp.begin(), temp.end());
                    onset_offset_idx.push_back(*min_element);
                }
            else {
                onset_offset_idx.push_back(temp[0]);
            }
            }
        }
    }
    // returning indicies of offsets or onsets 
    return onset_offset_idx;
}



// detect QRS onsets in signal
OperationStatus Waves::detect_QRS_onsets(std::vector<int> r_peaks, arma::vec derivative_zeros_idx){
    // checking if r_peaks is empty, if so return an error
    if (r_peaks.empty()) {
        return OperationStatus::ERROR;
    }

    // creating 2-dimentional matrix with windows in which onsets are searched
    arma::imat windows_qrs_onset(2, r_peaks.size());
 
    windows_qrs_onset.row(0) = arma::conv_to<arma::rowvec>(arma::rowvec(r_peaks) - 60); // beginings of the windows
    windows_qrs_onset.row(1) = arma::conv_to<arma::rowvec>(arma::rowvec(r_peaks) - 20); // ends of the windows

    std::vector<double> QRS_onsets = Waves::SearchForOnsetOffset(derivative_zeros_idx, windows_qrs_onset, "onset");

    return OperationStatus::SUCCESS;
}


// detect QRS offsets in signal
OperationStatus Waves::detect_QRS_ends(std::vector<int> r_peaks, arma::vec derivative_zeros_idx){
    // checking if r_peaks is empty, if so return an error
    if (r_peaks.empty()) {
        return OperationStatus::ERROR;
    }

    // creating 2-dimentional matrix with windows in which offsets are searched
    arma::imat windows_qrs_offset(2, r_peaks.size());

    windows_qrs_offset.row(0) = arma::conv_to<arma::rowvec>(arma::rowvec(r_peaks) + 15); // beginings of the windows
    windows_qrs_offset.row(1) = arma::conv_to<arma::rowvec>(arma::rowvec(r_peaks) + 60); // ends of the windows

    std::vector<double> QRS_offsets = Waves::SearchForOnsetOffset(derivative_zeros_idx, windows_qrs_offset, "offset");

    return OperationStatus::SUCCESS;
}

//calculating phase of the signal
arma::vec Waves::calculatePhase(std::vector<DataPoint> signal, double Rv){
    arma::vec ecg  = signal.y;

    //signal division by Rv
    arma::vec  signalDivision = ecg/Rv;
    // calculating phase = arcus tangent form divided signal
    arma::vec fi = arma::atan(signalDivision);

    return fi;
}

//calculating rr_intervals between r_peaks
std::vector<int> Waves::calculateRRintervals(std::vector<int> r_peaks){
    std::vector<int> rr_intervals(r_peaks.size() - 1);

    for (size_t i = 0; i <= r_peaks.size() - 1; i++) {
        rr_intervals[i] = r_peaks[i + 1] - r_peaks[i];
    }

    return rr_intervals;
}

// detecting T wave or T end or P wave or P onset or P offset in a given window
std::vector<double> Waves::detecting_T_P_points(std::vector<DataPoint> signal, arma::imat windows, double Rv, std::string method){
    std::vector<size_t> detected_points;
    //calculating 
    arma::vec fi = Waves::calculatePhase(signal, Rv);

    //detecting points in a given window
    for (int i = 0; i <= windows.row(0).size(), i++){
        int lower_idx = std::ceil(windows.row(0)(i));  //rounding to obtain indicies
        int upper_idx = std::floor(windows.row(1)(i)); //rounding to obtain indicies

        //choosing a fragment of phase within a given window
        arma::vec phase_fragment = fi.subvec(static_cast<arma::uword>(lower_idx), static_cast<arma::uword>(upper_idx));

        // finding an index of a maximum/minimum value of the fragment
        if (method == "wave"){
            double max_idx = phase_fragment.index_max();
        }else
        if (method = "onset_offset"){
            double max_idx = phase_fragment.index_min();
        }


        detected_points.push_back(max_idx + static_cast<double>(lower_idx) - 1);
    }

    return detected_points;
}


// detect T waves in signal
std::vector<double> Waves::finding_T_waves(std::vector<DataPoint> signal, std::vector<int> r_peaks){
    // setting parameters
    double Rv = 0.1; 
    // calculating phase of the signal
    arma::vec fi = Waves::calculatePhase(signal, Rv);

    //shortening r_peaks vector, becasuse there is fewer rr_intervals than r_peaks
    std::vector<int> r_peaks2(r_peaks.begin(), r_peaks.end() - 1);

    //calculating rr_intervals
    std::vector<int> rr_intervals = Waves::calculateRRintervals(r_peaks);

    // creating 2-dimentional matrix with windows in which T waves are searched
    arma::imat windows_T_wave(2, r_peaks.size()-1);
 
    windows_T_wave.row(0) = arma::conv_to<arma::rowvec>(arma::rowvec(r_peaks) + 0.16*arma::rowvec(rr_intervals)); // beginings of the windows
    windows_T_wave.row(1) = arma::conv_to<arma::rowvec>(arma::rowvec(r_peaks) + 0.57*arma::rowvec(rr_intervals)); // ends of the windows

    //finding T_wave indicies
    std::vector<double> T_waves = Waves::detecting_T_P_points(signal, windows_T_wave, Rv, "waves");

    return T_waves;
}


// detect T ends in signal
OperationStatus Waves::detect_T_ends(std::vector<DataPoint> signal, std::vector<int> r_peaks){
    // checking if r_peaks is empty, if so return an error
    if (r_peaks.empty()) {
        return OperationStatus::ERROR;
    }

    // finding T_wave idicies to determine proper bounds for searching windows
    std::vector<double> T_waves = Waves::finding_T_waves(signal, r_peaks);

    // creating 2-dimentional matrix with windows in which T ends are searched
    arma::imat windows_T_end(2, T_waves.size());
 
    windows_T_end.row(0) = arma::conv_to<arma::rowvec>(T_waves); // beginings of the windows
    windows_T_end.row(1) = arma::conv_to<arma::rowvec>((T_waves) + 55); // ends of the windows

    //finding indicies of ends of T wave
    Rv = 0.1;
    std::vector<double> T_ends = Waves::detecting_T_P_points(signal, windows_T_end, Rv, "onset_offset");

    return OperationStatus::SUCCESS;
}


// detect P waves in signal
std::vector<double> Waves::finding_P_waves(std::vector<DataPoint> signal, std::vector<int> r_peaks){
    // setting parameters
    double Rv = 0.05; 
    // calculating phase of the signal
    arma::vec fi = Waves::calculatePhase(signal, Rv);

    //shortening r_peaks vector, becasuse there is fewer rr_intervals than r_peaks
    std::vector<int> r_peaks2(r_peaks.begin()+1, r_peaks.end());

    //calculating rr_intervals
    std::vector<int> rr_intervals = Waves::calculateRRintervals(r_peaks);

    // creating 2-dimentional matrix with windows in which P waves are searched
    arma::imat windows_P_wave(2, r_peaks.size()-1);
 
    windows_T_wave.row(0) = arma::conv_to<arma::rowvec>(arma::rowvec(r_peaks) - 0.4*arma::rowvec(rr_intervals)); // beginings of the windows
    windows_T_wave.row(1) = arma::conv_to<arma::rowvec>(arma::rowvec(r_peaks) - 0.07*arma::rowvec(rr_intervals)); // ends of the windows

    //finding P_wave indicies
    std::vector<double> T_waves = Waves::detecting_T_P_points(signal, windows_T_onset, Rv, "waves");

    return P_waves;
}




// detect P offstes in signal
OperationStatus Waves::detect_P_offsets(std::vector<DataPoint> signal, std::vector<int> r_peaks){
    // checking if r_peaks is empty, if so return an error
    if (r_peaks.empty()) {
        return OperationStatus::ERROR;
    }

    // finding P_wave idicies to determine proper bounds for searching windows
    std::vector<double> P_waves = Waves::finding_P_waves(signal, r_peaks);

    // creating 2-dimentional matrix with windows in which P offsets are searched
    arma::imat windows_P_offsets(2, P_waves.size());
 
    windows_P_offsets.row(0) = arma::conv_to<arma::rowvec>(P_waves); // beginings of the windows
    windows_P_offsets.row(1) = arma::conv_to<arma::rowvec>(P_waves + 20); // ends of the windows

    //finding indicies of offsets of P wave
    Rv = 0.05;
    std::vector<double> P_offsets = Waves::detecting_T_P_points(signal, windows_P_offsets, Rv, "onset_offest");

    return OperationStatus::SUCCESS;
}


// detect P onsets in signal
OperationStatus Waves::detect_P_onsets(std::vector<DataPoint> signal, std::vector<int> r_peaks){
    // checking if r_peaks is empty, if so return an error
    if (r_peaks.empty()) {
        return OperationStatus::ERROR;
    }

    // finding P_wave idicies to determine proper bounds for searching windows
    std::vector<double> P_waves = Waves::finding_P_waves(signal, r_peaks);

    // creating 2-dimentional matrix with windows in which P onsets are searched
    arma::imat windows_P_onsets(2, P_waves.size());
 
    windows_P_onsets.row(0) = arma::conv_to<arma::rowvec>(P_waves - 30); // beginings of the windows
    windows_P_onsets.row(1) = arma::conv_to<arma::rowvec>(P_waves); // ends of the windows

    //finding indicies of onsets of P wave
    Rv = 0.05;
    std::vector<double> P_onsets = Waves::detecting_T_P_points(signal, windows_P_onsets, Rv, "onset_offset");

    return OperationStatus::SUCCESS;
}





