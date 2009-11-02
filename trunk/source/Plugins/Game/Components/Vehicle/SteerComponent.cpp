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

#include "SteerComponent.h"
#include "GameMessages.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/MessageSystem/AnyMessage.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"


namespace GASS
{
	SteerComponent::SteerComponent() : m_SteerForce(100),m_MaxSteerVelocity(1),m_CurrentAngle(0),m_DesiredAngle(0),m_MaxSteerAngle(45)
	{

	}

	SteerComponent::~SteerComponent()
	{

	}

	void SteerComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("SteerComponent",new Creator<SteerComponent, IComponent>);
		RegisterProperty<float>("SteerForce", &GetSteerForce, &SetSteerForce);
		RegisterProperty<float>("MaxSteerVelocity", &GetMaxSteerVelocity, &SetMaxSteerVelocity);
		RegisterProperty<float>("MaxSteerAngle", &GetMaxSteerAngle, &SetMaxSteerAngle);
		
	}

	void SteerComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(OBJECT_RM_LOAD_SIM_COMPONENTS, TYPED_MESSAGE_FUNC(SteerComponent::OnLoad,LoadSimComponentsMessage));
		GetSceneObject()->RegisterForMessage(OBJECT_NM_PHYSICS_HINGE_JOINT, TYPED_MESSAGE_FUNC(SteerComponent::OnJointUpdate,HingeJointNotifyMessage));
	}

	void SteerComponent::OnLoad(LoadSimComponentsMessagePtr message)
	{
		//get input from parent?
		SceneObjectPtr parent = boost::shared_dynamic_cast<SceneObject>(GetSceneObject()->GetParent());
		parent->RegisterForMessage((SceneObjectMessage) OBJECT_NM_PLAYER_INPUT, MESSAGE_FUNC(SteerComponent::OnInput));
	}

	void SteerComponent::OnInput(MessagePtr message)
	{
		AnyMessagePtr any_mess = boost::shared_static_cast<AnyMessage>(message);
		std::string name = boost::any_cast<std::string>(any_mess->GetData("Controller"));
		float value = boost::any_cast<float>(any_mess->GetData("Value"));
		
		if (name == "Steer")
		{
			float max_rad_angle = Math::Deg2Rad(m_MaxSteerAngle);
			m_DesiredAngle = -value*max_rad_angle;
		}
		
		/*float value = boost::any_cast<float>(any_mess->GetData("Value"));
		float angular_vel = value*m_MaxSteerVelocity;
		if (name == "Steer")
		{
			//send rotaion message to physics engine
			MessagePtr force_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_FORCE,m_SteerForce));
			MessagePtr vel_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_VELOCITY,angular_vel));

			GetSceneObject()->PostMessage(force_msg);
			GetSceneObject()->PostMessage(vel_msg);
		}*/
		
	}

	void SteerComponent::OnJointUpdate(HingeJointNotifyMessagePtr message)
	{
		m_CurrentAngle = message->GetAngle();
		float angular_vel = (m_DesiredAngle-m_CurrentAngle)*5;
		//std::cout << " " <<angular_vel << " " <<m_DesiredAngle << " " << m_CurrentAngle << std::endl;
		
		MessagePtr force_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_FORCE,m_SteerForce));
		MessagePtr vel_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_VELOCITY,angular_vel));

		GetSceneObject()->PostMessage(force_msg);
		GetSceneObject()->PostMessage(vel_msg);
		


		/*float angular_vel = value*m_MaxSteerVelocity;
		if (name == "Steer")
		{
			//send rotaion message to physics engine
			MessagePtr force_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_FORCE,m_SteerForce));
			MessagePtr vel_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_VELOCITY,angular_vel));
			GetSceneObject()->PostMessage(force_msg);
			GetSceneObject()->PostMessage(vel_msg);
		}*/
	}

	
}
