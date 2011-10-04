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
#include "Plugins/Havok/HavokSuspensionComponent.h"
#include "Plugins/Havok/HavokBodyComponent.h"
#include "Plugins/Havok/HavokPhysicsSceneManager.h"
//#include "Plugins/Havok/HavokCollisionSystem.h"
//#include "Plugins/Havok/HavokSphereGeometryComponent.h"
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
#include <Physics/Dynamics/Constraint/Bilateral/Wheel/hkpWheelConstraintData.h>
#include <Physics/Utilities/Actions/Motor/hkpMotorAction.h>

namespace GASS
{
	HavokSuspensionComponent::HavokSuspensionComponent() : m_ChassisBody (NULL),
		m_WheelBody (NULL),
		m_JointForce (0),
		m_SwayForce  (0),
		m_Strength(1),
		m_Damping(2),
		m_Anchor (0,0,0),
		m_Axis1 (0,0,0),
		m_Axis2 (0,0,0),
		m_WheelConstraintData (0),
		m_HighStop(0),
		m_LowStop(0)
	{
	}

	HavokSuspensionComponent::~HavokSuspensionComponent()
	{

	}

	void HavokSuspensionComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsSuspensionComponent",new Creator<HavokSuspensionComponent, IComponent>);

		RegisterProperty<std::string>("Body1Name", &GASS::HavokSuspensionComponent::GetBody1Name, &GASS::HavokSuspensionComponent::SetBody1Name);
		RegisterProperty<std::string>("Body2Name", &GASS::HavokSuspensionComponent::GetBody2Name, &GASS::HavokSuspensionComponent::SetBody2Name);
		RegisterProperty<float>("Axis1Force", &GASS::HavokSuspensionComponent::GetAxis1Force, &GASS::HavokSuspensionComponent::SetAxis1Force);
		RegisterProperty<float>("Axis2Force", &GASS::HavokSuspensionComponent::GetAxis2Force, &GASS::HavokSuspensionComponent::SetAxis2Force);
		RegisterProperty<float>("Damping", &GASS::HavokSuspensionComponent::GetDamping, &GASS::HavokSuspensionComponent::SetDamping);
		RegisterProperty<float>("Strength", &GASS::HavokSuspensionComponent::GetStrength, &GASS::HavokSuspensionComponent::SetStrength);
		RegisterProperty<float>("HighStop", &GASS::HavokSuspensionComponent::GetHighStop, &GASS::HavokSuspensionComponent::SetHighStop);
		RegisterProperty<float>("LowStop", &GASS::HavokSuspensionComponent::GetLowStop, &GASS::HavokSuspensionComponent::SetLowStop);
		RegisterProperty<float>("SwayForce", &GASS::HavokSuspensionComponent::GetSwayForce, &GASS::HavokSuspensionComponent::SetSwayForce);
		RegisterProperty<Vec3>("Axis1", &GASS::HavokSuspensionComponent::GetAxis1, &GASS::HavokSuspensionComponent::SetAxis1);
		RegisterProperty<Vec3>("Axis2", &GASS::HavokSuspensionComponent::GetAxis2, &GASS::HavokSuspensionComponent::SetAxis2);
		RegisterProperty<Vec3>("Anchor", &GASS::HavokSuspensionComponent::GetAnchor, &GASS::HavokSuspensionComponent::SetAnchor);
	}

	void HavokSuspensionComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokSuspensionComponent::OnLoad,LoadPhysicsComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokSuspensionComponent::OnParameterMessage,PhysicsJointMessage,0));
	}


	void HavokSuspensionComponent::OnParameterMessage(PhysicsJointMessagePtr message)
	{
		PhysicsJointMessage::PhysicsJointParameterType type = message->GetParameter();

		float value = message->GetValue();

		//wake body!!

		//m_Body1->Wake();
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

	void HavokSuspensionComponent::OnLoad(LoadPhysicsComponentsMessagePtr message)
	{
		HavokPhysicsSceneManagerPtr scene_manager = boost::shared_static_cast<HavokPhysicsSceneManager> (message->GetPhysicsSceneManager());
		assert(scene_manager);
		m_SceneManager = scene_manager;

		CreateJoint();
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokSuspensionComponent::SendJointUpdate,VelocityNotifyMessage,0));
	}

	void HavokSuspensionComponent::CreateJoint()
	{
		HavokPhysicsSceneManagerPtr scene_manager = HavokPhysicsSceneManagerPtr(m_SceneManager);
		hkpWorld* world = scene_manager->GetWorld();

		m_ChassisBody = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<HavokBodyComponent>().get();
		if(m_ChassisBody)
		{
			m_WheelBody = GetSceneObject()->GetFirstComponentByClass<HavokBodyComponent>().get();
			assert(m_WheelBody);

			
			m_WheelConstraintData = new hkpWheelConstraintData();

			hkVector4 suspension(0.0f, 1.0f, 0.0f);
			suspension.normalize3();
			hkVector4 steering(0.0f, 1.0f, 0.0f);

			hkVector4 axle(1.0f, 0.0f, 0.0f);

			hkTransform wheel_trans = m_WheelBody->GetHavokBody()->getTransform();
			hkTransform chassis_trans = m_ChassisBody->GetHavokBody()->getTransform();
			hkVector4 wheel_pos = m_WheelBody->GetHavokBody()->getPosition();

			m_WheelConstraintData->setInWorldSpace(wheel_trans , chassis_trans, 
											wheel_pos , axle, suspension, steering );
			m_WheelConstraintData->setSuspensionMaxLimit(0.2f);
			m_WheelConstraintData->setSuspensionMinLimit(-0.5f);
		
			m_WheelConstraintData->setSuspensionStrength(0.01f);
			m_WheelConstraintData->setSuspensionDamping(world->m_dynamicsStepInfo.m_solverInfo.m_damping * 0.25f);

			hkpConstraintInstance* constraint = new hkpConstraintInstance( m_WheelBody->GetHavokBody(), m_ChassisBody->GetHavokBody(), m_WheelConstraintData );
			

			hkVector4 drive_axis(1.0f, 0.0f, 0.0f);
			m_MotorAction = new hkpMotorAction( m_WheelBody->GetHavokBody(), drive_axis, 0, 2);
			
			
			world->lock();
			world->addConstraint(constraint);
			world->addAction( m_MotorAction );
			constraint->removeReference();
			world->unlock();
	
			/*UpdateAnchor();
			UpdateJointAxis();
			UpdateLimits();
			SetAxis1Force(m_JointForce);
			SetAxis2Force(m_JointForce);
			SetAxis1Vel(0);
			SetAxis2Vel(0);
			UpdateSuspension();*/
		}
	}

	void HavokSuspensionComponent::SetAxis1(const Vec3 &axis)
	{
		m_Axis1 = axis;
		if(m_WheelConstraintData)
			UpdateJointAxis();
	}

	void HavokSuspensionComponent::SetAxis2(const Vec3 &axis)
	{
		m_Axis2 = axis;
		if(m_WheelConstraintData)
			UpdateJointAxis();
	}

	void HavokSuspensionComponent::UpdateJointAxis()
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

	void HavokSuspensionComponent::SetAnchor(const Vec3 &value)
	{
		m_Anchor = value;
		if(m_WheelConstraintData)
			UpdateAnchor();
	}

	void HavokSuspensionComponent::UpdateAnchor()
	{
		LocationComponentPtr location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		LocationComponentPtr location2 = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();

		Vec3 pos_b1 = location1->GetPosition();
		Vec3 pos_b2 = location2->GetPosition();

		Vec3  world_anchor;

	
		world_anchor = m_Anchor + pos_b2;
		///dJointSetHinge2Anchor(m_WheelConstraintData,world_anchor.x,world_anchor.y,world_anchor.z);
			
	}

	void HavokSuspensionComponent::SetLowStop(float value)
	{
		m_LowStop = value;
		UpdateLimits();
	}

	void HavokSuspensionComponent::SetHighStop(float value)
	{
		m_HighStop = value;
		UpdateLimits();
	}


	void HavokSuspensionComponent::UpdateLimits()
	{
		if(m_WheelConstraintData)
		{
		/*	dJointSetHinge2Param(m_WheelConstraintData,dParamLoStop,m_LowStop);
			dJointSetHinge2Param(m_WheelConstraintData,dParamHiStop,m_HighStop);
			dJointSetHinge2Param(m_WheelConstraintData,dParamLoStop,m_LowStop);
			dJointSetHinge2Param(m_WheelConstraintData,dParamHiStop,m_HighStop);*/
		}
	}

	void HavokSuspensionComponent::SetAxis1Vel(float velocity)
	{
		if(m_WheelBody)
		{
			
			//m_WheelBody->SetAngularVelocity(Vec3(velocity,0,0));
	//		dJointSetHinge2Param(m_WheelConstraintData,dParamVel,velocity);
		}
	}

	void HavokSuspensionComponent::SetAxis2Vel(float value)
	{
		if(m_WheelConstraintData)
		{
			m_MotorAction->setSpinRate(value);
		//	dJointSetHinge2Param(m_WheelConstraintData, dParamVel2,value);
		}
	}

	void HavokSuspensionComponent::SetAxis2Force(float value)
	{
		if(m_WheelBody)
		{
			m_MotorAction->setGain(value*10);
//			m_WheelBody->AddTorque(Vec3(value,0,0));
		//	dJointSetHinge2Param(m_WheelConstraintData, dParamFMax2,value);
		}
	}


	float HavokSuspensionComponent::GetAxis2Force() const
	{
		if(m_WheelConstraintData)
		{
			return m_JointForce;//dJointGetHinge2Param(m_WheelConstraintData,dParamFMax2);
		}
		return 0;
	}

	void HavokSuspensionComponent::SetAxis1Force(float value)
	{
		m_JointForce = value;
		if(m_WheelConstraintData)
		{
			//dJointSetHinge2Param(m_WheelConstraintData, dParamFMax,value);
		}
	}


	void HavokSuspensionComponent::SetDamping(float value)
	{
		m_Damping= value;
		UpdateSuspension();
	}

	void HavokSuspensionComponent::SetStrength(float value)
	{
		m_Strength = value;
		UpdateSuspension();
	}

	void HavokSuspensionComponent::UpdateSuspension()
	{
		if(m_WheelConstraintData)
		{
			float s = 1.0f;
			float suspensionERP = s*m_Strength / (s*m_Strength + m_Damping);
			float suspensionCFM = 1.0f / (s*m_Strength + m_Damping);
			//dJointSetHinge2Param (m_WheelConstraintData,dParamSuspensionERP, suspensionERP);
			//dJointSetHinge2Param (m_WheelConstraintData,dParamSuspensionCFM, suspensionCFM);
		}
	}

	void HavokSuspensionComponent::SetSwayForce(float value)
	{
		m_SwayForce = value;
	}


	void HavokSuspensionComponent::SendJointUpdate(VelocityNotifyMessagePtr message)
	{
		MessagePtr joint_message;
		if(m_WheelBody)
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






