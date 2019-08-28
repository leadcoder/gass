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

#include "HingeInteractionComponent.h"
#include "Core/Math/GASSMath.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"

namespace GASS
{
	HingeInteractionComponent::HingeInteractionComponent() : m_MaxAngularVelocity(5)
	{

	}

	HingeInteractionComponent::~HingeInteractionComponent()
	{

	}

	void HingeInteractionComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<HingeInteractionComponent>();
		RegisterMember("InputMapping", &HingeInteractionComponent::m_InputMapping);
		RegisterMember("MaxAngularVelocity", &HingeInteractionComponent::m_MaxAngularVelocity);
	}

	void HingeInteractionComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(HingeInteractionComponent::OnInput,InputRelayEvent,0));
	}

	void HingeInteractionComponent::OnInput(InputRelayEventPtr message)
	{
		std::string name = message->GetController();
		float value = message->GetValue();
		if (name == m_InputMapping)
		{
			float angular_vel = value * static_cast<float>(Math::Deg2Rad(m_MaxAngularVelocity));
			GetSceneObject()->PostRequest(PhysicsHingeJointVelocityRequestPtr(new PhysicsHingeJointVelocityRequest(angular_vel)));
		}
	}
}
