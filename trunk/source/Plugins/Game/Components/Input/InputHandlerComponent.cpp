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

#include "InputHandlerComponent.h"
#include "GameMessages.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"

#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Components/Graphics/ICameraComponent.h"


namespace GASS
{
	InputHandlerComponent::InputHandlerComponent() 
	{

	}

	InputHandlerComponent::~InputHandlerComponent()
	{

	}

	void InputHandlerComponent::OnEnter(EnterVehicleMessagePtr message)
	{
		ControlSetting* cs = SimEngine::Get().GetControlSettingsManager()->GetControlSetting(m_ControlSetting);
		if(cs)
			cs->GetMessageManager()->RegisterForMessage(REG_TMESS(InputHandlerComponent::OnInput,ControllerMessage,0));
		else 
			Log::Warning("Failed to find control settings: %s",m_ControlSetting.c_str());
	
		
		IComponentContainerTemplate::ComponentVector components;
		GetSceneObject()->GetComponentsByClass(components,"OgreCameraComponent");
		
		if(components.size() > 0)
		{
			BaseSceneComponentPtr camera = boost::shared_dynamic_cast<BaseSceneComponent>(components[0]);

			if(camera)
			{
				MessagePtr cam_msg(new ChangeCameraMessage(camera->GetSceneObject()));
				GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->SendImmediate(cam_msg);
			}
		}

		//try find camera, move this to vehicle camera class

	}

	void InputHandlerComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("InputHandlerComponent",new Creator<InputHandlerComponent, IComponent>);
		RegisterProperty<std::string>("ControlSetting", &GetControlSetting, &SetControlSetting);
	}

	void InputHandlerComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(InputHandlerComponent::OnEnter,EnterVehicleMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(InputHandlerComponent::OnExit,ExitVehicleMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(InputHandlerComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(InputHandlerComponent::OnLoad,LoadGameComponentsMessage,0));
	}

	void InputHandlerComponent::OnExit(ExitVehicleMessagePtr message)
	{
		ControlSetting* cs = SimEngine::Get().GetControlSettingsManager()->GetControlSetting(m_ControlSetting);
		if(cs)
			cs->GetMessageManager()->UnregisterForMessage(UNREG_TMESS(InputHandlerComponent::OnInput,ControllerMessage));
	}

	void InputHandlerComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
	//	message->GetSimSceneManager()->GetScenarioScene()->RegisterForMessage(SCENARIO_RM_ENTER_VEHICLE,TYPED_MESSAGE_FUNC(InputHandlerComponent::OnEnter,AnyMessage));
	}

	void InputHandlerComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
	//	message->GetSimSceneManager()->GetScenarioScene()->UnregisterForMessage(SCENARIO_RM_ENTER_VEHICLE,TYPED_MESSAGE_FUNC(PlayerInputComponent::OnEnter,AnyMessage));
	}

	/*void InputHandlerComponent::OnEnter(AnyMessagePtr message)
	{
		Vec3 player_pos = boost::any_cast<Vec3>(message->GetData("PlayerPosition"));
		float dist= (player_pos - my_pos).Length();
		//Check if this pos is within enter radius
		if(dist < 10)
		{
			//Enter vehicle by sending enter messeage
			AnyMessagePtr exit_message(new AnyMessage((SceneObjectMessage)OBJECT_RM_ENTER_VEHICLE));
			GetSceneObject()->SendImmediate(exit_message);	
		}
	}*/

	void InputHandlerComponent::OnInput(ControllerMessagePtr message)
	{
		//relay message
		std::string name = message->GetController();
		float value = message->GetValue();
		
		//check if exit input
		if(name == "ExitVehicle" && value > 0)
		{
			MessagePtr exit_message(new EnterVehicleMessage());
			GetSceneObject()->PostMessage(exit_message);	
		}
		else
		{
			MessagePtr input_message(new PlayerInputMessage(name,value));
			GetSceneObject()->SendImmediate(input_message);	
		}
	}

	void InputHandlerComponent::SetControlSetting(const std::string &controlsetting)
	{
		m_ControlSetting = controlsetting;
		//TODO: unregister previous, and register new one?
	}

	std::string InputHandlerComponent::GetControlSetting() const
	{
		return m_ControlSetting;
	}

}
