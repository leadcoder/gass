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
		SetGain(kp,ki,kd);
	}
	void PIDControl::SetGain(double kp, double ki, double kd) {
		Kp = kp;
		Ki = ki;
		Kd = kd;
	}

	void PIDControl::Set(double r) {
		//intsum = 0;
		desired = r;
	}

	double PIDControl::Update(double input, double dt) {
		const double error = desired-input;
		const double der = (error-prev)/dt;
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

	void PIDControl::SetOutputLimit(double limit) {
		outlimit = limit;
	}

	void PIDControl::SetIntCap(double limit) {
		intcap = limit;
	}
}