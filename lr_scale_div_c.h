/*
	class, that contains tick lists
	MajorTicks[nMajorTicks] - coords of MajorTicks
	MajorTicksLabels[nMajorTicks] - labels of MajorTicks
	MediumTicks[nMediumTicks] - coords of MediumTicks
	MediumTicksLabels[nMediumTicks] - labels of MediumTicks - not sure if i need them. i'll keep 'em for additional info
	MinorTicks[nMinjorTicks] - coords of MinorTicks
	MinorTicksLabels[nMinorTicks] - labels of MinorTicks - same as medium ticks

	All the drawing will be done in veholder.
	
*/

#include <math.h>
#include <algorithm>
//#include <array>

namespace LogRun {
class TickDivider {
public:
	
	TickDivider() {};

	TickDivider(double depthValueUpper, double depthValueLower,
				double heightUpper, double heightLower)
				: d_upper_(fabs(depthValueUpper))
				, d_lower_(fabs(depthValueLower))
				, h_upper_(heightUpper)
				, h_lower_(heightLower)
	{
		//
		odepth_ = fabs(fabs(d_lower_) - fabs(d_upper_));
		oheight_ = h_lower_ - h_upper_;
		mInPx_ = odepth_/oheight_;
	}

	~TickDivider();

	/*
		Count correct step

		fontSize - to count the min step - in
		//mInPx - ratio of meters and pixels
		firstTick - before finishing has to be converted into pixel coords - out
		step - before finishing has to be converted into pixel coords - out
		decDigits - number of digits 
	*/
	void findEvenDivision(int fontSize, 
				double& firstTick, double& step)//,
				//int& decDigits) 
	{
		//
		double coef[] = {2, 2.5, 2}; 
		bool ready;
		double nextStep;
		//int k, k0;

		double minStep = 2*fontSize*mInPx_;

		nextStep = pow(10, ceil(log10(odepth_)));
		int coef_size = 3; 
		while(!ready) {
			for(int k=0; k< *std::max_element(coef, coef + coef_size); ++k) {
				step = nextStep;
				nextStep = step/coef[k];
				ready = (nextStep < minStep);
				if (ready) break;
			}
		}
		firstTick = ceil(d_upper_/step)*step;
		//firstTick /= mInPx_;
		//step /= mInPx_;

	}

	double mInPx_; //

private:
	
	double d_upper_;
	double d_lower_;
	double h_upper_;
	double h_lower_;

	double odepth_;
	double oheight_;

	

}; //TickDivider
} //ns LogRun