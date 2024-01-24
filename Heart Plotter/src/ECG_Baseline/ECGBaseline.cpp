/*
File - ECGBaseline.cpp
Desc - implementation of ECGBaseline class
*/

#include "ECGBaseline.h"

#include <armadillo>
#include <complex>
#include <cmath>
#include <numeric>

using namespace std::literals::complex_literals;


ECGBaseline::ECGBaseline() {}

ECGBaseline::~ECGBaseline() {}


class ECGBaseline::MovingAverageFilter {
public:
    void apply(const std::vector<DataPoint>& input, std::vector<DataPoint>& output, int windowSize) {
        // Create an Armadillo vector to store the y-values
        arma::vec yValues(input.size());
        for (size_t i = 0; i < input.size(); ++i) {
            yValues[i] = input[i].y;
        }

        // Create a filter vector for the moving average and perform the convolution
        arma::vec filter = arma::ones<arma::vec>(windowSize) / windowSize;
        arma::vec filteredSignal = arma::conv(yValues, filter, "same");

        // Fill the output vector with the original x-values and filtered y-values
        int half_window = std::floor(windowSize / 2.0);

        for (size_t i = 0; i < input.size(); ++i) {
            if (i < half_window || i >= input.size() - half_window) {
                // For edge cases, copy the input directly
                output.push_back(input[i]);
            }
            else {
                // For non-edge cases, use the filtered signal
                output.push_back(DataPoint(input[i].x, filteredSignal[i]));
            }
        }
    }
};


class ECGBaseline::LMSFilter {
public:
    void apply(const std::vector<DataPoint>& input, std::vector<DataPoint>& output, int windowSize, double delta) {
        // Calculate the desired signal
        std::vector<DataPoint> input_ref;
        MovingAverageFilter().apply(input, input_ref, windowSize);

        double error;
        double filtered_y;
        arma::vec weights = arma::zeros<arma::vec>(windowSize);
        arma::vec x = arma::zeros<arma::vec>(windowSize);

        for (int i = 0; i < input.size(); ++i) {
            // Extract vector of input samples
            for (int j = windowSize - 1; j > 0; j--) {
                x(j) = x(j - 1);
            }

            x(0) = input[i].y;

            // Calculate the filtered output
            filtered_y = arma::dot(weights, x);

            // Calculate the error
            error = input_ref[i].y - filtered_y;

            // Update the weights
            weights += delta * error * x;

            // Fill the output vector with the original x-values and filtered y-values
            output.push_back(DataPoint(input[i].x, filtered_y));
        }
    }
};


class ECGBaseline::ButterworthFilter {
private:
    // Multiply two polynomials represented as vectors of complex numbers
    std::vector<std::complex<double>> multiplyPolynomials(const std::vector<std::complex<double>>& p, const std::vector<std::complex<double>>& q) {
        std::vector<std::complex<double>> result(p.size() + q.size() - 1, 0.0);

        for (size_t i = 0; i < p.size(); ++i) {
            for (size_t j = 0; j < q.size(); ++j) {
                result[i + j] += p[i] * q[j];
            }
        }

        return result;
    }

    // Generate a polynomial from its roots
    std::vector<std::complex<double>> poly(const std::vector<std::complex<double>>& roots) {
        std::vector<std::complex<double>> result = { 1.0 };

        for (const auto& root : roots) {
            result = multiplyPolynomials(result, std::vector<std::complex<double>>{ -root, 1.0 });
        }

        std::reverse(result.begin(), result.end());
        return result;
    }

    // Design a Butterworth filter, calculate filter coefficients
    void calculateCoefficients(std::vector<double>& coeffs_a, std::vector<double>& coeffs_b, int order, double cutoff, double samplingRate)
    {
        static constexpr double PI = 3.14159265358979323846;
        std::vector<std::complex<double>> polesAnalog(order);
        std::vector<std::complex<double>> polesDigital(order);
        std::vector<std::complex<double>> zerosDigital(order, -1.0);

        // Calculate poles of analog filter (scaled in frequency)
        double Fc = samplingRate / PI * tan(PI * cutoff / samplingRate);
        double scaleFactor = 2 * PI * Fc;

        for (int j = 0; j < order; j++)
        {
            int k = j + 1;
            double theta = (2 * k - 1) * PI / (2 * order);
            polesAnalog[j] = (-sin(theta) + 1.0i * cos(theta)) * scaleFactor;
        }


        // Calculate poles of digital filter
        for (size_t i = 0; i < order; i++)
            polesDigital[i] = (1.0 + polesAnalog[i] / (2 * samplingRate)) / (1.0 - polesAnalog[i] / (2 * samplingRate));

        // Calculate coeffs
        auto a = poly(polesDigital);
        for (size_t i = 0; i < a.size(); i++)
            a[i] = a[i].real();

        auto b = poly(zerosDigital);
        auto K = std::accumulate(a.begin(), a.end(), 0.0i) / std::accumulate(b.begin(), b.end(), 0.0i);
        for (size_t i = 0; i < b.size(); i++)
            b[i] *= K;

        coeffs_a.clear();
        coeffs_b.clear();

        for (auto coeff : a)
            coeffs_a.push_back(coeff.real());
        for (auto coeff : b)
            coeffs_b.push_back(coeff.real());
    }

public:
    void apply(const std::vector<DataPoint>& input, std::vector<DataPoint>& output, int order, double cutoff, double samplingRate) {
        std::vector<double> a, b;
        calculateCoefficients(a, b, order, cutoff, samplingRate);

        std::vector<double> filteredSignal(input.size(), 0.0);

        // Apply the filter to the input signal
        for (size_t i = 0; i < input.size(); ++i) {
            for (size_t j = 0; j < b.size(); ++j) {
                if (i >= j) {
                    filteredSignal[i] += b[j] * input[i - j].y;
                }
            }
            for (size_t j = 1; j < a.size(); ++j) {
                if (i >= j) {
                    filteredSignal[i] -= a[j] * filteredSignal[i - j];
                }
            }
        }

        // Fill the output vector with the original x-values and filtered y-values
        for (size_t i = 0; i < input.size(); i++) {
            output.push_back(DataPoint(input[i].x, filteredSignal[i]));
        }
    }
}; 


