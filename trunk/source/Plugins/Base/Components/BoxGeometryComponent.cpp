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

#include "BoxGeometryComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
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
		GASS::ComponentFactory::GetPtr()->Register("BoxGeometryComponent",new GASS::Creator<BoxGeometryComponent, IComponent>);
		RegisterProperty<Vec3>("Size", &GASS::BoxGeometryComponent::GetSize, &GASS::BoxGeometryComponent::SetSize);
		RegisterProperty<bool>("Lines", &GASS::BoxGeometryComponent::GetLines, &GASS::BoxGeometryComponent::SetLines);
	}

	void BoxGeometryComponent::OnInitialize()
	{
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


	void BoxGeometryComponent::UpdateMesh()
	{
		Vec3 size= m_Size*0.5;
		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);
	
		sub_mesh_data->MaterialName = "Blue";

		std::vector<Vec3> conrners;
		std::vector<Vec4> tex_coords;

		conrners.push_back(Vec3( size.x ,size.y , size.z));
		conrners.push_back(Vec3(-size.x ,size.y , size.z));
		conrners.push_back(Vec3(-size.x ,size.y ,-size.z));
		conrners.push_back(Vec3( size.x ,size.y ,-size.z));

		conrners.push_back(Vec3( size.x ,-size.y , size.z));
		conrners.push_back(Vec3(-size.x ,-size.y , size.z));
		conrners.push_back(Vec3(-size.x ,-size.y ,-size.z));
		conrners.push_back(Vec3( size.x ,-size.y ,-size.z));

		if(m_Lines)
		{
			sub_mesh_data->Type = LINE_LIST;

			for(int i = 0; i < 4; i++)
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

			for(int i = 0; i < 4; i++)
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
			std::vector<Vec3> postions;

			postions.push_back(Vec3( -size.x ,-size.y , -size.z));
			postions.push_back(Vec3( -size.x ,-size.y , -size.z));
			postions.push_back(Vec3( -size.x ,-size.y , -size.z));
			tex_coords.push_back(Vec4( 0, 0 , 0,0));
			tex_coords.push_back(Vec4( 0, 0 , 0,0));
			tex_coords.push_back(Vec4( 0, 0 , 0,0));

			postions.push_back(Vec3( size.x ,-size.y , -size.z));
			postions.push_back(Vec3( size.x ,-size.y , -size.z));
			postions.push_back(Vec3( size.x ,-size.y , -size.z));
			tex_coords.push_back(Vec4( 1, 0 , 0,0));
			tex_coords.push_back(Vec4( 1, 0 , 0,0));
			tex_coords.push_back(Vec4( 1, 0 , 0,0));


			postions.push_back(Vec3( size.x ,size.y , -size.z));
			postions.push_back(Vec3( size.x ,size.y , -size.z));
			postions.push_back(Vec3( size.x ,size.y , -size.z));
			tex_coords.push_back(Vec4( 1, 1 , 0,0));
			tex_coords.push_back(Vec4( 1, 1 , 0,0));
			tex_coords.push_back(Vec4( 1, 1 , 0,0));

			
			postions.push_back(Vec3( -size.x ,size.y , -size.z));
			postions.push_back(Vec3( -size.x ,size.y , -size.z));
			postions.push_back(Vec3( -size.x ,size.y , -size.z));
			tex_coords.push_back(Vec4( 0, 1 , 0,0));
			tex_coords.push_back(Vec4( 0, 1 , 0,0));
			tex_coords.push_back(Vec4( 0, 1 , 0,0));


			postions.push_back(Vec3( -size.x ,-size.y , size.z));
			postions.push_back(Vec3( -size.x ,-size.y , size.z));
			postions.push_back(Vec3( -size.x ,-size.y , size.z));
			tex_coords.push_back(Vec4( 1, 0 , 0,0));
			tex_coords.push_back(Vec4( 1, 0 , 0,0));
			tex_coords.push_back(Vec4( 1, 0 , 0,0));


			postions.push_back(Vec3( size.x ,-size.y , size.z));
			postions.push_back(Vec3( size.x ,-size.y , size.z));
			postions.push_back(Vec3( size.x ,-size.y , size.z));
			tex_coords.push_back(Vec4( 0, 0 , 0,0));
			tex_coords.push_back(Vec4( 0, 0 , 0,0));
			tex_coords.push_back(Vec4( 0, 0 , 0,0));

			postions.push_back(Vec3( size.x ,size.y , size.z));
			postions.push_back(Vec3( size.x ,size.y , size.z));
			postions.push_back(Vec3( size.x ,size.y , size.z));
			tex_coords.push_back(Vec4( 0, 1 , 0,0));
			tex_coords.push_back(Vec4( 0, 1 , 0,0));
			tex_coords.push_back(Vec4( 0, 1 , 0,0));


			postions.push_back(Vec3( -size.x ,size.y , size.z));
			postions.push_back(Vec3( -size.x ,size.y , size.z));
			postions.push_back(Vec3( -size.x ,size.y , size.z));
			tex_coords.push_back(Vec4( 1, 1 , 0,0));
			tex_coords.push_back(Vec4( 1, 1 , 0,0));
			tex_coords.push_back(Vec4( 1, 1 , 0,0));

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


			std::vector<Vec3> normals;

			normals.push_back(Vec3(+1, 0, 0));
			normals.push_back(Vec3(-1, 0, 0));
			normals.push_back(Vec3(0, +1, 0));
			normals.push_back(Vec3(0, -1, 0));
			normals.push_back(Vec3(0, 0, +1));
			normals.push_back(Vec3(0, 0, -1));

			std::vector<int> normal_indcies;

			// bottom front left                   

			normal_indcies.push_back(5);
			normal_indcies.push_back(3);
			normal_indcies.push_back(0);
			// bottom front right
			normal_indcies.push_back(5);
			normal_indcies.push_back(2);
			normal_indcies.push_back(0);
			// bottom back right
			normal_indcies.push_back(5);
			normal_indcies.push_back(2);
			normal_indcies.push_back(1);
			// bottom back left
			normal_indcies.push_back(5);
			normal_indcies.push_back(3);
			normal_indcies.push_back(1);
			// top front left                  
			normal_indcies.push_back(4);
			normal_indcies.push_back(3);
			normal_indcies.push_back(0);
			// top front right
			normal_indcies.push_back(4);
			normal_indcies.push_back(2);
			normal_indcies.push_back(0);
			// top back right
			normal_indcies.push_back(4);
			normal_indcies.push_back(2);
			normal_indcies.push_back(1);
			// top back left
			normal_indcies.push_back(4);
			normal_indcies.push_back(3);
			normal_indcies.push_back(1);

			//std::vector<Vec4> tex_coords;
			for(int i = 0; i < postions.size(); i++)
			{
				Vec3 normal = normals[normal_indcies[i]]; 
				Vec4 tex_coord(tex_coords[i].x,tex_coords[i].y,0,0);
				sub_mesh_data->PositionVector.push_back(postions[i]);
				sub_mesh_data->NormalVector.push_back(normal);
				//tex_coords.push_back(tex_coord);
			}

			sub_mesh_data->TexCoordsVector.push_back(tex_coords);
		}
		
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		GetSceneObject()->PostMessage(mesh_message);
	}

	bool BoxGeometryComponent::IsPointInside(const Vec3 &point) const
	{
		return true;
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

		Mat4 trans;
		trans.SetTransformation(pos,rot,Vec3(1,1,1));
		location = trans*location;

		return location;
	}
}
