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
#include "Sim/GASS.h"
#include "Sim/Interface/GASSIPhysicsSuspensionComponent.h"

namespace GASS
{
	class PhysXPhysicsSceneManager;
	typedef GASS_WEAK_PTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;

	class PhysXSuspensionComponent : public Reflection<PhysXSuspensionComponent,BaseSceneComponent>,
		public IPhysicsSuspensionComponent
	{
	public:
		PhysXSuspensionComponent();
		~PhysXSuspensionComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void SetPosition(const Vec3 &value);

		//IPhysicsSuspensionComponent
		void SetDriveVelocity(float velocity) override;
		void SetMaxDriveTorque(float value) override;
		float GetMaxDriveTorque()const override { return m_DriveMaxTorque; }

		void SetAngularSteerVelocity(float value) override;
		void SetMaxSteerTorque(float value) override;
		float GetMaxSteerTorque() const override { return m_MaxSteerTorque; }
	protected:
		void OnPositionChanged(PositionRequestPtr message);
		void OnWorldPositionChanged(WorldPositionRequestPtr message);
		void OnLoad(PhysicsBodyLoadedEventPtr message);

		float GetRollAngle();
		float GetRollAngleRate();
		
		//Helpers
		void CreateJoint();
		void UpdateMotor();

		//get set section
		float GetDamping()const {return m_Damping;}
		void SetDamping(float value){m_Damping =value;}
		float GetStrength()const {return m_Strength;}
		void SetStrength(float value){m_Strength =value;}
		float GetSteerLimit()const {return m_SteerLimit;}
		void SetSteerLimit(float value);
		void SendJointUpdate(PhysicsVelocityEventPtr message);
	private:
		float m_DriveMaxTorque;
		float m_WheelJointDamping;

		float m_SpringJointMaxForce;
		float m_Strength;
		float m_Damping;

		float m_MaxSteerTorque;
		float m_SteerJointSpring;
		float m_SteerJointDamping;
		float m_SteerLimit;
		float m_WheelAngularVelocity;
		float m_AngularSteerVelocity;

		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
		physx::PxRigidDynamic *m_SuspensionActor;
		physx::PxD6Joint *m_SuspensionJoint;
		physx::PxD6Joint *m_WheelAxisJoint;
	};
	typedef GASS_SHARED_PTR<PhysXSuspensionComponent> PhysXSuspensionComponentPtr;
}

