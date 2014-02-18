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
#include "Sim/GASSSceneObjectRef.h"

namespace GASS
{
	class PhysXPhysicsSceneManager;
	typedef WPTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;
	
	/**
		Component that utialize physx raycast car. 
	*/
	
	class PhysXCharacterComponent : public Reflection<PhysXCharacterComponent,BaseSceneComponent>
	{
	public:
		PhysXCharacterComponent();
		virtual ~PhysXCharacterComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		physx::PxRigidDynamic* GetPxActor() {return m_Actor;}
		float GetMass() const {return m_Mass;}
		void SetMass(float mass);
		void SceneManagerTick(double delta);
	protected:
		void OnPostSceneObjectInitializedEvent(PostSceneObjectInitializedEventPtr message);
		void OnLocationLoaded(LocationLoadedMessagePtr message);
		void OnPositionChanged(PositionMessagePtr message);
		void OnWorldPositionChanged(WorldPositionMessagePtr message);
		void OnRotationChanged(RotationMessagePtr message);
		void OnMassMessage(PhysicsBodyMassRequestPtr message);
		void OnInput(InputControllerMessagePtr message);
		
		//reflection functions
		void SetPosition(const Vec3 &value);
		Vec3 GetPosition() const;
		void SetRotation(const Quaternion &rot);
		Quaternion GetRotation();
		//helpers
		void Reset();
	protected:
		bool m_Initialized;
		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
		physx::PxRigidDynamic* m_Actor;
		float m_ThrottleInput;
		float m_SteerInput;
		float m_Mass;
		physx::PxCapsuleController* m_Controller;
	};
	typedef SPTR<PhysXCharacterComponent> PhysXCharacterComponentPtr;
}

