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

#ifdef WIN32
#define NOMINMAX
#include <algorithm>
#endif

#include "Plugins/Havok/HavokBoxGeometryComponent.h"
#include "Plugins/Havok/HavokPhysicsSceneManager.h"
#include "Plugins/Havok/HavokBodyComponent.h"
#include "Sim/GASS.h"


namespace GASS
{
	HavokBoxGeometryComponent::HavokBoxGeometryComponent():
		m_Size(1,1,1),
		m_BoxShape(NULL)
	{

	}

	HavokBoxGeometryComponent::~HavokBoxGeometryComponent()
	{
		
	}

	void HavokBoxGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsBoxGeometryComponent",new Creator<HavokBoxGeometryComponent, IComponent>);
		RegisterProperty<Vec3>("Size", &GASS::HavokBoxGeometryComponent::GetSize, &GASS::HavokBoxGeometryComponent::SetSize);
	}

	void HavokBoxGeometryComponent::OnInitialize()
	{
		HavokBaseGeometryComponent::OnInitialize();
	}

	hkpShape*  HavokBoxGeometryComponent::CreateHavokShape()
	{
		hkReal fhkConvexShapeRadius=0.05;
		hkVector4 dimensions(m_Size.x,m_Size.y,m_Size.z,1);
		hkpBoxShape* boxShape = new hkpBoxShape(dimensions,fhkConvexShapeRadius);
		m_BoxShape = boxShape;
		return boxShape;
	}

	void HavokBoxGeometryComponent::SetSizeFromMesh(bool value)
	{
		m_SizeFromMesh = value;
		if(m_SizeFromMesh && IsInitialized())
		{
			GeometryComponentPtr geom  = GetGeometry();
			if(geom)
			{
				AABox box = geom->GetBoundingBox();
				SetSize((box.m_Max - box.m_Min));
				SetOffset((box.m_Max + box.m_Min)*0.5);
			}
		}
	}

	
	void HavokBoxGeometryComponent::SetSize(const Vec3 &size)
	{
		if(size.x > 0 && size.y > 0 && size.z > 0)
		{
			m_Size = size;
			if(m_BoxShape)
			{
				hkVector4 dimensions(m_Size.x*0.5,m_Size.y*0.5,m_Size.z*0.5,1);
				m_BoxShape->setHalfExtents(dimensions);
				UpdateBodyMass();
				UpdateDebug();
			}
		}
	}

	Vec3 HavokBoxGeometryComponent::GetSize() const
	{
		return m_Size;
	}

	
	void HavokBoxGeometryComponent::CreateDebugBox(const Vec3 &size,const Vec3 &offset)
	{
		ManualMeshDataPtr mesh_data(new ManualMeshData());
		MeshVertex vertex;
		mesh_data->Material = "WhiteTransparentNoLighting";

		vertex.TexCoord.Set(0,0);
		vertex.Color = Vec4(1,1,1,1);
		mesh_data->Type = LINE_LIST;
		std::vector<Vec3> conrners;

		conrners.push_back(Vec3( size.x/2.0 ,size.y/2.0 , size.z/2.0));
		conrners.push_back(Vec3(-size.x/2.0 ,size.y/2.0 , size.z/2.0));
		conrners.push_back(Vec3(-size.x/2.0 ,size.y/2.0 ,-size.z/2.0));
		conrners.push_back(Vec3( size.x/2.0 ,size.y/2.0 ,-size.z/2.0));

		conrners.push_back(Vec3( size.x/2.0 ,-size.y/2.0 , size.z/2.0));
		conrners.push_back(Vec3(-size.x/2.0 ,-size.y/2.0 , size.z/2.0));
		conrners.push_back(Vec3(-size.x/2.0 ,-size.y/2.0 ,-size.z/2.0));
		conrners.push_back(Vec3( size.x/2.0 ,-size.y/2.0 ,-size.z/2.0));

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

		SceneObjectPtr scene_object = GetDebugObject();
		MessagePtr mesh_message(new ManualMeshDataRequest(mesh_data));
		scene_object->PostMessage(mesh_message);

		//Vec3 pos  = m_Offset + offset;
		//scene_object->GetFirstComponentByClass<ILocationComponent>()->SetPosition(pos);
		scene_object->PostMessage(MessagePtr(new PositionRequest(offset,-1,0.3)));
	}

	void HavokBoxGeometryComponent::UpdateDebug()
	{
		if(m_Debug)
		{
			if(m_BoxShape)
			{
				//dVector3 temp_size;
				//dGeomBoxGetLengths (m_GeomID, temp_size);
				hkVector4 h_size = m_BoxShape->getHalfExtents();
				Vec3 size(h_size(0),h_size(1),h_size(2));
				//const dReal* pos =  dGeomGetPosition(m_GeomID);
				CreateDebugBox(size*2,m_Offset);
			}
		}
	}
}
