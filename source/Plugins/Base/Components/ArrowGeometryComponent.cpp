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

#include "ArrowGeometryComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	ArrowGeometryComponent::ArrowGeometryComponent(void) : m_Size(0.01,1),
		m_Color(1,1,1,1)
	{

	}

	ArrowGeometryComponent::~ArrowGeometryComponent(void)
	{
		
	}

	void ArrowGeometryComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<ArrowGeometryComponent>();
		ADD_DEPENDENCY("LocationComponent")
		ADD_DEPENDENCY("ManualMeshComponent")
		RegisterGetSet("Size", &GASS::ArrowGeometryComponent::GetSize, &GASS::ArrowGeometryComponent::SetSize);
		RegisterGetSet("Texture", &GASS::ArrowGeometryComponent::GetTexture, &GASS::ArrowGeometryComponent::SetTexture);
		RegisterGetSet("Color",&ArrowGeometryComponent::GetColor, &ArrowGeometryComponent::SetColor);
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
		}
	}

	void ArrowGeometryComponent::GenerateMesh()
	{
		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);
	
		Float box_volume = m_Size.x;

		Vec3 offset(-box_volume,box_volume,0);

		Vec3 pos = Vec3(box_volume,box_volume,0)  + offset;
		sub_mesh_data->PositionVector.push_back(pos);
		pos = Vec3(box_volume,-box_volume,0) + offset;
		sub_mesh_data->PositionVector.push_back(pos);
		pos = Vec3(-box_volume,-box_volume,0)+ offset;;
		sub_mesh_data->PositionVector.push_back(pos);
		pos = Vec3(-box_volume,box_volume,0) + offset;
		sub_mesh_data->PositionVector.push_back(pos);

		pos = Vec3(box_volume,box_volume,-m_Size.y) + offset;;
		sub_mesh_data->PositionVector.push_back(pos);
		pos = Vec3(box_volume,-box_volume,-m_Size.y)  + offset;
		sub_mesh_data->PositionVector.push_back(pos);
		pos = Vec3(-box_volume,-box_volume,-m_Size.y) + offset;
		sub_mesh_data->PositionVector.push_back(pos);
		pos = Vec3(-box_volume,box_volume,-m_Size.y)  + offset;
		sub_mesh_data->PositionVector.push_back(pos);



		sub_mesh_data->IndexVector.push_back(0);
		sub_mesh_data->IndexVector.push_back(4);
		sub_mesh_data->IndexVector.push_back(5);
		sub_mesh_data->IndexVector.push_back(0);
		sub_mesh_data->IndexVector.push_back(5);
		sub_mesh_data->IndexVector.push_back(1);


		sub_mesh_data->IndexVector.push_back(1);
		sub_mesh_data->IndexVector.push_back(5);
		sub_mesh_data->IndexVector.push_back(6);
		sub_mesh_data->IndexVector.push_back(1);
		sub_mesh_data->IndexVector.push_back(6);
		sub_mesh_data->IndexVector.push_back(2);


		sub_mesh_data->IndexVector.push_back(2);
		sub_mesh_data->IndexVector.push_back(6);
		sub_mesh_data->IndexVector.push_back(7);
		sub_mesh_data->IndexVector.push_back(2);
		sub_mesh_data->IndexVector.push_back(7);
		sub_mesh_data->IndexVector.push_back(3);

		sub_mesh_data->IndexVector.push_back(3);
		sub_mesh_data->IndexVector.push_back(7);
		sub_mesh_data->IndexVector.push_back(4);
		sub_mesh_data->IndexVector.push_back(3);
		sub_mesh_data->IndexVector.push_back(4);
		sub_mesh_data->IndexVector.push_back(0);


		sub_mesh_data->IndexVector.push_back(0);
		sub_mesh_data->IndexVector.push_back(1);
		sub_mesh_data->IndexVector.push_back(2);
		sub_mesh_data->IndexVector.push_back(0);
		sub_mesh_data->IndexVector.push_back(2);
		sub_mesh_data->IndexVector.push_back(3);



		//hat
		box_volume = box_volume*4;
		pos = Vec3(box_volume,box_volume,-m_Size.y) + offset;;
		sub_mesh_data->PositionVector.push_back(pos);
		pos = Vec3(box_volume,-box_volume,-m_Size.y)  + offset;
		sub_mesh_data->PositionVector.push_back(pos);
		pos = Vec3(-box_volume,-box_volume,-m_Size.y) + offset;
		sub_mesh_data->PositionVector.push_back(pos);
		pos = Vec3(-box_volume,box_volume,-m_Size.y)  + offset;
		sub_mesh_data->PositionVector.push_back(pos);

		pos = Vec3(0,0,-(m_Size.y + box_volume*5))  + offset;
		sub_mesh_data->PositionVector.push_back(pos);


		sub_mesh_data->IndexVector.push_back(8);
		sub_mesh_data->IndexVector.push_back(9);
		sub_mesh_data->IndexVector.push_back(10);
		sub_mesh_data->IndexVector.push_back(8);
		sub_mesh_data->IndexVector.push_back(10);
		sub_mesh_data->IndexVector.push_back(11);



		sub_mesh_data->IndexVector.push_back(8);
		sub_mesh_data->IndexVector.push_back(12);
		sub_mesh_data->IndexVector.push_back(9);

		sub_mesh_data->IndexVector.push_back(9);
		sub_mesh_data->IndexVector.push_back(12);
		sub_mesh_data->IndexVector.push_back(10);


		sub_mesh_data->IndexVector.push_back(10);
		sub_mesh_data->IndexVector.push_back(12);
		sub_mesh_data->IndexVector.push_back(11);

		sub_mesh_data->IndexVector.push_back(11);
		sub_mesh_data->IndexVector.push_back(12);
		sub_mesh_data->IndexVector.push_back(8);


		sub_mesh_data->MaterialName = "GizmoArrowMat";
		sub_mesh_data->Type = TRIANGLE_LIST;

		
		GetSceneObject()->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(mesh_data)));

		/*MessagePtr mat_mess(new MaterialMessage(Vec4(0,0,0,m_Color.w),
				Vec3(0,0,0),
				Vec3(0,0,0),
					Vec3(m_Color.x,m_Color.y,m_Color.z),
					0,
					false));
		GetSceneObject()->PostMessage(mat_mess);*/
	}
	
	void ArrowGeometryComponent::OnLoad(MessagePtr message)
	{
		
	}

	void ArrowGeometryComponent::SetTexture(const ResourceHandle &texture_name)
	{
		m_Texture = texture_name;
	}

	ResourceHandle ArrowGeometryComponent::GetTexture() const
	{
		return m_Texture;
	}
}
