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

#include "Plugins/PhysX3/PhysXSuspensionComponent.h"
#include "Plugins/PhysX3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"
#include "Sim/GASS.h"

namespace GASS
{
	PhysXSuspensionComponent::PhysXSuspensionComponent() : m_SpringJointMaxForce (PX_MAX_F32),
		m_MaxSteerTorque(1000),
		m_SteerJointSpring(10),
		m_SteerJointDamping(100),
		m_AngularSteerVelocity(0),
		m_DriveMaxTorque(PX_MAX_F32),
		m_WheelJointDamping(100),
		m_Strength(1000),
		m_Damping(10),
		m_WheelAxisJoint(0),
		m_SuspensionJoint (0),
		m_SuspensionActor(NULL),
		m_WheelAngularVelocity(0),
		m_SteerLimit(0.5)
	{

	}

	PhysXSuspensionComponent::~PhysXSuspensionComponent()
	{

	}

	void PhysXSuspensionComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsSuspensionComponent",new Creator<PhysXSuspensionComponent, Component>);
		//RegisterProperty<float>("Axis1Force", &GASS::PhysXSuspensionComponent::GetAxis1Force, &GASS::PhysXSuspensionComponent::SetAxis1Force);
		//RegisterProperty<float>("Axis2Force", &GASS::PhysXSuspensionComponent::GetAxis2Force, &GASS::PhysXSuspensionComponent::SetAxis2Force);
		RegisterProperty<float>("Damping", &GASS::PhysXSuspensionComponent::GetDamping, &GASS::PhysXSuspensionComponent::SetDamping);
		RegisterProperty<float>("Strength", &GASS::PhysXSuspensionComponent::GetStrength, &GASS::PhysXSuspensionComponent::SetStrength);
		RegisterProperty<float>("SteerLimit", &GASS::PhysXSuspensionComponent::GetSteerLimit, &GASS::PhysXSuspensionComponent::SetSteerLimit);
		//RegisterProperty<float>("HighStop", &GASS::PhysXSuspensionComponent::GetHighStop, &GASS::PhysXSuspensionComponent::SetHighStop);
		//RegisterProperty<float>("LowStop", &GASS::PhysXSuspensionComponent::GetLowStop, &GASS::PhysXSuspensionComponent::SetLowStop);
		//RegisterProperty<float>("SwayForce", &GASS::PhysXSuspensionComponent::GetSwayForce, &GASS::PhysXSuspensionComponent::SetSwayForce);
		//RegisterProperty<Vec3>("Axis1", &GASS::PhysXSuspensionComponent::GetAxis1, &GASS::PhysXSuspensionComponent::SetAxis1);
		//RegisterProperty<Vec3>("Axis2", &GASS::PhysXSuspensionComponent::GetAxis2, &GASS::PhysXSuspensionComponent::SetAxis2);
		//RegisterProperty<Vec3>("Anchor", &GASS::PhysXSuspensionComponent::GetAnchor, &GASS::PhysXSuspensionComponent::SetAnchor);

	}

	void PhysXSuspensionComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXSuspensionComponent::OnLoad,PhysicsBodyLoadedEvent,2));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXSuspensionComponent::SendJointUpdate,PhysicsVelocityEvent,0));
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
		//Vec3 world_wheel_pos = wheel_comp->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();

		physx::PxQuat no_rot(0, physx::PxVec3(0.0f, 0.0f, 1.0f));

		//Create actor to connect steer and suspension joint to
		//m_SuspensionActor = system->GetPxSDK()->createRigidDynamic(physx::PxTransform(PxConvert::ToPx(world_wheel_pos),no_rot));
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
		//m_SuspensionJoint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLIMITED);
		//m_SuspensionJoint->setLinearLimit(physx::PxJointLimit(1,0.1));
		m_SuspensionJoint->setLinearLimit(physx::PxJointLinearLimit(1,physx::PxSpring(1,1)));
		physx::PxD6JointDrive suspension_drive(m_Strength, m_Damping, m_SpringJointMaxForce, false);
		m_SuspensionJoint->setDrive(physx::PxD6Drive::eY, suspension_drive);

		physx::PxD6JointDrive steer_drive(m_SteerJointSpring, m_SteerJointDamping, m_MaxSteerTorque, false);
		m_SuspensionJoint->setDrive(physx::PxD6Drive::eSWING, steer_drive);
		SetSteerLimit(m_SteerLimit);

		m_WheelAxisJoint = PxD6JointCreate(*system->GetPxSDK(),
			m_SuspensionActor,physx::PxTransform(physx::PxVec3(0,0,0),no_rot), //parent
			wheel_actor, physx::PxTransform(physx::PxVec3(0,0,0), no_rot));
		m_WheelAxisJoint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eFREE);
		//SetDriveVelocity(3);
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

	float PhysXSuspensionComponent::GetRollAngle()
	{
		return 0;
	}

	float PhysXSuspensionComponent::GetRollAngleRate()
	{
		return 0;
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

	void PhysXSuspensionComponent::OnPositionChanged(PositionRequestPtr message)
	{
		int this_id = GASS_PTR_TO_INT(this); //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}

	void PhysXSuspensionComponent::OnWorldPositionChanged(WorldPositionRequestPtr message)
	{
		int this_id = GASS_PTR_TO_INT(this); //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}

	void PhysXSuspensionComponent::SendJointUpdate(PhysicsVelocityEventPtr message)
	{
		MessagePtr joint_message;
	/*	if(m_Joint)
		{
		physx::PxTransform  t1  = m_RollAxisActor->getGlobalPose();
		physx::PxTransform  t2 = m_WheelActor->getGlobalPose();
		//t2.q.toRadiansAndUnitAxis(angle, axis);
		physx::PxReal angle = t1.q.getAngle(t2.q);

		float angle_rate =  m_WheelActor->getAngularVelocity().x;

		if(GetSceneObject()->GetName()=="JimTankWheelL1[8]")
		std::cout << "Vel:" << m_WheelActor->getAngularVelocity().x << " " << m_WheelActor->getAngularVelocity().y << " " << m_WheelActor->getAngularVelocity().z << "\n";

		//std::cout << "diff:" << angle2 << " Rad:" << angle << " Axis:" << axis.x << " " << axis.y << " " << axis.z << "\n";
		//	float angle_rate = dJointGetHinge2Angle1Rate (m_ODEJoint);

		joint_message = ODEPhysicsHingeJointEventPtr(new ODEPhysicsHingeJointEvent(angle,0));
		if	(joint_message)
		GetSceneObject()->SendImmediate(joint_message);
		}*/
	}

}
