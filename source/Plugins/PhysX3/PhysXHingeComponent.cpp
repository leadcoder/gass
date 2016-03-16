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

#include "Plugins/PhysX3/PhysXHingeComponent.h"
#include "Plugins/PhysX3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"

#include "Sim/GASS.h"
#include <PxActor.h>
#include <PxScene.h>

namespace GASS
{
	PhysXHingeComponent::PhysXHingeComponent() : m_DriveTargetVelocity (0),
		m_DriveForceLimit(PX_MAX_F32),
		m_RevoluteJoint(0),
		m_HighStop(0),
		m_LowStop(0),
		m_EnableDrive(true),
		m_EnableLimit(false),
		m_RotationAxis(1,0,0),
		m_Offset(0,0,0),
		m_TargetAngle(0),
		m_Spring(0),
		m_Damping(0)
	{

	}

	PhysXHingeComponent::~PhysXHingeComponent()
	{

	}

	void PhysXHingeComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsHingeComponent",new Creator<PhysXHingeComponent, Component>);
		RegisterProperty<SceneObjectRef>("Body1", &GASS::PhysXHingeComponent::GetBody1, &GASS::PhysXHingeComponent::SetBody1);
		RegisterProperty<SceneObjectRef>("Body2", &GASS::PhysXHingeComponent::GetBody2, &GASS::PhysXHingeComponent::SetBody2);
		RegisterProperty<Vec3>("RotationAxis", &GASS::PhysXHingeComponent::GetRotationAxis, &GASS::PhysXHingeComponent::SetRotationAxis);
		RegisterProperty<float>("DriveForceLimit", &GASS::PhysXHingeComponent::GetDriveForceLimit, &GASS::PhysXHingeComponent::SetDriveForceLimit);
		RegisterProperty<float>("DriveTargetVelocity", &GASS::PhysXHingeComponent::GetDriveTargetVelocity, &GASS::PhysXHingeComponent::SetDriveTargetVelocity);
		RegisterProperty<float>("HighStop", &GASS::PhysXHingeComponent::GetHighStop, &GASS::PhysXHingeComponent::SetHighStop);
		RegisterProperty<float>("LowStop", &GASS::PhysXHingeComponent::GetLowStop, &GASS::PhysXHingeComponent::SetLowStop);
		RegisterProperty<bool>("EnableLimits", &GASS::PhysXHingeComponent::GetEnableLimits, &GASS::PhysXHingeComponent::SetEnableLimits);
		RegisterProperty<bool>("EnableDrive", &GASS::PhysXHingeComponent::GetEnableDrive, &GASS::PhysXHingeComponent::SetEnableDrive);
		RegisterProperty<Vec3>("Offset", &GASS::PhysXHingeComponent::GetOffset, &GASS::PhysXHingeComponent::SetOffset);
		RegisterProperty<float>("Spring", &GASS::PhysXHingeComponent::GetSpring, &GASS::PhysXHingeComponent::SetSpring);
		RegisterProperty<float>("Damping", &GASS::PhysXHingeComponent::GetDamping, &GASS::PhysXHingeComponent::SetDamping);
	}

	void PhysXHingeComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXHingeComponent::OnVelocityRequest,PhysicsHingeJointVelocityRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXHingeComponent::OnForceRequest,PhysicsHingeJointMaxTorqueRequest,0));
		PhysXBaseJointComponent::OnInitialize();

		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>()->Register(listener);
	}

	void PhysXHingeComponent::OnVelocityRequest(PhysicsHingeJointVelocityRequestPtr message)
	{
		if(m_Body2.IsValid())
		{
			m_Body2->GetFirstComponentByClass<PhysXBodyComponent>()->WakeUp();
			SetDriveTargetVelocity(static_cast<float>(message->GetVelocity()));
		}
	}

	void PhysXHingeComponent::OnForceRequest(PhysicsHingeJointMaxTorqueRequestPtr message)
	{
		if(m_Body2.IsValid())
		{
			m_Body2->GetFirstComponentByClass<PhysXBodyComponent>()->WakeUp();
			SetDriveForceLimit(static_cast<float>(message->GetMaxTorque()));
		}
	}

	void PhysXHingeComponent::CreateJoint()
	{
		if(!(m_Body1.IsValid() && m_Body1.IsValid()))
			return;

		physx::PxRigidDynamic* a1 = m_Body1->GetFirstComponentByClass<IPhysXRigidDynamic>()->GetPxRigidDynamic();
		physx::PxRigidDynamic* a2 = m_Body2->GetFirstComponentByClass<IPhysXRigidDynamic>()->GetPxRigidDynamic();
		PhysXPhysicsSceneManagerPtr sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();

		LocationComponentPtr location1 = m_Body1->GetFirstComponentByClass<ILocationComponent>();
		LocationComponentPtr location2 = m_Body2->GetFirstComponentByClass<ILocationComponent>();
		Vec3 a1_pos = location2->GetPosition() - location1->GetPosition();

		// use provided axis....maybe add support to use body rotation also?
		Vec3 base(1,0,0);
		Vec3 transform_axis = Math::Cross(base,m_RotationAxis);
		transform_axis.Normalize();
		//Vec3 transform_axis = m_RotationAxis;
		double angle = acos(Math::Dot(base,m_RotationAxis));
		physx::PxQuat rot(static_cast<float>(angle) ,PxConvert::ToPx(transform_axis));


		m_RevoluteJoint = PxD6JointCreate(*system->GetPxSDK(),
			a1, physx::PxTransform(PxConvert::ToPx(a1_pos),rot),
			a2, physx::PxTransform(PxConvert::ToPx(m_Offset),rot));

		
		m_RevoluteJoint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eFREE);
		m_RevoluteJoint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLOCKED);
		m_RevoluteJoint->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLOCKED);

		m_RevoluteJoint->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eLOCKED);
		m_RevoluteJoint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLOCKED);
		m_RevoluteJoint->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eLOCKED);
		SetEnableLimits(m_EnableLimit);
		SetEnableDrive(m_EnableDrive);
		SetDriveForceLimit(m_DriveForceLimit);
		UpdateLimits();
		
		m_RevoluteJoint->setConstraintFlag(physx::PxConstraintFlag::eREPORTING,m_Report);

		//m_RevoluteJoint->setProjectionLinearTolerance(0.01);
		//m_RevoluteJoint->setProjectionAngularTolerance(0.01);
		//m_RevoluteJoint->setConstraintFlag(physx::PxConstraintFlag::ePROJECTION, true);
		
	}

	void PhysXHingeComponent::SetEnableLimits(bool value)
	{
		m_EnableLimit = value;
		if(m_RevoluteJoint)
		{
			/*if(m_EnableLimit)
				m_RevoluteJoint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eLIMITED);
			else
				m_RevoluteJoint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eFREE);*/
		}
		if(m_EnableLimit)
			UpdateLimits();
	}

	void PhysXHingeComponent::SetEnableDrive(bool value)
	{
		m_EnableDrive = value;
		if(m_RevoluteJoint)
		{
			physx::PxD6JointDrive drive(m_Spring, m_Damping, m_DriveForceLimit, false);
			m_RevoluteJoint->setDrive(physx::PxD6Drive::eTWIST, drive);
		}
	}

	void PhysXHingeComponent::SetRotationAxis(const Vec3 &axis)
	{
		m_RotationAxis = axis;
		m_RotationAxis.Normalize();
	}

	void PhysXHingeComponent::SetLowStop(float value)
	{
		m_LowStop = value;
		UpdateLimits();
	}

	void PhysXHingeComponent::SetHighStop(float value)
	{
		m_HighStop = value;
		UpdateLimits();
	}

	void PhysXHingeComponent::UpdateLimits()
	{
		if(m_RevoluteJoint && m_EnableLimit)
		{
	//		m_RevoluteJoint->setSwingLimit(physx::PxJointLimitCone(m_HighStop,m_HighStop,0.1));
		}
	}

	void PhysXHingeComponent::SceneManagerTick(double delta_time)
	{
		m_TargetAngle -= m_DriveTargetVelocity*static_cast<float>(delta_time);

		if(m_EnableLimit)
		{
			if(m_TargetAngle > m_HighStop)
			{
				if(m_DriveTargetVelocity < 0)
					m_DriveTargetVelocity = 0;
				m_TargetAngle = m_HighStop;
			}

			if(m_TargetAngle < m_LowStop)
			{
				if(m_DriveTargetVelocity > 0)
					m_DriveTargetVelocity = 0;
				m_TargetAngle = m_LowStop;
			}
		}
		//clamp to limits
		UpdateMotor();

		if(m_Report)
		{

			physx::PxRigidActor* a1,*a2;
			m_RevoluteJoint->getActors(a1,a2);
			physx::PxVec3 force,torque;
			GetJoint()->getConstraint()->getForce(force, torque);
			//torque = a1->getGlobalPose().rotateInv(torque);
			//force = a1->getGlobalPose().rotateInv(force);

			PhysicsHingeJointReportEventPtr report_message(new PhysicsHingeJointReportEvent(m_DriveTargetVelocity,PxConvert::ToGASS(force), PxConvert::ToGASS(torque)));
			GetSceneObject()->PostEvent(report_message);
			//std::cout << " Torque:" << PxConvert::ToGASS(torque) << " Force: " << PxConvert::ToGASS(force) << std::endl;
			/*physx::PxVec3 vel = a2->is<physx::PxRigidDynamic>()->getAngularVelocity();
			vel = a1->getGlobalPose().rotateInv(vel);
			PhysicsJointVelocityEventPtr vel_message(new PhysicsJointVelocityEvent(Vec3(0,0,0), PxConvert::ToGASS(vel)));
			GetSceneObject()->PostMessage(vel_message);*/
		}
	}


	void PhysXHingeComponent::UpdateMotor()
	{
		if(m_RevoluteJoint)
		{
			//m_TargetAngle += m_DriveTargetVelocity;
			physx::PxQuat rot(m_TargetAngle, physx::PxVec3(1,0,0));
			physx::PxTransform drive_trans(physx::PxVec3(0,0,0), rot); 
			m_RevoluteJoint->setDrivePosition(drive_trans);

			


			/*if(m_Report)
			{
				//const physx::PxF32 cosTheta=physx::PxAbs(a1->getGlobalPose().q.getBasisVector1().y);
				//const physx::PxF32 theta=physx::PxAcos(cosTheta);
				//std::cout <<"Y: " << theta;
				physx::PxF32 theta;
				physx::PxVec3 axis;
				
				physx::PxTransform inv_t = a1->getGlobalPose().getInverse();
				physx::PxTransform local_t = inv_t.transform(a2->getGlobalPose());
				local_t.q.toRadiansAndUnitAxis(theta,axis);
				physx::PxVec3 vel = a2->is<physx::PxRigidDynamic>()->getAngularVelocity();
				vel = a1->getGlobalPose().rotateInv(vel);
				std::cout <<"Vel:" << vel.x << " " << vel.y << " " << vel.z << std::endl;
				//inv_t.transform()
				//std::cout <<"A: " << theta << " axis:" << axis.x << " " << axis.y << " " <<axis.z << std::endl;
				physx::PxQuat q1 = a1->getGlobalPose().q;
				physx::PxQuat q2 = a2->getGlobalPose().q;
				physx::PxReal angle = q1.getAngle(q2);
				std::cout <<"A: " << angle << std::endl;
			}*/
			//m_RevoluteJoint->setDriveVelocity(physx::PxVec3(0,0,0), physx::PxVec3(m_DriveTargetVelocity,m_DriveTargetVelocity,m_DriveTargetVelocity));
			//m_RevoluteJoint->setDriveForceLimit(m_DriveForceLimit);
			//m_RevoluteJoint->setBreakForce(1000000,1000000);
		}
	}

	void PhysXHingeComponent::SetDriveTargetVelocity(float velocity)
	{
		m_DriveTargetVelocity  = velocity;
		//UpdateMotor();
	}

	void PhysXHingeComponent::SetDriveForceLimit(float value)
	{
		m_DriveForceLimit = value;
		if(m_RevoluteJoint)
		{
			SetEnableDrive(m_EnableDrive);
		}
	}

}
