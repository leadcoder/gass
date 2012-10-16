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
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"

#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"

#include "Sim/Scene/GASSSceneObjectTemplate.h"

#include "Sim/Systems/Resource/GASSIResourceSystem.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Scheduling/GASSIRuntimeController.h"


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

	void VehicleDebugComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(VehicleDebugComponent::OnGotoPosition,GotoPositionMessage,0));
		
		//create waypoint text object
		SceneObjectPtr scene_object = GetSceneObject()->GetScene()->LoadObjectFromTemplate("VehicleDebugWaypointTemplate",GetSceneObject()->GetScene()->GetRootSceneObject());

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

			SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(debug_template);
			scene_object = GetSceneObject()->GetScene()->LoadObjectFromTemplate("VehicleDebugWaypointTemplate",GetSceneObject()->GetScene()->GetRootSceneObject());
		}
		m_WaypointObj = scene_object;
	}
	void VehicleDebugComponent::OnDelete()
	{
		m_WaypointObj->GetParentSceneObject()->RemoveChildSceneObject(m_WaypointObj);
		m_WaypointObj.reset();
		//GetSceneObject()->GetScene()->DeleteObject(m_WaypointObj);
	}

	void VehicleDebugComponent::OnGotoPosition(GotoPositionMessagePtr message)
	{
		Vec3 pos = message->GetPosition();
		MessagePtr pos_msg(new PositionMessage(pos));
		m_WaypointObj->PostMessage(pos_msg);
	}
}
