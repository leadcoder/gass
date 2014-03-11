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

#include "SoundVolumeComponent.h"
#include "GameMessages.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"

#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"

#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Messages/GASSSoundSceneObjectMessages.h"


namespace GASS
{
	SoundVolumeComponent::SoundVolumeComponent() : m_MaxVolumeAtSpeed(0.3)
	{

	}

	SoundVolumeComponent::~SoundVolumeComponent()
	{

	}

	void SoundVolumeComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("SoundVolumeComponent",new Creator<SoundVolumeComponent, IComponent>);
		RegisterProperty<Float>("MaxVolumeAtSpeed", &SoundVolumeComponent::GetMaxVolumeAtSpeed, &SoundVolumeComponent::SetMaxVolumeAtSpeed);
		
	}

	void SoundVolumeComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(SoundVolumeComponent::OnVelocityNotifyMessage,VelocityNotifyMessage,0));
		GetSceneObject()->PostRequest(SoundParameterMessagePtr(new SoundParameterMessage(SoundParameterMessage::PLAY,0)));
		GetSceneObject()->PostRequest(SoundParameterMessagePtr(new SoundParameterMessage(SoundParameterMessage::VOLUME,0)));
	}

	void SoundVolumeComponent::OnHingeUpdated(HingeJointNotifyMessagePtr message)
	{
		Float new_angle = message->GetAngle();
		Float speed =  fabs(m_HingeAngle - new_angle);
		m_HingeAngle = new_angle;
		if(speed < m_MaxVolumeAtSpeed)
		{
			//turret sound
			const float volume = (speed/m_MaxVolumeAtSpeed);
			GetSceneObject()->PostRequest(SoundParameterMessagePtr(new SoundParameterMessage(SoundParameterMessage::VOLUME,volume)));

			/*std::stringstream ss;
			ss << "Speed:"<< speed << " Volume:" << volume << "\n";
			std::string soundinfo = ss.str();
			MessagePtr debug_msg(new DebugPrintRequest(soundinfo));
			SimEngine::Get().GetSimSystemManager()->SendImmediate(debug_msg);
			*/
			//std::cout << speed << std::endl;
		}
	}

	void SoundVolumeComponent::OnVelocityNotifyMessage(VelocityNotifyMessagePtr message)
	{
		Vec3 ang_vel  = message->GetAngularVelocity();
		const float speed = fabs(ang_vel.y + ang_vel.x);
		if(speed < m_MaxVolumeAtSpeed)
		{
			
			//turret sound
			const float volume = (speed/m_MaxVolumeAtSpeed);
			
			GetSceneObject()->PostRequest(SoundParameterMessagePtr(new SoundParameterMessage(SoundParameterMessage::VOLUME,volume*0.5)));

			/*std::stringstream ss;
			ss << "Speed:"<< speed << " Volume:" << volume << "\n";
			std::string soundinfo = ss.str();
			MessagePtr debug_msg(new DebugPrintRequest(soundinfo));
			SimEngine::Get().GetSimSystemManager()->SendImmediate(debug_msg);
			*/
			//std::cout << speed << std::endl;
		}
		/*if(speed > 0)
		{
			float pitch = 0.8 + speed*0.01;
			MessagePtr sound_msg(new SoundParameterMessage(SoundParameterMessage::PITCH,pitch));
			GetSceneObject()->PostMessage(sound_msg);
		}*/
	}

	
}
