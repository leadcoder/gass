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
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"

#include "Sim/Messages/GASSGraphicsSceneMessages.h"

#include "Sim/Interface/GASSIResourceSystem.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Scheduling/GASSIRuntimeController.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSICameraComponent.h"


namespace GASS
{
	PlayerInputComponent::PlayerInputComponent()
	{

	}

	PlayerInputComponent::~PlayerInputComponent()
	{
	}

	void PlayerInputComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PlayerInputComponent",new Creator<PlayerInputComponent, IComponent>);
		RegisterProperty<std::string>("ControlSetting", &PlayerInputComponent::GetControlSetting, &PlayerInputComponent::SetControlSetting);
	}

	void PlayerInputComponent::OnInitialize()
	{
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(PlayerInputComponent::OnInput,ControllSettingsMessage,0));
	}

	void PlayerInputComponent::OnDelete()
	{
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(PlayerInputComponent::OnInput,ControllSettingsMessage));
	}

	void PlayerInputComponent::OnInput(ControllSettingsMessagePtr message)
	{
		if(m_ControlSetting != message->GetSettings())
			return;
		//relay message
		std::string name = message->GetController();
		float value = message->GetValue();
		static int seat = 0;

		//check if enter message
		if(name == "EnterVehicle" && value > 0)
		{
			LocationComponentPtr my_location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
			Vec3 my_pos = my_location->GetWorldPosition();
			//check all objects with in enter radius
			IComponentContainer::ComponentContainerIterator objects = GetSceneObject()->GetScene()->GetRootSceneObject()->GetChildren();
			while(objects.hasMoreElements())
			{
				ComponentContainerPtr cc = objects.getNext();
				SceneObjectPtr so = boost::shared_static_cast<SceneObject>(cc);
				InputHandlerComponentPtr ihc = so->GetFirstComponentByClass<InputHandlerComponent>();
				if(ihc)
				{
					LocationComponentPtr location = so->GetFirstComponentByClass<ILocationComponent>();
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
					LocationComponentPtr location = ih->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
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
			CameraComponentPtr camera = GetSceneObject()->GetFirstComponentByClass<ICameraComponent>();
			if(camera)
			{
				MessagePtr cam_msg(new ChangeCameraMessage(GetSceneObject(),"Viewport0"));
				GetSceneObject()->GetScene()->SendImmediate(cam_msg);
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
