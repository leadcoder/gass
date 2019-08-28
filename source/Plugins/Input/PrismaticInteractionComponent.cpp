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

#include "PrismaticInteractionComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"

namespace GASS
{
	PrismaticInteractionComponent::PrismaticInteractionComponent() : m_MaxVelocity(1)
	{

	}

	PrismaticInteractionComponent::~PrismaticInteractionComponent()
	{

	}

	void PrismaticInteractionComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<PrismaticInteractionComponent>();
		RegisterMember("InputMapping", &PrismaticInteractionComponent::m_InputMapping);
		RegisterMember("MaxVelocity", &PrismaticInteractionComponent::m_MaxVelocity);
	}

	void PrismaticInteractionComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PrismaticInteractionComponent::OnInput,InputRelayEvent,0));
	}

	void PrismaticInteractionComponent::OnInput(InputRelayEventPtr message)
	{
		std::string name = message->GetController();
		float value = message->GetValue();
		if (name == m_InputMapping)
		{
			//static float pos = 0;
			float linear_vel = value*static_cast<float>(m_MaxVelocity);
			//pos += linear_vel;
			//std::cout  << pos << "\n";
			//MessagePtr vel_msg2(new PhysicsPrismaticJointVelocityRequest(linear_vel));
			//GetSceneObject()->PostMessage(vel_msg2);

			//MessagePtr message(new PhysicsPrismaticJointPositionRequest(pos));
			//GetSceneObject()->PostMessage(message);
			GetSceneObject()->PostRequest(PhysicsPrismaticJointVelocityRequestPtr(new PhysicsPrismaticJointVelocityRequest(linear_vel)));
		}
	}
}
