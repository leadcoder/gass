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

#include "TurretComponent.h"
#include "GameMessages.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Scenario/Scene/Messages/SoundSceneObjectMessages.h"


namespace GASS
{
	TurretComponent::TurretComponent()  :m_Controller("Yaw"),m_MaxSteerVelocity(1),m_SteerForce(10),m_MaxSteerAngle(0),m_MinAngle(-1000),m_MaxAngle(1000),m_CurrentAngle(0)
	{

	}

	TurretComponent::~TurretComponent()
	{

	}

	void TurretComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("TurretComponent",new Creator<TurretComponent, IComponent>);
		RegisterProperty<std::string>("RotationController", &TurretComponent::GetController, &TurretComponent::SetController);
		RegisterProperty<float>("MaxSteerVelocity", &TurretComponent::GetMaxSteerVelocity, &TurretComponent::SetMaxSteerVelocity);
		RegisterProperty<float>("MaxSteerAngle", &TurretComponent::GetMaxSteerAngle, &TurretComponent::SetMaxSteerAngle);
		RegisterProperty<float>("SteerForce", &TurretComponent::GetSteerForce, &TurretComponent::SetSteerForce);
		RegisterProperty<Vec2>("MaxMinAngle", &TurretComponent::GetMaxMinAngle, &TurretComponent::SetMaxMinAngle);
	}

	void TurretComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(TurretComponent::OnLoad,LoadGameComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TurretComponent::OnInput,ControllerMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TurretComponent::OnJointUpdate,HingeJointNotifyMessage,0));
	}

	void TurretComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		MessagePtr force_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_FORCE,m_SteerForce));
		MessagePtr vel_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_VELOCITY,0));
		GetSceneObject()->PostMessage(force_msg);
		GetSceneObject()->PostMessage(vel_msg);

		MessagePtr play_msg(new SoundParameterMessage(SoundParameterMessage::PLAY,0));
		GetSceneObject()->PostMessage(play_msg);

		MessagePtr volume_msg(new SoundParameterMessage(SoundParameterMessage::VOLUME,0));
		GetSceneObject()->PostMessage(volume_msg);
	}

	void TurretComponent::OnInput(ControllerMessagePtr message)
	{
		std::string name = message->GetController();
		float value = message->GetValue();
		if (name == m_Controller)
		{
			if(fabs(value) < 0.1) //clamp
				value  = 0;
			//send rotaion message to physics engine

			//if((m_CurrentAngle > m_MinAngle && value > 0) || (m_CurrentAngle < m_MaxAngle && value < 0))
			{
				MessagePtr force_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_FORCE,m_SteerForce));
				MessagePtr vel_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_VELOCITY,m_MaxSteerVelocity*value));

				GetSceneObject()->PostMessage(force_msg);
				GetSceneObject()->PostMessage(vel_msg);

				MessagePtr volume_msg(new SoundParameterMessage(SoundParameterMessage::VOLUME,fabs(value)));
				GetSceneObject()->PostMessage(volume_msg);
			}

			/*if(fabs(value) > 0)
			{
				MessagePtr play_msg(new SoundParameterMessage(SoundParameterMessage::PLAY,0));
				GetSceneObject()->PostMessage(play_msg);
			}
			else
			{
				MessagePtr play_msg(new SoundParameterMessage(SoundParameterMessage::STOP,0));
				GetSceneObject()->PostMessage(play_msg);
			}*/
		}
	}

	void TurretComponent::OnJointUpdate(HingeJointNotifyMessagePtr message)
	{
		m_CurrentAngle = message->GetAngle();
	}

	/*void TurretComponent::OnRotation(VelocityNotifyMessagePtr message)
	{
		Vec3 ang_vel  = message->GetAngularVelocity();
		const float speed = fabs(ang_vel.y);
		const float max_volume_at_speed = 1;
		if(speed < max_volume_at_speed)
		{
			//std::cout << speed << std::endl;
			//Play engine sound
			const float volume = (speed/max_volume_at_speed);
			MessagePtr sound_msg(new SoundParameterMessage(SoundParameterMessage::VOLUME,volume));
			GetSceneObject()->PostMessage(sound_msg);
		}
		if(speed > 0)
		{
			float pitch = 0.8 + speed*0.01;
			MessagePtr sound_msg(new SoundParameterMessage(SoundParameterMessage::PITCH,pitch));
			GetSceneObject()->PostMessage(sound_msg);
		}
	}*/

	
}
