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

#include "PIDControl.h"
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