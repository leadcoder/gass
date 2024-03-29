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

#include "RecastOffmeshMeshConnectionComponent.h"

#include <memory>
#include "Core/Math/GASSMath.h"
#include "Sim/Interface/GASSIManualMeshComponent.h"
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{
	RecastOffmeshMeshConnectionComponent::RecastOffmeshMeshConnectionComponent() : m_ConnectionLine(new GraphicsMesh()),
		
		m_Mode("BIDIRECTIONAL")
	{

	}

	RecastOffmeshMeshConnectionComponent::~RecastOffmeshMeshConnectionComponent()
	{

	}

	void RecastOffmeshMeshConnectionComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<RecastOffmeshMeshConnectionComponent>();
		RegisterGetSet("Radius",&RecastOffmeshMeshConnectionComponent::GetRadius, &RecastOffmeshMeshConnectionComponent::SetRadius);
		RegisterMember("Mode",&RecastOffmeshMeshConnectionComponent::m_Mode);
	}

	void RecastOffmeshMeshConnectionComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RecastOffmeshMeshConnectionComponent::OnStartNodeTransformation,TransformationChangedEvent,0));
		SceneObjectPtr end_node = GetSceneObject()->GetChildByID("AI_OFF_MESH_CHILD_OBJECT");
		if(!end_node)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get child by ID, no child with AI_OFF_MESH_CHILD_OBJECT found", "RecastOffmeshMeshConnectionComponent::OnInitialize");
		end_node->RegisterForMessage(REG_TMESS(RecastOffmeshMeshConnectionComponent::OnEndNodeTransformation,TransformationChangedEvent,0));
		m_EndNode = end_node;

		//initlize visibility
		SetVisible(m_Visible);
		m_Initialized = true;

	}

	void RecastOffmeshMeshConnectionComponent::OnStartNodeTransformation(TransformationChangedEventPtr message)
	{
		m_StartPos = message->GetPosition();
		UpdateConnectionLine();
	}

	void RecastOffmeshMeshConnectionComponent::OnEndNodeTransformation(TransformationChangedEventPtr message)
	{
		m_EndPos = message->GetPosition();
		UpdateConnectionLine();
	}

	void RecastOffmeshMeshConnectionComponent::SetRadius(float value)
	{
		m_Radius = value;
		if(m_Initialized)
			UpdateConnectionLine();
	}

	float RecastOffmeshMeshConnectionComponent::GetRadius() const
	{
		return m_Radius;
	}

	bool RecastOffmeshMeshConnectionComponent::GetVisible()  const
	{
		return m_Visible;
	}

	void RecastOffmeshMeshConnectionComponent::SetVisible(bool value)
	{
		m_Visible = value;
		if(GetSceneObject())
		{
			GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->SetVisible(value);
			if(m_Visible)
				UpdateConnectionLine();
		}
	}

	void RecastOffmeshMeshConnectionComponent::UpdateConnectionLine()
	{
		m_ConnectionLine->SubMeshVector.clear();

		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		m_ConnectionLine->SubMeshVector.push_back(sub_mesh_data);
		sub_mesh_data->Type = LINE_LIST;
		sub_mesh_data->MaterialName = "WhiteTransparentNoLighting";

		ColorRGBA color(0.2,0.2,1,1);

		//draw circles
		const float samples = 24;
		const float rad = 2*static_cast<float>(GASS_PI)/samples;
		float x,y;
		Vec3 pos(0,0,0);
		sub_mesh_data->PositionVector.push_back(pos);
		for(float i = 0 ;i <= samples; i++)
		{
			x = sin(rad*i)*m_Radius;
			y = cos(rad*i)*m_Radius;
			pos.Set(x,0,y);
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(color);
		}
		pos = Vec3(0,0,0);
		sub_mesh_data->PositionVector.push_back(pos);
		sub_mesh_data->ColorVector.push_back(color);

		//draw circles
		Vec3 end_center = m_EndPos - m_StartPos;
		pos = end_center;
		sub_mesh_data->PositionVector.push_back(pos);
		sub_mesh_data->ColorVector.push_back(color);
		sub_mesh_data->PositionVector.push_back(pos);
		sub_mesh_data->ColorVector.push_back(color);

		for(float i = 0 ;i <= samples; i++)
		{
			x = sin(rad*i)*m_Radius;
			y = cos(rad*i)*m_Radius;
			pos.Set(x,0,y);
			pos += end_center;
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(color);
		}

		GetSceneObject()->GetFirstComponentByClass<IManualMeshComponent>()->SetMeshData(*m_ConnectionLine);

	}
}
