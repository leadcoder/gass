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

#include "VehicleDebugComponent.h"
#include "GameMessages.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"

#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/SceneObjectTemplate.h"

#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"



namespace GASS
{
	VehicleDebugComponent::VehicleDebugComponent() : m_ShowWaypoint(true)
	{
	}

	VehicleDebugComponent::~VehicleDebugComponent()
	{

	}

	void VehicleDebugComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("VehicleDebugComponent",new Creator<VehicleDebugComponent, IComponent>);
//		RegisterProperty<bool>("ShowWaypoint", &VehicleDebugComponent::GetShowWaypoint, &VehicleDebugComponent::SetShowWaypoint);
	}

	void VehicleDebugComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(OBJECT_RM_LOAD_SIM_COMPONENTS, TYPED_MESSAGE_FUNC(VehicleDebugComponent::OnLoad,LoadSimComponentsMessage));
		GetSceneObject()->RegisterForMessage(OBJECT_RM_UNLOAD_COMPONENTS, MESSAGE_FUNC(VehicleDebugComponent::OnUnload));
		GetSceneObject()->RegisterForMessage(OBJECT_RM_UNLOAD_COMPONENTS, MESSAGE_FUNC(VehicleDebugComponent::OnUnload));
		GetSceneObject()->RegisterForMessage((SceneObjectMessage) OBJECT_RM_GOTO_POSITION, TYPED_MESSAGE_FUNC(VehicleDebugComponent::OnGotoPosition,AnyMessage));
		
	}

	void VehicleDebugComponent::OnLoad(LoadSimComponentsMessagePtr message)
	{
		//create waypoint text object
		SceneObjectPtr scene_object = GetSceneObject()->GetSceneObjectManager()->LoadFromTemplate("VehicleDebugWaypointTemplate");

		if(!scene_object) //If no VehicleDebugWaypointTemplate template found, create one
		{
			SceneObjectTemplatePtr debug_template (new SceneObjectTemplate);
			debug_template->SetName("VehicleDebugWaypointTemplate");
			ComponentPtr location_comp (ComponentFactory::Get().Create("LocationComponent"));
			location_comp->SetName("LocationComp");
			BaseComponentPtr text_comp = boost::shared_dynamic_cast<BaseComponent>(ComponentFactory::Get().Create("TextComponent"));
			text_comp->SetName("TextComp");
			text_comp->SetPropertyByType("Text",std::string("Next waypoint"));

			BaseComponentPtr bb_comp = boost::shared_dynamic_cast<BaseComponent>(ComponentFactory::Get().Create("BillboardComponent"));
			bb_comp->SetName("BBComp");
			
			bb_comp->SetPropertyByType("Material",std::string("node.dds"));
			bb_comp->SetPropertyByType("Height",float(5));
			bb_comp->SetPropertyByType("Width",float(5));
			
			debug_template->AddComponent(location_comp);
			debug_template->AddComponent(text_comp);
			debug_template->AddComponent(bb_comp);

			SimEngine::Get().GetSimObjectManager()->AddTemplate(debug_template);
			scene_object = GetSceneObject()->GetSceneObjectManager()->LoadFromTemplate("VehicleDebugWaypointTemplate");
		}
		m_WaypointObj = scene_object;
	}
	void VehicleDebugComponent::OnUnload(MessagePtr message)
	{
		GetSceneObject()->GetSceneObjectManager()->DeleteObject(m_WaypointObj);
	}

	void VehicleDebugComponent::OnGotoPosition(AnyMessagePtr message)
	{
		Vec3 pos = boost::any_cast<Vec3>(message->GetData("Position"));
		MessagePtr pos_msg(new PositionMessage(pos));
		m_WaypointObj->PostMessage(pos_msg);
	}
}
