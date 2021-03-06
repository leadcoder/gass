/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"

#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"

#include "Sim/Messages/GASSSoundSceneObjectMessages.h"


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
		ComponentFactory::GetPtr()->Register("TrackComponent",new Creator<TrackComponent, Component>);
		RegisterProperty<SceneObjectRef>("DriveWheel", &TrackComponent::GetDriveWheel, &TrackComponent::SetDriveWheel);
		RegisterProperty<Vec2>("AnimationSpeedFactor", &TrackComponent::GetAnimationSpeedFactor, &TrackComponent::SetAnimationSpeedFactor);
		RegisterProperty<float>("ParticleEmissionFactor", &TrackComponent::GetParticleEmissionFactor, &TrackComponent::SetParticleEmissionFactor);
		RegisterProperty<float>("SoundVolumeFactor", &TrackComponent::GetSoundVolumeFactor, &TrackComponent::SetSoundVolumeFactor);
	}

	void TrackComponent::OnInitialize()
	{
		BaseSceneComponent::OnInitialize();
		GetSceneObject()->PostRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::PLAY,0)));
		GetSceneObject()->PostRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::VOLUME,0)));
		
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

		
		GetSceneObject()->SendImmediateRequest(TextureCoordinateRequestPtr(new TextureCoordinateRequest(m_AnimationValue)));

		float emission = fabs(ang_vel.x)*m_ParticleEmissionFactor;

		if(emission >50)
			emission =50;
		GetSceneObject()->SendImmediateRequest(ParticleSystemParameterRequestPtr(new ParticleSystemParameterRequest(ParticleSystemParameterRequest::EMISSION_RATE,0,emission)));

		float duration = fabs(ang_vel.x)*0.05;

		if(duration > 1.6)  
			duration = 1.6;

		//MessagePtr particle_duration_msg(new ParticleSystemParameterRequest(ParticleSystemParameterRequest::PARTICLE_LIFE_TIME,0,duration));
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
		GetSceneObject()->SendImmediateRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::VOLUME,volume)));

		if(speed > 0)
		{
			float pitch = 0.8 + speed*0.015;

			if(pitch > 1.7)
				pitch = 1.7;
			//std::cout << "pitch:" << pitch << " Speed:" << speed <<"\n";
			GetSceneObject()->SendImmediateRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::PITCH,pitch)));
		}
	}
}
