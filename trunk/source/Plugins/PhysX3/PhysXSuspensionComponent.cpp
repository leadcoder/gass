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

#include "Plugins/PhysX3/PhysXSuspensionComponent.h"
#include "Plugins/PhysX3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"

#include "Sim/GASS.h"
#include <PxActor.h>
#include <PxScene.h>

namespace GASS
{
	PhysXSuspensionComponent::PhysXSuspensionComponent() : m_RollJointForce (0),
		m_SpringJointForce (0),
		m_SwayForce  (0),
		m_Strength(1),
		m_Damping(2),
		m_Anchor (0,0,0),
		m_RollAxis (0,0,0),
		m_SpringAxis (0,0,0),
		m_RollJoint (0),
		m_SpringJoint (0),
		m_HighStop(0),
		m_LowStop(0),
		m_RollAxisActor(NULL),
		m_RollAngularVelocity(0),
		m_Joint(0)
	{

	}

	PhysXSuspensionComponent::~PhysXSuspensionComponent()
	{

	}

	void PhysXSuspensionComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsSuspensionComponent",new Creator<PhysXSuspensionComponent, IComponent>);

		//RegisterProperty<float>("Axis1Force", &GASS::PhysXSuspensionComponent::GetAxis1Force, &GASS::PhysXSuspensionComponent::SetAxis1Force);
		//RegisterProperty<float>("Axis2Force", &GASS::PhysXSuspensionComponent::GetAxis2Force, &GASS::PhysXSuspensionComponent::SetAxis2Force);
		RegisterProperty<float>("Damping", &GASS::PhysXSuspensionComponent::GetDamping, &GASS::PhysXSuspensionComponent::SetDamping);
		RegisterProperty<float>("Strength", &GASS::PhysXSuspensionComponent::GetStrength, &GASS::PhysXSuspensionComponent::SetStrength);
		//RegisterProperty<float>("HighStop", &GASS::PhysXSuspensionComponent::GetHighStop, &GASS::PhysXSuspensionComponent::SetHighStop);
		//RegisterProperty<float>("LowStop", &GASS::PhysXSuspensionComponent::GetLowStop, &GASS::PhysXSuspensionComponent::SetLowStop);
		//RegisterProperty<float>("SwayForce", &GASS::PhysXSuspensionComponent::GetSwayForce, &GASS::PhysXSuspensionComponent::SetSwayForce);
		//RegisterProperty<Vec3>("Axis1", &GASS::PhysXSuspensionComponent::GetAxis1, &GASS::PhysXSuspensionComponent::SetAxis1);
		//RegisterProperty<Vec3>("Axis2", &GASS::PhysXSuspensionComponent::GetAxis2, &GASS::PhysXSuspensionComponent::SetAxis2);
		//RegisterProperty<Vec3>("Anchor", &GASS::PhysXSuspensionComponent::GetAnchor, &GASS::PhysXSuspensionComponent::SetAnchor);

	}

	void PhysXSuspensionComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXSuspensionComponent::OnLoad,BodyLoadedMessage,2));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXSuspensionComponent::OnParameterMessage,PhysicsJointMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXSuspensionComponent::SendJointUpdate,VelocityNotifyMessage,0));
		//GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXSuspensionComponent::OnPositionChanged,PositionMessage,0));
		//GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXSuspensionComponent::OnWorldPositionChanged,WorldPositionMessage,0));
		
	}

	void PhysXSuspensionComponent::OnParameterMessage(PhysicsJointMessagePtr message)
	{
		PhysicsJointMessage::PhysicsJointParameterType type = message->GetParameter();

		float value = message->GetValue();

		//wake body!!

		//m_Body1->Enable();
		switch(type)
		{
		case PhysicsJointMessage::AXIS1_VELOCITY:
			break;
		case PhysicsJointMessage::AXIS2_VELOCITY:
			SetRollAxisVel(value);
			break;
		case PhysicsJointMessage::AXIS1_FORCE:
			break;
		case PhysicsJointMessage::AXIS2_FORCE:
				SetRollAxisForce(value);
			break;
		}
	}

	void PhysXSuspensionComponent::OnLoad(BodyLoadedMessagePtr message)
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

		physx::PxRigidDynamic* chassis_actor = chassis_comp->GetPxActor();
		physx::PxRigidDynamic* wheel_actor = wheel_comp->GetPxActor();

		//ignore collision for all parent bodies 
		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetParentSceneObject()->GetComponentsByClass(components,"PhysXBodyComponent");
		for(int i = 0; i < components.size(); i++)
		{
			PhysXBodyComponentPtr body = STATIC_PTR_CAST<PhysXBodyComponent>(components[i]);
			//if(body->GetPxActor() && body->GetPxActor() != a2)
			//	PhysXPhysicsSceneManagerPtr(m_SceneManager)->GetPxScene()->setActorPairFlags(*body->GetPxActor(), *a2, PX_IGNORE_PAIR);
		}
		
		Vec3 chassis_pos = chassis_comp->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
		Vec3 wheel_pos = wheel_comp->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
		

		
		//physx::PxVec3 px_wheel_pos(wheel_pos.x,wheel_pos.y,wheel_pos.z);
		physx::PxQuat no_rot(0, physx::PxVec3(0.0f, 0.0f, 1.0f));
		physx::PxTransform roll_actor_trans(PxConvert::ToPx(wheel_pos),no_rot);
	
		m_RollAxisActor = system->GetPxSDK()->createRigidDynamic(roll_actor_trans);
		//m_RollAxisActor->setMass(0.1);
		sm->GetPxScene()->addActor(*m_RollAxisActor);
		//m_RollAxisActor = wheel_actor;
		physx::PxVec3 susp_pos = PxConvert::ToPx(wheel_pos - chassis_pos);

		physx::PxQuat susp_rot(-physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f));
		//physx::PxQuat susp_rot(0, physx::PxVec3(0.0f, 0.0f, 1.0f));
		m_SpringJoint = PxPrismaticJointCreate(*system->GetPxSDK(),
			 chassis_actor,physx::PxTransform(physx::PxTransform(susp_pos,susp_rot)), //parent
			 m_RollAxisActor, physx::PxTransform(physx::PxVec3(0,0,0),susp_rot));
		physx::PxJointLimitPair params(-1.0f, 1.0f, 0.001f);
		/*params.restitution = 10.0;
		params.spring = 1000;
		params.damping = 1000;
		params.contactDistance = 1;*/
		m_SpringJoint->setLimit(params);
		m_SpringJoint->setPrismaticJointFlag(physx::PxPrismaticJointFlag::eLIMIT_ENABLED, true);

		/*physx::PxD6Joint* joint = PxD6JointCreate(*system->GetPxSDK(),
			 a1,physx::PxTransform(pos,trot), //parent
			 m_RollAxisActor, physx::PxTransform(physx::PxVec3(0,0,0),trot));
		
		physx::PxD6Joint* joint2 = PxD6JointCreate(*system->GetPxSDK(),
			 m_RollAxisActor,physx::PxTransform(physx::PxVec3(0,0,0),trot), //parent
			 a2, physx::PxTransform(physx::PxVec3(0,0,0),trot));
		
		joint2->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eFREE);
		joint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eLOCKED);
		joint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLOCKED);
		physx::PxD6JointDrive drive1(-6000.0f, -100.0f, PX_MAX_F32, true);
		joint->setDrive(physx::PxD6Drive::eY, drive1);
		physx::PxD6JointDrive drive2(10.0f, 100, 1000, false);
		joint2->setDrive(physx::PxD6Drive::eTWIST, drive2);
		joint2->setDriveVelocity(physx::PxVec3(0,0,0), physx::PxVec3(0,0,0));
		m_Joint = joint2;
		m_WheelActor = a2;*/

		/*m_Joint = PxD6JointCreate(*system->GetPxSDK(),
			m_RollAxisActor,physx::PxTransform(physx::PxVec3(0,0,0),no_rot), //parent
			wheel_actor, physx::PxTransform(physx::PxVec3(0,0,0), no_rot));
		
		//joint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eFREE);
		m_Joint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eFREE);
		//joint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eFREE);
		physx::PxD6JointDrive drive1(6000.0f, 1000.0f, PX_MAX_F32, true);
		physx::PxD6JointDrive drive2(10.0f, 100, 1000, false);
		//joint->setDrive(physx::PxD6Drive::eY, drive1);
		m_Joint->setDrive(physx::PxD6Drive::eTWIST, drive2);
		m_Joint->setDriveVelocity(physx::PxVec3(0,0,0), physx::PxVec3(0,0,0));
	*/
		//physx::PxTransform trans(physx::PxVec3(0,0,0), trot);
		/*physx::PxQuat trot(-physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f));
		
		m_SpringJoint = PxPrismaticJointCreate(*system->GetPxSDK(),
			 a1,physx::PxTransform(pos,trot), //parent
			 a2, physx::PxTransform(physx::PxVec3(0,0,0),trot));

		physx::PxJointLimitPair params(-0.5f, 0.5f, 1000.f);
		//params.restitution = 10.0;
		//params.spring = 1000;
		//params.damping = 1000;
		//params.contactDistance = 1;
		m_SpringJoint->setLimit(params);
		m_SpringJoint->setPrismaticJointFlag(physx::PxPrismaticJointFlag::eLIMIT_ENABLED, true);
		*/
