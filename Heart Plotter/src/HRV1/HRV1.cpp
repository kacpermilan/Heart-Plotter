#include "HRV1.h"

#define PI 3.14159265358979323846


OperationStatus HRV1::calculate_time_parameters(std::vector<DataPoint> signal,
	std::vector<int> r_peaks)
{
	OperationStatus status = ERROR;

	try
	{
		arma::vec r_peaks_intervals = prepare_data(signal, r_peaks);

		RR_mean = arma::mean(r_peaks_intervals);

		SDNN = arma::stddev(r_peaks_intervals);

		arma::vec RR_diffs = arma::abs(arma::diff(r_peaks_intervals));

		arma::mat pow_sig = arma::pow(RR_diffs, 2);
		double mean_val = arma::mean(arma::mean(pow_sig));
		RMSSD = sqrt(mean_val);

		arma::umat mask = (RR_diffs > 50);
		NN50 = arma::accu(mask);

		pNN50 = (NN50 / RR_diffs.n_elem) * 100;

		status = SUCCESS;
	}
	catch (...)
	{
		status = ERROR;
	}

	return status;
}


OperationStatus HRV1::calculate_frequency_parameters(std::vector<DataPoint> signal,
	std::vector<int> r_peaks)
{
	OperationStatus status = ERROR;

	try
	{
		arma::vec r_peaks_intervals = prepare_data(signal, r_peaks);
		arma::vec t = arma::cumsum(r_peaks_intervals) / 1000;
		
		r_peaks_intervals = r_peaks_intervals - arma::mean(r_peaks_intervals);

		double x_time_length = t.back();
		double number_of_samples = r_peaks_intervals.n_elem;
		double x_period = x_time_length / number_of_samples;

		// set frequnecy vector 
		int nfft = 512;
		double f_max = (1 / x_period) / 2;
		// f_interval = f_max / nfft;
		arma::rowvec f_vec = arma::linspace<arma::rowvec>(0, f_max, nfft);

		// interpolation
		arma::vec t_interp = arma::regspace(t[0], x_period, x_period * (number_of_samples - 1));
		arma::vec r_peaks_interp;
		arma::interp1(t, r_peaks_intervals, t_interp, r_peaks_interp, "linear");
		r_peaks_interp = r_peaks_interp - arma::mean(r_peaks_interp);

		// calculate periodogram
		arma::rowvec P = my_periodogram(f_vec, r_peaks_interp, t_interp);

		// calculate frequency parameters in absolute values [ms2/Hz]
		double length_of_the_signal_in_hours = ((x_time_length / 1000) / 60) / 60;

		std::vector<double> HF_bounds = { 0.15, 0.4 }; // (>
		std::vector<double> LF_bounds = { 0.04, 0.15 }; // (>
		std::vector<double> VLF_bounds = { 0.0033, 0.04 }; // (>
		std::vector<double> ULF_bounds = { 0, 0.0033 }; // (>


		arma::uvec HF_ind = arma::find(f_vec > HF_bounds[0] && f_vec <= HF_bounds[1]);
		arma::uvec LF_ind = arma::find(f_vec > LF_bounds[0] && f_vec <= LF_bounds[1]);
		arma::uvec VLF_ind = arma::find(f_vec > VLF_bounds[0] && f_vec <= VLF_bounds[1]);
		arma::uvec ULF_ind = arma::find(f_vec > ULF_bounds[0] && f_vec <= ULF_bounds[1]);

		arma::mat HF_power = arma::trapz(f_vec.elem(HF_ind), P.elem(HF_ind));
		HF = HF_power(0);
		arma::mat LF_power = arma::trapz(f_vec.elem(LF_ind), P.elem(LF_ind));
		LF = LF_power(0);
		arma::mat VLF_power = arma::trapz(f_vec.elem(VLF_ind), P.elem(VLF_ind));
		VLF = VLF_power(0);
		LFHF = LF / HF;

		if (length_of_the_signal_in_hours > 24) {
			arma::mat ULF_power = arma::trapz(f_vec.elem(ULF_ind), P.elem(ULF_ind));
			ULF = ULF_power(0);
			TP = HF + LF + VLF + ULF;
		}
		else {
			ULF = -1; // if signal length is less than 24h, set ULF param to -1
			TP = HF + LF + VLF;
		}

		status = SUCCESS;
	}
	catch (...)
	{
		status = ERROR;
	}

	return status;
}

// calculate periodogram for interpolated (evenly spaced) signal
arma::rowvec HRV1::my_periodogram(arma::rowvec f_vec, arma::vec signal_interp, arma::vec t_interp)
{
	arma::rowvec P = arma::zeros<arma::rowvec>(f_vec.n_elem);
	int N = signal_interp.n_elem;

	arma::mat temp_cos = arma::cos(2 * PI * t_interp * f_vec);
	arma::mat temp_sin = arma::sin(2 * PI * t_interp * f_vec);

	arma::mat temp1 = arma::zeros(arma::size(temp_cos));
	arma::mat temp2 = arma::zeros(arma::size(temp_sin));

	for (int i = 0; i < f_vec.n_elem; i++)
	{
		temp1.col(i) = signal_interp % temp_cos.col(i);
		temp2.col(i) = signal_interp % temp_sin.col(i);
	}

	arma::mat sum_cos = arma::sum(temp1);
	arma::mat sum_sin = arma::sum(temp2);

	P = (arma::pow(sum_cos, 2) + arma::pow(sum_sin, 2)) / N;

	return P;
}

// calculating intervals between R peaks in [ms], assuming that r_peaks is vector of indices where R peaks are located
arma::vec HRV1::prepare_data(std::vector<DataPoint> signal, std::vector<int> r_peaks)
{
	std::sort(r_peaks.begin(), r_peaks.end());
	arma::vec t = arma::zeros(r_peaks.size());

	for (int i = 0; i < r_peaks.size(); i++)
	{
		t(i) = signal[r_peaks[i]].x;
	}

	arma::vec r_peaks_sig = arma::diff(t);
	r_peaks_sig = r_peaks_sig * 1000; // to [ms]

	return r_peaks_sig;
}