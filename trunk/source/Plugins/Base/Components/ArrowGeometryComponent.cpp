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

#include "ArrowGeometryComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSResourceManager.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSMeshData.h"

#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"


namespace GASS
{
	ArrowGeometryComponent::ArrowGeometryComponent(void) : m_Size(0.01,1),
		//m_Transparency(1),
		m_Color(1,1,1,1)
	{

	}

	ArrowGeometryComponent::~ArrowGeometryComponent(void)
	{
		
	}

	void ArrowGeometryComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("ArrowGeometryComponent",new GASS::Creator<ArrowGeometryComponent, IComponent>);
		RegisterProperty<Vec2>("Size", &GASS::ArrowGeometryComponent::GetSize, &GASS::ArrowGeometryComponent::SetSize);
		RegisterProperty<ResourceHandle>("Texture", &GASS::ArrowGeometryComponent::GetTexture, &GASS::ArrowGeometryComponent::SetTexture);
		RegisterProperty<Vec4>("Color",&ArrowGeometryComponent::GetColor, &ArrowGeometryComponent::SetColor);
	}

	void ArrowGeometryComponent::OnInitialize()
	{
		GenerateMesh();
	}

	Vec2 ArrowGeometryComponent::GetSize() const
	{
		return m_Size;
	}

	void ArrowGeometryComponent::SetSize(const Vec2 &value)
	{
		m_Size = value;
		if(GetSceneObject()) //initlized
		{
			GenerateMesh();
			//SetTexture(m_Texture);
		}
	}

	void ArrowGeometryComponent::GenerateMesh()
	{
		//Vec2 size(m_Size.x*0.5,m_Size.y*0.5);
		ManualMeshDataPtr mesh_data(new ManualMeshData());


		MeshVertex vertex;
		float box_volume = m_Size.x;

		vertex.TexCoord.Set(0,0);
		vertex.Color  = Vec4(1,1,1,1);
		vertex.Normal = Vec3(0,1,0);
		Vec3 offset(-box_volume,box_volume,0);

		vertex.Pos = Vec3(box_volume,box_volume,0)  + offset;
		mesh_data->VertexVector.push_back(vertex);
		vertex.Pos = Vec3(box_volume,-box_volume,0) + offset;
		mesh_data->VertexVector.push_back(vertex);
		vertex.Pos = Vec3(-box_volume,-box_volume,0)+ offset;;
		mesh_data->VertexVector.push_back(vertex);
		vertex.Pos = Vec3(-box_volume,box_volume,0) + offset;
		mesh_data->VertexVector.push_back(vertex);

		vertex.Pos = Vec3(box_volume,box_volume,-m_Size.y) + offset;;
		mesh_data->VertexVector.push_back(vertex);
		vertex.Pos= Vec3(box_volume,-box_volume,-m_Size.y)  + offset;
		mesh_data->VertexVector.push_back(vertex);
		vertex.Pos= Vec3(-box_volume,-box_volume,-m_Size.y) + offset;
		mesh_data->VertexVector.push_back(vertex);
		vertex.Pos= Vec3(-box_volume,box_volume,-m_Size.y)  + offset;
		mesh_data->VertexVector.push_back(vertex);



		mesh_data->IndexVector.push_back(0);
		mesh_data->IndexVector.push_back(4);
		mesh_data->IndexVector.push_back(5);
		mesh_data->IndexVector.push_back(0);
		mesh_data->IndexVector.push_back(5);
		mesh_data->IndexVector.push_back(1);


		mesh_data->IndexVector.push_back(1);
		mesh_data->IndexVector.push_back(5);
		mesh_data->IndexVector.push_back(6);
		mesh_data->IndexVector.push_back(1);
		mesh_data->IndexVector.push_back(6);
		mesh_data->IndexVector.push_back(2);


		mesh_data->IndexVector.push_back(2);
		mesh_data->IndexVector.push_back(6);
		mesh_data->IndexVector.push_back(7);
		mesh_data->IndexVector.push_back(2);
		mesh_data->IndexVector.push_back(7);
		mesh_data->IndexVector.push_back(3);

		mesh_data->IndexVector.push_back(3);
		mesh_data->IndexVector.push_back(7);
		mesh_data->IndexVector.push_back(4);
		mesh_data->IndexVector.push_back(3);
		mesh_data->IndexVector.push_back(4);
		mesh_data->IndexVector.push_back(0);


		mesh_data->IndexVector.push_back(0);
		mesh_data->IndexVector.push_back(1);
		mesh_data->IndexVector.push_back(2);
		mesh_data->IndexVector.push_back(0);
		mesh_data->IndexVector.push_back(2);
		mesh_data->IndexVector.push_back(3);



		//hat
		box_volume = box_volume*4;
		vertex.Pos = Vec3(box_volume,box_volume,-m_Size.y) + offset;;
		mesh_data->VertexVector.push_back(vertex);
		vertex.Pos= Vec3(box_volume,-box_volume,-m_Size.y)  + offset;
		mesh_data->VertexVector.push_back(vertex);
		vertex.Pos= Vec3(-box_volume,-box_volume,-m_Size.y) + offset;
		mesh_data->VertexVector.push_back(vertex);
		vertex.Pos= Vec3(-box_volume,box_volume,-m_Size.y)  + offset;
		mesh_data->VertexVector.push_back(vertex);

		vertex.Pos= Vec3(0,0,-(m_Size.y + box_volume*5))  + offset;
		mesh_data->VertexVector.push_back(vertex);


		mesh_data->IndexVector.push_back(8);
		mesh_data->IndexVector.push_back(9);
		mesh_data->IndexVector.push_back(10);
		mesh_data->IndexVector.push_back(8);
		mesh_data->IndexVector.push_back(10);
		mesh_data->IndexVector.push_back(11);



		mesh_data->IndexVector.push_back(8);
		mesh_data->IndexVector.push_back(12);
		mesh_data->IndexVector.push_back(9);

		mesh_data->IndexVector.push_back(9);
		mesh_data->IndexVector.push_back(12);
		mesh_data->IndexVector.push_back(10);


		mesh_data->IndexVector.push_back(10);
		mesh_data->IndexVector.push_back(12);
		mesh_data->IndexVector.push_back(11);

		mesh_data->IndexVector.push_back(11);
		mesh_data->IndexVector.push_back(12);
		mesh_data->IndexVector.push_back(8);


		mesh_data->Material = "GizmoArrowMat";
		mesh_data->Type = TRIANGLE_LIST;

		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		GetSceneObject()->PostMessage(mesh_message);


		MessagePtr mat_mess(new MaterialMessage(Vec4(0,0,0,m_Color.w),
				Vec3(0,0,0),
				Vec3(0,0,0),
					Vec3(m_Color.x,m_Color.y,m_Color.z),
					0,
					false));
		GetSceneObject()->PostMessage(mat_mess);
	}
	
	void ArrowGeometryComponent::OnLoad(MessagePtr message)
	{
		
		//SetTexture(m_Texture);
	}

	void ArrowGeometryComponent::SetTexture(const ResourceHandle &texture_name)
	{
		m_Texture = texture_name;
		if(GetSceneObject())
		{
			MessagePtr texture_message(new TextureMessage(texture_name.Name()));
			GetSceneObject()->PostMessage(texture_message);
		}
	}

	ResourceHandle ArrowGeometryComponent::GetTexture() const
	{
		return m_Texture;
	}
}
