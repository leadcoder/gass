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
#include "Plugins/PhysX/PhysXBaseGeometryComponent.h"
#include "PhysXCommon.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSIPlatformComponent.h"
#include "Sim/Messages/GASSInputMessages.h"
#include "Sim/GASSSceneObjectRef.h"
#include "IPhysXRigidDynamic.h"

namespace GASS
{
	class ISoundComponent;
	class PhysXPhysicsSceneManager;
	using PhysXPhysicsSceneManagerWeakPtr = std::weak_ptr<PhysXPhysicsSceneManager>;

	class PhysXVehicleComponent : public Reflection<PhysXVehicleComponent,Component> , public IPhysXRigidDynamic, public IPlatformComponent
	{
	public:
		PhysXVehicleComponent();
		~PhysXVehicleComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		physx::PxRigidDynamic* GetPxRigidDynamic() const override {return m_Actor;}
		void SceneManagerTick(double delta) override;

		//IPlatformComponent
		PlatformType GetType() const override {return PT_CAR;}
		Vec3 GetSize() const override;
		Float GetMaxSpeed() const override { return m_MaxSpeed; }
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
					const physx::PxVehicleDrive4WRawInputData& carRawInputs,
		bool& toggleAutoReverse, 
		bool& newIsMovingForwardSlowly) const;
		bool CheckCollisions(const Vec3 &pos, const Quaternion &rot, Float speed) const;
	protected:
		Float m_MaxSpeed{20};
		SceneObjectRef m_FrontLeftWheel;
		SceneObjectRef m_FrontRightWheel;
		SceneObjectRef m_RearLeftWheel;
		SceneObjectRef m_RearRightWheel;
		std::vector<SceneObjectRef>m_ExtraWheels;
		bool m_UseAutoReverse{false};
		float m_ScaleMass{1.0};
		float m_EnginePeakTorque{500};
		float m_EngineMaxRotationSpeed{200};
		float m_ClutchStrength{10};
		float m_Mass{1500};
		float m_GearSwitchTime{0.5};
		std::vector<float> m_GearRatios;
		bool m_Debug{false};

		std::vector<SceneObjectWeakPtr> m_AllWheels;
		bool m_Initialized;
		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
		physx::PxRigidDynamic* m_Actor{nullptr};
		physx::PxVehicleDrive4W* m_Vehicle{nullptr};
		float m_ThrottleInput{0};
		float m_SteerInput{0};
		float m_BreakInput{0};
		bool m_DigBrakeInput{false};
		bool m_DigAccelInput{false};
		bool m_IsMovingForwardSlowly{false};
		bool m_InReverseMode{false};
		bool m_UseDigitalInputs{false};
		bool m_TrackTransformation{true};
		AABox m_MeshBounds;
		Vec3 m_ChassisDim;
		ISoundComponent* m_Sound = nullptr;
	};
	using PhysXVehicleComponentPtr = std::shared_ptr<PhysXVehicleComponent>;
}

