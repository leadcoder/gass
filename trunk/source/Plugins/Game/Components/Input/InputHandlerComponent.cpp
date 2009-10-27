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
#include "Core/MessageSystem/AnyMessage.h"
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

	void InputHandlerComponent::OnEnter(AnyMessagePtr message)
	{
		ControlSetting* cs = SimEngine::Get().GetControlSettingsManager()->GetControlSetting(m_ControlSetting);
		if(cs)
			cs->GetMessageManager()->RegisterForMessage(CONTROLLER_MESSAGE_NEW_INPUT, TYPED_MESSAGE_FUNC(InputHandlerComponent::OnInput,ControllerMessage));
		else 
			Log::Warning("Failed to find control settings: InputHandlerComponentInputSettings");

		//try find camera, move this to vehicle camera class
		IComponentContainer::ComponentContainerIterator cc_iter = GetSceneObject()->GetChildren();
		while(cc_iter.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_dynamic_cast<GASS::SceneObject>(cc_iter.getNext());
			CameraComponentPtr camera = child->GetFirstComponent<ICameraComponent>();//.find("CameraObject");
			if(camera)
			{
				MessagePtr cam_msg(new ChangeCameraMessage(child));
				GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->SendImmediate(cam_msg);
				break;
			}
		}
	}

	void InputHandlerComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("InputHandlerComponent",new Creator<InputHandlerComponent, IComponent>);
		RegisterProperty<std::string>("ControlSetting", &GetControlSetting, &SetControlSetting);
	}

	void InputHandlerComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage((SceneObjectMessage)OBJECT_RM_ENTER_VEHICLE, TYPED_MESSAGE_FUNC(GASS::InputHandlerComponent::OnEnter,AnyMessage));
		GetSceneObject()->RegisterForMessage((SceneObjectMessage)OBJECT_RM_EXIT_VEHICLE, TYPED_MESSAGE_FUNC(InputHandlerComponent::OnExit,AnyMessage));
		GetSceneObject()->RegisterForMessage(OBJECT_RM_UNLOAD_COMPONENTS, MESSAGE_FUNC(InputHandlerComponent::OnUnload));
		GetSceneObject()->RegisterForMessage(OBJECT_RM_LOAD_SIM_COMPONENTS, TYPED_MESSAGE_FUNC(InputHandlerComponent::OnLoad,LoadSimComponentsMessage));
	}

	void InputHandlerComponent::OnExit(AnyMessagePtr message)
	{
		ControlSetting* cs = SimEngine::Get().GetControlSettingsManager()->GetControlSetting(m_ControlSetting);
		if(cs)
			cs->GetMessageManager()->UnregisterForMessage(CONTROLLER_MESSAGE_NEW_INPUT, TYPED_MESSAGE_FUNC(InputHandlerComponent::OnInput,ControllerMessage));
	}

	void InputHandlerComponent::OnLoad(LoadSimComponentsMessagePtr message)
	{
	//	message->GetSimSceneManager()->GetScenarioScene()->RegisterForMessage(SCENARIO_RM_ENTER_VEHICLE,TYPED_MESSAGE_FUNC(InputHandlerComponent::OnEnter,AnyMessage));
	}

	void InputHandlerComponent::OnUnload(MessagePtr message)
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
		
		//check if exit message
		if(name == "ExitVehicle")
		{
			AnyMessagePtr exit_message(new AnyMessage((SceneObjectMessage)OBJECT_RM_EXIT_VEHICLE));
			GetSceneObject()->PostMessage(exit_message);	
		}
		else
		{
			AnyMessagePtr input_message(new AnyMessage(OBJECT_NM_PLAYER_INPUT));
			input_message->SetData("Controller",name);
			input_message->SetData("Value",value);
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
