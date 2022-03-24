#include "GASSPIDControl.h"
#include <limits>

using namespace std;
namespace GASS
{

	PIDControl::PIDControl() {
		m_Kp = m_Ki = m_Kd = 0;
		m_Desired = 0;
		m_Intsum = m_Prev = 0;
		m_Outlimit = numeric_limits<double>::infinity();
		m_Intcap = numeric_limits<double>::infinity();
	}

	PIDControl::PIDControl(double kp, double ki, double kd) {
		m_Kp = m_Ki = m_Kd = 0;
		m_Desired = 0;
		m_Intsum = m_Prev = 0;
		m_Outlimit = numeric_limits<double>::infinity();
		m_Intcap = numeric_limits<double>::infinity();
		SetGain(kp,ki,kd);
	}
	void PIDControl::SetGain(double kp, double ki, double kd) {
		m_Kp = kp;
		m_Ki = ki;
		m_Kd = kd;
	}

	void PIDControl::Set(double r) {
		//intsum = 0;
		m_Desired = r;
	}

	double PIDControl::Update(double input, double dt) {
		const double error = m_Desired-input;
		const double der = (error-m_Prev)/dt;
		m_Intsum += error*dt;
		if (m_Intsum > m_Intcap)
			m_Intsum = m_Intcap;
		if (m_Intsum < -m_Intcap)
			m_Intsum = -m_Intcap;
		double output = m_Kp*error + m_Ki*m_Intsum + m_Kd*der;
		m_Prev = error;
		if (output > m_Outlimit)
			output = m_Outlimit;
		if (output < -m_Outlimit)
			output = -m_Outlimit;
		return output;
	}

	void PIDControl::SetOutputLimit(double limit) {
		m_Outlimit = limit;
	}

	void PIDControl::SetIntCap(double limit) {
		m_Intcap = limit;
	}
}