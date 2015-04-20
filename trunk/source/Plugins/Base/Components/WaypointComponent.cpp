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


#include "WaypointComponent.h"
#include "WaypointListComponent.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneManagerFactory.h"

#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSGraphicsMesh.h"


namespace GASS
{

	WaypointComponent::WaypointComponent() : m_TangentWeight(1.0),
		m_Initialized(false),
		m_Tangent(0,0,0),
		m_CustomTangent(false)

	{

	}

	WaypointComponent::~WaypointComponent()
	{

	}

	void WaypointComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("WaypointComponent",new Creator<WaypointComponent, Component>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("WaypointComponent", OF_VISIBLE)));
		RegisterProperty<Float>("TangentWeight", &WaypointComponent::GetTangentWeight, &WaypointComponent::SetTangentWeight);
	}

	void WaypointComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointComponent::OnPostInitializedEvent,PostInitializedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointComponent::OnMoved,PositionRequest,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointComponent::OnMoved,WorldPositionRequest,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointComponent::OnRotate,WorldRotationRequest,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointComponent::OnChangeName,SceneObjectNameMessage,0));
		m_Initialized = true;
	}

	void WaypointComponent::OnPostInitializedEvent(PostInitializedEventPtr message)
	{
	
		//notify parent
		SceneObjectPtr tangent = GetSceneObject()->GetFirstChildByName("Tangent",false);
		if(tangent)
		{
			tangent->RegisterForMessage(REG_TMESS(WaypointComponent::OnTangentMoved,WorldPositionRequest,1));
			tangent->RegisterForMessage(REG_TMESS(WaypointComponent::OnTangentMoved,PositionRequest,1));
		}
		else
			std::cout << "Failed to find tangent in waypoint component\n";
		
		SPTR<WaypointListComponent> list = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<WaypointListComponent>();
		if(list)
		{
			bool show = list->GetShowWaypoints();
			GetSceneObject()->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(show)));
			GetSceneObject()->PostRequest(CollisionSettingsRequestPtr(new CollisionSettingsRequest(show)));

			SceneObjectPtr tangent = GetSceneObject()->GetFirstChildByName("Tangent",false);
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
		NotifyUpdate();
		m_Initialized = false;
	}

	void WaypointComponent::OnMoved(MessagePtr message)
	{
		//notify parent
		int id = PTR_TO_INT(this);
		if(id != message->GetSenderID())
		{
			NotifyUpdate();
		}
	}


	void WaypointComponent::OnTangentMoved(MessagePtr message)
	{
		//notify parent
		int id = PTR_TO_INT(this);
		if(id != message->GetSenderID())
		{
			NotifyUpdate();
		}
		UpdateTangentLine();
	}

	void WaypointComponent::OnChangeName(GASS::SceneObjectNameMessagePtr message)
	{
		//notify parent
		//NotifyUpdate();
	}

	void WaypointComponent::Rotate(const Quaternion &rot)
	{
		int id = PTR_TO_INT(this);
		GetSceneObject()->PostRequest(WorldRotationRequestPtr(new WorldRotationRequest(rot,id)));
	}

	void WaypointComponent::OnRotate(WorldRotationRequestPtr message)
	{
		//use custom tangent?
		int id = PTR_TO_INT(this);
		if(id != message->GetSenderID())
		{
			NotifyUpdate();
		}
	}

	void WaypointComponent::NotifyUpdate()
	{
		if(m_Initialized)
		{
			SPTR<WaypointListComponent> list = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<WaypointListComponent>();
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
		NotifyUpdate();
	}

	void WaypointComponent::SetTangentLength(Float value)
	{
		m_TangentWeight = value;
	}

	void WaypointComponent::SetTangent(const Vec3 &tangent)
	{
		//m_Tangent = tangent;
		//if(!m_CustomTangent)
		{

			//rotate node
			Mat4 rot_mat;
			rot_mat.Identity();

			Vec3 up(0,1,0);

			Float l = tangent.Length();
			Vec3 dir = tangent*(1.0/l);

			Vec3 left = Math::Cross(dir,up);
			left.Normalize();
			up = Math::Cross(left,dir);
			up.Normalize();

			rot_mat.SetXAxis(-left);
			rot_mat.SetYAxis(up);
			rot_mat.SetZAxis(dir);

			Quaternion rot;
			rot.FromRotationMatrix(rot_mat);
			int id = PTR_TO_INT(this);
			//GetSceneObject()->PostMessage(MessagePtr(new RotationRequest(rot,id)));


			//GetSceneObject()->GetFirstChildByClass<ILocationComponent>();
			//LocationComponentPtr t_location = GetSceneObject()->GetFirstChildByName("Tangent",false)->GetFirstComponentByClass<ILocationComponent>();
			Vec3 t_pos = tangent;
			SceneObjectPtr tangent = GetSceneObject()->GetFirstChildByName("Tangent",false);
			if(tangent)
				tangent->PostRequest(PositionRequestPtr(new PositionRequest(t_pos*0.1,id)));
			else
				std::cout << "Failed to find tangent in waypoint compoenent\n";

		}
	}

	Vec3 WaypointComponent::GetTangent() const
	{

		LocationComponentPtr t_location = GetSceneObject()->GetFirstChildByName("Tangent",false)->GetFirstComponentByClass<ILocationComponent>(true);
		return t_location->GetPosition()*10;

		LocationComponentPtr wp_location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>(true);
		Mat4 rot_mat;
		rot_mat.Identity();
		Quaternion rot = wp_location->GetRotation();
		rot.ToRotationMatrix(rot_mat);
		Vec3 tangent = rot_mat.GetZAxis();

		return tangent;
	}


	void WaypointComponent::UpdateTangentLine()
	{
		if(!m_Initialized)
			return;

		LocationComponentPtr t_location = GetSceneObject()->GetFirstChildByName("Tangent",false)->GetFirstComponentByClass<ILocationComponent>(true);
		Vec3 t_pos = t_location->GetPosition();

		//collect all children and update path
		//const double line_steps = 115;
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
