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

#include "VelocityToSoundComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSSoundSceneObjectMessages.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSIMissionSceneManager.h"

namespace GASS
{
	VelocityToSoundComponent::VelocityToSoundComponent() : 
		m_TargetPitch(1.0),
		m_Pitch(1.0),
		m_Volume(1.0),
		m_MinMaxVolume(0,1),
		m_MinMaxPitch(1,1),
		m_VelocityLimit(16),
		m_MaxVelRequest(0)
	{

	}

	VelocityToSoundComponent::~VelocityToSoundComponent()
	{

	}

	void VelocityToSoundComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("VelocityToSoundComponent",new Creator<VelocityToSoundComponent, Component>);
		RegisterMember("MinMaxPitch", &VelocityToSoundComponent::m_MinMaxPitch);
		RegisterMember("MinMaxVolume", &VelocityToSoundComponent::m_MinMaxVolume);
		RegisterMember("VelocityLimit", &VelocityToSoundComponent::m_VelocityLimit);
	}

	void VelocityToSoundComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(VelocityToSoundComponent::OnHingeReport,PhysicsHingeJointReportEvent,0));
		
		RegisterForPostUpdate<IMissionSceneManager>();

		//Play engine sound
		GetSceneObject()->PostRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::PLAY,0)));
	}

	void VelocityToSoundComponent::SceneManagerTick(double /*delta_time*/)
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

		
		Float  target_volume = m_MinMaxVolume.x + normalized_vel* (m_MinMaxVolume.y-m_MinMaxVolume.x);
		m_Volume = target_volume;
		/*if(m_TargetVolume > m_Volume) 
			m_Volume += delta_time*2.1;
		else
			m_Volume -= delta_time*2.1;
			*/
		m_Volume = std::max<Float>(m_Volume,m_MinMaxVolume.x);
		m_Volume = std::min<Float>(m_Volume,m_MinMaxVolume.y);
		GetSceneObject()->PostRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::PITCH, static_cast<float>(m_Pitch))));
		GetSceneObject()->PostRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::VOLUME, static_cast<float>(m_Volume))));
		//reset!
		m_MaxVelRequest = 0;
	}

	void VelocityToSoundComponent::OnHingeReport(PhysicsHingeJointReportEventPtr message)
	{
		m_MaxVelRequest = std::max<Float>(fabs(message->GetTargetVelocity()),m_MaxVelRequest);
	}
	
}
