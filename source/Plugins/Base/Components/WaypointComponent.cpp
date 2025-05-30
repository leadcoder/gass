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

#include "WaypointComponent.h"

#include <memory>
#include "WaypointListComponent.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/Math/GASSMath.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSIManualMeshComponent.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/GASSComponentFactory.h"

namespace GASS
{
	WaypointComponent::WaypointComponent() : 
		m_Tangent(0,0,0)
		
	{

	}

	WaypointComponent::~WaypointComponent()
	{

	}

	void WaypointComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<WaypointComponent>();
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("WaypointComponent", OF_VISIBLE));
		RegisterGetSet("CustomTangent", &WaypointComponent::GetCustomTangent, &WaypointComponent::SetCustomTangent, PF_VISIBLE | PF_EDITABLE,"");
		RegisterMember("TangentWeight", &WaypointComponent::m_TangentWeight, PF_VISIBLE | PF_EDITABLE, "");
	}

	void WaypointComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointComponent::OnTransformation, TransformationChangedEvent, 0));
	}

	void WaypointComponent::OnSceneObjectInitialized()
	{
		auto list = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<WaypointListComponent>();
		if (list)
		{
			bool show = list->GetShowWaypoints();
			GetSceneObject()->SetVisible(show);
			SceneObjectPtr tangent = GetOrCreateTangent();
			if (tangent)
			{
				tangent->RegisterForMessage(REG_TMESS(WaypointComponent::OnTangentTransformation, TransformationChangedEvent, 1));
				tangent->SetVisible(show);
			}
		}
		m_Initialized = true;
	}

	void WaypointComponent::OnDelete()
	{
		//notify parent
		m_Active = false;
		NotifyUpdate();
		m_Initialized = false;
	}

	void WaypointComponent::OnTransformation(TransformationChangedEventPtr /*message*/)
	{
		if(m_TrackTransformation)
			NotifyUpdate();
	}

	void WaypointComponent::OnTangentTransformation(TransformationChangedEventPtr /*message*/)
	{
		//notify parent
		if(m_TrackTransformation)
		{
			NotifyUpdate();
		}
		UpdateTangentLine();
	}

	void WaypointComponent::Rotate(const Quaternion &rot)
	{
		m_TrackTransformation = false;
		GetSceneObject()->SetRotation(rot);
		m_TrackTransformation = true;
	}

	void WaypointComponent::NotifyUpdate()
	{
		if(m_Initialized)
		{
			auto list = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<WaypointListComponent>();
			if(list)
				list->SetDirty(true);
		}
	}

	void WaypointComponent::SetTangent(const Vec3 &tangent)
	{
		SceneObjectPtr tangent_obj = GetOrCreateTangent();
		if (tangent_obj && tangent_obj->GetFirstComponentByClass<ILocationComponent>())
		{
			m_TrackTransformation = false;
			tangent_obj->GetFirstComponentByClass<ILocationComponent>()->SetPosition(tangent*0.1);
			m_TrackTransformation = true;
		}
	}

	Vec3 WaypointComponent::GetTangent()
	{
		SceneObjectPtr tangent = GetOrCreateTangent();
		if (tangent)
		{
			LocationComponentPtr t_location = GetOrCreateTangent()->GetFirstComponentByClass<ILocationComponent>(true);
			return tangent->GetPosition() * 10;
		}
		return Vec3(0, 1, 0);
	}

	void WaypointComponent::SetCustomTangent(bool value)
	{ 
		m_CustomTangent = value;
		if(!m_CustomTangent && m_Initialized)
			NotifyUpdate();
	}

	void WaypointComponent::UpdateTangentLine()
	{
		if(!m_Initialized)
			return;

		auto t_location = GetOrCreateTangent()->GetFirstComponentByClass<ILocationComponent>(true);
		Vec3 t_pos = t_location->GetPosition();

		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);
		sub_mesh_data->MaterialName = "WhiteTransparentNoLighting";

		sub_mesh_data->Type = LINE_LIST;
		Vec3 pos(0,0,0);
		sub_mesh_data->PositionVector.push_back(pos);

		pos = t_pos;
		sub_mesh_data->PositionVector.push_back(pos);
		GetSceneObject()->GetFirstComponentByClass<IManualMeshComponent>()->SetMeshData(*mesh_data);
	}

	SceneObjectPtr WaypointComponent::GetOrCreateTangent()
	{
		auto obj = m_TangentObject.lock();
		if (obj)
			return obj;
#if 1 //Legacy
		obj = GetSceneObject()->GetFirstChildByName("Tangent", false);

		if (obj)
		{
			m_TangentObject = obj;
		}
		return obj;
#else
		obj = std::make_shared<SceneObject>();
		obj->SetName("Tangent");
		obj->SetID("TANGENT_OBJECT");
		obj->SetSerialize(false);
		ComponentPtr location_comp = ComponentFactory::Get().Create("LocationComponent");
		location_comp->SetPropertyValue("AttachToParent", true);
		obj->AddComponent(location_comp);
		ComponentPtr bb_comp = ComponentFactory::Get().Create("BillboardComponent");
		bb_comp->SetPropertyValue("Material", std::string("default.dds"));
		bb_comp->SetPropertyValue("Height", 1.0f);
		bb_comp->SetPropertyValue("Width", 1.0f);
		obj->AddComponent(bb_comp);
		m_TangentObject = obj;
		GetSceneObject()->AddChildSceneObject(obj, true);
		return obj;
#endif
	}
}
