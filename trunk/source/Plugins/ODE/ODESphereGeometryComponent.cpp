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


#include "Plugins/ODE/ODESphereGeometryComponent.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Plugins/ODE/ODEBodyComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"

#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Math/GASSAABox.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"

#include "Sim/GASSSceneObjectTemplate.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"

namespace GASS
{
	ODESphereGeometryComponent::ODESphereGeometryComponent():
		m_Radius(1)
	{

	}

	ODESphereGeometryComponent::~ODESphereGeometryComponent()
	{
		
	}

	void ODESphereGeometryComponent::RegisterReflection()
	{
		RegisterProperty<Float>("Radius", &GASS::ODESphereGeometryComponent::GetRadius, &GASS::ODESphereGeometryComponent::SetRadius);
	}

	void ODESphereGeometryComponent::OnInitialize()
	{
		ODEBaseGeometryComponent::OnInitialize();
	}

	
	void ODESphereGeometryComponent::SetSizeFromMesh(bool value)
	{
		m_SizeFromMesh = value;
		if(m_SizeFromMesh && IsInitialized())
		{
			GeometryComponentPtr geom  = GetGeometry();
			if(geom)
			{
				Sphere sphere = geom->GetBoundingSphere();
				if(sphere.m_Radius > 0)
				{
					SetRadius(sphere.m_Radius);
					AABox box = geom->GetBoundingBox();
					SetOffset((box.m_Max + box.m_Min)*0.5);
				}
			}
		}
	}


	dGeomID ODESphereGeometryComponent::CreateODEGeom()
	{
		return dCreateSphere(0, m_Radius);
	}

	void ODESphereGeometryComponent::SetRadius(Float value)
	{
		if(value > 0)
		{
			m_Radius = value;
			if(m_GeomID)
			{
				dGeomSphereSetRadius(m_GeomID, value);
				UpdateBodyMass();
				UpdateDebug();
			}
		}
	}

	Float ODESphereGeometryComponent::GetRadius() const
	{
		return m_Radius;
	}

	void ODESphereGeometryComponent::UpdateBodyMass()
	{
		if(m_Body && m_Body->GetMassRepresentation() == ODEBodyComponent::MR_GEOMETRY)
		{
			dMass ode_mass;
			dMassSetSphereTotal(&ode_mass, m_Body->GetMass(), m_Radius);
			m_Body->SetODEMass(ode_mass);
		}
	}

	void ODESphereGeometryComponent::CreateDebugSphere(Float size,const Vec3 &offset)
	{
		MeshDataPtr mesh_data(new MeshData());
		
		SubMeshDataPtr sub_mesh_data(new SubMeshData());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);
		sub_mesh_data->MaterialName = "WhiteTransparentNoLighting";
		sub_mesh_data->Type = LINE_LIST;
	
		const float samples = 24;
		const float rad = 2*MY_PI/samples;
		Vec3 pos(0,0,0);
		float x,y,z;
		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*size;
			y = sin(rad*i)*size;
			pos.Set(x,y,0);
			sub_mesh_data->PositionVector.push_back(pos);
			if(i > 0 && i < samples)
				sub_mesh_data->PositionVector.push_back(pos);
		}
		//sub_mesh_data->PositionVector.push_back(pos);
		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*size;
			z = sin(rad*i)*size;
			pos.Set(x,0,z);
			sub_mesh_data->PositionVector.push_back(pos);
			if(i > 0 && i < samples)
				sub_mesh_data->PositionVector.push_back(pos);
		}
		//sub_mesh_data->PositionVector.push_back(pos);

		for(float i = 0 ;i <= samples; i++)
		{
			y = cos(rad*i)*size;
			z = sin(rad*i)*size;
			pos.Set(0,y,z);
			sub_mesh_data->PositionVector.push_back(pos);
			if(i > 0 && i < samples)
				sub_mesh_data->PositionVector.push_back(pos);
		}
		//sub_mesh_data->PositionVector.push_back(pos);


		SceneObjectPtr scene_object = GetDebugObject();
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		scene_object->PostMessage(mesh_message);
		//scene_object->GetFirstComponentByClass<ILocationComponent>()->SetPosition(offset);
		scene_object->PostMessage(MessagePtr(new PositionMessage(offset)));
	}

	void ODESphereGeometryComponent::UpdateDebug()
	{

		if(m_Debug)
		{
			if(m_GeomID)
			{
				dReal radius = dGeomSphereGetRadius (m_GeomID);
				const dReal* pos =  dGeomGetPosition(m_GeomID);
				CreateDebugSphere(radius,Vec3(pos[0],pos[1],pos[2]));
			}
		}
		
	}
}
