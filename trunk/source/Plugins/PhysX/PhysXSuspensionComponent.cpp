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
#include "Plugins/PhysX/PhysXBody.h"
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
		m_JointForce1 (0),
		m_JointForce2 (0),
		m_SwayForce  (0),
		m_Strength(1),
		m_Damping(2),
		m_Anchor (0,0,0),
		m_Axis1 (0,0,0),
		m_Axis2 (0,0,0),
		m_PhysXSuspensionComponent (0),
		m_HighStop(0),
		m_LowStop(0),
		m_AxisActor(NULL)
	{

	}

	PhysXSuspensionComponent::~PhysXSuspensionComponent()
	{

	}

	void PhysXSuspensionComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysXSuspensionComponent",new Creator<PhysXSuspensionComponent, IComponent>);

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
			{
				SetAxis1Vel(value);
			}
			break;
		case PhysicsJointMessage::AXIS2_VELOCITY:
			{
				SetAxis2Vel(value);
			}
			break;
		case PhysicsJointMessage::AXIS1_FORCE:
			{
				SetAxis1Force(value);

			}
			break;
		case PhysicsJointMessage::AXIS2_FORCE:
			{
				SetAxis2Force(value);
			}
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
		PhysXBodyPtr body1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponent<PhysXBody>();
		PhysXBodyPtr body2 = GetSceneObject()->GetFirstComponent<PhysXBody>();

		NxActor* a1 = body1->GetNxActor();
		NxActor* a2 = body2->GetNxActor();


		//ignore collision for all parent bodies 
		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetParentSceneObject()->GetComponentsByClass(components,"PhysXBody");

		for(int i = 0; i < components.size(); i++)
		{
			PhysXBodyPtr body = boost::shared_static_cast<PhysXBody>(components[i]);
			if(body->GetNxActor() && body->GetNxActor() != a2)
				m_SceneManager->GetNxScene()->setActorPairFlags(*body->GetNxActor(), *a2, NX_IGNORE_PAIR);
		}

		//m_PhysXSuspensionComponent = dJointCreateHinge(world,0);
		/*	NxRevoluteJointDesc revoluteDesc;
		revoluteDesc.setToDefault();
		revoluteDesc.actor[0] = a1;
		revoluteDesc.actor[1] = a2;
		revoluteDesc.setGlobalAxis(NxVec3(1,0,0));
		m_PhysXSuspensionComponent =	m_SceneManager->GetNxScene()->createJoint(revoluteDesc);
		*/

		ILocationComponent *location2 = GetSceneObject()->GetFirstComponent<ILocationComponent>().get();
		Vec3 pos_b1 = location2->GetPosition();


		NxVec3 pos(pos_b1.x,pos_b1.y,pos_b1.z);
		NxActorDesc actorDesc;
		NxBodyDesc bodyDesc;
		actorDesc.setToDefault();
		bodyDesc.setToDefault();

		bodyDesc.mass = 0.1;
		bodyDesc.massSpaceInertia = NxVec3(0.1,0.1,0.1);
		actorDesc.body			= &bodyDesc;
		actorDesc.shapes.clear();
		actorDesc.globalPose.t  = pos;
		NxActor * rollAxis = m_SceneManager->GetNxScene()->createActor(actorDesc);

		m_AxisActor = rollAxis;

		// revolution joint connecting wheel with rollAxis
		NxRevoluteJointDesc revJointDesc;
		revJointDesc.setToDefault();
		revJointDesc.projectionMode = NX_JPM_POINT_MINDIST;
		revJointDesc.actor[0] = rollAxis;
		revJointDesc.actor[1] = a2;
		revJointDesc.setGlobalAnchor(pos);
		revJointDesc.setGlobalAxis(NxVec3(1,0,0));
		NxJoint * joint = m_SceneManager->GetNxScene()->createJoint(revJointDesc);
		NxRevoluteJoint *rollJoint = static_cast<NxRevoluteJoint*>(joint);
		m_PhysXSuspensionComponent = joint;


		NxPrismaticJointDesc prisJointDesc;
		prisJointDesc.setToDefault();
		prisJointDesc.actor[0] = a1;
		prisJointDesc.actor[1] = rollAxis;
		prisJointDesc.setGlobalAnchor(pos);
		prisJointDesc.setGlobalAxis(NxVec3(0,1,0));
		joint = m_SceneManager->GetNxScene()->createJoint(prisJointDesc);

		//add springs and dampers to the suspension (i.e. the related actors)
		float springLength = 0.2f;
		NxSpringAndDamperEffector * springNdamp = m_SceneManager->GetNxScene()->createSpringAndDamperEffector(NxSpringAndDamperEffectorDesc());

		springNdamp->setBodies(a1, pos, rollAxis, pos + NxVec3(0,springLength,0));
		springNdamp->setLinearSpring(0, springLength, 2*springLength, 10, 10);
		springNdamp->setLinearDamper(-1,1, 10, 10);


		UpdateAnchor();
		UpdateJointAxis();
