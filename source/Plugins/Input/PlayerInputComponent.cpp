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

#include "PlayerInputComponent.h"

#include <memory>
#include "InputHandlerComponent.h"
#include "Sim/GASSComponentFactory.h"
#include "Sim/GASSSceneObjectTemplate.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
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
		ComponentFactory::Get().Register<PlayerInputComponent>();
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("PlayerInputComponent", OF_VISIBLE));
		RegisterGetSet("ControlSetting", &PlayerInputComponent::GetControlSetting, &PlayerInputComponent::SetControlSetting);
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
			SceneObject::ComponentVector comps;
			GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<InputHandlerComponent>(comps);
			for(size_t i = 0 ; i < comps.size();i++)
			{
				InputHandlerComponentPtr ihc = GASS_DYNAMIC_PTR_CAST<InputHandlerComponent>(comps[i]);
				LocationComponentPtr location = ihc->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
				Vec3 obj_pos = location->GetWorldPosition();
				Float dist = (my_pos-obj_pos).Length();
				if(dist < 5)
				{
					//enter and return
					ihc->GetSceneObject()->PostRequest(std::make_shared<EnterVehicleRequest>());
					m_CurrentVehicle = ihc->GetSceneObject();
					m_CurrentSeat = ihc->GetSceneObject();
					seat = 0;
					return;
				}
			}
		}
		else if(name == "CycleVehicle" && value > 0)
		{
			SceneObjectTemplate::ComponentVector components;
			if(m_CurrentVehicle)
			{
				m_CurrentVehicle->GetComponentsByClass<InputHandlerComponent>(components);//,"InputHandlerComponent");
				seat = seat % components.size();
				//std::cout << "seat:" << seat << std::endl;
				if(components.size() > 0)
				{
					if(m_CurrentSeat)
					{
						m_CurrentSeat->PostRequest(std::make_shared<ExitVehicleRequest>());
					}
					InputHandlerComponentPtr ih = GASS_DYNAMIC_PTR_CAST<InputHandlerComponent>(components[seat]);
					m_CurrentSeat = ih->GetSceneObject();
					m_CurrentSeat->PostRequest(std::make_shared<EnterVehicleRequest>());
					seat++;
				}
			}
		}

		else if(name == "ExitVehicle" && value > 0)
		{
			//restore camera
			CameraComponentPtr camera = GetSceneObject()->GetFirstComponentByClass<ICameraComponent>();
			if(camera)
			{
				camera->ShowInViewport();
			}
			m_CurrentVehicle.reset();
			m_CurrentSeat.reset();
		}
	}

	void PlayerInputComponent::SetControlSetting(const std::string &controlsetting)
	{
		m_ControlSetting = controlsetting;
	}

	std::string PlayerInputComponent::GetControlSetting() const
	{
		return m_ControlSetting;
	}

}
