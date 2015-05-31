#include "GASSPIDControl.h"
#include <limits>

using namespace std;
namespace GASS
{

	PIDControl::PIDControl() {
		Kp = Ki = Kd = 0;
		desired = 0;
		intsum = prev = 0;
		outlimit = numeric_limits<double>::infinity();
		intcap = numeric_limits<double>::infinity();
	}

	PIDControl::PIDControl(double kp, double ki, double kd) {
		Kp = Ki = Kd = 0;
		desired = 0;
		intsum = prev = 0;
		outlimit = numeric_limits<double>::infinity();
		intcap = numeric_limits<double>::infinity();
		setGain(kp,ki,kd);
	}
	void PIDControl::setGain(double kp, double ki, double kd) {
		Kp = kp;
		Ki = ki;
		Kd = kd;
	}

	void PIDControl::set(double r) {
		//intsum = 0;
		desired = r;
	}

	double PIDControl::update(double input, double dt) {
		double error = desired-input;
		double der = (error-prev)/dt;
		intsum += error*dt;
		if (intsum > intcap)
			intsum = intcap;
		if (intsum < -intcap)
			intsum = -intcap;
		double output = Kp*error + Ki*intsum + Kd*der;
		prev = error;
		if (output > outlimit)
			output = outlimit;
		if (output < -outlimit)
			output = -outlimit;
		return output;
	}

	void PIDControl::setOutputLimit(double limit) {
		outlimit = limit;
	}

	void PIDControl::setIntCap(double limit) {
		intcap = limit;
	}
}