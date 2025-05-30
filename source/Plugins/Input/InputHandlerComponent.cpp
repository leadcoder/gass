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

#include "InputHandlerComponent.h"

#include <memory>
#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Messages/GASSInputMessages.h"

namespace GASS
{
	InputHandlerComponent::InputHandlerComponent()  
	{

	}

	InputHandlerComponent::~InputHandlerComponent()
	{

	}

	void InputHandlerComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<InputHandlerComponent>();
		RegisterGetSet("ControlSetting", &InputHandlerComponent::GetControlSetting, &InputHandlerComponent::SetControlSetting);
	}

	void InputHandlerComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(InputHandlerComponent::OnEnter,EnterVehicleRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(InputHandlerComponent::OnExit,ExitVehicleRequest,0));
	}
	
	void InputHandlerComponent::OnDelete()
	{
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(InputHandlerComponent::OnInput,ControllSettingsMessage));
	}

	void InputHandlerComponent::OnEnter(EnterVehicleRequestPtr /*message*/)
	{
		if(m_Empty)
		{
			SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(InputHandlerComponent::OnInput,ControllSettingsMessage,0));
			m_Empty = false;
		}
	}

	void InputHandlerComponent::OnExit(ExitVehicleRequestPtr /*message*/)
	{
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(InputHandlerComponent::OnInput,ControllSettingsMessage));
		m_Empty = true;
	}
	
	void InputHandlerComponent::OnInput(ControllSettingsMessagePtr message)
	{
		const std::string name = message->GetController();
		const float value = message->GetValue();

		if(name == "ExitVehicle" && value > 0) //check if exit input
		{
			GetSceneObject()->PostRequest(std::make_shared<ExitVehicleRequest>());
		}

		if(m_ControlSetting != message->GetSettings())
			return;
		//relay message
		GetSceneObject()->SendImmediateEvent(std::make_shared<InputRelayEvent>(message->GetSettings(),name,value,message->GetControllerType()));
	}

	void InputHandlerComponent::SetControlSetting(const std::string &controlsetting)
	{
		m_ControlSetting = controlsetting;
	}

	std::string InputHandlerComponent::GetControlSetting() const
	{
		return m_ControlSetting;
	}

}
