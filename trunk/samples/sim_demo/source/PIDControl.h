/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
*                                                                           *
* GASS is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* GASS is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with GASS. If not, see <http://www.gnu.org/licenses/>.              *
*****************************************************************************/

#ifndef PIDControl_H
#define PIDControl_H

namespace GASS 
{

	class PIDControl {
	public:
		PIDControl();
		PIDControl(double kp, double ki = 0.0, double kd = 0.0);

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