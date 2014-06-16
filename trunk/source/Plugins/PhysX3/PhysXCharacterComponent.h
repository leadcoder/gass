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
#include "Plugins/PhysX3/PhysXBaseGeometryComponent.h"
#include "PhysXCommon.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSIPlatformComponent.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Plugins/Game/GameMessages.h"
#include "PxSimulationEventCallback.h"
#include "characterkinematic/PxControllerBehavior.h"


namespace GASS
{
	class PhysXPhysicsSceneManager;
	typedef WPTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;
	
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
		virtual ~PhysXCharacterComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		physx::PxRigidDynamic* GetPxActor() {return m_Actor;}
		float GetMass() const {return m_Mass;}
		void SetMass(float mass);
		void SceneManagerTick(double delta);
		
		PlatformType GetType() const {return PT_HUMAN;}
		Vec3 GetSize() const{return Vec3(m_Radius*2,m_StandingSize,m_Radius*2);}
		ADD_PROPERTY(Float,MaxSpeed)

		// Implements PxControllerBehaviorCallback
		virtual physx::PxU32 getBehaviorFlags(const physx::PxShape&)		{ return 0;	}
		virtual physx::PxU32 getBehaviorFlags(const physx::PxController&)	{ return 0;	}
		virtual physx::PxU32 getBehaviorFlags(const physx::PxObstacle&)		{ return 0;	}

		// Implements PxUserControllerHitReport
		virtual void onShapeHit(const physx::PxControllerShapeHit& hit);
		virtual void onControllerHit(const physx::PxControllersHit& hit) {}
		virtual void onObstacleHit(const physx::PxControllerObstacleHit& hit) {}
	protected:
		void OnLocationLoaded(LocationLoadedEventPtr message);
		void OnPositionChanged(PositionRequestPtr message);
		void OnWorldPositionChanged(WorldPositionRequestPtr message);
		void OnRotationChanged(RotationRequestPtr message);
		void OnWorldRotationChanged(WorldRotationRequestPtr message);
		void OnMassMessage(PhysicsBodyMassRequestPtr message);
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
	};
	typedef SPTR<PhysXCharacterComponent> PhysXCharacterComponentPtr;
}

