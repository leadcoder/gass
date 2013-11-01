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
	ForceToSoundComponent::ForceToSoundComponent() : m_MaxAngularVelocity(5),
		m_TargetPitch(1.0),
		m_Pitch(1.0),
		m_CurrentVel(0,0,0)
	{

	}

	ForceToSoundComponent::~ForceToSoundComponent()
	{

	}

	void ForceToSoundComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("ForceToSoundComponent",new Creator<ForceToSoundComponent, IComponent>);
	}

	void ForceToSoundComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(ForceToSoundComponent::OnForceEvent,PhysicsJointForceEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ForceToSoundComponent::OnVelocityEvent,PhysicsJointVelocityEvent,0));
		
		GetSceneObject()->RegisterForMessage(REG_TMESS(ForceToSoundComponent::OnHingeVelocity,PhysicsHingeJointVelocityRequest,0));

		
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<GameSceneManager>()->Register(listener);

		//Play engine sound
		MessagePtr sound_msg(new SoundParameterMessage(SoundParameterMessage::PLAY,0));
		GetSceneObject()->PostMessage(sound_msg);
	}

	void ForceToSoundComponent::SceneManagerTick(double delta_time)
	{
		m_DT = delta_time;
		//Vec3 accel = m_CurrentAcc*delta_time;
		
		if(m_TargetPitch > m_Pitch) 
			m_Pitch += delta_time*0.5;
		else
			m_Pitch -= delta_time*0.5;

		MessagePtr sound_msg(new SoundParameterMessage(SoundParameterMessage::PITCH,m_Pitch));
		GetSceneObject()->PostMessage(sound_msg);
	}


	void ForceToSoundComponent::OnVelocityEvent(PhysicsJointVelocityEventPtr message)
	{
		
		/*m_CurrentAcc = (m_CurrentVel - message->GetAngularVelocity())*(1.0);
		m_CurrentVel = message->GetAngularVelocity();
		std::cout << "accel:" << m_CurrentAcc.x << " vel:" << m_CurrentVel.x << std::endl;
		Float max_accel = Math::Deg2Rad(3);
		Float accel = fabs(m_CurrentAcc.x);
		accel = std::min<Float>(max_accel, accel);
		Float pitch = accel/max_accel;
		pitch *= 0.25;
		pitch = 1.2-pitch;
		m_TargetPitch = pitch;*/
	}

	void ForceToSoundComponent::OnForceEvent(PhysicsJointForceEventPtr message)
	{
		m_TargetPitch = 1.2;
		
		if(abs(m_CurrentVelRequest) < 0.00001)
			return;
		//Float m_MaxTorq = 100;
		Vec3 force = message->GetForce();
		Float t = force.Length();

		
		if(t > 20)
			m_TargetPitch = 1.15;

		if(t > 40)
			m_TargetPitch = 1.1;

		if(t > 100)
			m_TargetPitch = 1.0;

		if(t > 120)
			m_TargetPitch = 0.94;

		if(t > 300)
			m_TargetPitch = 0.8;

		std::cout << " t:" <<  t << std::endl;
	}

	void ForceToSoundComponent::OnHingeVelocity(PhysicsHingeJointVelocityRequestPtr message)
	{
		//Float diff = m_CurrentVelRequest - message->GetVelocity();
		m_CurrentVelRequest = message->GetVelocity();
		/*Float max_vel = Math::Deg2Rad(15);
		Float vel = fabs(message->GetVelocity());
		vel = std::min<Float>(max_vel, vel);
		Float pitch = vel/max_vel;
		pitch *= 0.25;
		pitch = 1.2-pitch;
		m_TargetPitch = pitch;*/
		//std::cout << "vel:" << vel << " p:" << pitch << std::endl;
	}
}
