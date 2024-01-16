#include "iHRV1.h"
#include <armadillo>

class HRV1 : public iHRV1
{
	public:
	OperationStatus calculate_time_parameters(std::vector<DataPoint> signal,
		std::vector<int> r_peaks);

	OperationStatus calculate_frequency_parameters(std::vector<DataPoint> signal,
		std::vector<int> r_peaks);

	private:
	// calculate periodogram
	arma::rowvec my_periodogram(arma::rowvec f_vec, arma::vec signal_interp, arma::vec t_interp);
	// calculate intervals in [ms]
	arma::vec prepare_data(std::vector<DataPoint> signal, std::vector<int> r_peaks);

};