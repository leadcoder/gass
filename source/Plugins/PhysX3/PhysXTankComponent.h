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
		~PhysXTankComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		physx::PxRigidDynamic* GetPxRigidDynamic() const override {return m_Actor;}
		void SceneManagerTick(double delta) override;
		
		//IPlatformComponent
		PlatformType GetType() const override {return PT_TANK;}
		Vec3 GetSize() const override;
		Float GetMaxSpeed() const override { return m_MaxSpeed;}
	protected:
		void OnPostSceneObjectInitializedEvent(PostSceneObjectInitializedEventPtr message);
		void OnLocationLoaded(LocationLoadedEventPtr message);
		void OnTransformationChanged(TransformationChangedEventPtr event);
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
		std::vector<SceneObjectRef> m_Wheels;
		bool m_UseAutoReverse;
		float m_ScaleMass;
		float m_EnginePeakTorque;
		float m_EngineMaxRotationSpeed;
		float m_ClutchStrength;
		float m_Mass;
		Vec3 m_MassOffset;
		float m_GearSwitchTime;
		std::vector<float> m_GearRatios;
		bool m_Debug;
		float m_SteerLimit;
		Float m_MaxSpeed;
		std::vector<SceneObjectWeakPtr> m_AllWheels;
		bool m_Initialized;
		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
		physx::PxRigidDynamic* m_Actor;
		physx::PxVehicleDriveTank* m_Vehicle;
		float m_ThrottleInput;
		float m_SteerInput;
		float m_BreakInput;
		bool m_DigBrakeInput;
		bool m_DigAccelInput;
		bool m_IsMovingForwardSlowly;
		bool m_InReverseMode;
		bool m_UseDigitalInputs;
		bool m_TrackTransformation;
		AABox m_MeshBounds;
		Vec3 m_ChassisDim;
		
	};
	typedef GASS_SHARED_PTR<PhysXTankComponent> PhysXTankComponentPtr;
}

