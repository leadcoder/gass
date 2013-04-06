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
	PhysXHingeComponent::PhysXHingeComponent() : m_RevoluteJointForce (0),
		m_SpringJointForce (0),
		m_Strength(1),
		m_Damping(2),
		m_RollAxis (0,0,0),
		m_RevoluteJoint (0),
		m_HighStop(0),
		m_LowStop(0),
		m_RollAngularVelocity(0),
		m_Body1Loaded(false),
		m_Body2Loaded(false)
	{

	}

	PhysXHingeComponent::~PhysXHingeComponent()
	{

	}

	void PhysXHingeComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsHingeComponent",new Creator<PhysXHingeComponent, IComponent>);
		RegisterProperty<SceneObjectRef>("Body1", &GASS::PhysXHingeComponent::GetBody1, &GASS::PhysXHingeComponent::SetBody1);
		RegisterProperty<SceneObjectRef>("Body2", &GASS::PhysXHingeComponent::GetBody2, &GASS::PhysXHingeComponent::SetBody2);
		RegisterProperty<float>("Damping", &GASS::PhysXHingeComponent::GetDamping, &GASS::PhysXHingeComponent::SetDamping);
		RegisterProperty<float>("Strength", &GASS::PhysXHingeComponent::GetStrength, &GASS::PhysXHingeComponent::SetStrength);

		
		
		//RegisterProperty<std::string>("Body1Name", &GASS::PhysXHingeComponent::GetBody1Name, &GASS::PhysXHingeComponent::SetBody1Name);
		//RegisterProperty<std::string>("Body2Name", &GASS::PhysXHingeComponent::GetBody2Name, &GASS::PhysXHingeComponent::SetBody2Name);
		//RegisterProperty<float>("Axis1Force", &GASS::PhysXHingeComponent::GetAxis1Force, &GASS::PhysXHingeComponent::SetAxis1Force);
		//RegisterProperty<float>("Axis2Force", &GASS::PhysXHingeComponent::GetAxis2Force, &GASS::PhysXHingeComponent::SetAxis2Force);
		//RegisterProperty<float>("HighStop", &GASS::PhysXHingeComponent::GetHighStop, &GASS::PhysXHingeComponent::SetHighStop);
		//RegisterProperty<float>("LowStop", &GASS::PhysXHingeComponent::GetLowStop, &GASS::PhysXHingeComponent::SetLowStop);
		//RegisterProperty<float>("SwayForce", &GASS::PhysXHingeComponent::GetSwayForce, &GASS::PhysXHingeComponent::SetSwayForce);
		//RegisterProperty<Vec3>("Axis1", &GASS::PhysXHingeComponent::GetAxis1, &GASS::PhysXHingeComponent::SetAxis1);
		//RegisterProperty<Vec3>("Axis2", &GASS::PhysXHingeComponent::GetAxis2, &GASS::PhysXHingeComponent::SetAxis2);
		//RegisterProperty<Vec3>("Anchor", &GASS::PhysXHingeComponent::GetAnchor, &GASS::PhysXHingeComponent::SetAnchor);

	}

	void PhysXHingeComponent::OnInitialize()
	{
		m_SceneManager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		
		
		PhysXBodyComponentPtr b1 = m_Body1->GetFirstComponentByClass<PhysXBodyComponent>();
		if(!b1)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"First body not found, your second object don't have a PhysXBodyComponent","PhysXHingeComponent::RegisterReflection");

		PhysXBodyComponentPtr b2 = m_Body2->GetFirstComponentByClass<PhysXBodyComponent>();
		if(!b2)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Second body not found, your second object don't have a PhysXBodyComponent","PhysXHingeComponent::RegisterReflection");
	
		// Check if bodies are already are loaded
		if(m_Body1->GetFirstComponentByClass<PhysXBodyComponent>()->GetPxActor())
			m_Body1Loaded = true;
		else //wait for body to be loaded
			m_Body1->RegisterForMessage(REG_TMESS(PhysXHingeComponent::OnBody1Loaded,BodyLoadedMessage,0));

		if(m_Body2->GetFirstComponentByClass<PhysXBodyComponent>()->GetPxActor())
			m_Body2Loaded = true;
		else //wait for body to be loaded
			m_Body2->RegisterForMessage(REG_TMESS(PhysXHingeComponent::OnBody2Loaded,BodyLoadedMessage,0));
	
		//both bodies are loaded -> create joint
		if(m_Body1Loaded && m_Body2Loaded)
			CreateJoint();
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXHingeComponent::OnParameterMessage,PhysicsJointMessage,0));
		//GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXHingeComponent::SendJointUpdate,VelocityNotifyMessage,0));
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

	void PhysXHingeComponent::OnBody1Loaded(BodyLoadedMessagePtr message)
	{
		m_Body1Loaded = true;
		if(m_Body2Loaded)
			CreateJoint();
	}

	void PhysXHingeComponent::OnBody2Loaded(BodyLoadedMessagePtr message)
	{
		m_Body2Loaded = true;
		if(m_Body1Loaded)
			CreateJoint();
	}

	void PhysXHingeComponent::CreateJoint()
	{
		if(!(m_Body1.IsValid() && m_Body1.IsValid()))
			return;
		
		physx::PxRigidDynamic* a1 = m_Body1->GetFirstComponentByClass<PhysXBodyComponent>()->GetPxActor();
		physx::PxRigidDynamic* a2 = m_Body2->GetFirstComponentByClass<PhysXBodyComponent>()->GetPxActor();
		PhysXPhysicsSceneManagerPtr sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
		
		LocationComponentPtr location = m_Body2->GetFirstComponentByClass<ILocationComponent>();
		Vec3 a2_pos = location->GetPosition();
		physx::PxQuat rot(MY_PI/2.0,physx::PxVec3(0,1,0));
		m_RevoluteJoint = PxRevoluteJointCreate(*system->GetPxSDK(),
			 a1, physx::PxTransform(physx::PxVec3(a2_pos.x, a2_pos.y, a2_pos.z),rot), 
			 a2, physx::PxTransform(physx::PxVec3(0,0,0),rot));

		m_RevoluteJoint->setRevoluteJointFlag(physx::PxRevoluteJointFlag::eLIMIT_ENABLED, false);
		m_RevoluteJoint->setRevoluteJointFlag(physx::PxRevoluteJointFlag::eDRIVE_ENABLED, true);
		m_RevoluteJoint->setDriveGearRatio(1);
		//m_RevoluteJoint->setDriveForceLimit(0.00000001);

		/*physx::PxD6Joint* joint = PxD6JointCreate(*system->GetPxSDK(),
			 a1, physx::PxTransform(physx::PxVec3(a2_pos.x,a2_pos.y,a2_pos.z),rot), //parent
			 a2, physx::PxTransform(physx::PxVec3(0,0,0),rot));
		
		//joint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eFREE);
		joint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eFREE);
		
		physx::PxD6JointDrive drive(10.0f, 100, 1000, false);
		//joint->setDrive(physx::PxD6Drive::eSWING, drive);
		joint->setDrive(physx::PxD6Drive::eTWIST, drive);
		joint->setDriveVelocity(physx::PxVec3(0,0,0), physx::PxVec3(0,0,0));
		m_Joint = joint;*/
	}

	void PhysXHingeComponent::SetRollAxis(const Vec3 &axis)
	{
		m_RollAxis = axis;
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
			if(m_RevoluteJoint)
			{
				NxRevoluteJoint* joint = static_cast<NxRevoluteJoint*>(m_RevoluteJoint);
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
			if(m_RevoluteJoint)
				return 0;//m_RevoluteJoint->getAngle();
			return 0;
		}

		float PhysXHingeComponent::GetRollAngleRate()
		{
			if(m_RevoluteJoint)
				return 0;//m_RevoluteJoint->getVelocity();
			return 0;
		}

		void PhysXHingeComponent::UpdateMotor()
		{
/*			if(m_Joint)
			{
				m_Joint->setDriveVelocity(physx::PxVec3(0,0,0), physx::PxVec3(m_RollAngularVelocity,m_RollAngularVelocity,m_RollAngularVelocity));
				std::cout << "vel:" << m_RollAngularVelocity << "\n";
				/*NxMotorDesc motor;
				motor.velTarget = m_RollAngularVelocity;
				motor.maxForce = m_RevoluteJointForce;
				m_RevoluteJoint->setMotor(motor);*/
	//		}
			if(m_RevoluteJoint)
			{
				m_RevoluteJoint->setDriveVelocity(m_RollAngularVelocity);
			}
		}

		void PhysXHingeComponent::SetRollAxisVel(float velocity)
		{
			m_RollAngularVelocity  = velocity;
			UpdateMotor();
		}

		void PhysXHingeComponent::SetRollAxisForce(float value)
		{
			m_RevoluteJointForce = value;
			UpdateMotor();
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
