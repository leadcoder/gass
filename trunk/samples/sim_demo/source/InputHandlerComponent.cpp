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
#include "VehicleMessages.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
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


namespace GASS
{
	InputHandlerComponent::InputHandlerComponent() 
	{

	}

	InputHandlerComponent::~InputHandlerComponent()
	{

	}

	void InputHandlerComponent::OnEnter(MessagePtr message)
	{
		ControlSetting* cs = SimEngine::Get().GetControlSettingsManager()->GetControlSetting(m_ControlSetting);
		if(cs)
			cs->GetMessageManager()->RegisterForMessage(ControlSetting::CONTROLLER_MESSAGE_NEW_INPUT, MESSAGE_FUNC(InputHandlerComponent::OnInput));
		else 
			Log::Warning("Failed to find control settings: InputHandlerComponentInputSettings");

		//try find camera 
		/*IComponentContainer::ComponentContainerIterator cc_iter = GetSceneObject()->GetChildren();
		while(cc_iter.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_dynamic_cast<GASS::SceneObject>(cc_iter.getNext());
			std::string::size_type pos = child->GetName().find("CameraObject");
			if(child && std::string::npos != pos)
			{
				MessagePtr cam_msg(new Message(GASS::ScenarioScene::SCENARIO_RM_CHANGE_CAMERA));
				cam_msg->SetData("CameraObject",child);
				GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->SendImmediate(cam_msg);
				break;
			}
		}*/
	}

	void InputHandlerComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("InputHandlerComponent",new Creator<InputHandlerComponent, IComponent>);
		RegisterProperty<std::string>("ControlSetting", &GetControlSetting, &SetControlSetting);
	}

	void InputHandlerComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage((SceneObject::ObjectMessage)OBJECT_RM_ENTER_VEHICLE, MESSAGE_FUNC(GASS::InputHandlerComponent::OnEnter));
		GetSceneObject()->RegisterForMessage((SceneObject::ObjectMessage)OBJECT_RM_EXIT_VEHICLE, MESSAGE_FUNC(InputHandlerComponent::OnExit));
	}

	void InputHandlerComponent::OnExit(MessagePtr message)
	{
		ControlSetting* cs = SimEngine::Get().GetControlSettingsManager()->GetControlSetting(m_ControlSetting);
		if(cs)
			cs->GetMessageManager()->UnregisterForMessage(ControlSetting::CONTROLLER_MESSAGE_NEW_INPUT, MESSAGE_FUNC(InputHandlerComponent::OnInput));
	}

	void InputHandlerComponent::OnInput(MessagePtr message)
	{
		//relay message
		std::string name = boost::any_cast<std::string>(message->GetData("Controller"));
		float value = boost::any_cast<float>(message->GetData("Value"));
		
		//check if exit message
		if(name == "Exit")
		{
			MessagePtr exit_message(new Message((SceneObject::ObjectMessage)OBJECT_RM_EXIT_VEHICLE));
			GetSceneObject()->SendImmediate(exit_message);	
		}
		else
		{
			MessagePtr input_message(new Message(OBJECT_NM_PLAYER_INPUT));
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
