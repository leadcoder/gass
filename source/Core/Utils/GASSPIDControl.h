// -*-c++-*- 
// $Id: PIDControl,v 1.1 2007-01-15 16:33:44 main Exp $

#ifndef GASS_PIDControl_H
#define GASS_PIDControl_H

#include "Core/Common.h"
#include <iostream>
#include <iomanip>

namespace GASS 
{

	class GASSCoreExport PIDControl {
	public:
		PIDControl();
		explicit PIDControl(double kp, double ki = 0.0, double kd = 0.0);
		void SetGain(double kp, double ki = 0.0, double kd = 0.0);
		void Set(double r);
		double Update(double input, double dt);

		void SetOutputLimit(double limit);
		double GetOutputLimit() const { return outlimit; }
		double GetKp() const { return Kp; }
		double GetKi() const { return Ki; }
		double GetKd() const { return Kd; }
		void SetIntCap(double limit);
		double GetIntCap() const { return intcap; }
		void SetIntSum(double value) { intsum = value; }
		double GetIntSum() const { return intsum; }

		friend std::ostream& operator << (std::ostream& os, const PIDControl& pid)
		{
			os << std::setprecision(std::numeric_limits<Float>::digits10 + 1) << pid.Kp << " " << pid.Ki << " " << pid.Kd;
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