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
		double GetOutputLimit() const { return m_Outlimit; }
		double GetKp() const { return m_Kp; }
		double GetKi() const { return m_Ki; }
		double GetKd() const { return m_Kd; }
		void SetIntCap(double limit);
		double GetIntCap() const { return m_Intcap; }
		void SetIntSum(double value) { m_Intsum = value; }
		double GetIntSum() const { return m_Intsum; }

		friend std::ostream& operator << (std::ostream& os, const PIDControl& pid)
		{
			os << std::setprecision(std::numeric_limits<Float>::digits10 + 1) << pid.m_Kp << " " << pid.m_Ki << " " << pid.m_Kd;
			return os;
		}

		friend std::istream& operator >> (std::istream& os, PIDControl& pid)
		{
			os >> pid.m_Kp >> pid.m_Ki >> pid.m_Kd;
			return os;
		}
	protected:
		// Proportional, integerator and derivator gains
		double m_Kp, m_Ki, m_Kd;
		// Reference value (control input)
		double m_Desired;
		// Integrator sum and previous values for derivator
		double m_Intsum, m_Prev;
		// Limits and such
		double m_Outlimit, m_Intcap;
	};

}

#endif