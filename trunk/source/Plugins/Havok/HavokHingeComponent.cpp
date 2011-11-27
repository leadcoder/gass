/****************************************************************************
* This file is part of GASS.                                                *
* See http://cHavok.google.com/p/gass/                                 *
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

#include "Sim/Common.h"
#include "Plugins/Havok/HavokHingeComponent.h"
#include "Plugins/Havok/HavokBodyComponent.h"
#include "Plugins/Havok/HavokPhysicsSceneManager.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/ComponentSystem/IComponentContainer.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include <boost/bind.hpp>
#include <Physics/Dynamics/Constraint/Bilateral/Hinge/hkpHingeConstraintData.h>
#include <Physics/Utilities/Actions/Motor/hkpMotorAction.h>

//#include <Physics/Utilities/Actions/Motor/hkpMotorAction.h>

namespace GASS
{
	HavokHingeComponent::HavokHingeComponent() : m_Body1 (NULL),
		m_Body2 (NULL),
		m_JointForce (0),
		m_Anchor (0,0,0),
		m_Axis (0,0,0),
		m_HingeConstraintData (0),
		m_MotorAction(NULL),
		m_HighStop(0),
		m_LowStop(0)
	{
	}

	HavokHingeComponent::~HavokHingeComponent()
	{

	}

	void HavokHingeComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsHingeComponent",new Creator<HavokHingeComponent, IComponent>);

		RegisterProperty<std::string>("Body1Name", &GASS::HavokHingeComponent::GetBody1Name, &GASS::HavokHingeComponent::SetBody1Name);
		RegisterProperty<std::string>("Body2Name", &GASS::HavokHingeComponent::GetBody2Name, &GASS::HavokHingeComponent::SetBody2Name);
		RegisterProperty<Vec3>("Axis", &GASS::HavokHingeComponent::GetAxis, &GASS::HavokHingeComponent::SetAxis);
		RegisterProperty<float>("AxisForce", &GASS::HavokHingeComponent::GetAxisForce, &GASS::HavokHingeComponent::SetAxisForce);
/*		RegisterProperty<float>("HighStop", &GASS::HavokHingeComponent::GetHighStop, &GASS::HavokHingeComponent::SetHighStop);
		RegisterProperty<float>("LowStop", &GASS::HavokHingeComponent::GetLowStop, &GASS::HavokHingeComponent::SetLowStop);
		RegisterProperty<Vec3>("Axis", &GASS::HavokHingeComponent::GetAxis, &GASS::HavokHingeComponent::SetAxis);
		RegisterProperty<Vec3>("Anchor", &GASS::HavokHingeComponent::GetAnchor, &GASS::HavokHingeComponent::SetAnchor);
	*/}

	void HavokHingeComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokHingeComponent::OnLoad,LoadPhysicsComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokHingeComponent::OnParameterMessage,PhysicsJointMessage,0));
	}


	void HavokHingeComponent::OnParameterMessage(PhysicsJointMessagePtr message)
	{
		PhysicsJointMessage::PhysicsJointParameterType type = message->GetParameter();

		float value = message->GetValue();

		//wake body!!
		if(m_Body1)
		{
			switch(type)
			{
			case PhysicsJointMessage::AXIS1_VELOCITY:
				{
					SetAxisVel(value);
				}
				break;
			case PhysicsJointMessage::AXIS1_FORCE:
				{
					SetAxisForce(value);
				}
				break;
			}
		}
	}

	void HavokHingeComponent::OnLoad(LoadPhysicsComponentsMessagePtr message)
	{
		HavokPhysicsSceneManagerPtr scene_manager = boost::shared_static_cast<HavokPhysicsSceneManager> (message->GetPhysicsSceneManager());
		assert(scene_manager);
		m_SceneManager = scene_manager;

		CreateJoint();
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokHingeComponent::SendJointUpdate,VelocityNotifyMessage,0));
	}

	void HavokHingeComponent::CreateJoint()
	{
	
		HavokPhysicsSceneManagerPtr scene_manager = HavokPhysicsSceneManagerPtr(m_SceneManager);
		hkpWorld* world = scene_manager->GetWorld();

		LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		Vec3  world_anchor  = location->GetWorldPosition();

		m_Body1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<HavokBodyComponent>().get();
		m_Body2 = GetSceneObject()->GetFirstComponentByClass<HavokBodyComponent>().get();
			
		m_HingeConstraintData = new hkpHingeConstraintData();

		hkVector4 pivot = m_Body2->GetHavokBody()->getPosition();//(world_anchor.x,world_anchor.y,world_anchor.z);		
		hkVector4 axis(m_Axis.x, m_Axis.y, m_Axis.z);

		// Create constraint
		m_HingeConstraintData->setInWorldSpace(m_Body2->GetHavokBody()->getTransform(), m_Body1->GetHavokBody()->getTransform(), pivot, axis);		
		hkpConstraintInstance* constraint = new hkpConstraintInstance(m_Body2->GetHavokBody(), m_Body1->GetHavokBody(), m_HingeConstraintData);


		m_MotorAction = new hkpMotorAction(m_Body2->GetHavokBody(), axis, 0, 2);

		world->lock();
		world->addConstraint(constraint);
		world->addAction( m_MotorAction );
		constraint->removeReference();
		world->unlock();
	
		
	}

	void HavokHingeComponent::SetAxis(const Vec3 &axis)
	{
		m_Axis = axis;
		if(m_HingeConstraintData)
			UpdateJointAxis();
	}

	void HavokHingeComponent::UpdateJointAxis()
	{
		LocationComponentPtr location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		//LocationComponentPtr location2 = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();

		Quaternion rot = location1->GetRotation();

		/*if (m_Axis1.Length() != 0)
			dJointSetHinge2Axis1(m_WheelConstraintData,m_Axis1.x,m_Axis1.y,m_Axis1.z);
		else
			//dJointSetHinge2Axis1(m_WheelConstraintData,Havok_rot_mat[4],Havok_rot_mat[6],Havok_rot_mat[5]);
			dJointSetHinge2Axis1(m_WheelConstraintData,Havok_rot_mat[4],Havok_rot_mat[5],Havok_rot_mat[6]);
		dJointSetHinge2Axis2(m_WheelConstraintData,    Havok_rot_mat[0],    Havok_rot_mat[1],    Havok_rot_mat[2]);*/
	}

	void HavokHingeComponent::SetAnchor(const Vec3 &value)
	{
		m_Anchor = value;
		if(m_HingeConstraintData)
			UpdateAnchor();
	}

	void HavokHingeComponent::UpdateAnchor()
	{
		LocationComponentPtr location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		LocationComponentPtr location2 = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();

		Vec3 pos_b1 = location1->GetPosition();
		Vec3 pos_b2 = location2->GetPosition();

		Vec3  world_anchor;

	
		world_anchor = m_Anchor + pos_b2;
		///dJointSetHinge2Anchor(m_WheelConstraintData,world_anchor.x,world_anchor.y,world_anchor.z);
			
	}

	void HavokHingeComponent::SetLowStop(float value)
	{
		m_LowStop = value;
		UpdateLimits();
	}

	void HavokHingeComponent::SetHighStop(float value)
	{
		m_HighStop = value;
		UpdateLimits();
	}

	void HavokHingeComponent::UpdateLimits()
	{
		if(m_HingeConstraintData)
		{
		/*	dJointSetHinge2Param(m_WheelConstraintData,dParamLoStop,m_LowStop);
			dJointSetHinge2Param(m_WheelConstraintData,dParamHiStop,m_HighStop);
			dJointSetHinge2Param(m_WheelConstraintData,dParamLoStop,m_LowStop);
			dJointSetHinge2Param(m_WheelConstraintData,dParamHiStop,m_HighStop);*/
		}
	}

	void HavokHingeComponent::SetAxisVel(float velocity)
	{
		if(m_MotorAction)
		{
			m_MotorAction->setSpinRate(velocity);
			//m_WheelBody->SetAngularVelocity(Vec3(velocity,0,0));
	//		dJointSetHinge2Param(m_WheelConstraintData,dParamVel,velocity);
		}
	}


	float HavokHingeComponent::GetAxisForce() const
	{
		if(m_HingeConstraintData)
		{
			return m_JointForce;//dJointGetHinge2Param(m_WheelConstraintData,dParamFMax2);
		}
		return 0;
	}

	void HavokHingeComponent::SetAxisForce(float value)
	{
		m_JointForce = value;
		if(m_MotorAction)
		{
			m_MotorAction->setGain(value*10);
			//dJointSetHinge2Param(m_WheelConstraintData, dParamFMax,value);
		}
	}



	void HavokHingeComponent::SendJointUpdate(VelocityNotifyMessagePtr message)
	{
		MessagePtr joint_message;
		if(m_Body1)
		{
			//Vec3 vel = m_WheelBody->GetAngularVelocity();
			//float angle = dJointGetHinge2Angle1(m_WheelConstraintData);
			//float angle_rate = dJointGetHinge2Angle1Rate (m_WheelConstraintData);
			float angle = 0;
			float angle_rate = 0;
			joint_message = HingeJointNotifyMessagePtr(new HingeJointNotifyMessage(angle,angle_rate));
		//	if(joint_message)
		//		GetSceneObject()->PostMessage(joint_message);
		}
	}

	

	

}