/*		m_SpringJoint = PxPrismaticJointCreate(*system->GetPxSDK(),
			 a1, physx::PxTransform(pos,trot),
			 m_RollAxisActor,trans);

		
		m_RollJoint = PxRevoluteJointCreate(*system->GetPxSDK(),
			 m_RollAxisActor, trans,
			 a2,trans);
*/
		
		//add springs and dampers to the suspension (i.e. the related actors)
		/*float springLength = 0.2f;
		NxSpringAndDamperEffector * springNdamp = PhysXPhysicsSceneManagerPtr(m_SceneManager)->GetNxScene()->createSpringAndDamperEffector(NxSpringAndDamperEffectorDesc());

		springNdamp->setBodies(a1, pos, m_RollAxisActor, pos + NxVec3(0,springLength,0));
		springNdamp->setLinearSpring(0, springLength, 2*springLength, 10, 10);
		springNdamp->setLinearDamper(-1,1, 10, 10);

		UpdateAnchor();
		UpdateJointAxis();
//		UpdateLimits();
		SetRollAxisForce(m_RollJointForce);*/
	}

	void PhysXSuspensionComponent::SetRollAxis(const Vec3 &axis)
	{
		m_RollAxis = axis;
		if(m_RollJoint)
			UpdateJointAxis();
	}

	void PhysXSuspensionComponent::SetSpringAxis(const Vec3 &axis)
	{
		m_SpringAxis = axis;
		if(m_SpringJoint)
			UpdateJointAxis();
	}

	void PhysXSuspensionComponent::SetPosition(const Vec3 &value)
	{
		/*if(m_RollAxisActor)
		{
			m_RollAxisActor->setGlobalPose(physx::PxTransform(PxConvert::ToPx(value), m_RollAxisActor->getGlobalPose().q));
		}*/
	}


	void PhysXSuspensionComponent::UpdateJointAxis()
	{
		if (m_RollAxis.Length() != 0)
		{
			//m_RollJoint->setGlobalAxis(NxVec3(m_RollAxis.x,m_RollAxis.y,m_RollAxis.z));
		}
		else
		{
			//not thread safe!!! fix this
			/*ILocationComponent *location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponent<ILocationComponent>().get();
			//ILocationComponent *location2 = GetSceneObject()->GetFirstComponent<ILocationComponent>().get();
			Quaternion rot = location1->GetRotation();
			Mat4 rot_mat;
			rot_mat.Identity();
			rot.ToRotationMatrix(rot_mat);
			Vec3 axis = rot_mat.GetRightVector();
			m_RollJoint->setGlobalAxis(NxVec3(axis.x,axis.y,axis.z));*/
		}
	}

	void PhysXSuspensionComponent::SetAnchor(const Vec3 &value)
	{
		m_Anchor = value;
		if(m_RollJoint)
			UpdateAnchor();
	}

	void PhysXSuspensionComponent::UpdateAnchor()
	{
		//ILocationComponent *location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponent<ILocationComponent>().get();
		ILocationComponent *location2 = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>().get();

		//Vec3 pos_b1 = location1->GetPosition();
		Vec3 pos_b2 = location2->GetPosition();
		Vec3  world_anchor;

		if(m_RollJoint)
		{
			world_anchor = m_Anchor + pos_b2;
			//m_RollJoint->setGlobalAnchor(NxVec3(world_anchor.x,world_anchor.y,world_anchor.z));
			
		}
	}

	/*	void PhysXSuspensionComponent::SetLowStop(float value)
		{
			m_LowStop = value;
			UpdateLimits();
		}

		void PhysXSuspensionComponent::SetHighStop(float value)
		{
			m_HighStop = value;
			UpdateLimits();
		}


		void PhysXSuspensionComponent::UpdateLimits()
		{
			if(m_RollJoint)
			{
				NxRevoluteJoint* joint = static_cast<NxRevoluteJoint*>(m_RollJoint);
				NxJointLimitPairDesc limits;
				limits.high.setToDefault();
				limits.high.value = m_HighStop;
				limits.low.setToDefault();
				limits.low.value = m_LowStop;
				joint->setLimits(limits);
				
				
			}
		}
*/
		float PhysXSuspensionComponent::GetRollAngle()
		{
			if(m_RollJoint)
				return 0;//m_RollJoint->getAngle();
			return 0;
		}

		float PhysXSuspensionComponent::GetRollAngleRate()
		{
			if(m_RollJoint)
				return 0;//m_RollJoint->getVelocity();
			return 0;
		}

		void PhysXSuspensionComponent::UpdateMotor()
		{
			if(m_Joint)
			{
				m_Joint->setDriveVelocity(physx::PxVec3(0,0,0), physx::PxVec3(m_RollAngularVelocity,0,0));
				//std::cout << "vel:" << m_RollAngularVelocity << "\n";
				/*NxMotorDesc motor;
				motor.velTarget = m_RollAngularVelocity;
				motor.maxForce = m_RollJointForce;
				m_RollJoint->setMotor(motor);*/
			}
		}

		void PhysXSuspensionComponent::SetRollAxisVel(float velocity)
		{
			m_RollAngularVelocity  = velocity;
			UpdateMotor();
		}

		void PhysXSuspensionComponent::SetRollAxisForce(float value)
		{
			m_RollJointForce = value;
			UpdateMotor();
		}

		/*void PhysXSuspensionComponent::SetAxis2Vel(float value)
		{
			m_AngularVelocity2  = value;
			UpdateMotor();

		}

		void PhysXSuspensionComponent::SetAxis2Force(float value)
		{
			m_JointForce2 = value;
			UpdateMotor();

		}

		float PhysXSuspensionComponent::GetAxis2Force() const
		{
			return m_JointForce2;
		}

		void PhysXSuspensionComponent::SetAxis1Force(float value)
		{
			m_JointForce1 = value;
			UpdateMotor();

		}*/

		void PhysXSuspensionComponent::OnPositionChanged(PositionMessagePtr message)
		{
			int this_id = (int)this; //we used address as id
			if(message->GetSenderID() != this_id) //Check if this message was from this class
			{
				Vec3 pos = message->GetPosition();
				SetPosition(pos);
			}
		}
		void PhysXSuspensionComponent::OnWorldPositionChanged(WorldPositionMessagePtr message)
		{
			int this_id = (int)this; //we used address as id
			if(message->GetSenderID() != this_id) //Check if this message was from this class
			{
				Vec3 pos = message->GetPosition();
				SetPosition(pos);
			}
		}

		void PhysXSuspensionComponent::SendJointUpdate(VelocityNotifyMessagePtr message)
		{
			MessagePtr joint_message;
			/*if(m_Joint)
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
					
					joint_message = HingeJointNotifyMessagePtr(new HingeJointNotifyMessage(angle,0));
					if	(joint_message)
						GetSceneObject()->SendImmediate(joint_message);
			}*/
		}

	}
