

#include "iSTSegment.h"
#include "../Common_Types/CommonTypes.h"

class STSegment : public iSTSegment {
public:
		STSegment();
		virtual ~STSegment();

		OperationStatus stsegment_main(
			std::vector<double> qrs_offset,
			std::vector<DataPoint> signal,
			std::vector<double> t_waves);

private:
	std::list<double> calculateSecondDerivative(const std::list<double>& ecg_fragment);
	std::list<bool> findInflectionPoints(const std::list<double>& second_derivative, double threshold);
	std::vector<double> polyfit(const std::vector<double>& x, const std::vector<double>& y, int degree);
	int fitStraight(const std::vector<double>& series, int index, int x, int position);


		

};