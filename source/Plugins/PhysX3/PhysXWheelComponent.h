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

#include "PhysXCommon.h"
#include "Sim/GASS.h"

namespace GASS
{
	class PhysXPhysicsSceneManager;
	typedef GASS_WEAK_PTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;

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
		bool IsInitialized() const{return m_Initialized;}
	protected:
		void OnGeometryChanged(GeometryChangedEventPtr message);
		void ComputeWheelDim(physx::PxConvexMesh* wheel_convex_mesh, physx::PxF32 &wheel_width, physx::PxF32 &wheel_rad);
		ADD_PROPERTY(float,Mass)
		ADD_PROPERTY(float,MaxBrakeTorque)
		ADD_PROPERTY(float,MaxHandBrakeTorque)
		ADD_PROPERTY(float,MaxSteer)
		ADD_PROPERTY(float,DampingRate)
		ADD_PROPERTY(float,SuspensionMaxCompression)
		ADD_PROPERTY(float,SuspensionMaxDroop)
		ADD_PROPERTY(float,SuspensionSpringStrength)
		ADD_PROPERTY(float,SuspensionSpringDamperRate)
		ADD_PROPERTY(float,TireLongitudinalStiffnessPerUnitGravity)
		ADD_PROPERTY(std::string,TireType)

	private:
		physx::PxVehicleSuspensionData m_SuspensionData;
		physx::PxVehicleTireData m_TireData;
		physx::PxVehicleWheelData m_WheelData;
		bool m_Initialized;
	};
	typedef GASS_SHARED_PTR<PhysXWheelComponent> PhysXWheelComponentPtr;
}

