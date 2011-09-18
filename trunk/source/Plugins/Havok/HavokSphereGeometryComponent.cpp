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

#include "Plugins/Havok/HavokSphereGeometryComponent.h"
#include "Plugins/Havok/HavokPhysicsSceneManager.h"
#include "Plugins/Havok/HavokBodyComponent.h"
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
	HavokSphereGeometryComponent::HavokSphereGeometryComponent(): m_Radius(1),
		m_SphereShape(NULL)
	{

	}

	HavokSphereGeometryComponent::~HavokSphereGeometryComponent()
	{
		
	}

	void HavokSphereGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsSphereGeometryComponent",new Creator<HavokSphereGeometryComponent, IComponent>);
		RegisterProperty<Float>("Radius", &GASS::HavokSphereGeometryComponent::GetRadius, &GASS::HavokSphereGeometryComponent::SetRadius);
	}

	void HavokSphereGeometryComponent::OnCreate()
	{
		HavokBaseGeometryComponent::OnCreate();
	}

	
	void HavokSphereGeometryComponent::SetSizeFromMesh(bool value)
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


	
	hkpShape*  HavokSphereGeometryComponent::CreateHavokShape()
	{
		m_SphereShape = new hkpSphereShape(m_Radius);
		return m_SphereShape;
	}


	void HavokSphereGeometryComponent::SetRadius(Float value)
	{
		if(value > 0)
		{
			m_Radius = value;
			if(m_SphereShape)
			{
				m_SphereShape->setRadius(m_Radius);
				//UpdateBodyMass();
				UpdateDebug();
			}
		}
	}

	Float HavokSphereGeometryComponent::GetRadius() const
	{
		return m_Radius;
	}

	void HavokSphereGeometryComponent::UpdateBodyMass()
	{
		/*if(m_Body && m_Body->GetMassRepresentation() == ODEBodyComponent::MR_GEOMETRY)
		{
			dMass ode_mass;
			dMassSetSphereTotal(&ode_mass, m_Body->GetMass(), m_Radius);
			m_Body->SetODEMass(ode_mass);
		}*/
	}


	void HavokSphereGeometryComponent::CreateDebugSphere(Float size,const Vec3 &offset)
	{
		ManualMeshDataPtr mesh_data(new ManualMeshData());
		MeshVertex vertex;
		mesh_data->Material = "WhiteTransparentNoLighting";

		vertex.TexCoord.Set(0,0);
		vertex.Color = Vec4(1,1,1,1);
		mesh_data->Type = LINE_LIST;

		const float samples = 24;
		const float rad = 2*MY_PI/samples;

		float x,y,z;
		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*size;
			y = sin(rad*i)*size;
			vertex.Pos.Set(x,y,0);
			mesh_data->VertexVector.push_back(vertex);
			if(i > 0 && i < samples)
				mesh_data->VertexVector.push_back(vertex);
		}
		//mesh_data->VertexVector.push_back(vertex);
		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*size;
			z = sin(rad*i)*size;
			vertex.Pos.Set(x,0,z);
			mesh_data->VertexVector.push_back(vertex);
			if(i > 0 && i < samples)
				mesh_data->VertexVector.push_back(vertex);
		}
		//mesh_data->VertexVector.push_back(vertex);

		for(float i = 0 ;i <= samples; i++)
		{
			y = cos(rad*i)*size;
			z = sin(rad*i)*size;
			vertex.Pos.Set(0,y,z);
			mesh_data->VertexVector.push_back(vertex);
			if(i > 0 && i < samples)
				mesh_data->VertexVector.push_back(vertex);
		}
		//mesh_data->VertexVector.push_back(vertex);


		SceneObjectPtr scene_object = GetDebugObject();
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		scene_object->PostMessage(mesh_message);
		//scene_object->GetFirstComponentByClass<ILocationComponent>()->SetPosition(offset);
		scene_object->PostMessage(MessagePtr(new PositionMessage(offset)));
	}

	void HavokSphereGeometryComponent::UpdateDebug()
	{

		if(m_Debug)
		{
			if(m_SphereShape)
			{
				
				//dReal radius = dGeomSphereGetRadius (m_GeomID);
				//const dReal* pos =  dGeomGetPosition(m_GeomID);
				CreateDebugSphere(m_SphereShape->getRadius(),Vec3( 0, 0, 0));
			}
		}
		
	}
}