//		UpdateLimits();
		SetAxis1Force(m_JointForce1);

	}

	void PhysXSuspensionComponent::SetAxis1(const Vec3 &axis)
	{
		m_Axis1 = axis;
		if(m_PhysXSuspensionComponent)
			UpdateJointAxis();
	}

	void PhysXSuspensionComponent::SetAxis2(const Vec3 &axis)
	{
		m_Axis2 = axis;
		if(m_PhysXSuspensionComponent)
			UpdateJointAxis();
	}

	void PhysXSuspensionComponent::SetPosition(const Vec3 &value)
	{
		if(m_AxisActor)
		{
			m_AxisActor->setGlobalPosition(NxVec3(value.x, value.y, value.z));
		}
	}


	void PhysXSuspensionComponent::UpdateJointAxis()
	{
		//not thread safe!!! fix this
		ILocationComponent *location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponent<ILocationComponent>().get();
		ILocationComponent *location2 = GetSceneObject()->GetFirstComponent<ILocationComponent>().get();

		Quaternion rot = location1->GetRotation();

		//dReal ode_rot_mat[12];
		Mat4 rot_mat;
		rot_mat.Identity();
		rot.ToRotationMatrix(rot_mat);
		//ODEPhysicsSceneManager::CreateODERotationMatrix(rot_mat,ode_rot_mat);

		Vec3 pos_b1 = location1->GetPosition();
		Vec3 pos_b2 = location2->GetPosition();

		//Vec3 pos_b1(p1[0],p1[1],p1[2]);
		//Vec3 pos_b2(p2[0],p2[1],p2[2]);

		NxRevoluteJoint* joint = static_cast<NxRevoluteJoint*>(m_PhysXSuspensionComponent);
		if (m_Axis1.Length() != 0)
			joint->setGlobalAxis(NxVec3(m_Axis1.x,m_Axis1.y,m_Axis1.z));
		else
		{
			Vec3 axis = rot_mat.GetRightVector();
			joint->setGlobalAxis(NxVec3(axis.x,axis.y,axis.z));

		}
	}

	void PhysXSuspensionComponent::SetAnchor(const Vec3 &value)
	{
		m_Anchor = value;
		if(m_PhysXSuspensionComponent)
			UpdateAnchor();
	}

	void PhysXSuspensionComponent::UpdateAnchor()
	{
		ILocationComponent *location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponent<ILocationComponent>().get();
		ILocationComponent *location2 = GetSceneObject()->GetFirstComponent<ILocationComponent>().get();

		Vec3 pos_b1 = location1->GetPosition();
		Vec3 pos_b2 = location2->GetPosition();

		Vec3  world_anchor;

		if(m_PhysXSuspensionComponent)
		{
			{
				world_anchor = m_Anchor + pos_b2;
				NxRevoluteJoint* joint = static_cast<NxRevoluteJoint*>(m_PhysXSuspensionComponent);
				joint->setGlobalAnchor(NxVec3(world_anchor.x,world_anchor.y,world_anchor.z));
			}
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
			if(m_PhysXSuspensionComponent)
			{
				NxRevoluteJoint* joint = static_cast<NxRevoluteJoint*>(m_PhysXSuspensionComponent);
				NxJointLimitPairDesc limits;
				limits.high.setToDefault();
				limits.high.value = m_HighStop;
				limits.low.setToDefault();
				limits.low.value = m_LowStop;
				joint->setLimits(limits);
				
				
			}
		}
*/
		float PhysXSuspensionComponent::GetAngle()
		{
			NxRevoluteJoint* joint = static_cast<NxRevoluteJoint*>(m_PhysXSuspensionComponent);
			return joint->getAngle();
		}

		float PhysXSuspensionComponent::GetAngleRate()
		{
			NxRevoluteJoint* joint = static_cast<NxRevoluteJoint*>(m_PhysXSuspensionComponent);
			return joint->getVelocity();
		}

		void PhysXSuspensionComponent::UpdateMotor()
		{
			if(m_PhysXSuspensionComponent)
			{
				NxRevoluteJoint* joint = static_cast<NxRevoluteJoint*>(m_PhysXSuspensionComponent);
				NxMotorDesc motor;
				motor.velTarget = m_AngularVelocity2;
				motor.maxForce = m_JointForce2;
				joint->setMotor(motor);
			}
		}

		void PhysXSuspensionComponent::SetAxis1Vel(float velocity)
		{
			m_AngularVelocity1  = velocity;
			UpdateMotor();

		}

		void PhysXSuspensionComponent::SetAxis2Vel(float value)
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

		}

	}
