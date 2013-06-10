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

#include "SphereGeometryComponent.h"
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
	SphereGeometryComponent::SphereGeometryComponent(void) : m_Radius(1), 
		m_Wireframe(true),
		m_Color(0,0,1,1)
	{

	}

	SphereGeometryComponent::~SphereGeometryComponent(void)
	{
		
	}

	void SphereGeometryComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("SphereGeometryComponent",new GASS::Creator<SphereGeometryComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ObjectMetaDataPtr(new ObjectMetaData("SphereGeometryComponent", OF_VISIBLE)));
		RegisterProperty<Float>("Radius", &GASS::SphereGeometryComponent::GetRadius, &GASS::SphereGeometryComponent::SetRadius);
		RegisterProperty<bool>("Wireframe", &GASS::SphereGeometryComponent::GetWireframe, &GASS::SphereGeometryComponent::SetWireframe);
		RegisterProperty<ColorRGBA>("Color", &GASS::SphereGeometryComponent::GetColor, &GASS::SphereGeometryComponent::SetColor);
	}

	void SphereGeometryComponent::OnInitialize()
	{
		UpdateMesh();
	}

	void SphereGeometryComponent::OnDelete()
	{
		UpdateMesh();
	}

	Float SphereGeometryComponent::GetRadius() const
	{
		return m_Radius;
	}

	void SphereGeometryComponent::SetRadius(Float value)
	{
		m_Radius = value;
		if(GetSceneObject())
			UpdateMesh();
	}


	void SphereGeometryComponent::SetColor(const ColorRGBA &value)
	{
		m_Color = value;
		if(GetSceneObject())
			UpdateMesh();
	}

	void SphereGeometryComponent::UpdateMesh()
	{
		if(m_Wireframe)
			DrawWireframe();
		else
			DrawSolid(10,10);
	}

	void SphereGeometryComponent::DrawWireframe()
	{
		ManualMeshDataPtr mesh_data(new ManualMeshData());
		MeshVertex vertex;
		mesh_data->Material = "WhiteTransparentNoLighting";

		vertex.TexCoord.Set(0,0);
		vertex.Color = Vec4(m_Color.r,m_Color.g,m_Color.b,m_Color.a);
		vertex.Normal = Vec3(0,1,0);
		mesh_data->Type = LINE_LIST;

		//Vec3 up = GetSceneObject()->GetSceneObjectManager()->GetScene()->GetSceneUp();
		float samples = 30;
		float rad = 2*MY_PI/samples;

		float x,y,z;
		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*m_Radius;
			y = sin(rad*i)*m_Radius;
			vertex.Pos.Set(x,y,0);
			mesh_data->VertexVector.push_back(vertex);
		}
		mesh_data->VertexVector.push_back(vertex);

		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*m_Radius;
			z = sin(rad*i)*m_Radius;
			vertex.Pos.Set(x,0,z);
			mesh_data->VertexVector.push_back(vertex);
		}
		mesh_data->VertexVector.push_back(vertex);

		for(float i = 0 ;i <= samples; i++)
		{
			y = cos(rad*i)*m_Radius;
			z = sin(rad*i)*m_Radius;
			vertex.Pos.Set(0,y,z);
			mesh_data->VertexVector.push_back(vertex);
		}
		mesh_data->VertexVector.push_back(vertex);
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		GetSceneObject()->PostMessage(mesh_message);
	}

	void SphereGeometryComponent::DrawSolid(int nSlice, int nStack)
	{
		int i, j;
		double phi; //
		double theta; //long
		Float p[31][31][3]; //Vertex
		Float *p1,*p2,*p3,*p4;

		if(nSlice > 30) nSlice = 30;
		if(nStack > 30) nStack = 30;

		ManualMeshDataPtr mesh_data(new ManualMeshData());
		MeshVertex vertex;
		mesh_data->Material = "WhiteTransparentNoLighting";

		vertex.TexCoord.Set(0,0);
		vertex.Color = Vec4(m_Color.r,m_Color.g,m_Color.b,m_Color.a);
		vertex.Normal = Vec3(0,1,0);
		mesh_data->Type = TRIANGLE_LIST;
		
		//Vertex
		for(i = 0;i <= nSlice;i++)
		{   
			phi = 2.0 * MY_PI * (double)i / (double)nSlice;
			for(j = 0;j <= nStack;j++)
			{   
				theta = MY_PI * (double)j / (double)nStack;
				p[i][j][0] = (m_Radius * sin(theta) * cos(phi));//x
				p[i][j][1] = (m_Radius * sin(theta) * sin(phi));//y
				p[i][j][2] = (m_Radius * cos(theta));           //z
			}
		}

		//Top(j=0)
		for(i = 0;i < nSlice; i++)
		{
			p1 = p[i][0];     p2 = p[i][1];
			p3 = p[i+1][1]; 

			vertex.Pos.Set(p1[0],p1[1],p1[2]);
			mesh_data->VertexVector.push_back(vertex);

			vertex.Pos.Set(p2[0],p2[1],p2[2]);
			mesh_data->VertexVector.push_back(vertex);

			vertex.Pos.Set(p3[0],p3[1],p3[2]);
			mesh_data->VertexVector.push_back(vertex);
			
		}
		//Bottom
		j=nStack-1;
		for(i = 0;i < nSlice; i++)
		{
			p1 = p[i][j];     p2 = p[i][j+1];
			p3 = p[i+1][j]; 

			vertex.Pos.Set(p1[0],p1[1],p1[2]);
			mesh_data->VertexVector.push_back(vertex);

			vertex.Pos.Set(p2[0],p2[1],p2[2]);
			mesh_data->VertexVector.push_back(vertex);

			vertex.Pos.Set(p3[0],p3[1],p3[2]);
			mesh_data->VertexVector.push_back(vertex);
		}

		for(i = 0;i < nSlice;i++)
		{
			for(j = 1;j < nStack-1; j++)
			{
				p1 = p[i][j];     p2 = p[i][j+1];
				p3 = p[i+1][j+1]; p4 = p[i+1][j];
				
				
				vertex.Pos.Set(p1[0],p1[1],p1[2]);
				mesh_data->VertexVector.push_back(vertex);

				vertex.Pos.Set(p2[0],p2[1],p2[2]);
				mesh_data->VertexVector.push_back(vertex);

				vertex.Pos.Set(p3[0],p3[1],p3[2]);
				mesh_data->VertexVector.push_back(vertex);


				vertex.Pos.Set(p1[0],p1[1],p1[2]);
				mesh_data->VertexVector.push_back(vertex);

				vertex.Pos.Set(p3[0],p3[1],p3[2]);
				mesh_data->VertexVector.push_back(vertex);

				vertex.Pos.Set(p4[0],p4[1],p4[2]);
				mesh_data->VertexVector.push_back(vertex);
				
			}
		}
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		GetSceneObject()->PostMessage(mesh_message);
	}

	bool SphereGeometryComponent::IsPointInside(const Vec3 &point) const
	{

		Vec3 location;
		LocationComponentPtr loc_comp = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		assert(loc_comp);
		Vec3 pos = loc_comp->GetWorldPosition();

		if((pos - point).Length() < m_Radius)
			return true;

		return false;
	}

	Vec3 SphereGeometryComponent::GetRandomPoint() const
	{
		Vec3 location;
		LocationComponentPtr loc_comp = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();

		assert(loc_comp);

		Vec3 p1 = loc_comp->GetWorldPosition();

		Float rand1 = rand() / Float(RAND_MAX);
		Float rand2 = rand() / Float(RAND_MAX);

		Float theta = rand1 * MY_PI * 2;
		Float l = sqrt(rand2) * m_Radius;

		location.x = p1.x + cos(theta) * l;
		location.z = p1.z + sin(theta) * l;
		location.y = p1.y;

		return location;
	}

}
