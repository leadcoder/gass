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

#pragma once

#include "PhysXCommon.h"
#include "Sim/GASS.h"

namespace GASS
{
	class PhysXPhysicsSceneManager;
	typedef WPTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;

	class PhysXWheelComponent : public Reflection<PhysXWheelComponent,BaseSceneComponent>
	{
	public:
		PhysXWheelComponent();
		virtual ~PhysXWheelComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		physx::PxVehicleWheelData GetWheelData() const {return m_WheelData;}
		physx::PxVehicleSuspensionData GetSuspensionData() const {return m_SuspensionData;}
		physx::PxVehicleTireData  GetTireData () const {return m_TireData;}
	protected:
		void OnGeometryChanged(GeometryChangedMessagePtr message);
		void ComputeWheelDim(physx::PxConvexMesh* wheel_convex_mesh, physx::PxF32 &wheel_width, physx::PxF32 &wheel_rad);
		float GetMass() const;
		void SetMass(float  mass);
	private:
		physx::PxVehicleSuspensionData m_SuspensionData;
		physx::PxVehicleTireData m_TireData;
		physx::PxVehicleWheelData m_WheelData;
	};
	typedef SPTR<PhysXWheelComponent> PhysXWheelComponentPtr;
}

