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

#include "Plugins/PhysX3/PhysXHingeComponent.h"
#include "Plugins/PhysX3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"

#include "Sim/GASS.h"
#include <PxActor.h>
#include <PxScene.h>

namespace GASS
{
	PhysXHingeComponent::PhysXHingeComponent() : m_RollJointForce (0),
		m_SpringJointForce (0),
		m_SwayForce  (0),
		m_Strength(1),
		m_Damping(2),
		m_Anchor (0,0,0),
		m_RollAxis (0,0,0),
		m_SpringAxis (0,0,0),
		m_RollJoint (0),
		m_HighStop(0),
		m_LowStop(0),
		m_RollAngularVelocity(0),
		m_Joint(0)
	{

	}

	PhysXHingeComponent::~PhysXHingeComponent()
	{

	}

	void PhysXHingeComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsHingeComponent",new Creator<PhysXHingeComponent, IComponent>);

		RegisterProperty<std::string>("Body1Name", &GASS::PhysXHingeComponent::GetBody1Name, &GASS::PhysXHingeComponent::SetBody1Name);
		RegisterProperty<std::string>("Body2Name", &GASS::PhysXHingeComponent::GetBody2Name, &GASS::PhysXHingeComponent::SetBody2Name);
		//RegisterProperty<float>("Axis1Force", &GASS::PhysXHingeComponent::GetAxis1Force, &GASS::PhysXHingeComponent::SetAxis1Force);
		//RegisterProperty<float>("Axis2Force", &GASS::PhysXHingeComponent::GetAxis2Force, &GASS::PhysXHingeComponent::SetAxis2Force);
		RegisterProperty<float>("Damping", &GASS::PhysXHingeComponent::GetDamping, &GASS::PhysXHingeComponent::SetDamping);
		RegisterProperty<float>("Strength", &GASS::PhysXHingeComponent::GetStrength, &GASS::PhysXHingeComponent::SetStrength);
		//RegisterProperty<float>("HighStop", &GASS::PhysXHingeComponent::GetHighStop, &GASS::PhysXHingeComponent::SetHighStop);
		//RegisterProperty<float>("LowStop", &GASS::PhysXHingeComponent::GetLowStop, &GASS::PhysXHingeComponent::SetLowStop);
		//RegisterProperty<float>("SwayForce", &GASS::PhysXHingeComponent::GetSwayForce, &GASS::PhysXHingeComponent::SetSwayForce);
		//RegisterProperty<Vec3>("Axis1", &GASS::PhysXHingeComponent::GetAxis1, &GASS::PhysXHingeComponent::SetAxis1);
		//RegisterProperty<Vec3>("Axis2", &GASS::PhysXHingeComponent::GetAxis2, &GASS::PhysXHingeComponent::SetAxis2);
		//RegisterProperty<Vec3>("Anchor", &GASS::PhysXHingeComponent::GetAnchor, &GASS::PhysXHingeComponent::SetAnchor);

	}

	void PhysXHingeComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXHingeComponent::OnLoad,BodyLoadedMessage,2));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXHingeComponent::OnParameterMessage,PhysicsJointMessage,0));
