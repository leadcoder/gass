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
#include "Plugins/PhysX3/PhysXBaseGeometryComponent.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSIPlatformComponent.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Sim/Messages/GASSInputMessages.h"
#include "characterkinematic/PxControllerBehavior.h"

namespace GASS
{
	class PhysXPhysicsSceneManager;
	typedef GASS_WEAK_PTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;
	
	/**
		Component that utilize physx character controller. 
	*/
	
	class PhysXCharacterComponent : public Reflection<PhysXCharacterComponent,BaseSceneComponent>, 
		public physx::PxControllerBehaviorCallback, 
		public physx::PxUserControllerHitReport,
		public IPlatformComponent
	{
	public:
		PhysXCharacterComponent();
		~PhysXCharacterComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		physx::PxRigidDynamic* GetPxActor() {return m_Actor;}
		float GetMass() const {return m_Mass;}
		void SetMass(float mass);
		void SceneManagerTick(double delta) override;
		
		PlatformType GetType() const override {return PT_HUMAN;}
		Vec3 GetSize() const override{return Vec3(m_Radius*2,m_StandingSize,m_Radius*2);}
		ADD_PROPERTY(Float,MaxSpeed)

		// Implements PxControllerBehaviorCallback
		physx::PxControllerBehaviorFlags getBehaviorFlags(const physx::PxShape& /*shape*/, const physx::PxActor& /*actor*/) override{return physx::PxControllerBehaviorFlags();}
		physx::PxControllerBehaviorFlags getBehaviorFlags(const physx::PxController& /*controller*/) override{return physx::PxControllerBehaviorFlags();}
		physx::PxControllerBehaviorFlags getBehaviorFlags(const physx::PxObstacle& /*obstacle*/) override{return physx::PxControllerBehaviorFlags();}

		// Implements PxUserControllerHitReport
		void onShapeHit(const physx::PxControllerShapeHit& hit) override;
		void onControllerHit(const physx::PxControllersHit& /*hit*/) override {}
		void onObstacleHit(const physx::PxControllerObstacleHit& /*hit*/) override {}
	protected:
		void OnLocationLoaded(LocationLoadedEventPtr message);
		void OnTransformationChanged(TransformationChangedEventPtr event);
		void OnInput(InputRelayEventPtr message);
		void OnPostUpdate(PostPhysicsSceneUpdateEventPtr message);
	
		//reflection functions
		void SetPosition(const Vec3 &value);
		Vec3 GetPosition() const;
		void SetRotation(const Quaternion &rot);
		Quaternion GetRotation();
		//helpers
		void Reset();
	protected:
		ADD_PROPERTY(Float,StandingSize)
		ADD_PROPERTY(Float,Radius)
		ADD_PROPERTY(Float,YawMaxVelocity)
		ADD_PROPERTY(Float,Acceleration)
		
		bool m_Initialized;
		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
		physx::PxRigidDynamic* m_Actor;
		float m_ThrottleInput;
		float m_SteerInput;
		float m_Mass;
		Float m_Yaw;
		Float m_CurrentVel;
		physx::PxCapsuleController* m_Controller;
		bool m_TrackTransformation;
	};
	typedef GASS_SHARED_PTR<PhysXCharacterComponent> PhysXCharacterComponentPtr;
}

