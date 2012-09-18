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


#include "Plugins/Physx3/PhysXBoxGeometryComponent.h"
#include "Plugins/Physx3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"

namespace GASS
{
	PhysXBoxGeometryComponent::PhysXBoxGeometryComponent():
		m_Size(1,1,1)
	{

	}

	PhysXBoxGeometryComponent::~PhysXBoxGeometryComponent()
	{
		
	}

	void PhysXBoxGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsBoxGeometryComponent",new Creator<PhysXBoxGeometryComponent, IComponent>);
		RegisterProperty<Vec3>("Size", &GASS::PhysXBoxGeometryComponent::GetSize, &GASS::PhysXBoxGeometryComponent::SetSize);
	}

	void PhysXBoxGeometryComponent::OnInitialize()
	{
		PhysXBaseGeometryComponent::OnInitialize();
	}

	physx::PxShape* PhysXBoxGeometryComponent::CreateShape()
	{
		if(!m_Body)
			return NULL;
		//Create shape
		Vec3 size; 
		if(m_SizeFromMesh)
		{
			GeometryComponentPtr geom  = GetGeometry();
			if(geom)
			{
				AABox box = geom->GetBoundingBox();
				SetSize((box.m_Max - box.m_Min));
				SetOffset((box.m_Max + box.m_Min)*0.5);
			}
		}
		size = GetSize();
		
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<PhysXPhysicsSystem>();
		physx::PxMaterial* material = system->GetDefaultMaterial();
		physx::PxShape* shape = m_Body->GetPxActor()->createShape(physx::PxBoxGeometry(size.x/2.0,size.y/2.0,size.z/2.0), *material);
		return shape;
	}

	void PhysXBoxGeometryComponent::SetSizeFromMesh(bool value)
	{
		m_SizeFromMesh = value;
	}

	
	void PhysXBoxGeometryComponent::SetSize(const Vec3 &size)
	{
		if(size.x > 0 && size.y > 0 && size.z > 0)
		{
			m_Size = size;
		}
	}

	Vec3 PhysXBoxGeometryComponent::GetSize() const
	{
		return m_Size;
	}

	
	void PhysXBoxGeometryComponent::CreateDebugBox(const Vec3 &size,const Vec3 &offset)
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

/*		SceneObjectPtr scene_object = GetDebugObject();
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		scene_object->PostMessage(mesh_message);

		Vec3 pos  = m_Offset + offset;
		scene_object->GetFirstComponentByClass<ILocationComponent>()->SetPosition(pos);
		scene_object->PostMessage(MessagePtr(new PositionMessage(offset,-1,0.3)));*/
	}

	void PhysXBoxGeometryComponent::UpdateDebug()
	{
		/*if(m_Debug)
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
		}*/
	}
}
