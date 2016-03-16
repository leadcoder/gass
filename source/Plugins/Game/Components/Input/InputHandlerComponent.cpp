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

#include "InputHandlerComponent.h"
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
#include "Sim/Interface/GASSICameraComponent.h"

namespace GASS
{
	InputHandlerComponent::InputHandlerComponent() : m_Empty(true)
	{

	}

	InputHandlerComponent::~InputHandlerComponent()
	{

	}

	void InputHandlerComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("InputHandlerComponent",new Creator<InputHandlerComponent, Component>);
		RegisterProperty<std::string>("ControlSetting", &InputHandlerComponent::GetControlSetting, &InputHandlerComponent::SetControlSetting);
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

	void InputHandlerComponent::OnEnter(EnterVehicleRequestPtr message)
	{
		if(m_Empty)
		{
			SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(InputHandlerComponent::OnInput,ControllSettingsMessage,0));
			m_Empty = false;
		}
		/*ComponentContainerTemplate::ComponentVector components;
		GetSceneObject()->GetComponentsByClass<ICameraComponent>(components);
		
		//configure all cameras
		if(components.size() > 0)
		{
			BaseSceneComponentPtr camera = DYNAMIC_PTR_CAST<BaseSceneComponent>(components[0]);
			if(camera)
			{
				//const std::string vp_name = camera->GetPrefredViewport();
				MessagePtr cam_msg(new ChangeCameraRequest(camera->GetSceneObject(),vp_name));
				GetSceneObject()->GetScene()->SendImmediate(cam_msg);
			}
		}*/

		//try find camera, move this to vehicle camera class
	}

	void InputHandlerComponent::OnExit(ExitVehicleRequestPtr message)
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
			GetSceneObject()->PostRequest(ExitVehicleRequestPtr(new ExitVehicleRequest()));
		}

		if(m_ControlSetting != message->GetSettings())
			return;
		//relay message
		GetSceneObject()->SendImmediateEvent(InputRelayEventPtr(new InputRelayEvent(message->GetSettings(),name,value,message->GetControllerType())));
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
