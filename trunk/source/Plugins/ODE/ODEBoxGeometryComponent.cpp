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

#include "Plugins/ODE/ODEBoxGeometryComponent.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Plugins/ODE/ODEBodyComponent.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"

#include "Core/MessageSystem/MessageManager.h"
#include "Core/Math/AABox.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/SceneObjectTemplate.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/SimEngine.h"
#include <boost/bind.hpp>

namespace GASS
{
	ODEBoxGeometryComponent::ODEBoxGeometryComponent():
		m_Size(1,1,1)
		
	{

	}

	ODEBoxGeometryComponent::~ODEBoxGeometryComponent()
	{
		
	}

	void ODEBoxGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsBoxGeometryComponent",new Creator<ODEBoxGeometryComponent, IComponent>);
		RegisterProperty<Vec3>("Size", &GASS::ODEBoxGeometryComponent::GetSize, &GASS::ODEBoxGeometryComponent::SetSize);
	}

	void ODEBoxGeometryComponent::OnCreate()
	{
		ODEBaseGeometryComponent::OnCreate();
	}

	void ODEBoxGeometryComponent::SetSizeFromMesh(bool value)
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

	dGeomID ODEBoxGeometryComponent::CreateODEGeom()
	{
		return dCreateBox(0, m_Size.x, m_Size.y, m_Size.z); 
	}

	void ODEBoxGeometryComponent::SetSize(const Vec3 &size)
	{
		if(size.x > 0 && size.y > 0 && size.z > 0)
		{
			m_Size = size;
			if(m_GeomID)
			{
				dGeomBoxSetLengths(m_GeomID, m_Size.x, m_Size.y, m_Size.z);
				UpdateBodyMass();
				UpdateDebug();
			}
		}
	}

	Vec3 ODEBoxGeometryComponent::GetSize() const
	{
		return m_Size;
	}

	void ODEBoxGeometryComponent::UpdateBodyMass()
	{
		if(m_Body && m_Body->GetMassRepresentation() == ODEBodyComponent::MR_GEOMETRY)
		{
			dMass ode_mass;
			dMassSetBoxTotal(&ode_mass, m_Body->GetMass(), m_Size.x, m_Size.y, m_Size.z);
			m_Body->SetODEMass(ode_mass);
		}
	}
	
	void ODEBoxGeometryComponent::CreateDebugBox(const Vec3 &size,const Vec3 &offset)
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
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		scene_object->PostMessage(mesh_message);

		//Vec3 pos  = m_Offset + offset;
		//scene_object->GetFirstComponentByClass<ILocationComponent>()->SetPosition(pos);
		scene_object->PostMessage(MessagePtr(new PositionMessage(offset,-1,0.3)));
	}

	void ODEBoxGeometryComponent::UpdateDebug()
	{
		if(m_Debug)
		{
			if(m_GeomID)
			{
				dVector3 temp_size;
				dGeomBoxGetLengths (m_GeomID, temp_size);
				Vec3 size(temp_size[0],temp_size[1],temp_size[2]);
				const dReal* pos =  dGeomGetPosition(m_GeomID);
				CreateDebugBox(size,Vec3(pos[0],pos[1],pos[2]));
			}
		}
	}
}
