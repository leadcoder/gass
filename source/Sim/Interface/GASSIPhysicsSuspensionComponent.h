/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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

#pragma once

#include "Sim/GASSCommon.h"

namespace GASS
{
	class IPhysicsSuspensionComponent
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(IPhysicsSuspensionComponent)
	public:
		virtual void SetDriveVelocity(float velocity) = 0;
		virtual void SetMaxDriveTorque(float value) = 0;
		virtual float GetMaxDriveTorque() const = 0;
		virtual void SetAngularSteerVelocity(float value) = 0;
		virtual void SetMaxSteerTorque(float value) = 0;
		virtual float GetMaxSteerTorque() const = 0;
		virtual float GetSteerAngle() const = 0;
	};
	typedef GASS_SHARED_PTR<IPhysicsSuspensionComponent> PhysicsSuspensionComponentPtr;
}
