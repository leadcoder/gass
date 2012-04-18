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
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Components/Graphics/MeshData.h"

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
		ComponentFactory::GetPtr()->Register("HUDComponent",new Creator<HUDComponent, IComponent>);
		RegisterProperty<std::string>("Material", &HUDComponent::GetMaterial, &HUDComponent::SetMaterial);
	}

	void HUDComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(HUDComponent::OnLoad,LoadGameComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(HUDComponent::OnUnload,UnloadComponentsMessage,0));
	}

	void HUDComponent::OnChangeCamera(ChangeCameraMessagePtr message)
	{
		if(message->GetCamera() == 	GetSceneObject()->GetParentSceneObject())
		{
			//show hud
			GetSceneObject()->PostMessage(MessagePtr(new VisibilityMessage(true)));
		}
		else //hide
		{
			GetSceneObject()->PostMessage(MessagePtr(new VisibilityMessage(false)));
		}
	}


	void HUDComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		GetSceneObject()->GetSceneObjectManager()->GetScenario()->UnregisterForMessage(UNREG_TMESS(HUDComponent::OnChangeCamera,ChangeCameraMessage));
	}
	

	void HUDComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		GetSceneObject()->GetSceneObjectManager()->GetScenario()->RegisterForMessage(REG_TMESS(HUDComponent::OnChangeCamera,ChangeCameraMessage,1));
		GetSceneObject()->PostMessage(MessagePtr(new VisibilityMessage(false)));
		UpdateHUD();
		m_Initialized = true;
	}

	void HUDComponent::SetMaterial(const std::string &material) 
	{
		m_Material= material;
		if(m_Initialized)
			UpdateHUD();
	}

	void HUDComponent::UpdateHUD()
	{
		ManualMeshDataPtr mesh_data(new ManualMeshData());
		//mesh_data->Material = "WhiteTransparentNoLighting";
		
		mesh_data->Material = m_Material;
		mesh_data->ScreenSpace = true;

		//mesh_data->Type = LINE_LIST;
		mesh_data->Type = TRIANGLE_LIST;

		MeshVertex vertex;

		Vec2 size;
		size.x = 1;
		size.y = 1;

		
		vertex.TexCoord.Set(0,0);
		vertex.Color = Vec4(1,1,1,1);
		vertex.Pos.Set( size.x ,size.y, 0);
		mesh_data->VertexVector.push_back(vertex);

		vertex.TexCoord.Set(1,0);
		vertex.Color = Vec4(1,1,1,1);
		vertex.Pos.Set( -size.x ,size.y, 0);
		mesh_data->VertexVector.push_back(vertex);

		vertex.TexCoord.Set(1,1);
		vertex.Color = Vec4(1,1,1,1);
		vertex.Pos.Set( -size.x ,-size.y, 0);
		mesh_data->VertexVector.push_back(vertex);

		vertex.TexCoord.Set(0,1);
		vertex.Color = Vec4(1,1,1,1);
		vertex.Pos.Set( size.x ,-size.y, 0);
		mesh_data->VertexVector.push_back(vertex);

		mesh_data->IndexVector.push_back(0);
		mesh_data->IndexVector.push_back(1);
		mesh_data->IndexVector.push_back(2);

		mesh_data->IndexVector.push_back(0);
		mesh_data->IndexVector.push_back(2);
		mesh_data->IndexVector.push_back(3);
		
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		GetSceneObject()->PostMessage(mesh_message);
	}
}



