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

#include "Plugins/PhysX/PhysXSuspensionComponent.h"
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
	PhysXSuspensionComponent::PhysXSuspensionComponent() : m_SceneManager(NULL), 
		m_RollJointForce (0),
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
		m_RollAxisActor(NULL)
	{

	}

	PhysXSuspensionComponent::~PhysXSuspensionComponent()
	{

	}

	void PhysXSuspensionComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsSuspensionComponent",new Creator<PhysXSuspensionComponent, IComponent>);

		RegisterProperty<std::string>("Body1Name", &GASS::PhysXSuspensionComponent::GetBody1Name, &GASS::PhysXSuspensionComponent::SetBody1Name);
		RegisterProperty<std::string>("Body2Name", &GASS::PhysXSuspensionComponent::GetBody2Name, &GASS::PhysXSuspensionComponent::SetBody2Name);
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

	void PhysXSuspensionComponent::OnCreate()
	{
		REGISTER_OBJECT_MESSAGE_CLASS(PhysXSuspensionComponent::OnLoad,LoadPhysicsComponentsMessage,2);
		REGISTER_OBJECT_MESSAGE_CLASS(PhysXSuspensionComponent::OnParameterMessage,PhysicsJointMessage,0);
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

	void PhysXSuspensionComponent::OnLoad(LoadPhysicsComponentsMessagePtr message)
	{
		m_SceneManager = static_cast<PhysXPhysicsSceneManager*>(message->GetPhysicsSceneManager());
		assert(m_SceneManager);
		CreateJoint();
	}

	void PhysXSuspensionComponent::CreateJoint()
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

		ILocationComponent *location2 = GetSceneObject()->GetFirstComponent<ILocationComponent>().get();
		Vec3 pos_b1 = location2->GetPosition();

		NxVec3 pos(pos_b1.x,pos_b1.y,pos_b1.z);
		NxActorDesc actorDesc;
		NxBodyDesc bodyDesc;
		actorDesc.setToDefault();
		bodyDesc.setToDefault();


		//Get mass from wheel
		bodyDesc.mass = a2->getMass()*0.4;
		bodyDesc.massSpaceInertia = NxVec3(0.1,0.1,0.1);
		actorDesc.body			= &bodyDesc;
		actorDesc.shapes.clear();
		actorDesc.globalPose.t  = pos;
		m_RollAxisActor = m_SceneManager->GetNxScene()->createActor(actorDesc);

		// revolution joint connecting wheel with rollAxis
		NxRevoluteJointDesc revJointDesc;
		revJointDesc.setToDefault();
		revJointDesc.projectionMode = NX_JPM_POINT_MINDIST;
		revJointDesc.actor[0] = m_RollAxisActor;
		revJointDesc.actor[1] = a2;
		revJointDesc.setGlobalAnchor(pos);
		revJointDesc.setGlobalAxis(NxVec3(1,0,0));
		NxJoint * joint = m_SceneManager->GetNxScene()->createJoint(revJointDesc);
		m_RollJoint = static_cast<NxRevoluteJoint*>(joint);

		NxPrismaticJointDesc prisJointDesc;
		prisJointDesc.setToDefault();
		prisJointDesc.actor[0] = a1;
		prisJointDesc.actor[1] = m_RollAxisActor;
		prisJointDesc.setGlobalAnchor(pos);
		prisJointDesc.setGlobalAxis(NxVec3(0,1,0));
		joint = m_SceneManager->GetNxScene()->createJoint(prisJointDesc);
		m_SpringJoint = static_cast<NxPrismaticJoint*>(joint);

		//add springs and dampers to the suspension (i.e. the related actors)
		float springLength = 0.2f;
		NxSpringAndDamperEffector * springNdamp = m_SceneManager->GetNxScene()->createSpringAndDamperEffector(NxSpringAndDamperEffectorDesc());

		springNdamp->setBodies(a1, pos, m_RollAxisActor, pos + NxVec3(0,springLength,0));
		springNdamp->setLinearSpring(0, springLength, 2*springLength, 10, 10);
		springNdamp->setLinearDamper(-1,1, 10, 10);

		UpdateAnchor();
		UpdateJointAxis();
//		UpdateLimits();
		SetRollAxisForce(m_RollJointForce);
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
		if(m_RollAxisActor)
		{
			m_RollAxisActor->setGlobalPosition(NxVec3(value.x, value.y, value.z));
		}
	}


	void PhysXSuspensionComponent::UpdateJointAxis()
	{
		if (m_RollAxis.Length() != 0)
			m_RollJoint->setGlobalAxis(NxVec3(m_RollAxis.x,m_RollAxis.y,m_RollAxis.z));
		else
		{
			//not thread safe!!! fix this
			ILocationComponent *location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponent<ILocationComponent>().get();
			//ILocationComponent *location2 = GetSceneObject()->GetFirstComponent<ILocationComponent>().get();
			Quaternion rot = location1->GetRotation();
			Mat4 rot_mat;
			rot_mat.Identity();
			rot.ToRotationMatrix(rot_mat);
			Vec3 axis = rot_mat.GetRightVector();
			m_RollJoint->setGlobalAxis(NxVec3(axis.x,axis.y,axis.z));
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
		ILocationComponent *location2 = GetSceneObject()->GetFirstComponent<ILocationComponent>().get();

		//Vec3 pos_b1 = location1->GetPosition();
		Vec3 pos_b2 = location2->GetPosition();
		Vec3  world_anchor;

		if(m_RollJoint)
		{
			world_anchor = m_Anchor + pos_b2;
			m_RollJoint->setGlobalAnchor(NxVec3(world_anchor.x,world_anchor.y,world_anchor.z));
			
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
				return m_RollJoint->getAngle();
			return 0;
		}

		float PhysXSuspensionComponent::GetRollAngleRate()
		{
			if(m_RollJoint)
				return m_RollJoint->getVelocity();
			return 0;
		}

		void PhysXSuspensionComponent::UpdateMotor()
		{
			if(m_RollJoint)
			{
				NxMotorDesc motor;
				motor.velTarget = m_RollAngularVelocity;
				motor.maxForce = m_RollJointForce;
				m_RollJoint->setMotor(motor);
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

	}
