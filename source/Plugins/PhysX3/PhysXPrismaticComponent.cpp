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

#include "Plugins/PhysX3/PhysXPrismaticComponent.h"
#include "Plugins/PhysX3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"

#include "Sim/GASS.h"
#include <PxActor.h>
#include <PxScene.h>

namespace GASS
{
	PhysXPrismaticComponent::PhysXPrismaticComponent() : m_PrismaticJoint(0),
		m_Limit(0),
		m_EnableDrive(true),
		m_EnableLimit(false),
		m_RotationAxis(1,0,0),
		m_Offset(0,0,0),
		m_Damping(0),
		m_Spring(0),
		m_DriveTargetVelocity(0),
		m_DriveTargetPosition(0),
		m_DriveForceLimit(PX_MAX_F32)
	{

	}

	PhysXPrismaticComponent::~PhysXPrismaticComponent()
	{

	}

	void PhysXPrismaticComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsPrismaticComponent",new Creator<PhysXPrismaticComponent, Component>);
		RegisterProperty<SceneObjectRef>("Body1", &GASS::PhysXPrismaticComponent::GetBody1, &GASS::PhysXPrismaticComponent::SetBody1);
		RegisterProperty<SceneObjectRef>("Body2", &GASS::PhysXPrismaticComponent::GetBody2, &GASS::PhysXPrismaticComponent::SetBody2);
		RegisterProperty<Vec3>("RotationAxis", &GASS::PhysXPrismaticComponent::GetRotationAxis, &GASS::PhysXPrismaticComponent::SetRotationAxis);
		RegisterProperty<float>("Spring", &GASS::PhysXPrismaticComponent::GetSpring, &GASS::PhysXPrismaticComponent::SetSpring);
		RegisterProperty<float>("Damping", &GASS::PhysXPrismaticComponent::GetDamping, &GASS::PhysXPrismaticComponent::SetDamping);
		RegisterProperty<float>("Limit", &GASS::PhysXPrismaticComponent::GetLimit, &GASS::PhysXPrismaticComponent::SetLimit);
		RegisterProperty<bool>("EnableLimits", &GASS::PhysXPrismaticComponent::GetEnableLimits, &GASS::PhysXPrismaticComponent::SetEnableLimits);
		RegisterProperty<bool>("EnableDrive", &GASS::PhysXPrismaticComponent::GetEnableDrive, &GASS::PhysXPrismaticComponent::SetEnableDrive);
		RegisterProperty<Vec3>("Offset", &GASS::PhysXPrismaticComponent::GetOffset, &GASS::PhysXPrismaticComponent::SetOffset);
		RegisterProperty<float>("DriveForceLimit", &GASS::PhysXPrismaticComponent::GetDriveForceLimit, &GASS::PhysXPrismaticComponent::SetDriveForceLimit);

	}

	void PhysXPrismaticComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXPrismaticComponent::OnVelocityRequest,PhysicsPrismaticJointVelocityRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXPrismaticComponent::OnPositionRequest,PhysicsPrismaticJointPositionRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXPrismaticComponent::OnForceRequest,PhysicsPrismaticJointMaxForceRequest,0));

		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>()->Register(listener);

		PhysXBaseJointComponent::OnInitialize();
	}

	void PhysXPrismaticComponent::OnVelocityRequest(PhysicsPrismaticJointVelocityRequestPtr message)
	{
		if(m_Body2.IsValid())
		{
			m_Body2->GetFirstComponentByClass<PhysXBodyComponent>()->WakeUp();
			SetDriveTargetVelocity(static_cast<float>(message->GetVelocity()));
		}
	}

	void PhysXPrismaticComponent::OnPositionRequest(PhysicsPrismaticJointPositionRequestPtr message)
	{
		if(m_Body2.IsValid())
		{
			m_Body2->GetFirstComponentByClass<PhysXBodyComponent>()->WakeUp();
			SetDriveTargetPosition(static_cast<float>(message->GetPosition()));
		}
	}

	void PhysXPrismaticComponent::OnForceRequest(PhysicsPrismaticJointMaxForceRequestPtr message)
	{
		if(m_Body2.IsValid())
		{
			m_Body2->GetFirstComponentByClass<PhysXBodyComponent>()->WakeUp();
			SetDriveForceLimit(static_cast<float>(message->GetMaxForce()));
		}
	}

	void PhysXPrismaticComponent::CreateJoint()
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
		double angle = acos(Math::Dot(base,m_RotationAxis));
		physx::PxQuat rot(static_cast<float>(angle), PxConvert::ToPx(transform_axis));

		m_PrismaticJoint = PxD6JointCreate(*system->GetPxSDK(),
			a1, physx::PxTransform(PxConvert::ToPx(a1_pos),rot), 
			a2, physx::PxTransform(PxConvert::ToPx(m_Offset),rot));

		m_PrismaticJoint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eFREE);

		SetEnableLimits(m_EnableLimit);
		SetEnableDrive(m_EnableDrive);
		UpdateLimits();
		m_PrismaticJoint->setProjectionLinearTolerance(0.1f);
		m_PrismaticJoint->setProjectionAngularTolerance(0.1f);

		physx::PxTransform drive_trans(physx::PxVec3(0, 0, 0)); 
		m_PrismaticJoint->setDrivePosition(drive_trans);
	}

	void PhysXPrismaticComponent::SetEnableLimits(bool value)
	{
		m_EnableLimit = value;
		if(m_PrismaticJoint)
		{
			if(m_EnableLimit)
				m_PrismaticJoint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLIMITED);
			else
				m_PrismaticJoint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eFREE);
		}
		if(m_EnableLimit)
			UpdateLimits();
	}

	void PhysXPrismaticComponent::SetDamping(float value)
	{
		m_Damping = value;
		//update values
		SetEnableDrive(m_EnableDrive);
	}

	void PhysXPrismaticComponent::SetSpring(float value)
	{
		m_Spring = value;
		//update values
		SetEnableDrive(m_EnableDrive);
	}

	void PhysXPrismaticComponent::SetEnableDrive(bool value)
	{
		m_EnableDrive = value;
		if(m_PrismaticJoint)
		{
			//m_PrismaticJoint->setPrismaticJointFlag(physx::PxPrismaticJointFlag::eDRIVE_ENABLED, m_EnableDrive);
			physx::PxD6JointDrive drive(m_Spring, m_Damping, m_DriveForceLimit, false);
			m_PrismaticJoint->setDrive(physx::PxD6Drive::eY, drive);
		}
	}

	void PhysXPrismaticComponent::SetRotationAxis(const Vec3 &axis)
	{
		m_RotationAxis = axis;
		m_RotationAxis.Normalize();
	}


	void PhysXPrismaticComponent::SetLimit(float value)
	{
		m_Limit = value;
		UpdateLimits();
	}

	void PhysXPrismaticComponent::UpdateLimits()
	{
		if(m_PrismaticJoint && m_EnableLimit)
		{
			m_PrismaticJoint->setLinearLimit(physx::PxJointLinearLimit(m_Limit,physx::PxSpring(0,0)));
		}
	}


	void PhysXPrismaticComponent::SceneManagerTick(double delta_time)
	{
		m_DriveTargetPosition += m_DriveTargetVelocity*delta_time;
		//clamp to limits
		UpdateMotor();

		if(m_Report)
		{
			/*physx::PxVec3 force,torque;
			GetJoint()->getConstraint()->getForce(force, torque);
			PhysicsJointForceEventPtr message(new PhysicsJointForceEvent(PxConvert::ToGASS(force),PxConvert::ToGASS(torque)));
			GetSceneObject()->PostMessage(message);*/
		}
	}


	void PhysXPrismaticComponent::UpdateMotor()
	{
		if(m_PrismaticJoint)
		{
			//force = spring * (targetPosition - position) + damping * (targetVelocity - velocity)
			m_PrismaticJoint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLIMITED);
			physx::PxTransform drive_trans(physx::PxVec3(0, static_cast<float>(m_DriveTargetPosition),0));
			m_PrismaticJoint->setDrivePosition(drive_trans);
			m_PrismaticJoint->setDriveVelocity(physx::PxVec3(0,-m_DriveTargetVelocity,0), physx::PxVec3(0,0,0));
		}
	}

	void PhysXPrismaticComponent::SetDriveTargetVelocity(float velocity)
	{
		m_DriveTargetVelocity  = velocity;
		UpdateMotor();
	}

	void PhysXPrismaticComponent::SetDriveForceLimit(float value)
	{
		m_DriveForceLimit = value;
		SetEnableDrive(m_EnableDrive);
	}

	void PhysXPrismaticComponent::SetDriveTargetPosition(float position)
	{
		m_DriveTargetPosition  = position;
		UpdateMotor();
	}
}
