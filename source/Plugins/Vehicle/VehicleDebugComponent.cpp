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

#include "VehicleDebugComponent.h"
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObjectTemplateManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneObjectTemplate.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSILocationComponent.h"

namespace GASS
{
	VehicleDebugComponent::VehicleDebugComponent()  
	{

	}

	VehicleDebugComponent::~VehicleDebugComponent()
	{

	}

	void VehicleDebugComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<VehicleDebugComponent>();
//		RegisterGetSet("ShowWaypoint", &VehicleDebugComponent::GetShowWaypoint, &VehicleDebugComponent::SetShowWaypoint);
	}

	void VehicleDebugComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(VehicleDebugComponent::OnGotoPosition,GotoPositionRequest,0));
		
		//create waypoint text object
		SceneObjectPtr scene_object = GetSceneObject()->GetScene()->LoadObjectFromTemplate("VehicleDebugWaypointTemplate",GetSceneObject()->GetScene()->GetRootSceneObject());

		if(!scene_object) //If no VehicleDebugWaypointTemplate template found, create one
		{
			SceneObjectTemplatePtr debug_template (new SceneObjectTemplate);
			debug_template->SetName("VehicleDebugWaypointTemplate");
			ComponentPtr location_comp (ComponentFactory::Get().Create("LocationComponent"));
			location_comp->SetName("LocationComp");
			ComponentPtr text_comp = ComponentFactory::Get().Create("TextComponent");
			text_comp->SetName("TextComp");
			text_comp->SetPropertyValue("Text", std::string("Next waypoint"));

			ComponentPtr bb_comp = ComponentFactory::Get().Create("BillboardComponent");
			bb_comp->SetName("BBComp");
			
			bb_comp->SetPropertyValue("Material", std::string("node.dds"));
			bb_comp->SetPropertyValue("Height", float(5));
			bb_comp->SetPropertyValue("Width", float(5));
			
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

	void VehicleDebugComponent::OnGotoPosition(GotoPositionRequestPtr message)
	{
		Vec3 pos = message->GetPosition();
		m_WaypointObj->GetFirstComponentByClass<ILocationComponent>()->SetPosition(pos);
	}
}
