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

#include "InputProxyComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObject.h"

namespace GASS
{
	InputProxyComponent::InputProxyComponent()
	{

	}

	InputProxyComponent::~InputProxyComponent()
	{

	}

	void InputProxyComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<InputProxyComponent>();
		RegisterMember("InputHandlerObject", &InputProxyComponent::m_InputHandlerObject);
	}

	void InputProxyComponent::OnInitialize()
	{
		//Init pointers
		BaseSceneComponent::OnInitialize();
		m_InputHandlerObject->RegisterForMessage(REG_TMESS(GASS::InputProxyComponent::OnPlayerInput,InputRelayEvent,0));
	}

	void InputProxyComponent::OnDelete()
	{
		m_InputHandlerObject->UnregisterForMessage(UNREG_TMESS(GASS::InputProxyComponent::OnPlayerInput,InputRelayEvent));
	}

	void InputProxyComponent::OnPlayerInput(InputRelayEventPtr message)
	{
		GetSceneObject()->PostEvent(message);
	}

}