class ECGBaseline::SavitzkyGolayFilter {
private:
    arma::vec calculateCoefficients(int windowSize, int order) {
        int half_window = std::floor(windowSize / 2.0);
        arma::mat V(windowSize, order + 1);

        // Fill the Vandermonde matrix
        for (int i = -half_window; i <= half_window; ++i) {
            for (int j = 0; j <= order; ++j) {
                V(i + half_window, j) = std::pow(i, j);
            }
        }

        // Compute the pseudoinverse
        arma::mat Vtrans = arma::trans(V);
        arma::mat coeffs = arma::inv(Vtrans * V) * Vtrans;

        // Return the coefficients
        return coeffs.row(0).t();
    }

public:
    void apply(const std::vector<DataPoint>& input, std::vector<DataPoint>& output, int windowSize, int order) {
        // Create an Armadillo vector to store the y-values
        arma::vec yValues(input.size());
        for (size_t i = 0; i < input.size(); ++i) {
            yValues[i] = input[i].y;
        }

        // Create a filter vector and perform the convolution
        arma::vec filter = calculateCoefficients(windowSize, order);
        arma::vec filteredSignal = arma::conv(yValues, filter, "same");

        // Fill the output vector with the original x-values and filtered y-values
        int half_window = std::floor(windowSize / 2.0);

        for (size_t i = 0; i < input.size(); ++i) {
            if (i < half_window || i >= input.size() - half_window) {
                // For edge cases, copy the input directly
                output.push_back(input[i]);
            }
            else {
                // For non-edge cases, use the filtered signal
                output.push_back(DataPoint(input[i].x, filteredSignal[i]));
            }
        }
    }
};



OperationStatus ECGBaseline::set_filter_type(iECGBaseline::FilterType filterType) {
    this->filter_type = filterType;
    return success;
}


OperationStatus ECGBaseline::set_filter_parameters(iECGBaseline::FilterParameters filterParams) {
    //Validate parameters
    switch (this->filter_type) {
    case MOVING_AVERAGE_FILTER:
        if (filterParams.windowSize <= 0) {
            return error;
        }
        break;

    case BUTTERWORTH_FILTER:
        if (filterParams.order <= 0 || filterParams.cutoff <= 0 || filterParams.cutoff >= filterParams.samplingRate / 2) {
            return error;
        }
        break;

    case SAVITZKY_GOLAY_FILTER:
        if (filterParams.windowSize <= 0 || filterParams.windowSize % 2 == 0 || filterParams.order < 0 || filterParams.order >= filterParams.windowSize) {
            return error;
        }
        break;

    case LMS_FILTER:
        if (filterParams.windowSize <= 0 || filterParams.delta <= 0 || filterParams.delta > 1) {
            return error;
        }
        break;
    }

    // If all validations pass, set the parameters
    this->filter_parameters = filterParams;
    return success;
}


OperationStatus ECGBaseline::filter_signal(std::vector<DataPoint> inputSignal) {
    // Clear the existing filtered signal
    this->filtered_signal.clear();
    this->filtered_signal.reserve(inputSignal.size());

    // Check the filter type and apply the corresponding filter
    switch (this->filter_type) {
        case MOVING_AVERAGE_FILTER: {
            MovingAverageFilter SMA_filter;
            SMA_filter.apply(inputSignal, this->filtered_signal, this->filter_parameters.windowSize);
            break;
        }
        case SAVITZKY_GOLAY_FILTER: {
            SavitzkyGolayFilter SG_filter;
            SG_filter.apply(inputSignal, this->filtered_signal, this->filter_parameters.windowSize, this->filter_parameters.order);
            break;
        }
        case LMS_FILTER: {
            LMSFilter LMS_filter;
            LMS_filter.apply(inputSignal, this->filtered_signal, this->filter_parameters.windowSize, this->filter_parameters.delta);
            break;
        }
        case BUTTERWORTH_FILTER: {
            ButterworthFilter BW_filter;
            BW_filter.apply(inputSignal, this->filtered_signal, this->filter_parameters.order, this->filter_parameters.cutoff, this->filter_parameters.samplingRate);
            break;
        }
    }
    return success;
}