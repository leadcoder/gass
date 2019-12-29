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
		~PhysXWheelComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		physx::PxVehicleWheelData GetWheelData() const {return m_WheelData;}
		physx::PxVehicleSuspensionData GetSuspensionData() const {return m_SuspensionData;}
		physx::PxVehicleTireData  GetTireData () const {return m_TireData;}
		bool IsInitialized() const{return m_Initialized;}
	protected:
		void OnGeometryChanged(GeometryChangedEventPtr message);
		void ComputeWheelDim(physx::PxConvexMesh* wheel_convex_mesh, physx::PxF32 &wheel_width, physx::PxF32 &wheel_rad);
	private:
		physx::PxVehicleSuspensionData m_SuspensionData;
		physx::PxVehicleTireData m_TireData;
		physx::PxVehicleWheelData m_WheelData;
		bool m_Initialized;
		float m_Mass;
		float m_MaxBrakeTorque;
		float m_MaxHandBrakeTorque;
		float m_MaxSteer;
		float m_DampingRate;
		float m_SuspensionMaxCompression;
		float m_SuspensionMaxDroop;
		float m_SuspensionSpringStrength;
		float m_SuspensionSpringDamperRate;
		float m_TireLongitudinalStiffnessPerUnitGravity;
		std::string m_TireType;
	};
	typedef GASS_SHARED_PTR<PhysXWheelComponent> PhysXWheelComponentPtr;
}

