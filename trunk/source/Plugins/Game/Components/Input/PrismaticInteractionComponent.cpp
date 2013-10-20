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

#include "PrismaticInteractionComponent.h"
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
#include "Sim/Interface/GASSIControlSettingsSystem.h"


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
		ComponentFactory::GetPtr()->Register("PrismaticInteractionComponent",new Creator<PrismaticInteractionComponent, IComponent>);
		RegisterProperty<std::string>("InputMapping", &PrismaticInteractionComponent::GetInputMapping, &PrismaticInteractionComponent::SetInputMapping);
		RegisterProperty<Float>("MaxVelocity", &PrismaticInteractionComponent::GetMaxVelocity, &PrismaticInteractionComponent::SetMaxVelocity);
	}

	void PrismaticInteractionComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PrismaticInteractionComponent::OnInput,InputControllerMessage,0));
	}

	void PrismaticInteractionComponent::OnInput(InputControllerMessagePtr message)
	{
		std::string name = message->GetController();
		float value = message->GetValue();
		if (name == m_InputMapping)
		{
			//static float pos = 0;
			float linear_vel = value*m_MaxVelocity;
			//pos += linear_vel;
			//std::cout  << pos << "\n";
			//MessagePtr vel_msg2(new PhysicsPrismaticJointVelocityRequest(linear_vel));
			//GetSceneObject()->PostMessage(vel_msg2);

			//MessagePtr message(new PhysicsPrismaticJointPositionRequest(pos));
			//GetSceneObject()->PostMessage(message);

			MessagePtr vel_msg2(new PhysicsPrismaticJointVelocityRequest(linear_vel));
			GetSceneObject()->PostMessage(vel_msg2);
		}
	}
}
