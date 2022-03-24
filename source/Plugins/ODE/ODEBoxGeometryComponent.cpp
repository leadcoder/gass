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


#include <memory>

#include "Plugins/ODE/ODEBoxGeometryComponent.h"
#include "Plugins/ODE/ODEBodyComponent.h"
#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Math/GASSAABox.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneObjectTemplate.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSIManualMeshComponent.h"


namespace GASS
{
	ODEBoxGeometryComponent::ODEBoxGeometryComponent():
		m_Size(1,1,1),
		m_Scale(1,1,1)
		
	{

	}

	ODEBoxGeometryComponent::~ODEBoxGeometryComponent()
	{
		
	}

	void ODEBoxGeometryComponent::RegisterReflection()
	{
		RegisterGetSet("Size", &GASS::ODEBoxGeometryComponent::GetSize, &GASS::ODEBoxGeometryComponent::SetSize);
	}

	void ODEBoxGeometryComponent::OnInitialize()
	{
		ODEBaseGeometryComponent::OnInitialize();
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
				SetSize((box.Max - box.Min));
				//only override offset if not set
				if(m_Offset == Vec3(0,0,0))
					SetOffset(((box.Max + box.Min)*0.5));
				else
					SetOffset(m_Offset);
			}
		}
	}

	dGeomID ODEBoxGeometryComponent::CreateODEGeom()
	{
		return dCreateBox(GetSpace(), m_Size.x, m_Size.y, m_Size.z); 
	}

	void ODEBoxGeometryComponent::SetSize(const Vec3 &size)
	{
		if(size.x > 0 && size.y > 0 && size.z > 0)
		{
			m_Size = size;
			if(m_GeomID)
			{
				dGeomBoxSetLengths(m_GeomID, m_Size.x*m_Scale.x, m_Size.y*m_Scale.y, m_Size.z*m_Scale.z);
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
			//Vec3 mass_offset = m_Body->GetCGPosition();
			//dMassTranslate(&ode_mass, mass_offset.x, mass_offset.y, mass_offset.z);
			m_Body->SetODEMass(ode_mass);
		}
	}
	
	void ODEBoxGeometryComponent::CreateDebugBox(const Vec3 &size,const Vec3 &offset)
	{
		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);
		sub_mesh_data->MaterialName = "WhiteTransparentNoLighting";
		sub_mesh_data->Type = LINE_LIST;
		std::vector<Vec3> conrners;

		conrners.emplace_back( size.x/2.0 ,size.y/2.0 , size.z/2.0);
		conrners.emplace_back(-size.x/2.0 ,size.y/2.0 , size.z/2.0);
		conrners.emplace_back(-size.x/2.0 ,size.y/2.0 ,-size.z/2.0);
		conrners.emplace_back( size.x/2.0 ,size.y/2.0 ,-size.z/2.0);

		conrners.emplace_back( size.x/2.0 ,-size.y/2.0 , size.z/2.0);
		conrners.emplace_back(-size.x/2.0 ,-size.y/2.0 , size.z/2.0);
		conrners.emplace_back(-size.x/2.0 ,-size.y/2.0 ,-size.z/2.0);
		conrners.emplace_back( size.x/2.0 ,-size.y/2.0 ,-size.z/2.0);

		Vec3 pos(0,0,0);
		for(int i = 0; i < 4; i++)
		{
			pos =conrners[i];
			sub_mesh_data->PositionVector.push_back(pos);
			pos = conrners[(i+1)%4];
			sub_mesh_data->PositionVector.push_back(pos);

			pos = conrners[i];
			sub_mesh_data->PositionVector.push_back(pos);
			pos = conrners[i+4];
			sub_mesh_data->PositionVector.push_back(pos);
		}

		for(int i = 0; i < 4; i++)
		{
			pos = conrners[4 + i];
			sub_mesh_data->PositionVector.push_back(pos);
			pos = conrners[4 + ((i+1)%4)];
			sub_mesh_data->PositionVector.push_back(pos);
		}

		SceneObjectPtr scene_object = GetDebugObject();
		scene_object->GetFirstComponentByClass<IManualMeshComponent>()->SetMeshData(*mesh_data);
		scene_object->GetFirstComponentByClass<ILocationComponent>()->SetPosition(offset);
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
