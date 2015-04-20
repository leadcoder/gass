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

#include "HUDComponent.h"
#include "GameMessages.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"


#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"

#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSIViewport.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/GASSGraphicsMesh.h"

namespace GASS
{
	HUDComponent::HUDComponent()   : m_Initialized(false)
	{
	}

	HUDComponent::~HUDComponent()
	{

	}

	void HUDComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("HUDComponent",new Creator<HUDComponent, Component>);
		RegisterProperty<std::string>("Material", &HUDComponent::GetMaterial, &HUDComponent::SetMaterial);
	}

	void HUDComponent::OnInitialize()
	{
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(HUDComponent::OnCameraChanged,CameraChangedEvent,1));
		GetSceneObject()->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(false)));
		UpdateHUD();
		m_Initialized = true;
	}

	void HUDComponent::OnDelete()
	{
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(HUDComponent::OnCameraChanged,CameraChangedEvent));
	}

	void HUDComponent::OnCameraChanged(CameraChangedEventPtr message)
	{
		CameraComponentPtr camera = message->GetViewport()->GetCamera();
		SceneObjectPtr cam_obj = DYNAMIC_PTR_CAST<BaseSceneComponent>(camera)->GetSceneObject();
		if(cam_obj == 	GetSceneObject()->GetParentSceneObject())
		{
			//show hud
			GetSceneObject()->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(true)));
		}
		else //hide
		{
			GetSceneObject()->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(false)));
		}
	}

	

	void HUDComponent::SetMaterial(const std::string &material) 
	{
		m_Material= material;
		if(m_Initialized)
			UpdateHUD();
	}

	void HUDComponent::UpdateHUD()
	{
		//mesh_data->ScreenSpace = true;

		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);
		sub_mesh_data->MaterialName = m_Material;
		sub_mesh_data->Type = TRIANGLE_LIST;
//		MeshVertex vertex;

		Vec2 size;
		size.x = 1;
		size.y = 1;

		
		Vec4 t1(0,0,0,0);
		Vec3 p1( size.x ,size.y, 0);

		Vec4 t2(1,0,0,0);
		Vec3 p2( -size.x ,size.y, 0);

		Vec4 t3(1,1,0,0);
		Vec3 p3( -size.x ,-size.y, 0);

		Vec4 t4(0,1,0,0);
		Vec3 p4( size.x ,-size.y, 0);

		sub_mesh_data->PositionVector.push_back(p1);
		sub_mesh_data->PositionVector.push_back(p2);
		sub_mesh_data->PositionVector.push_back(p3);
		sub_mesh_data->PositionVector.push_back(p4);

		std::vector<Vec4> tex_coords;
		tex_coords.push_back(t1);
		tex_coords.push_back(t2);
		tex_coords.push_back(t3);
		tex_coords.push_back(t4);

		sub_mesh_data->TexCoordsVector.push_back(tex_coords);


		sub_mesh_data->IndexVector.push_back(0);
		sub_mesh_data->IndexVector.push_back(2);
		sub_mesh_data->IndexVector.push_back(3);
		GetSceneObject()->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(mesh_data)));
	}
}




