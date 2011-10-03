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

#include "TrackComponent.h"
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
	TrackComponent::TrackComponent() : m_Initialized(false), m_AnimationValue(0,0), m_AnimationSpeedFactor(1,1),m_ParticleEmissionFactor(0.6),m_SoundVolumeFactor(1.0f)
	{
	}

	TrackComponent::~TrackComponent()
	{

	}

	void TrackComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("TrackComponent",new Creator<TrackComponent, IComponent>);
		RegisterProperty<std::string>("DriveWheel", &TrackComponent::GetDriveWheel, &TrackComponent::SetDriveWheel);
		RegisterProperty<Vec2>("AnimationSpeedFactor", &TrackComponent::GetAnimationSpeedFactor, &TrackComponent::SetAnimationSpeedFactor);
		RegisterProperty<float>("ParticleEmissionFactor", &TrackComponent::GetParticleEmissionFactor, &TrackComponent::SetParticleEmissionFactor);
		RegisterProperty<float>("SoundVolumeFactor", &TrackComponent::GetSoundVolumeFactor, &TrackComponent::SetSoundVolumeFactor);
	}

	void TrackComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(TrackComponent::OnLoad,LoadGameComponentsMessage,0));
		//register for physics messages on engine?
	}

	void TrackComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		m_Initialized = true;
		SetDriveWheel(m_DriveWheelName);

		MessagePtr play_msg(new SoundParameterMessage(SoundParameterMessage::PLAY,0));
		GetSceneObject()->PostMessage(play_msg);

		MessagePtr volume_msg(new SoundParameterMessage(SoundParameterMessage::VOLUME,0));
		GetSceneObject()->PostMessage(volume_msg);

		
	}

	std::string TrackComponent::GetDriveWheel() const
	{
		return m_DriveWheelName;
	}

	void TrackComponent::SetDriveWheel(const std::string &wheel)
	{
		m_DriveWheelName = wheel;
		if(m_Initialized)
		{
			SceneObjectPtr obj = GetSceneObject()->GetObjectUnderRoot()->GetFirstChildByName(m_DriveWheelName,false);
			if(obj)
			{
				m_DriveWheel = obj;
				obj->RegisterForMessage(REG_TMESS(TrackComponent::OnDriveWheelPhysicsMessage,VelocityNotifyMessage,0));
			}
		}
	}

	void TrackComponent::OnDriveWheelPhysicsMessage(VelocityNotifyMessagePtr message)
	{
		Vec3 ang_vel  = message->GetAngularVelocity();
		//Vec2 speed(ang_vel.x,0);
		m_AnimationValue.x += (ang_vel.x*m_AnimationSpeedFactor.x);
		m_AnimationValue.y += (ang_vel.x*m_AnimationSpeedFactor.y);

		MessagePtr mesh_msg(new TextureCoordinateMessage(m_AnimationValue));
		GetSceneObject()->PostMessage(mesh_msg);

		float emission = fabs(ang_vel.x)*m_ParticleEmissionFactor;

		if(emission >50)
			emission =50;
		MessagePtr particle_msg(new ParticleSystemParameterMessage(ParticleSystemParameterMessage::EMISSION_RATE,0,emission));
		GetSceneObject()->PostMessage(particle_msg);

		float duration = fabs(ang_vel.x)*0.05;

		if(duration > 1.6)  
			duration = 1.6;

		//MessagePtr particle_duration_msg(new ParticleSystemParameterMessage(ParticleSystemParameterMessage::PARTICLE_LIFE_TIME,0,duration));
		//GetSceneObject()->PostMessage(particle_duration_msg);
		
		//std::cout << "speed:" << speed.x << std::endl;

		const float speed = fabs(ang_vel.x);
		const float max_volume_at_speed = 10;
		if(speed < max_volume_at_speed)
		{
			//std::cout << speed << std::endl;
			//Play engine sound
			const float volume = m_SoundVolumeFactor* (speed/max_volume_at_speed);
			MessagePtr sound_msg(new SoundParameterMessage(SoundParameterMessage::VOLUME,volume));
			GetSceneObject()->PostMessage(sound_msg);
		}

		if(speed > 0)
		{
			float pitch = 0.8 + speed*0.015;
			MessagePtr sound_msg(new SoundParameterMessage(SoundParameterMessage::PITCH,pitch));
			GetSceneObject()->PostMessage(sound_msg);
		}
	}
}
