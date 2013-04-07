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
#include "Sim/GASSMeshData.h"
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
		ManualMeshDataPtr mesh_data(new ManualMeshData());
		MeshVertex vertex;
		mesh_data->Material = "WhiteTransparentNoLighting";

		vertex.TexCoord.Set(0,0);
		vertex.Color = Vec4(0,0,1,1);
		vertex.Normal = Vec3(0,1,0);

		std::vector<Vec3> conrners;

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
			mesh_data->Type = LINE_LIST;

			for(int i = 0; i < 4; i++)
			{
				vertex.Pos = conrners[i];
				mesh_data->VertexVector.push_back(vertex);
				vertex.Pos = conrners[(i+1)%4];
				mesh_data->VertexVector.push_back(vertex);

				vertex.Pos = conrners[i];
				mesh_data->VertexVector.push_back(vertex);
				vertex.Pos = conrners[i+4];
				mesh_data->VertexVector.push_back(vertex);
			}

			for(int i = 0; i < 4; i++)
			{
				vertex.Pos = conrners[4 + i];
				mesh_data->VertexVector.push_back(vertex);
				vertex.Pos = conrners[4 + ((i+1)%4)];
				mesh_data->VertexVector.push_back(vertex);
			}
		}
		else
		{
			mesh_data->Type = TRIANGLE_LIST;
			for(int i = 0; i < conrners.size(); i++)
			{
				vertex.Pos = conrners[i];
				vertex.Normal = Vec3(0,1,0); 
				mesh_data->VertexVector.push_back(vertex);
			}

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