//		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXHingeComponent::SendJointUpdate,VelocityNotifyMessage,0));
		//GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXHingeComponent::OnPositionChanged,PositionMessage,0));
		//GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXHingeComponent::OnWorldPositionChanged,WorldPositionMessage,0));
		
	}

	void PhysXHingeComponent::OnParameterMessage(PhysicsJointMessagePtr message)
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

	void PhysXHingeComponent::OnLoad(BodyLoadedMessagePtr message)
	{
		m_SceneManager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
//		assert(m_SceneManager);
		CreateJoint();
	}

	void PhysXHingeComponent::CreateJoint()
	{
		PhysXBodyComponentPtr body1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<PhysXBodyComponent>();
		PhysXBodyComponentPtr body2 = GetSceneObject()->GetFirstComponentByClass<PhysXBodyComponent>();

		physx::PxRigidDynamic* a1 = body1->GetPxActor();
		physx::PxRigidDynamic* a2 = body2->GetPxActor();

		PhysXPhysicsSceneManagerPtr sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
		
		ILocationComponent *location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>().get();
		Vec3 a2_pos = location->GetPosition();
		Quaternion a2_rot = location->GetRotation();
		physx::PxQuat rot(0, physx::PxVec3(0.0f, 0.0f, 1.0f));
		
		physx::PxD6Joint* joint = PxD6JointCreate(*system->GetPxSDK(),
			 a1,physx::PxTransform(physx::PxVec3(a2_pos.x,a2_pos.y,a2_pos.z),rot), //parent
			 a2, physx::PxTransform(physx::PxVec3(0,0,0),rot));
		
		joint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eFREE);
		physx::PxD6JointDrive drive(10.0f, 100, 1000, false);
		joint->setDrive(physx::PxD6Drive::eSWING, drive);
		joint->setDriveVelocity(physx::PxVec3(0,0,0), physx::PxVec3(0,0,0));
		m_Joint = joint;
	}

	void PhysXHingeComponent::SetRollAxis(const Vec3 &axis)
	{
		m_RollAxis = axis;
		if(m_RollJoint)
			UpdateJointAxis();
	}

	void PhysXHingeComponent::SetSpringAxis(const Vec3 &axis)
	{
		m_SpringAxis = axis;
		//if(m_SpringJoint)
		//	UpdateJointAxis();
	}

	void PhysXHingeComponent::SetPosition(const Vec3 &value)
	{
//		if(m_RollAxisActor)
		{
	//		m_RollAxisActor->setGlobalPose(physx::PxTransform(PxConvert::ToPx(value), m_RollAxisActor->getGlobalPose().q));
		}
	}


	void PhysXHingeComponent::UpdateJointAxis()
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

	void PhysXHingeComponent::SetAnchor(const Vec3 &value)
	{
		m_Anchor = value;
		if(m_RollJoint)
			UpdateAnchor();
	}

	void PhysXHingeComponent::UpdateAnchor()
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

	/*	void PhysXHingeComponent::SetLowStop(float value)
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
		float PhysXHingeComponent::GetRollAngle()
		{
			if(m_RollJoint)
				return 0;//m_RollJoint->getAngle();
			return 0;
		}

		float PhysXHingeComponent::GetRollAngleRate()
		{
			if(m_RollJoint)
				return 0;//m_RollJoint->getVelocity();
			return 0;
		}

		void PhysXHingeComponent::UpdateMotor()
		{
			if(m_Joint)
			{
				m_Joint->setDriveVelocity(physx::PxVec3(0,0,0), physx::PxVec3(m_RollAngularVelocity,0,0));
				std::cout << "vel:" << m_RollAngularVelocity << "\n";
				/*NxMotorDesc motor;
				motor.velTarget = m_RollAngularVelocity;
				motor.maxForce = m_RollJointForce;
				m_RollJoint->setMotor(motor);*/
			}
		}

		void PhysXHingeComponent::SetRollAxisVel(float velocity)
		{
			m_RollAngularVelocity  = velocity;
			UpdateMotor();
		}

		void PhysXHingeComponent::SetRollAxisForce(float value)
		{
			m_RollJointForce = value;
			UpdateMotor();
		}

		/*void PhysXHingeComponent::SetAxis2Vel(float value)
		{
			m_AngularVelocity2  = value;
			UpdateMotor();

		}

		void PhysXHingeComponent::SetAxis2Force(float value)
		{
			m_JointForce2 = value;
			UpdateMotor();

		}

		float PhysXHingeComponent::GetAxis2Force() const
		{
			return m_JointForce2;
		}

		void PhysXHingeComponent::SetAxis1Force(float value)
		{
			m_JointForce1 = value;
			UpdateMotor();

		}*/

		void PhysXHingeComponent::OnPositionChanged(PositionMessagePtr message)
		{
			int this_id = (int)this; //we used address as id
			if(message->GetSenderID() != this_id) //Check if this message was from this class
			{
				Vec3 pos = message->GetPosition();
				SetPosition(pos);
			}
		}
		void PhysXHingeComponent::OnWorldPositionChanged(WorldPositionMessagePtr message)
		{
			int this_id = (int)this; //we used address as id
			if(message->GetSenderID() != this_id) //Check if this message was from this class
			{
				Vec3 pos = message->GetPosition();
				SetPosition(pos);
			}
		}

		/*void PhysXHingeComponent::SendJointUpdate(VelocityNotifyMessagePtr message)
		{
			MessagePtr joint_message;
			if(m_Joint)
			{
				
			//	float angle_rate = dJointGetHinge2Angle1Rate (m_ODEJoint);
			//	joint_message = HingeJointNotifyMessagePtr(new HingeJointNotifyMessage(angle,angle_rate));
			//	if	(joint_message)
			//		GetSceneObject()->SendImmediate(joint_message);
			}
		}*/

	}
