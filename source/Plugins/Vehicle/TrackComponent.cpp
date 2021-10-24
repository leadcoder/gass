/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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

#include <memory>
#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSSoundSceneObjectMessages.h"

namespace GASS
{
	TrackComponent::TrackComponent() :  m_AnimationValue(0,0), m_AnimationSpeedFactor(1,1) 
	{
	}

	TrackComponent::~TrackComponent()
	{

	}

	void TrackComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<TrackComponent>();
		RegisterMember("DriveWheel", &TrackComponent::m_DriveWheel);
		RegisterGetSet("AnimationSpeedFactor", &TrackComponent::GetAnimationSpeedFactor, &TrackComponent::SetAnimationSpeedFactor);
		RegisterGetSet("ParticleEmissionFactor", &TrackComponent::GetParticleEmissionFactor, &TrackComponent::SetParticleEmissionFactor);
		RegisterGetSet("SoundVolumeFactor", &TrackComponent::GetSoundVolumeFactor, &TrackComponent::SetSoundVolumeFactor);
	}

	void TrackComponent::OnInitialize()
	{
		BaseSceneComponent::OnInitialize();
		GetSceneObject()->PostRequest(std::make_shared<SoundParameterRequest>(SoundParameterRequest::PLAY,0.0f));
		GetSceneObject()->PostRequest(std::make_shared<SoundParameterRequest>(SoundParameterRequest::VOLUME,0.0f));
		
		if(m_DriveWheel.IsValid())
			m_DriveWheel->RegisterForMessage(REG_TMESS(TrackComponent::OnDriveWheelPhysicsMessage,PhysicsVelocityEvent,0));
		else
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find drive wheel","TrackComponent::OnLoad");
		m_Initialized = true;
	}

	void TrackComponent::OnDriveWheelPhysicsMessage(PhysicsVelocityEventPtr message)
	{
		Vec3 ang_vel  = message->GetAngularVelocity();
		//Vec2 speed(ang_vel.x,0);
		m_AnimationValue.x += (ang_vel.x*m_AnimationSpeedFactor.x);
		m_AnimationValue.y += (ang_vel.x*m_AnimationSpeedFactor.y);

		
		GetSceneObject()->SendImmediateRequest(std::make_shared<TextureCoordinateRequest>(m_AnimationValue));

		float emission = static_cast<float>(fabs(ang_vel.x)*m_ParticleEmissionFactor);

		if(emission >50)
			emission =50;
		GetSceneObject()->SendImmediateRequest(std::make_shared<ParticleSystemParameterRequest>(ParticleSystemParameterRequest::EMISSION_RATE,0,emission));

		//float duration = fabs(static_cast<float>(ang_vel.x))*0.05f;

		//if(duration > 1.6f)  
		//	duration = 1.6f;

		//MessagePtr particle_duration_msg(new ParticleSystemParameterRequest(ParticleSystemParameterRequest::PARTICLE_LIFE_TIME,0,duration));
		//GetSceneObject()->PostMessage(particle_duration_msg);
		
		//std::cout << "speed:" << speed.x << std::endl;

		const float speed = static_cast<float>(fabs(ang_vel.x));
		const float max_volume_at_speed = 10;
		float volume = m_SoundVolumeFactor;
		if(speed < max_volume_at_speed)
		{
			//std::cout << speed << std::endl;
			//Play engine sound
			volume = m_SoundVolumeFactor* (speed/max_volume_at_speed);
		}
		GetSceneObject()->SendImmediateRequest(std::make_shared<SoundParameterRequest>(SoundParameterRequest::VOLUME,volume));

		if(speed > 0)
		{
			float pitch = 0.8f + speed*0.015f;

			if(pitch > 1.7f)
				pitch = 1.7f;
			//std::cout << "pitch:" << pitch << " Speed:" << speed <<"\n";
			GetSceneObject()->SendImmediateRequest(std::make_shared<SoundParameterRequest>(SoundParameterRequest::PITCH,pitch));
		}
	}
}
