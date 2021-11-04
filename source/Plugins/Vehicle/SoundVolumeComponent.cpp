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

#include <memory>
#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSSoundSceneObjectMessages.h"
#include "Sim/Interface/GASSISoundComponent.h"

namespace GASS
{
	SoundVolumeComponent::SoundVolumeComponent()  
		
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

		m_Sound = GetSceneObject()->GetFirstComponentByClass<ISoundComponent>().get();
		if (m_Sound)
		{
			m_Sound->SetVolume(0);
			m_Sound->SetPlay(true);
		}
	}

	void SoundVolumeComponent::OnHingeUpdated(ODEPhysicsHingeJointEventPtr message)
	{
		Float new_angle = message->GetAngle();
		Float speed =  fabs(m_HingeAngle - new_angle);
		m_HingeAngle = new_angle;
		if(speed < m_MaxVolumeAtSpeed)
		{
			//turret sound
			const auto volume = static_cast<float>((speed/m_MaxVolumeAtSpeed));
			if (m_Sound)
			{
				m_Sound->SetVolume(volume);
			}
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
			const auto volume = static_cast<float>(speed/m_MaxVolumeAtSpeed);
			
			if (m_Sound)
			{
				m_Sound->SetVolume(volume);
			}
		}
		/*if(speed > 0)
		{
			float pitch = 0.8 + speed*0.01;
			MessagePtr sound_msg(new SoundParameterRequest(SoundParameterRequest::PITCH,pitch));
			GetSceneObject()->PostMessage(sound_msg);
		}*/
	}

	
}
