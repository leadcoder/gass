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

#include "ForceToSoundComponent.h"
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
	ForceToSoundComponent::ForceToSoundComponent() : 
		m_TargetPitch(1.0),
		m_Pitch(1.0),
		m_MaxVelRequest(0),
		m_MaxForce(0),
		m_ForceLimit(300)
	{

	}

	ForceToSoundComponent::~ForceToSoundComponent()
	{

	}

	void ForceToSoundComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("ForceToSoundComponent",new Creator<ForceToSoundComponent, Component>);
		REG_PROPERTY(std::vector<Vec2>, ForceToPitch, ForceToSoundComponent);
		REG_PROPERTY(Float ,ForceLimit,ForceToSoundComponent);
	}

	void ForceToSoundComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(ForceToSoundComponent::OnHingeReport,PhysicsHingeJointReportEvent,0));
		
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<GameSceneManager>()->Register(listener);

		//Play engine sound
		
		GetSceneObject()->PostRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::PLAY,0)));
	}

	void ForceToSoundComponent::SceneManagerTick(double delta_time)
	{
		//m_DT = delta_time;
		m_TargetPitch = 1.2;
		Float normalized_force = std::min<Float>(1.0, m_MaxForce/m_ForceLimit);

		if(abs(m_MaxVelRequest) > 0.05)
		{
			for(size_t i = 0; i < m_ForceToPitch.size(); i++)
			{
				if(normalized_force > m_ForceToPitch[i].x)
					m_TargetPitch = m_ForceToPitch[i].y;
			}
		}
		
		if(m_TargetPitch > m_Pitch) 
			m_Pitch += delta_time*0.5;
		else
			m_Pitch -= delta_time*0.5;
		GetSceneObject()->PostRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::PITCH,m_Pitch)));
		//reset!
		m_MaxVelRequest = 0;
		m_MaxForce = 0;
	}

	void ForceToSoundComponent::OnHingeReport(PhysicsHingeJointReportEventPtr message)
	{
		Float t = message->GetForce().Length();
		m_MaxForce = std::max<Float>(t,m_MaxForce);
		m_MaxVelRequest = std::max<Float>(abs(message->GetTargetVelocity()),m_MaxVelRequest);
	}
	
}
