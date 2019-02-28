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

#include "Core/Math/GASSVec3.h"
#include "Sim/GASSCommon.h"


namespace GASS
{
	class IPhysicsBodyComponent
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(IPhysicsBodyComponent)
	public:
		virtual void SetVelocity(const Vec3 &vel, bool relative = false) = 0;
		virtual Vec3 GetVelocity(bool relative = false) const = 0;
		virtual void AddForce(const Vec3 &force_vec, bool relative = false) = 0;
		virtual void AddForceAtPos(const Vec3 &force_vec, const Vec3 &pos_vec, bool rel_force = false, bool rel_pos = false) = 0;
		virtual void AddTorque(const Vec3 &torque_vec, bool relative = false) = 0;
		virtual float GetMass() const = 0;
		virtual void SetMass(float mass) = 0;
		virtual void SetActive(bool value) = 0;
		virtual bool GetActive() const = 0;
	};
	typedef GASS_SHARED_PTR<IPhysicsBodyComponent> PhysicsBodyComponentPtr;
}
