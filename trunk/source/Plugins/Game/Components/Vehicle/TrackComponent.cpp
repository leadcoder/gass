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
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Systems/Resource/GASSIResourceSystem.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Scheduling/GASSIRuntimeController.h"
#include "Sim/Systems/Input/GASSControlSettingsManager.h"
#include "Sim/Systems/Input/GASSControlSetting.h"
#include "Sim/Scene/GASSSoundSceneObjectMessages.h"


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
		RegisterProperty<SceneObjectLink>("DriveWheel", &TrackComponent::GetDriveWheel, &TrackComponent::SetDriveWheel);
		RegisterProperty<Vec2>("AnimationSpeedFactor", &TrackComponent::GetAnimationSpeedFactor, &TrackComponent::SetAnimationSpeedFactor);
		RegisterProperty<float>("ParticleEmissionFactor", &TrackComponent::GetParticleEmissionFactor, &TrackComponent::SetParticleEmissionFactor);
		RegisterProperty<float>("SoundVolumeFactor", &TrackComponent::GetSoundVolumeFactor, &TrackComponent::SetSoundVolumeFactor);
	}

	void TrackComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(TrackComponent::OnLoad,LoadComponentsMessage,0));
		BaseSceneComponent::OnInitialize();
	}

	void TrackComponent::OnLoad(LoadComponentsMessagePtr message)
	{
		
		//SetDriveWheel(m_DriveWheelName);

		MessagePtr play_msg(new SoundParameterMessage(SoundParameterMessage::PLAY,0));
		GetSceneObject()->PostMessage(play_msg);

		MessagePtr volume_msg(new SoundParameterMessage(SoundParameterMessage::VOLUME,0));
		GetSceneObject()->PostMessage(volume_msg);
		
		if(m_DriveWheel.IsValid())
			m_DriveWheel->RegisterForMessage(REG_TMESS(TrackComponent::OnDriveWheelPhysicsMessage,VelocityNotifyMessage,0));
		else
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find drive wheel","TrackComponent::OnLoad");
		m_Initialized = true;
		
	}

	void TrackComponent::OnDriveWheelPhysicsMessage(VelocityNotifyMessagePtr message)
	{
		Vec3 ang_vel  = message->GetAngularVelocity();
		//Vec2 speed(ang_vel.x,0);
		m_AnimationValue.x += (ang_vel.x*m_AnimationSpeedFactor.x);
		m_AnimationValue.y += (ang_vel.x*m_AnimationSpeedFactor.y);

		MessagePtr mesh_msg(new TextureCoordinateMessage(m_AnimationValue));
		GetSceneObject()->SendImmediate(mesh_msg);

		float emission = fabs(ang_vel.x)*m_ParticleEmissionFactor;

		if(emission >50)
			emission =50;
		MessagePtr particle_msg(new ParticleSystemParameterMessage(ParticleSystemParameterMessage::EMISSION_RATE,0,emission));
		GetSceneObject()->SendImmediate(particle_msg);

		float duration = fabs(ang_vel.x)*0.05;

		if(duration > 1.6)  
			duration = 1.6;

		//MessagePtr particle_duration_msg(new ParticleSystemParameterMessage(ParticleSystemParameterMessage::PARTICLE_LIFE_TIME,0,duration));
		//GetSceneObject()->PostMessage(particle_duration_msg);
		
		//std::cout << "speed:" << speed.x << std::endl;

		const float speed = fabs(ang_vel.x);
		const float max_volume_at_speed = 10;
		float volume = m_SoundVolumeFactor;
		if(speed < max_volume_at_speed)
		{
			//std::cout << speed << std::endl;
			//Play engine sound
			volume = m_SoundVolumeFactor* (speed/max_volume_at_speed);
		}
		MessagePtr sound_msg(new SoundParameterMessage(SoundParameterMessage::VOLUME,volume));
		GetSceneObject()->SendImmediate(sound_msg);

		if(speed > 0)
		{
			float pitch = 0.8 + speed*0.015;

			if(pitch > 1.7)
				pitch = 1.7;
			//std::cout << "pitch:" << pitch << " Speed:" << speed <<"\n";
			
			MessagePtr sound_msg(new SoundParameterMessage(SoundParameterMessage::PITCH,pitch));
			GetSceneObject()->SendImmediate(sound_msg);
		}
	}
}
