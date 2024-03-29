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
#include "PhysXBaseJointComponent.h"
#include "Sim/GASS.h"
#include "Sim/Interface/GASSIPhysicsHingeJointComponent.h"

namespace GASS
{
	class PhysXPhysicsSceneManager;
	using PhysXPhysicsSceneManagerWeakPtr = std::weak_ptr<PhysXPhysicsSceneManager>;

	class PhysXHingeComponent : public Reflection<PhysXHingeComponent,PhysXBaseJointComponent> , 
		public IPhysicsHingeJointComponent
	{
	public:
		PhysXHingeComponent();
		~PhysXHingeComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;

		//IPhysicsHingeJointComponent
		void SetDriveTargetVelocity(float value) override;
		float GetDriveTargetVelocity() const override { return m_DriveTargetVelocity; }

		void CreateJoint() override;
		physx::PxJoint* GetJoint() const override  {return m_RevoluteJoint;}
	protected:
		
		void OnVelocityRequest(PhysicsHingeJointVelocityRequestPtr message);
		void OnForceRequest(PhysicsHingeJointMaxTorqueRequestPtr message);
		void SceneManagerTick(double delta_time) override;
		//Helpers
		void UpdateMotor();
		void UpdateLimits();

		//get set section
		float GetDriveForceLimit()const override {return m_DriveForceLimit;}
		void SetDriveForceLimit(float value) override;	
		float GetDamping()const {return m_Damping;}
		void SetDamping(float value){m_Damping =value;}
		float GetSpring()const {return m_Spring;}
		void SetSpring(float value){m_Spring =value;}
		Vec3 GetRotationAxis()const {return m_RotationAxis;}
		void SetRotationAxis(const Vec3 &value);
		float GetHighStop()const {return m_HighStop;}
		void SetHighStop(float value);
		float GetLowStop()const {return m_LowStop;}
		void SetLowStop(float value);
		void SetEnableLimits(bool value);
		bool GetEnableLimits() const {return m_EnableLimit;}
		void SetEnableDrive(bool value);
		bool GetEnableDrive() const {return m_EnableDrive;}
	private:
		float m_DriveForceLimit{PX_MAX_F32};
		float m_DriveTargetVelocity{0};
		Vec3 m_RotationAxis;
		Vec3 m_Offset;
		float m_SpringJointForce;
		float m_HighStop{0};
		float m_LowStop{0};
		bool m_EnableLimit{false};
		bool m_EnableDrive{true};
		float m_TargetAngle{0};
		float m_Damping{0};
		float m_Spring{0};
		
		physx::PxD6Joint *m_RevoluteJoint{nullptr};
	};
	using PhysXHingeComponentPtr = std::shared_ptr<PhysXHingeComponent>;
}

