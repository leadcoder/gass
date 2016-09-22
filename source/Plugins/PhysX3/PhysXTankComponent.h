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
#include "Plugins/PhysX3/PhysXBaseGeometryComponent.h"
#include "PhysXCommon.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSIPlatformComponent.h"
#include "Sim/Messages/GASSInputMessages.h"
#include "Sim/GASSSceneObjectRef.h"
#include "IPhysXRigidDynamic.h"

namespace GASS
{
	class PhysXPhysicsSceneManager;
	typedef GASS_WEAK_PTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;

	class PhysXTankComponent : public Reflection<PhysXTankComponent,BaseSceneComponent> , public IPhysXRigidDynamic, public IPlatformComponent
	{
	public:
		PhysXTankComponent();
		virtual ~PhysXTankComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		physx::PxRigidDynamic* GetPxRigidDynamic() const {return m_Actor;}
		void SceneManagerTick(double delta);
		
		PlatformType GetType() const {return PT_CAR;}
		Vec3 GetSize() const;
		ADD_PROPERTY(Float,MaxSpeed);
	protected:
		void OnPostSceneObjectInitializedEvent(PostSceneObjectInitializedEventPtr message);
		void OnLocationLoaded(LocationLoadedEventPtr message);
		void OnPositionChanged(PositionRequestPtr message);
		void OnWorldPositionChanged(WorldPositionRequestPtr message);
		void OnRotationChanged(RotationRequestPtr message);
		void OnWorldRotationChanged(WorldRotationRequestPtr message);
		void OnMassMessage(PhysicsBodyMassRequestPtr message);
		void OnInput(InputRelayEventPtr message);
		
		//reflection functions
		void SetPosition(const Vec3 &value);
		Vec3 GetPosition() const;
		void SetRotation(const Quaternion &rot);
		Quaternion GetRotation() const;
		
		//helpers
		void Reset();
		physx::PxVec3 ComputeDim(const physx::PxConvexMesh* cm);
		void ProcessAutoReverse(const physx::PxVehicleWheels& focusVehicle, 
					const physx::PxVehicleDriveDynData& driveDynData, 
					const physx::PxVehicleDriveTankRawInputData& tankRawInputs,
		bool& toggleAutoReverse, 
		bool& newIsMovingForwardSlowly) const;
		bool CheckCollisions(const Vec3 &pos, const Quaternion &rot, Float speed) const;
	protected:
		ADD_PROPERTY(std::vector<SceneObjectRef>,Wheels)
		ADD_PROPERTY(bool,UseAutoReverse);
		ADD_PROPERTY(float,ScaleMass);
		ADD_PROPERTY(float,EnginePeakTorque)
		ADD_PROPERTY(float,EngineMaxRotationSpeed)
		ADD_PROPERTY(float,ClutchStrength)
		ADD_PROPERTY(float,Mass)
		ADD_PROPERTY(Vec3,MassOffset)
		ADD_PROPERTY(float,GearSwitchTime)
		ADD_PROPERTY(std::vector<float>,GearRatios)
		ADD_PROPERTY(bool,Debug)
		ADD_PROPERTY(float,SteerLimit)

		std::vector<SceneObjectWeakPtr> m_AllWheels;
		bool m_Initialized;
		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
		physx::PxRigidDynamic* m_Actor;
		physx::PxVehicleDriveTank* m_Vehicle;
		float m_ThrottleInput;
		float m_SteerInput;
		bool m_DigBrakeInput;
		bool m_DigAccelInput;
		bool m_IsMovingForwardSlowly;
		bool m_InReverseMode;
		bool m_UseDigitalInputs;
		AABox m_MeshBounds;
		Vec3 m_ChassisDim;
		
	};
	typedef GASS_SHARED_PTR<PhysXTankComponent> PhysXTankComponentPtr;
}

