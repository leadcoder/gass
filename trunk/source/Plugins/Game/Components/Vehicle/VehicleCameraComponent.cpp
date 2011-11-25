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

#include "VehicleCameraComponent.h"
#include "GameMessages.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/Graphics/IGraphicsSystem.h"


#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Components/Graphics/ICameraComponent.h"
#include "Sim/Scenario/Scene/Messages/GraphicsScenarioSceneMessages.h"
#include "Plugins/Game/Components/Input/InputHandlerComponent.h"




namespace GASS
{
	VehicleCameraComponent::VehicleCameraComponent() : m_PreferredViewport("ALL")
	{

	}

	VehicleCameraComponent::~VehicleCameraComponent()
	{

	}

	void VehicleCameraComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("VehicleCameraComponent",new Creator<VehicleCameraComponent, IComponent>);
		RegisterProperty<std::string>("PreferredViewport", &VehicleCameraComponent::GetPreferredViewport, &VehicleCameraComponent::SetPreferredViewport);
		RegisterProperty<SceneObjectLink>("InputHandlerObject", &VehicleCameraComponent::GetInputHandlerObject, &VehicleCameraComponent::SetInputHandlerObject);
	}

	void VehicleCameraComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(VehicleCameraComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(VehicleCameraComponent::OnLoad,LoadGameComponentsMessage,0));
		BaseSceneComponent::OnCreate();
	}

	void VehicleCameraComponent::SetPreferredViewport(const std::string &viewport)
	{
		m_PreferredViewport = viewport;
	}

	std::string VehicleCameraComponent::GetPreferredViewport() const
	{
		return m_PreferredViewport;
	}

	void VehicleCameraComponent::OnEnter(EnterVehicleMessagePtr message)
	{
		MessagePtr cam_msg(new ChangeCameraMessage(GetSceneObject(),m_PreferredViewport));
		GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->SendImmediate(cam_msg);
	}

	void VehicleCameraComponent::OnExit(ExitVehicleMessagePtr message)
	{
		/*if(m_PreferredViewport != "ALL")
		{
			SceneObjectPtr obj;
			MessagePtr cam_msg(new ChangeCameraMessage(obj,m_PreferredViewport));
			GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->SendImmediate(cam_msg);
		}*/
	}

	void VehicleCameraComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		
		m_InputHandlerObject->RegisterForMessage(REG_TMESS(VehicleCameraComponent::OnEnter,EnterVehicleMessage,0));
		m_InputHandlerObject->RegisterForMessage(REG_TMESS(VehicleCameraComponent::OnExit,ExitVehicleMessage,0));
		GraphicsSystemPtr gfx_sys = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IGraphicsSystem>();
		/*if(gfx_sys && m_PreferredViewport != "ALL")
		{
			gfx_sys->CreateViewport(m_PreferredViewport, "MainWindow", 0.2, 0.2, 0.3, 0.5);
		}*/

		//message->GetSimSceneManager()->GetScenarioScene()->RegisterForMessage(SCENARIO_RM_ENTER_VEHICLE,TYPED_MESSAGE_FUNC(VehicleCameraComponent::OnEnter,AnyMessage));
		//register on enter message
	}

	void VehicleCameraComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		InputHandlerComponentPtr vehicle = GetSceneObject()->GetFirstParentComponentByClass<InputHandlerComponent>();
		if(vehicle)
		{
			vehicle->GetSceneObject()->UnregisterForMessage(UNREG_TMESS(VehicleCameraComponent::OnEnter,EnterVehicleMessage));
			vehicle->GetSceneObject()->UnregisterForMessage(UNREG_TMESS(VehicleCameraComponent::OnExit,ExitVehicleMessage));
		}
	}

}
