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

#include "SoundVolumeComponent.h"
#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSSoundSceneObjectMessages.h"

namespace GASS
{
	SoundVolumeComponent::SoundVolumeComponent() : m_MaxVolumeAtSpeed(0.3), 
		m_HingeAngle(0)
	{

	}

	SoundVolumeComponent::~SoundVolumeComponent()
	{

	}

	void SoundVolumeComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<SoundVolumeComponent>();
		RegisterMember("MaxVolumeAtSpeed", &SoundVolumeComponent::m_MaxVolumeAtSpeed);
		
	}

	void SoundVolumeComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(SoundVolumeComponent::OnVelocityNotifyMessage,PhysicsVelocityEvent,0));
		GetSceneObject()->PostRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::PLAY,0)));
		GetSceneObject()->PostRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::VOLUME,0)));
	}

	void SoundVolumeComponent::OnHingeUpdated(ODEPhysicsHingeJointEventPtr message)
	{
		Float new_angle = message->GetAngle();
		Float speed =  fabs(m_HingeAngle - new_angle);
		m_HingeAngle = new_angle;
		if(speed < m_MaxVolumeAtSpeed)
		{
			//turret sound
			const float volume = static_cast<float>((speed/m_MaxVolumeAtSpeed));
			GetSceneObject()->PostRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::VOLUME,volume)));
			/*
			GASS_PRINT("Speed:"<< speed << " Volume:" << volume)
			*/
		}
	}

	void SoundVolumeComponent::OnVelocityNotifyMessage(PhysicsVelocityEventPtr message)
	{
		Vec3 ang_vel  = message->GetAngularVelocity();
		const float speed = fabs(static_cast<float>(ang_vel.y + ang_vel.x));
		if(speed < m_MaxVolumeAtSpeed)
		{
			
			//turret sound
			const float volume = static_cast<float>(speed/m_MaxVolumeAtSpeed);
			
			GetSceneObject()->PostRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::VOLUME,volume*0.5f)));

			/*
			GASS_PRINT("Speed:"<< speed << " Volume:" << volume)
			*/
		}
		/*if(speed > 0)
		{
			float pitch = 0.8 + speed*0.01;
			MessagePtr sound_msg(new SoundParameterRequest(SoundParameterRequest::PITCH,pitch));
			GetSceneObject()->PostMessage(sound_msg);
		}*/
	}

	
}
