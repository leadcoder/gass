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

#include "VelocityToSoundComponent.h"
#include "GameMessages.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSSoundSceneObjectMessages.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Plugins/Game/GameSceneManager.h"


#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"


namespace GASS
{
	VelocityToSoundComponent::VelocityToSoundComponent() : 
		m_TargetPitch(1.0),
		m_Pitch(1.0),
		m_Volume(1.0),
		m_MinMaxVolume(0,1),
		m_MinMaxPitch(1,1),
		m_VelocityLimit(16)
	{

	}

	VelocityToSoundComponent::~VelocityToSoundComponent()
	{

	}

	void VelocityToSoundComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("VelocityToSoundComponent",new Creator<VelocityToSoundComponent, IComponent>);
		RegisterProperty<Vec2>("MinMaxPitch", &VelocityToSoundComponent::GetMinMaxPitch, &VelocityToSoundComponent::SetMinMaxPitch);
		RegisterProperty<Vec2>("MinMaxVolume", &VelocityToSoundComponent::GetMinMaxVolume, &VelocityToSoundComponent::SetMinMaxVolume);
		REG_PROPERTY(Float ,VelocityLimit,VelocityToSoundComponent);
	}

	void VelocityToSoundComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(VelocityToSoundComponent::OnHingeReport,PhysicsHingeJointReportEvent,0));
		
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<GameSceneManager>()->Register(listener);

		//Play engine sound
		GetSceneObject()->PostRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::PLAY,0)));
	}

	void VelocityToSoundComponent::SceneManagerTick(double delta_time)
	{
		//std::cout << m_MaxVelRequest << std::endl;
		m_TargetPitch = 1.0;
		Float normalized_vel = std::min<Float>(1.0, m_MaxVelRequest/m_VelocityLimit);

		m_TargetPitch = m_MinMaxPitch.x + normalized_vel* (m_MinMaxPitch.y-m_MinMaxPitch.x);
		m_Pitch  = m_TargetPitch ; 
		/*if(m_TargetPitch > m_Pitch) 
			m_Pitch += delta_time*0.5;
		else
			m_Pitch -= delta_time*0.5;
		*/

		m_Pitch = std::max<Float>(m_Pitch,m_MinMaxPitch.x);
		m_Pitch = std::min<Float>(m_Pitch,m_MinMaxPitch.y);

		m_TargetVolume = 0.0;
		
		m_TargetVolume = m_MinMaxVolume.x + normalized_vel* (m_MinMaxVolume.y-m_MinMaxVolume.x);
		m_Volume = m_TargetVolume; 
		/*if(m_TargetVolume > m_Volume) 
			m_Volume += delta_time*2.1;
		else
			m_Volume -= delta_time*2.1;
			*/
		m_Volume = std::max<Float>(m_Volume,m_MinMaxVolume.x);
		m_Volume = std::min<Float>(m_Volume,m_MinMaxVolume.y);
		GetSceneObject()->PostRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::PITCH,m_Pitch)));
		GetSceneObject()->PostRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::VOLUME,m_Volume)));
		//reset!
		m_MaxVelRequest = 0;
	}

	void VelocityToSoundComponent::OnHingeReport(PhysicsHingeJointReportEventPtr message)
	{
		m_MaxVelRequest = std::max<Float>(abs(message->GetTargetVelocity()),m_MaxVelRequest);
	}
	
}
