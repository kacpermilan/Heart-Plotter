#include <iostream>
#include <list>
#include <Eigen/Dense>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include "STSegment.h"


std::list<double> STSegment::calculateSecondDerivative(const std::list<double>& ecg_fragment) {
    std::list<double> result;

    auto it = ecg_fragment.begin();
    if (it != ecg_fragment.end()) {
        auto prev1 = it;
        auto prev2 = it;
        ++it;

        for (; it != ecg_fragment.end(); ++it) {
            double diff1 = *it - *prev1;
            double diff2 = diff1 - (*prev1 - *prev2);
            result.push_back(diff2);

            ++prev1;
            ++prev2;
        }
    }

    return result;
}
std::list<bool> STSegment::findInflectionPoints(const std::list<double>& second_derivative, double threshold) {
    std::list<bool> inflection_points;

    for (const auto& value : second_derivative) {
        inflection_points.push_back(std::abs(value) > threshold);
    }

    return inflection_points;
}
std::vector<double> STSegment::polyfit(const std::vector<double>& x, const std::vector<double>& y, int degree) {
    int n = x.size();
    Eigen::MatrixXd X(n, degree + 1);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= degree; ++j) {
            X(i, j) = std::pow(x[i], j);
        }
    }

    Eigen::VectorXd Y(n);
    for (int i = 0; i < n; ++i) {
        Y(i) = y[i];
    }

    Eigen::VectorXd coeffs = X.colPivHouseholderQr().solve(Y);

    std::vector<double> coefficients(coeffs.data(), coeffs.data() + coeffs.size());
    return coefficients;
}
int STSegment::fitStraight(const std::vector<double>& series, int index, int x, int position) {


    // Extract the window around the given index
    index = index - 1;
    int startIndex = std::max(index - x, 0);
    int endIndex = std::min(index + x, static_cast<int>(series.size()) - 1);

    std::vector<double> window_series;

    if (position == 4) {
        // Set the window_series for 4th point 
        int customStartIndex = std::max(index - x, 0);
        int customEndIndex = index;

        window_series.assign(series.begin() + customStartIndex, series.begin() + customEndIndex);
    }
    else {
        // Use the regular range
        window_series.assign(series.begin() + startIndex, series.begin() + endIndex + 1);
    }

    // Fit a straight line using polyfit with degree 1
    std::vector<double> indices(window_series.size());
    std::iota(indices.begin(), indices.end(), 1);

    std::vector<double> coeffs = polyfit(indices, window_series, 1);

    // Determine the slope based on the coefficient
    double slope = coeffs[1];
    if (slope > 0.0002) {
        return 1;
    }
    else if (-0.0002 <= slope && slope <= 0.0002) {
        return 0;
    }
    else if (slope < -0.0002) {
        return -1;
    }

    return 0; // Default case, should not reach here
}

OperationStatus STSegment::stsegment_main(
    std::vector<double> qrs_offset,
    std::vector<DataPoint> signal,
    std::vector<double> t_waves) {
    /* main method od STSegment class
    run with:
    qrs_offset -> QRS_offset from Waves.cpp
    signal -> same imput signal as in Waves::calculatePhase method in Waves.cpp
    t_waves -> T_waves from Waves.cpp
    */
    try {
        // initialize variable
        std::vector<float> ecg = signal.y;
        int t = 0;
        int max_sublist = 0;
        double T_point, Qrs_end_point;
        std::list<double>ecg_fragment;
        std::list<double> second_derivative;
        std::vector<std::vector<int>> listed;
        SegmentType segmenttype;
        double threshold = 0.005;
        int Third_point = 0;
        int Third_value = 0;
        this->classified_segments.clear();

        //this->classified_segments.reserve(output.size());
        while (t < t_waves.size()) {
            T_point = t_waves[t];
            Qrs_end_point = qrs_offset[t];

            // Check if indices are within bounds
            if (Qrs_end_point >= 0 && T_point < ecg.size() && Qrs_end_point < T_point) {

                // Extract the ECG STSegment using iterators
                auto startIt = std::next(ecg.begin(), Qrs_end_point);
                auto endIt = std::next(ecg.begin(), T_point);

                ecg_fragment.assign(startIt, endIt);
            }
            else {
                std::cerr << "Invalid indices: Qrs_end_point=" << Qrs_end_point << ", T_point=" << T_point << std::endl;
            }
            try {
                second_derivative = calculateSecondDerivative(ecg_fragment);
                std::list<bool> inflection_points = findInflectionPoints(second_derivative, threshold);
                int n = 0;

                auto it = inflection_points.begin();
                for (int count = 1; it != inflection_points.end(); ++count, ++it) {
                    bool value = *it;
                    if (value == true) {
                        n++;
                        listed.push_back({ n, count });
                    }
                    else if (value == false) {
                        n = 0;
                    }
                }
                for (const auto& pair : listed) {
                    if (pair[0] > max_sublist) {
                        max_sublist = pair[0];
                    }
                }
                // Find Third_point and Third_value
                for (const auto& pair : listed) {
                    if (pair[0] == max_sublist) {
                        Third_point = pair[1] - static_cast<int>(max_sublist / 2) + Qrs_end_point;
                        Third_value = 0;
                        break; // Stop searching after finding the first occurrence with the maximum sublist
                    }
                }
            }
            catch (const std::exception& e) {
                Third_point = Qrs_end_point + ((T_point - Qrs_end_point) / 2);
                Third_value = fitStraight(ecg, Third_point, 6, 3);
                std::cerr << "Error: " << e.what() << std::endl;
            }
            int Second_point = Qrs_end_point + ((Third_point - Qrs_end_point) / 2);
            int First_value = fitStraight(ecg, Qrs_end_point, 6, 1);
            int Second_value = fitStraight(ecg, Second_point - 4, 6, 2);
            int Fourth_value = fitStraight(ecg, T_point, 6, 4);

            std::vector<int> T_segment = { First_value, Second_value, Third_value, Fourth_value };

            if (T_segment == std::vector<int>{1, -1, 0, 1} || T_segment == std::vector<int>{-1, -1, 0, 1}) {
                segmenttype = SegmentType::SEVERE_MYO_INFARCTION;
            }
            else if (T_segment == std::vector<int>{1, 1, 1, 1} || T_segment == std::vector<int>{0, 1, 1, 1}) {
                segmenttype = SegmentType::ACUTE_MYO_INFARCTION;
            }
            else if (T_segment == std::vector<int>{-1, -1, -1, -1} || T_segment == std::vector<int>{-1, -1, -1, 0}) {
                segmenttype = SegmentType::MYOCARDIALSCHEMIA;
            }
            else {
                segmenttype = SegmentType::HEALTHY;
            }
            classified_segments.push_back(segmenttype);
            ++t;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return OperationStatus::ERROR;
    }
    return OperationStatus::SUCCESS;
}
