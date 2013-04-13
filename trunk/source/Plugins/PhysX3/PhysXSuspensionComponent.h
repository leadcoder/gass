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
#include <extensions/PxRevoluteJoint.h>
#include <extensions/PxPrismaticJoint.h>
#include "PhysXCommon.h"
#include "Sim/GASS.h"

namespace GASS
{
	class PhysXPhysicsSceneManager;
	typedef WPTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;

	class PhysXSuspensionComponent : public Reflection<PhysXSuspensionComponent,BaseSceneComponent>
	{
	public:
		PhysXSuspensionComponent();
		virtual ~PhysXSuspensionComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		void SetPosition(const Vec3 &value);
	protected:
		
		void OnPositionChanged(PositionMessagePtr message);
		void OnWorldPositionChanged(WorldPositionMessagePtr message);
		
		void OnParameterMessage(PhysicsJointMessagePtr message);
		void OnLoad(BodyLoadedMessagePtr message);

		//virtual void UpdateTransformation();
		float GetRollAngle();
		float GetRollAngleRate();
		void SetRollAxisVel(float velocity);
		float GetWheelJointMaxForce()const {return m_WheelJointMaxForce;}
		void SetWheelJointMaxForce(float value);
		//Helpers

		void CreateJoint();
		void UpdateSuspension();
		void UpdateAnchor();
		void UpdateJointAxis();
		void UpdateMotor();

		//get set section
		float GetDamping()const {return m_Damping;}
		void SetDamping(float value){m_Damping =value;}
		float GetStrength()const {return m_Strength;}
		void SetStrength(float value){m_Strength =value;}
		float GetSteerLimit()const {return m_SteerLimit;}
		void SetSteerLimit(float value);

		
		void SendJointUpdate(VelocityNotifyMessagePtr message);

	private:
		float m_WheelJointMaxForce;
		float m_WheelJointSpring;
		float m_WheelJointDamping;

		float m_SpringJointMaxForce;
		float m_SteerJointMaxForce;

		
		
		float m_Strength;
		float m_Damping;
		float m_SteerLimit;
		
		
		float m_RollAngularVelocity;//?

		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
		physx::PxRigidDynamic *m_SuspensionActor;
		physx::PxD6Joint *m_SuspensionJoint;
		physx::PxD6Joint *m_WheelAxisJoint;
	};
	typedef SPTR<PhysXSuspensionComponent> PhysXSuspensionComponentPtr;
}

