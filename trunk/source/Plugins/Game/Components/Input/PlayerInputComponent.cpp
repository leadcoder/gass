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

#include "PlayerInputComponent.h"
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
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Components/Graphics/ICameraComponent.h"


namespace GASS
{
	PlayerInputComponent::PlayerInputComponent() 
	{

	}

	PlayerInputComponent::~PlayerInputComponent()
	{
		ControlSetting* cs = SimEngine::Get().GetControlSettingsManager()->GetControlSetting(m_ControlSetting);
		if(cs)
			cs->GetMessageManager()->UnregisterForMessage(UNREG_TMESS(PlayerInputComponent::OnInput,ControllerMessage));
	}


	void PlayerInputComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PlayerInputComponent",new Creator<PlayerInputComponent, IComponent>);
		RegisterProperty<std::string>("ControlSetting", &GetControlSetting, &SetControlSetting);
	}

	void PlayerInputComponent::OnCreate()
	{
	//	GetSceneObject()->RegisterForMessage((SceneObjectMessage)OBJECT_RM_ENTER_VEHICLE, TYPED_MESSAGE_FUNC(GASS::PlayerInputComponent::OnEnter,AnyMessage));
	//	GetSceneObject()->RegisterForMessage((SceneObjectMessage)OBJECT_RM_EXIT_VEHICLE, TYPED_MESSAGE_FUNC(PlayerInputComponent::OnExit,AnyMessage));
		ControlSetting* cs = SimEngine::Get().GetControlSettingsManager()->GetControlSetting(m_ControlSetting);
		if(cs)
			cs->GetMessageManager()->RegisterForMessage(REG_TMESS(PlayerInputComponent::OnInput,ControllerMessage,0));
		else 
			Log::Warning("Failed to find control settings: PlayerInputComponentInputSettings");

	}

	
	/*void PlayerInputComponent::OnExit(AnyMessagePtr message)
	{
		ControlSetting* cs = SimEngine::Get().GetControlSettingsManager()->GetControlSetting(m_ControlSetting);
		if(cs)
			cs->GetMessageManager()->UnregisterForMessage(CONTROLLER_MESSAGE_NEW_INPUT, TYPED_MESSAGE_FUNC(PlayerInputComponent::OnInput,ControllerMessage));
	}*/

	void PlayerInputComponent::OnInput(ControllerMessagePtr message)
	{
		//relay message
		std::string name = message->GetController();
		float value = message->GetValue();

		static int seat = 0;
		
		//check if enter message
		if(name == "EnterVehicle" && value > 0)
		{
			LocationComponentPtr my_location = GetSceneObject()->GetFirstComponent<ILocationComponent>();
			Vec3 my_pos = my_location->GetWorldPosition();
			//check all objects with in enter radius
			IComponentContainer::ComponentContainerIterator objects = GetSceneObject()->GetSceneObjectManager()->GetSceneRoot()->GetChildren();
			while(objects.hasMoreElements())
			{
				ComponentContainerPtr cc = objects.getNext();
				SceneObjectPtr so = boost::shared_static_cast<SceneObject>(cc);
				InputHandlerComponentPtr ihc = so->GetFirstComponent<InputHandlerComponent>();
				if(ihc)
				{
					LocationComponentPtr location = so->GetFirstComponent<ILocationComponent>();
					Vec3 obj_pos = location->GetWorldPosition();
					Float dist = (my_pos-obj_pos).FastLength();
					if(dist < 5)
					{
						//enter and return
						MessagePtr enter_msg(new EnterVehicleMessage());
						so->PostMessage(enter_msg);
						m_CurrentVehicle = so;
						m_CurrentSeat = so;
						seat = 0;
						return;
					}
				}
				/*IComponentContainerTemplate::ComponentVector components;
				so->GetComponentsByClass(components,"InputHandlerComponent");
				seat = seat % components.size();
				if(components.size() > 0)
				{
					InputHandlerComponentPtr ih = boost::shared_dynamic_cast<InputHandlerComponent>(components[seat]);
					LocationComponentPtr location = ih->GetSceneObject()->GetFirstComponent<ILocationComponent>();
					Vec3 obj_pos = location->GetWorldPosition();
					Float dist = (my_pos-obj_pos).FastLength();
					if(dist < 5)
					{
						//enter and return
						MessagePtr enter_msg(new AnyMessage((SceneObjectMessage)OBJECT_RM_ENTER_VEHICLE));
						ih->GetSceneObject()->PostMessage(enter_msg);
						seat++;
						return;
					}
				}*/
			}
		}
		else if(name == "CycleVehicle" && value > 0)
		{
			
			IComponentContainerTemplate::ComponentVector components;
			if(m_CurrentVehicle)
			{
				m_CurrentVehicle->GetComponentsByClass(components,"InputHandlerComponent");
				seat = seat % components.size();
				//std::cout << "seat:" << seat << std::endl;
				if(components.size() > 0)
				{
					if(m_CurrentSeat)
					{
						MessagePtr exit_msg(new ExitVehicleMessage());
						m_CurrentSeat->PostMessage(exit_msg);
					}
					InputHandlerComponentPtr ih = boost::shared_dynamic_cast<InputHandlerComponent>(components[seat]);
					MessagePtr enter_msg(new EnterVehicleMessage());
					m_CurrentSeat = ih->GetSceneObject();
					m_CurrentSeat->PostMessage(enter_msg);
					
					seat++;
				}
			}
		}

		else if(name == "ExitVehicle" && value > 0)
		{
			//Take camera control
			CameraComponentPtr camera = GetSceneObject()->GetFirstComponent<ICameraComponent>();
			if(camera)
			{
				MessagePtr cam_msg(new ChangeCameraMessage(GetSceneObject()));
				GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->SendImmediate(cam_msg);
			}
			m_CurrentVehicle.reset();
			m_CurrentSeat.reset();
		}
	}

	void PlayerInputComponent::SetControlSetting(const std::string &controlsetting)
	{
		m_ControlSetting = controlsetting;
		//TODO: unregister previous, and register new one?
	}

	std::string PlayerInputComponent::GetControlSetting() const
	{
		return m_ControlSetting;
	}

}
