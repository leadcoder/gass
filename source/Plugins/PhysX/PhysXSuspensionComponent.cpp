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

#include "Plugins/PhysX/PhysXSuspensionComponent.h"
#include "Plugins/PhysX/PhysXBodyComponent.h"
#include "Plugins/PhysX/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX/PhysXPhysicsSystem.h"
#include "Sim/GASS.h"

namespace GASS
{
	PhysXSuspensionComponent::PhysXSuspensionComponent() 
		
	{

	}

	PhysXSuspensionComponent::~PhysXSuspensionComponent()
	{

	}

	void PhysXSuspensionComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<PhysXSuspensionComponent>("PhysicsSuspensionComponent");
		RegisterGetSet("Damping", &GASS::PhysXSuspensionComponent::GetDamping, &GASS::PhysXSuspensionComponent::SetDamping);
		RegisterGetSet("Strength", &GASS::PhysXSuspensionComponent::GetStrength, &GASS::PhysXSuspensionComponent::SetStrength);
		RegisterGetSet("SteerLimit", &GASS::PhysXSuspensionComponent::GetSteerLimit, &GASS::PhysXSuspensionComponent::SetSteerLimit);
	}

	void PhysXSuspensionComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXSuspensionComponent::OnLoad,PhysicsBodyLoadedEvent,2));
	}

	void PhysXSuspensionComponent::SetAngularSteerVelocity(float value)
	{
		m_AngularSteerVelocity = value;
		if(m_SuspensionJoint)
			m_SuspensionJoint->setDriveVelocity(physx::PxVec3(0,0,0), physx::PxVec3(0,m_AngularSteerVelocity,0));
	}

	void PhysXSuspensionComponent::SetMaxSteerTorque(float value)
	{
		m_MaxSteerTorque = value;
	}

	void PhysXSuspensionComponent::OnLoad(PhysicsBodyLoadedEventPtr message)
	{
		m_SceneManager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		CreateJoint();
	}

	void PhysXSuspensionComponent::CreateJoint()
	{
		PhysXPhysicsSceneManagerPtr sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();

		PhysXBodyComponentPtr chassis_comp = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<PhysXBodyComponent>();
		PhysXBodyComponentPtr wheel_comp = GetSceneObject()->GetFirstComponentByClass<PhysXBodyComponent>();

		physx::PxRigidDynamic* chassis_actor= chassis_comp->GetPxRigidDynamic();
		physx::PxRigidDynamic* wheel_actor = wheel_comp->GetPxRigidDynamic();

		Vec3 chassis_pos = chassis_comp->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
		Vec3 wheel_pos = wheel_comp->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();

		physx::PxQuat no_rot(0, physx::PxVec3(0.0f, 0.0f, 1.0f));

		//Create actor to connect steer and suspension joint to
		m_SuspensionActor = system->GetPxSDK()->createRigidDynamic(physx::PxTransform(physx::PxVec3(0, 0, 0), no_rot));
		m_SuspensionActor->setMass(wheel_comp->GetMass());
		sm->GetPxScene()->addActor(*m_SuspensionActor);

		//Calculate positions relative to parent for suspension joint
		physx::PxVec3 susp_pos = PxConvert::ToPx(wheel_pos - chassis_pos);
		//physx::PxQuat susp_rot(-physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f));

		m_SuspensionJoint = PxD6JointCreate(*system->GetPxSDK(),
			chassis_actor,physx::PxTransform(susp_pos,no_rot), //parent
			m_SuspensionActor, physx::PxTransform(physx::PxVec3(0,0,0),no_rot));

		m_SuspensionJoint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLIMITED);
		m_SuspensionJoint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLIMITED);
		float slack = 0.01;
		m_SuspensionJoint->setLinearLimit(physx::PxJointLinearLimit(slack,physx::PxSpring(m_Strength, m_Damping)));
		
		physx::PxD6JointDrive steer_drive(m_SteerJointSpring, m_SteerJointDamping, m_MaxSteerTorque, false);
		m_SuspensionJoint->setDrive(physx::PxD6Drive::eSWING, steer_drive);
		SetSteerLimit(m_SteerLimit);

		m_WheelAxisJoint = PxD6JointCreate(*system->GetPxSDK(),
			m_SuspensionActor,physx::PxTransform(physx::PxVec3(0,0,0),no_rot), //parent
			wheel_actor, physx::PxTransform(physx::PxVec3(0,0,0), no_rot));
		m_WheelAxisJoint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eFREE);
	}

	void PhysXSuspensionComponent::SetSteerLimit(float value)
	{
		m_SteerLimit = value;
		if(m_SuspensionJoint)
			m_SuspensionJoint->setSwingLimit(physx::PxJointLimitCone(static_cast<float>(m_SteerLimit), static_cast<float>(m_SteerLimit),0.1f));
	}

	void PhysXSuspensionComponent::SetPosition(const Vec3 &value)
	{
		if(m_SuspensionActor)
		{
			m_SuspensionActor->setGlobalPose(physx::PxTransform(PxConvert::ToPx(value), m_SuspensionActor->getGlobalPose().q));
		}
	}

	void PhysXSuspensionComponent::UpdateMotor()
	{
		if(m_WheelAxisJoint)
		{
			physx::PxD6JointDrive drive(0, m_WheelJointDamping, m_DriveMaxTorque, false);
			m_WheelAxisJoint->setDrive(physx::PxD6Drive::eTWIST, drive);
			m_WheelAxisJoint->setDriveVelocity(physx::PxVec3(0,0,0), physx::PxVec3(m_WheelAngularVelocity,0,0));
		}
	}

	float PhysXSuspensionComponent::GetSteerAngle() const
	{
		return m_SuspensionJoint->getSwingYAngle();
	}

	void PhysXSuspensionComponent::SetDriveVelocity(float velocity)
	{
		m_WheelAngularVelocity  = velocity;
		UpdateMotor();
	}

	void PhysXSuspensionComponent::SetMaxDriveTorque(float value)
	{
		m_DriveMaxTorque = value;
		UpdateMotor();
	}
}
