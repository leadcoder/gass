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
	SoundVolumeComponent::SoundVolumeComponent()  
	{

	}

	SoundVolumeComponent::~SoundVolumeComponent()
	{

	}

	void SoundVolumeComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("SoundVolumeComponent",new Creator<SoundVolumeComponent, IComponent>);
	}

	void SoundVolumeComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(SoundVolumeComponent::OnLoad,LoadGameComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(SoundVolumeComponent::OnVelocityNotifyMessage,VelocityNotifyMessage,0));
	}

	void SoundVolumeComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		MessagePtr play_msg(new SoundParameterMessage(SoundParameterMessage::PLAY,0));
		GetSceneObject()->PostMessage(play_msg);

		MessagePtr volume_msg(new SoundParameterMessage(SoundParameterMessage::VOLUME,0));
		GetSceneObject()->PostMessage(volume_msg);
	}

	void SoundVolumeComponent::OnVelocityNotifyMessage(VelocityNotifyMessagePtr message)
	{
		Vec3 ang_vel  = message->GetAngularVelocity();
		const float speed = fabs(ang_vel.y + ang_vel.x);
		const float max_volume_at_speed = 0.5;
		if(speed < max_volume_at_speed)
		{
			//std::cout << speed << std::endl;
			//turret sound
			const float volume = (speed/max_volume_at_speed);
			MessagePtr sound_msg(new SoundParameterMessage(SoundParameterMessage::VOLUME,volume));
			GetSceneObject()->PostMessage(sound_msg);
		}
		/*if(speed > 0)
		{
			float pitch = 0.8 + speed*0.01;
			MessagePtr sound_msg(new SoundParameterMessage(SoundParameterMessage::PITCH,pitch));
			GetSceneObject()->PostMessage(sound_msg);
		}*/
	}

	
}
