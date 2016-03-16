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
#include <extensions/PxRevoluteJoint.h>
#include <extensions/PxPrismaticJoint.h>
#include "PhysXCommon.h"
#include "PhysXBaseJointComponent.h"
#include "Sim/GASS.h"
#include "Sim/GASSSceneObjectRef.h"


namespace GASS
{
	class PhysXPhysicsSceneManager;
	typedef GASS_WEAK_PTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;

	class PhysXPrismaticComponent : public Reflection<PhysXPrismaticComponent,PhysXBaseJointComponent>
	{
	public:
		PhysXPrismaticComponent();
		virtual ~PhysXPrismaticComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void CreateJoint();
		virtual void SceneManagerTick(double delta_time);
		physx::PxJoint* GetJoint() const  {return m_PrismaticJoint;}
	protected:
		ADD_PROPERTY(Vec3,Offset)
		void OnVelocityRequest(PhysicsPrismaticJointVelocityRequestPtr message);
		void OnForceRequest(PhysicsPrismaticJointMaxForceRequestPtr message);
		void OnPositionRequest(PhysicsPrismaticJointPositionRequestPtr message);
		//Helpers
		
		void UpdateSuspension();
		void UpdateMotor();
		void UpdateLimits();

		//get set section
		float GetSpring()const {return m_Spring;}
		void SetSpring(float value);	
		void SetDamping(float value);
		float GetDamping() const {return m_Damping;}
		Vec3 GetRotationAxis()const {return m_RotationAxis;}
		void SetRotationAxis(const Vec3 &value);
		void SetLimit(float value);
		float GetLimit()const {return m_Limit;}
		void SetEnableLimits(bool value);
		bool GetEnableLimits() const {return m_EnableLimit;}
		void SetEnableDrive(bool value);
		bool GetEnableDrive() const {return m_EnableDrive;}

		void SetDriveTargetVelocity(float velocity);
		float GetDriveTargetVelocity() const {return m_DriveTargetVelocity;}
		void SetDriveForceLimit(float value);
		float GetDriveForceLimit() const {return m_DriveForceLimit;}

		void SetDriveTargetPosition(float position);
		float GetDriveTargetPosition() const {return static_cast<float>(m_DriveTargetPosition);}

	private:
		float m_Damping;
		float m_Spring;
		Vec3 m_RotationAxis;
		float m_Limit;
		bool m_EnableLimit;
		bool m_EnableDrive;
		float m_DriveTargetVelocity;
		float m_DriveForceLimit;
		Float m_DriveTargetPosition;
		physx::PxD6Joint *m_PrismaticJoint;
	};
	typedef GASS_SHARED_PTR<PhysXPrismaticComponent> PhysXPrismaticComponentPtr;
}

