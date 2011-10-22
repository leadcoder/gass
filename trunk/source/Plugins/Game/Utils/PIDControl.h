// -*-c++-*- 
// $Id: PIDControl,v 1.1 2007-01-15 16:33:44 main Exp $

#ifndef PIDControl_H
#define PIDControl_H

#include <iostream>

namespace GASS 
{

	class PIDControl {
	public:
		PIDControl();
		PIDControl(double kp, double ki = 0.0, double kd = 0.0);
		virtual ~PIDControl() {}
		void setGain(double kp, double ki = 0.0, double kd = 0.0);
		void set(double r);
		double update(double input, double dt);

		void setOutputLimit(double limit);
		double getOutputLimit() { return outlimit; }
		double getKp() { return Kp; }
		double getKi() { return Ki; }
		double getKd() { return Kd; }
		void setIntCap(double limit);
		double getIntCap() { return intcap; }

		friend std::ostream& operator << (std::ostream& os, const PIDControl& pid)
		{
			os << pid.Kp << pid.Ki << pid.Kd;
			return os;
		}

		friend std::istream& operator >> (std::istream& os, PIDControl& pid)
		{
			os >> pid.Kp >> pid.Ki >> pid.Kd;
			return os;
		}
	protected:
		// Proportional, integerator and derivator gains
		double Kp, Ki, Kd;
		// Reference value (control input)
		double desired;
		// Integrator sum and previous values for derivator
		double intsum, prev;
		// Limits and such
		double outlimit, intcap;
	};

}

#endif