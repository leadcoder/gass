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
#include "WaypointListComponent.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/Math/GASSMath.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSGraphicsMesh.h"

namespace GASS
{
	WaypointComponent::WaypointComponent() : m_TangentWeight(1.0),
		m_Initialized(false),
		m_Tangent(0,0,0),
		m_CustomTangent(false),
		m_Active(true),
		m_TrackTransformation(true)
	{

	}

	WaypointComponent::~WaypointComponent()
	{

	}

	void WaypointComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<WaypointComponent>();
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("WaypointComponent", OF_VISIBLE)));
		RegisterProperty<Float>("TangentWeight", &WaypointComponent::GetTangentWeight, &WaypointComponent::SetTangentWeight);
		RegisterProperty<bool>("CustomTangent", &WaypointComponent::GetCustomTangent, &WaypointComponent::SetCustomTangent,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("", PF_VISIBLE | PF_EDITABLE)));
	}

	void WaypointComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointComponent::OnPostInitializedEvent,PostInitializedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointComponent::OnTransformation, TransformationChangedEvent, 0));
		m_Initialized = true;
	}

	void WaypointComponent::OnPostInitializedEvent(PostInitializedEventPtr message)
	{
		//notify parent
		SceneObjectPtr tangent = GetSceneObject()->GetFirstChildByName("Tangent",false);
		if(tangent)
		{
			tangent->RegisterForMessage(REG_TMESS(WaypointComponent::OnTangentTransformation, TransformationChangedEvent,1));
		}
		else
			std::cout << "Failed to find tangent in waypoint component\n";
		
		GASS_SHARED_PTR<WaypointListComponent> list = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<WaypointListComponent>();
		if(list)
		{
			bool show = list->GetShowWaypoints();
			GetSceneObject()->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(show)));
			GetSceneObject()->PostRequest(CollisionSettingsRequestPtr(new CollisionSettingsRequest(show)));

			if(tangent)
			{
				tangent->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(show)));
				tangent->PostRequest(CollisionSettingsRequestPtr(new CollisionSettingsRequest(show)));
			}
		}
	}

	void WaypointComponent::OnDelete()
	{
		//notify parent
		m_Active = false;
		_NotifyUpdate();
		m_Initialized = false;
	}

	void WaypointComponent::OnTransformation(TransformationChangedEventPtr event)
	{
		if(m_TrackTransformation)
			_NotifyUpdate();
	}

	void WaypointComponent::OnTangentTransformation(TransformationChangedEventPtr message)
	{
		//notify parent
		if(m_TrackTransformation)
		{
			_NotifyUpdate();
		}
		_UpdateTangentLine();
	}

	void WaypointComponent::Rotate(const Quaternion &rot)
	{
		m_TrackTransformation = false;
		GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->SetRotation(rot);
		m_TrackTransformation = true;
	}

	void WaypointComponent::_NotifyUpdate()
	{
		if(m_Initialized)
		{
			GASS_SHARED_PTR<WaypointListComponent> list = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<WaypointListComponent>();
			if(list)
				list->UpdatePath();
		}
	}

	Float WaypointComponent::GetTangentWeight()const
	{
		return m_TangentWeight;
	}

	void WaypointComponent::SetTangentWeight(Float value)
	{
		m_TangentWeight = value;
		_NotifyUpdate();
	}

	/*void WaypointComponent::SetTangentLength(Float value)
	{
		m_TangentWeight = value;
	}*/

	void WaypointComponent::SetTangent(const Vec3 &tangent)
	{
		SceneObjectPtr tangent_obj = GetSceneObject()->GetFirstChildByName("Tangent", false);
		if (tangent_obj && tangent_obj->GetFirstComponentByClass<ILocationComponent>())
		{
			m_TrackTransformation = false;
			tangent_obj->GetFirstComponentByClass<ILocationComponent>()->SetPosition(tangent*0.1);
			m_TrackTransformation = true;
		}
		else
			std::cout << "Failed to find tangent in waypoint compoenent\n";
	}

	Vec3 WaypointComponent::GetTangent() const
	{
		LocationComponentPtr t_location = GetSceneObject()->GetFirstChildByName("Tangent",false)->GetFirstComponentByClass<ILocationComponent>(true);
		return t_location->GetPosition()*10;
	}

	void WaypointComponent::SetCustomTangent(bool value)
	{ 
		m_CustomTangent = value;
		if(!m_CustomTangent && m_Initialized)
			_NotifyUpdate();
	}

	void WaypointComponent::_UpdateTangentLine()
	{
		if(!m_Initialized)
			return;

		LocationComponentPtr t_location = GetSceneObject()->GetFirstChildByName("Tangent",false)->GetFirstComponentByClass<ILocationComponent>(true);
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
		GetSceneObject()->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(mesh_data)));
	}
}
