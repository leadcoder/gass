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
	PhysXHingeComponent::PhysXHingeComponent() : m_DriveTargetVelocity (0),
		m_DriveForceLimit(0),
		m_Strength(1),
		m_Damping(2),
		m_RevoluteJoint(0),
		m_HighStop(0),
		m_LowStop(0),
		m_EnableDrive(true),
		m_EnableLimit(false),
		m_Body1Loaded(false),
		m_Body2Loaded(false),
		m_RotationAxis(1,0,0)
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
		RegisterProperty<Vec3>("RotationAxis", &GASS::PhysXHingeComponent::GetRotationAxis, &GASS::PhysXHingeComponent::SetRotationAxis);
		RegisterProperty<float>("DriveForceLimit", &GASS::PhysXHingeComponent::GetDriveForceLimit, &GASS::PhysXHingeComponent::SetDriveForceLimit);
		RegisterProperty<float>("DriveTargetVelocity", &GASS::PhysXHingeComponent::GetDriveTargetVelocity, &GASS::PhysXHingeComponent::SetDriveTargetVelocity);
		RegisterProperty<float>("HighStop", &GASS::PhysXHingeComponent::GetHighStop, &GASS::PhysXHingeComponent::SetHighStop);
		RegisterProperty<float>("LowStop", &GASS::PhysXHingeComponent::GetLowStop, &GASS::PhysXHingeComponent::SetLowStop);
		RegisterProperty<bool>("EnableLimits", &GASS::PhysXHingeComponent::GetEnableLimits, &GASS::PhysXHingeComponent::SetEnableLimits);
		RegisterProperty<bool>("EnableDrive", &GASS::PhysXHingeComponent::GetEnableDrive, &GASS::PhysXHingeComponent::SetEnableDrive);
	}




	void PhysXHingeComponent::OnInitialize()
	{
		m_SceneManager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXHingeComponent::OnVelocityRequest,PhysicsHingeJointVelocityRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXHingeComponent::OnForceRequest,PhysicsHingeJointMaxTorqueRequest,0));

		if(!(m_Body1.IsValid() && m_Body2.IsValid()))
		{
			//Check if this hinge should link this parent with this node
			PhysXBodyComponentPtr bc1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<PhysXBodyComponent>();
			PhysXBodyComponentPtr bc2 = GetSceneObject()->GetFirstComponentByClass<PhysXBodyComponent>();
			if(bc1 && bc2)
			{
				m_Body1 = SceneObjectRef(bc1->GetSceneObject());
				m_Body2 = SceneObjectRef(bc2->GetSceneObject());
			}
			else
				return;
		}
		
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
		
		//GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXHingeComponent::SendJointUpdate,VelocityNotifyMessage,0));
		//GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXHingeComponent::OnPositionChanged,PositionMessage,0));
		//GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXHingeComponent::OnWorldPositionChanged,WorldPositionMessage,0));
	}


	
	void PhysXHingeComponent::OnVelocityRequest(PhysicsHingeJointVelocityRequestPtr message)
	{
		if(m_Body2.IsValid())
		{
			m_Body2->GetFirstComponentByClass<PhysXBodyComponent>()->WakeUp();
			SetDriveTargetVelocity(message->GetVelocity());
		}
	}

	void PhysXHingeComponent::OnForceRequest(PhysicsHingeJointMaxTorqueRequestPtr message)
	{
		if(m_Body2.IsValid())
		{
			m_Body2->GetFirstComponentByClass<PhysXBodyComponent>()->WakeUp();
			SetDriveForceLimit(message->GetForce());
		}
	}

	void PhysXHingeComponent::CreateJoint()
	{
		if(!(m_Body1.IsValid() && m_Body1.IsValid()))
			return;
		
		physx::PxRigidDynamic* a1 = m_Body1->GetFirstComponentByClass<PhysXBodyComponent>()->GetPxActor();
		physx::PxRigidDynamic* a2 = m_Body2->GetFirstComponentByClass<PhysXBodyComponent>()->GetPxActor();
		PhysXPhysicsSceneManagerPtr sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
		
		LocationComponentPtr location1 = m_Body1->GetFirstComponentByClass<ILocationComponent>();
		LocationComponentPtr location2 = m_Body2->GetFirstComponentByClass<ILocationComponent>();
		Vec3 a2_pos = location2->GetPosition() - location1->GetPosition();
		
		// use provided axis....maybe add support to use body rotation also?
		Vec3 base(1,0,0);
		Vec3 transform_axis = Math::Cross(base,m_RotationAxis);
		transform_axis.Normalize();
		double angle = acos(Math::Dot(base,m_RotationAxis));
		physx::PxQuat rot(angle,physx::PxVec3(transform_axis.x,transform_axis.y,transform_axis.z));
		
		m_RevoluteJoint = PxRevoluteJointCreate(*system->GetPxSDK(),
			 a1, physx::PxTransform(physx::PxVec3(a2_pos.x, a2_pos.y, a2_pos.z),rot), 
			 a2, physx::PxTransform(physx::PxVec3(0,0,0),rot));
		m_RevoluteJoint->setDriveGearRatio(1);

		SetEnableLimits(m_EnableLimit);
		SetEnableDrive(m_EnableDrive);
		UpdateLimits();
	}

	void PhysXHingeComponent::SetBody1(SceneObjectRef value) 
	{
		 m_Body1 = value;
		 if(m_Body1.IsValid())
		 {
			 if(m_Body1->GetFirstComponentByClass<PhysXBodyComponent>()->GetPxActor())
				 m_Body1Loaded = true;
			 else
				 m_Body1Loaded = false;
			 if(m_Body1Loaded && m_Body2Loaded)
			 	CreateJoint();
		 }
		 else
			 m_Body1Loaded = false;
	}

	void PhysXHingeComponent::SetBody2(SceneObjectRef value) 
	{
		 m_Body2 = value;
		 if(m_Body2.IsValid())
		 {
			 if(m_Body2->GetFirstComponentByClass<PhysXBodyComponent>()->GetPxActor())
				 m_Body2Loaded = true;
			 else
				 m_Body2Loaded = false;
			 if(m_Body1Loaded && m_Body2Loaded)
				CreateJoint();
		 }
		 else
			m_Body2Loaded = false;
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

	
	void PhysXHingeComponent::SetEnableLimits(bool value)
	{
		m_EnableLimit = value;
		if(m_RevoluteJoint)
			m_RevoluteJoint->setRevoluteJointFlag(physx::PxRevoluteJointFlag::eLIMIT_ENABLED, m_EnableLimit);
		if(m_EnableLimit)
			UpdateLimits();
	}

	void PhysXHingeComponent::SetEnableDrive(bool value)
	{
		m_EnableDrive = value;
		if(m_RevoluteJoint)
			m_RevoluteJoint->setRevoluteJointFlag(physx::PxRevoluteJointFlag::eDRIVE_ENABLED, m_EnableDrive);
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
				m_RevoluteJoint->setLimit(physx::PxJointLimitPair(Math::Deg2Rad(m_LowStop), Math::Deg2Rad(m_HighStop), 0.1f)); 
			}
		}

		/*float PhysXHingeComponent::GetAngle()
		{
			if(m_RevoluteJoint)
				return m_RevoluteJoint->getAngle();
			return 0;
		}

		float PhysXHingeComponent::GetAngleRate()
		{
			if(m_RevoluteJoint)
				return 0;//m_RevoluteJoint->getVelocity();
			return 0;
		}*/

		void PhysXHingeComponent::UpdateMotor()
		{
			if(m_RevoluteJoint)
			{
				m_RevoluteJoint->setDriveVelocity(m_DriveTargetVelocity);
				m_RevoluteJoint->setDriveForceLimit(m_DriveForceLimit);
			}
		}

		void PhysXHingeComponent::SetDriveTargetVelocity(float velocity)
		{
			m_DriveTargetVelocity  = velocity;
			UpdateMotor();
		}

		void PhysXHingeComponent::SetDriveForceLimit(float value)
		{
			m_DriveForceLimit = value;
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
