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

#include "BoxGeometryComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	BoxGeometryComponent::BoxGeometryComponent(void) : m_Size(1,1,1), 
		m_Lines(true)
	{

	}

	BoxGeometryComponent::~BoxGeometryComponent(void)
	{

	}

	void BoxGeometryComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<BoxGeometryComponent>();
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("BoxGeometryComponent", OF_VISIBLE)));
	
		RegisterProperty<Vec3>("Size", &GASS::BoxGeometryComponent::GetSize, &GASS::BoxGeometryComponent::SetSize,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Size of box",PF_VISIBLE | PF_EDITABLE)));

		RegisterGetSet("Lines", &GASS::BoxGeometryComponent::GetLines, &GASS::BoxGeometryComponent::SetLines,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Wireframe or solid",PF_VISIBLE | PF_EDITABLE)));
	}

	void BoxGeometryComponent::OnInitialize()
	{
		const std::string mat_name = "BoxMaterial";
		GraphicsSystemPtr gfx_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IGraphicsSystem>();
		if(!gfx_sys->HasMaterial(mat_name))
		{
			GraphicsMaterial mat;
			mat.Name = mat_name;
			mat.Diffuse.Set(1,1,1,1);
			mat.Ambient.Set(1,1,1);
			mat.DepthTest = true;
			mat.TrackVertexColor = false;
			gfx_sys->AddMaterial(mat);
		}
		UpdateMesh();
	}

	Vec3 BoxGeometryComponent::GetSize() const
	{
		return m_Size;
	}

	void BoxGeometryComponent::SetSize(const Vec3 &value)
	{
		m_Size = value;
		if(GetSceneObject())
			UpdateMesh();
	}

	bool BoxGeometryComponent::GetLines() const
	{
		return m_Lines;
	}

	void BoxGeometryComponent::SetLines(bool value)
	{
		m_Lines = value;
		if (GetSceneObject())
			UpdateMesh();
	}

	void BoxGeometryComponent::UpdateMesh()
	{
		const Vec3 size= m_Size*0.5;
		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);
	
		sub_mesh_data->MaterialName = "BoxMaterial";

		if(m_Lines)
		{
			std::vector<Vec3> conrners;
			conrners.push_back(Vec3( size.x ,size.y , size.z));
			conrners.push_back(Vec3(-size.x ,size.y , size.z));
			conrners.push_back(Vec3(-size.x ,size.y ,-size.z));
			conrners.push_back(Vec3( size.x ,size.y ,-size.z));

			conrners.push_back(Vec3( size.x ,-size.y , size.z));
			conrners.push_back(Vec3(-size.x ,-size.y , size.z));
			conrners.push_back(Vec3(-size.x ,-size.y ,-size.z));
			conrners.push_back(Vec3( size.x ,-size.y ,-size.z));

			sub_mesh_data->Type = LINE_LIST;

			for(size_t i = 0; i < 4; i++)
			{
				Vec3 pos =conrners[i];
				sub_mesh_data->PositionVector.push_back(pos);
				pos =conrners[(i+1)%4];
				sub_mesh_data->PositionVector.push_back(pos);

				pos =conrners[i];
				sub_mesh_data->PositionVector.push_back(pos);
				pos =conrners[i+4];
				sub_mesh_data->PositionVector.push_back(pos);
			}

			for(size_t i = 0; i < 4; i++)
			{
				Vec3 pos = conrners[4 + i];
				sub_mesh_data->PositionVector.push_back(pos);
				pos =conrners[4 + ((i+1)%4)];
				sub_mesh_data->PositionVector.push_back(pos);
			}
		}
		else
		{

			sub_mesh_data->Type = TRIANGLE_LIST;
			std::vector<Vec4> tex_coords;

			sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,-size.y , -size.z));
			sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,-size.y , -size.z));
			sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,-size.y , -size.z));
			tex_coords.push_back(Vec4( 0, 0 , 0,0));
			tex_coords.push_back(Vec4( 0, 0 , 0,0));
			tex_coords.push_back(Vec4( 0, 0 , 0,0));
			
			sub_mesh_data->PositionVector.push_back(Vec3( size.x ,-size.y , -size.z));
			sub_mesh_data->PositionVector.push_back(Vec3( size.x ,-size.y , -size.z));
			sub_mesh_data->PositionVector.push_back(Vec3( size.x ,-size.y , -size.z));
			tex_coords.push_back(Vec4( 1, 0 , 0,0));
			tex_coords.push_back(Vec4( 1, 0 , 0,0));
			tex_coords.push_back(Vec4( 1, 0 , 0,0));


			sub_mesh_data->PositionVector.push_back(Vec3( size.x ,size.y , -size.z));
			sub_mesh_data->PositionVector.push_back(Vec3( size.x ,size.y , -size.z));
			sub_mesh_data->PositionVector.push_back(Vec3( size.x ,size.y , -size.z));
			tex_coords.push_back(Vec4( 1, 1 , 0,0));
			tex_coords.push_back(Vec4( 1, 1 , 0,0));
			tex_coords.push_back(Vec4( 1, 1 , 0,0));

			
			sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,size.y , -size.z));
			sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,size.y , -size.z));
			sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,size.y , -size.z));
			tex_coords.push_back(Vec4( 0, 1 , 0,0));
			tex_coords.push_back(Vec4( 0, 1 , 0,0));
			tex_coords.push_back(Vec4( 0, 1 , 0,0));


			sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,-size.y , size.z));
			sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,-size.y , size.z));
			sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,-size.y , size.z));
			tex_coords.push_back(Vec4( 1, 0 , 0,0));
			tex_coords.push_back(Vec4( 1, 0 , 0,0));
			tex_coords.push_back(Vec4( 1, 0 , 0,0));


			sub_mesh_data->PositionVector.push_back(Vec3( size.x ,-size.y , size.z));
			sub_mesh_data->PositionVector.push_back(Vec3( size.x ,-size.y , size.z));
			sub_mesh_data->PositionVector.push_back(Vec3( size.x ,-size.y , size.z));
			tex_coords.push_back(Vec4( 0, 0 , 0,0));
			tex_coords.push_back(Vec4( 0, 0 , 0,0));
			tex_coords.push_back(Vec4( 0, 0 , 0,0));

			sub_mesh_data->PositionVector.push_back(Vec3( size.x ,size.y , size.z));
			sub_mesh_data->PositionVector.push_back(Vec3( size.x ,size.y , size.z));
			sub_mesh_data->PositionVector.push_back(Vec3( size.x ,size.y , size.z));
			tex_coords.push_back(Vec4( 0, 1 , 0,0));
			tex_coords.push_back(Vec4( 0, 1 , 0,0));
			tex_coords.push_back(Vec4( 0, 1 , 0,0));


			sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,size.y , size.z));
			sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,size.y , size.z));
			sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,size.y , size.z));
			tex_coords.push_back(Vec4( 1, 1 , 0,0));
			tex_coords.push_back(Vec4( 1, 1 , 0,0));
			tex_coords.push_back(Vec4( 1, 1 , 0,0));

			sub_mesh_data->NormalVector.push_back(Vec3(0,0,-1));
			sub_mesh_data->NormalVector.push_back(Vec3(-1,0,0));
			sub_mesh_data->NormalVector.push_back(Vec3(0,-1,0));
			sub_mesh_data->NormalVector.push_back(Vec3(0,0,-1));
			sub_mesh_data->NormalVector.push_back(Vec3(1,0,0));
			sub_mesh_data->NormalVector.push_back(Vec3(0,-1,0));
			sub_mesh_data->NormalVector.push_back(Vec3(0,0,-1));
			sub_mesh_data->NormalVector.push_back(Vec3(1,0,0));
			sub_mesh_data->NormalVector.push_back(Vec3(0,1,0));
			sub_mesh_data->NormalVector.push_back(Vec3(0,0,-1));
			sub_mesh_data->NormalVector.push_back(Vec3(-1,0,0));
			sub_mesh_data->NormalVector.push_back(Vec3(0,1,0));
			sub_mesh_data->NormalVector.push_back(Vec3(0,0,1));
			sub_mesh_data->NormalVector.push_back(Vec3(-1,0,0));
			sub_mesh_data->NormalVector.push_back(Vec3(0,-1,0));
			sub_mesh_data->NormalVector.push_back(Vec3(0,0,1));
			sub_mesh_data->NormalVector.push_back(Vec3(1,-0,0));
			sub_mesh_data->NormalVector.push_back(Vec3(0,-1,0));
			sub_mesh_data->NormalVector.push_back(Vec3(0,0,1));
			sub_mesh_data->NormalVector.push_back(Vec3(1,0,0));
			sub_mesh_data->NormalVector.push_back(Vec3(0,1,0));
			sub_mesh_data->NormalVector.push_back(Vec3(-0,0,1));
			sub_mesh_data->NormalVector.push_back(Vec3(-1,-0,-0));
			sub_mesh_data->NormalVector.push_back(Vec3(0,1,0));

			//bottom
			sub_mesh_data->IndexVector.push_back(0);
			sub_mesh_data->IndexVector.push_back(9);
			sub_mesh_data->IndexVector.push_back(3);
			sub_mesh_data->IndexVector.push_back(9);
			sub_mesh_data->IndexVector.push_back(6);
			sub_mesh_data->IndexVector.push_back(3);

			// top
			sub_mesh_data->IndexVector.push_back(21);  //face 3
			sub_mesh_data->IndexVector.push_back(12);
			sub_mesh_data->IndexVector.push_back(18);
			sub_mesh_data->IndexVector.push_back(12);  //face 4
			sub_mesh_data->IndexVector.push_back(15);
			sub_mesh_data->IndexVector.push_back(18);
			// left
			sub_mesh_data->IndexVector.push_back(22);  //face 5
			sub_mesh_data->IndexVector.push_back(10);
			sub_mesh_data->IndexVector.push_back(13);
			sub_mesh_data->IndexVector.push_back(10);  //face 6
			sub_mesh_data->IndexVector.push_back(1);
			sub_mesh_data->IndexVector.push_back(13);
			// right
			sub_mesh_data->IndexVector.push_back(16);  //face 7
			sub_mesh_data->IndexVector.push_back(4);
			sub_mesh_data->IndexVector.push_back(19);
			sub_mesh_data->IndexVector.push_back(4);  //face 8
			sub_mesh_data->IndexVector.push_back(7);
			sub_mesh_data->IndexVector.push_back(19);
			// front
			sub_mesh_data->IndexVector.push_back(14);  //face 9
			sub_mesh_data->IndexVector.push_back(2);
			sub_mesh_data->IndexVector.push_back(17);
			sub_mesh_data->IndexVector.push_back(2);   //face 10
			sub_mesh_data->IndexVector.push_back(5);
			sub_mesh_data->IndexVector.push_back(17);
			// back
			sub_mesh_data->IndexVector.push_back(20);  //face 11
			sub_mesh_data->IndexVector.push_back(8);
			sub_mesh_data->IndexVector.push_back(23);
			sub_mesh_data->IndexVector.push_back(8);   //face 12
			sub_mesh_data->IndexVector.push_back(11);
			sub_mesh_data->IndexVector.push_back(23);

			sub_mesh_data->TexCoordsVector.push_back(tex_coords);
		}
		GetSceneObject()->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(mesh_data)));
	}

	bool BoxGeometryComponent::IsPointInside(const Vec3 &point) const
	{
		LocationComponentPtr loc_comp = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		assert(loc_comp);
		Vec3 pos = loc_comp->GetWorldPosition();
		AABox box;
		box.Min = pos - m_Size * 0.5;
		box.Max = pos + m_Size * 0.5;
		return box.PointInside(point);
	}

	Vec3 BoxGeometryComponent::GetRandomPoint() const
	{
		Vec3 location;
		LocationComponentPtr loc_comp = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();

		assert(loc_comp);

		Vec3 pos = loc_comp->GetWorldPosition();
		Quaternion rot = loc_comp->GetWorldRotation();

		Float rand1 = rand() / Float(RAND_MAX);
		Float rand2 = rand() / Float(RAND_MAX);
		Float rand3 = rand() / Float(RAND_MAX);

		location.x = (2*rand1-1) * m_Size.x*0.5;
		location.y = (2*rand2-1) * m_Size.y*0.5;
		location.z = (2*rand3-1) * m_Size.z*0.5;

		Mat4 trans(rot,pos);
		location = trans*location;

		return location;
	}
}
