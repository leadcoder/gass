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

#include "Plugins/PhysX/PhysXHingeComponent.h"
#include "Plugins/PhysX/PhysXBodyComponent.h"
#include "Plugins/PhysX/PhysXPhysicsSceneManager.h"
#include "Core/Math/AABox.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponentContainer.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include <boost/bind.hpp>

namespace GASS
{
	PhysXHingeComponent::PhysXHingeComponent() : m_SceneManager(NULL), 
		m_JointForce (0),
		m_Strength(1),
		m_Damping(2),
		m_Anchor (0,0,0),
		m_Axis (0,0,0),
		m_RevoluteJoint (0),
		m_HighStop(0),
		m_LowStop(0)
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
		RegisterProperty<float>("AxisForce", &GASS::PhysXHingeComponent::GetAxisForce, &GASS::PhysXHingeComponent::SetAxisForce);
		RegisterProperty<float>("Damping", &GASS::PhysXHingeComponent::GetDamping, &GASS::PhysXHingeComponent::SetDamping);
		RegisterProperty<float>("Strength", &GASS::PhysXHingeComponent::GetStrength, &GASS::PhysXHingeComponent::SetStrength);
		RegisterProperty<float>("HighStop", &GASS::PhysXHingeComponent::GetHighStop, &GASS::PhysXHingeComponent::SetHighStop);
		RegisterProperty<float>("LowStop", &GASS::PhysXHingeComponent::GetLowStop, &GASS::PhysXHingeComponent::SetLowStop);
		RegisterProperty<Vec3>("Axis", &GASS::PhysXHingeComponent::GetAxis, &GASS::PhysXHingeComponent::SetAxis);
		RegisterProperty<Vec3>("Anchor", &GASS::PhysXHingeComponent::GetAnchor, &GASS::PhysXHingeComponent::SetAnchor);
	}

	void PhysXHingeComponent::OnCreate()
	{

		REGISTER_OBJECT_MESSAGE_CLASS(PhysXHingeComponent::OnLoad,LoadPhysicsComponentsMessage,2);
		REGISTER_OBJECT_MESSAGE_CLASS(PhysXHingeComponent::OnParameterMessage,PhysicsJointMessage,0);
	}

	void PhysXHingeComponent::OnParameterMessage(PhysicsJointMessagePtr message)
	{
		PhysicsJointMessage::PhysicsJointParameterType type = message->GetParameter();
		float value = message->GetValue();
		switch(type)
		{
		case PhysicsJointMessage::AXIS1_VELOCITY:
			SetAxisVel(value);
			break;
		case PhysicsJointMessage::AXIS2_VELOCITY:
			break;
		case PhysicsJointMessage::AXIS1_FORCE:
			SetAxisForce(value);
			break;
		case PhysicsJointMessage::AXIS2_FORCE:
			break;
		}
	}

	void PhysXHingeComponent::OnLoad(LoadPhysicsComponentsMessagePtr message)
	{
		m_SceneManager = static_cast<PhysXPhysicsSceneManager*>(message->GetPhysicsSceneManager());
		assert(m_SceneManager);
		CreateJoint();
	}

	void PhysXHingeComponent::CreateJoint()
	{
		PhysXBodyComponentPtr body1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponent<PhysXBodyComponent>();
		PhysXBodyComponentPtr body2 = GetSceneObject()->GetFirstComponent<PhysXBodyComponent>();

		NxActor* a1 = body1->GetNxActor();
		NxActor* a2 = body2->GetNxActor();


		//ignore collision for all parent bodies 
		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetParentSceneObject()->GetComponentsByClass(components,"PhysXBodyComponent");
			
		for(int i = 0; i < components.size(); i++)
		{
			PhysXBodyComponentPtr body = boost::shared_static_cast<PhysXBodyComponent>(components[i]);
			if(body->GetNxActor() && body->GetNxActor() != a2)
				m_SceneManager->GetNxScene()->setActorPairFlags(*body->GetNxActor(), *a2, NX_IGNORE_PAIR);
		}

		NxRevoluteJointDesc revoluteDesc;
		revoluteDesc.setToDefault();
		revoluteDesc.actor[0] = a2;
		revoluteDesc.actor[1] = a1;
		revoluteDesc.setGlobalAxis(NxVec3(0,1,0));
		m_RevoluteJoint = static_cast<NxRevoluteJoint*>(m_SceneManager->GetNxScene()->createJoint(revoluteDesc));
	
		UpdateAnchor();
		UpdateJointAxis();
		UpdateLimits();
		SetAxisForce(m_JointForce);
	}

	void PhysXHingeComponent::SetAxis(const Vec3 &axis)
	{
		m_Axis = axis;
		if(m_RevoluteJoint)
			UpdateJointAxis();
	}

	void PhysXHingeComponent::UpdateJointAxis()
	{
		if (m_Axis.Length() != 0)
			m_RevoluteJoint->setGlobalAxis(NxVec3(m_Axis.x,m_Axis.y,m_Axis.z));
		else
		{
			//not thread safe!!! fix this
			LocationComponentPtr location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponent<ILocationComponent>();
			Quaternion rot = location1->GetRotation();
			
			Mat4 rot_mat;
			rot_mat.Identity();
			rot.ToRotationMatrix(rot_mat);

			Vec3 axis = rot_mat.GetUpVector();
			m_RevoluteJoint->setGlobalAxis(NxVec3(axis.x,axis.y,axis.z));
		}
	}

	void PhysXHingeComponent::SetAnchor(const Vec3 &value)
	{
		m_Anchor = value;
		if(m_RevoluteJoint)
			UpdateAnchor();
	}

	void PhysXHingeComponent::UpdateAnchor()
	{
		LocationComponentPtr location2 = GetSceneObject()->GetFirstComponent<ILocationComponent>();
		Vec3 pos_b2 = location2->GetPosition();
		Vec3  world_anchor;

		if(m_RevoluteJoint)
		{
			world_anchor = m_Anchor + pos_b2;
			m_RevoluteJoint->setGlobalAnchor(NxVec3(world_anchor.x,world_anchor.y,world_anchor.z));
		}
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
		if(m_RevoluteJoint)
		{
			NxJointLimitPairDesc limits;
			m_RevoluteJoint->getLimits(limits);
			limits.high.value = m_HighStop;
			limits.high.restitution = 0;
			limits.low.value = m_LowStop;
			limits.low.restitution = 0;
			
			m_RevoluteJoint->setLimits(limits);
		/*	NxSpringDesc spring;
			m_RevoluteJoint->getSpring(spring);
			spring.damper = 0;
			spring.spring = 0;
			m_RevoluteJoint->setSpring(spring);*/
		}
	}

	float PhysXHingeComponent::GetAngle()
	{
		if(m_RevoluteJoint)
			return m_RevoluteJoint->getAngle();

		return 0;
	}

	float PhysXHingeComponent::GetAngleRate()
	{
		if(m_RevoluteJoint)
			return m_RevoluteJoint->getVelocity();
		return 0.0;
	}

	void PhysXHingeComponent::UpdateMotor()
	{
		if(m_RevoluteJoint)
		{
			std::cout << "force:" << m_JointForce << "vel:" << m_AngularVelocity <<  std::endl;
			NxRevoluteJoint* joint = static_cast<NxRevoluteJoint*>(m_RevoluteJoint);
			NxMotorDesc motor;
			motor.velTarget = m_AngularVelocity;
			motor.maxForce = m_JointForce;
			m_RevoluteJoint->setMotor(motor);
		}
	}

	void PhysXHingeComponent::SetAxisVel(float velocity)
	{
		m_AngularVelocity  = velocity;
		UpdateMotor();
	}

	void PhysXHingeComponent::SetAxisForce(float value)
	{
		m_JointForce = value;
		UpdateMotor();
	}

}
