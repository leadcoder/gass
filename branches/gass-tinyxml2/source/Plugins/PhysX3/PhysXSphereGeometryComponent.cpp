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


#include "Plugins/Physx3/PhysXSphereGeometryComponent.h"
#include "Plugins/Physx3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"

namespace GASS
{
	PhysXSphereGeometryComponent::PhysXSphereGeometryComponent():
		m_Radius(1),
		m_Material("DEFAULT")
	{

	}

	PhysXSphereGeometryComponent::~PhysXSphereGeometryComponent()
	{
		
	}

	void PhysXSphereGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsSphereGeometryComponent",new Creator<PhysXSphereGeometryComponent, Component>);
		RegisterProperty<Float>("Radius", &GASS::PhysXSphereGeometryComponent::GetRadius, &GASS::PhysXSphereGeometryComponent::SetRadius);
		RegisterProperty<std::string>("Material", &GASS::PhysXSphereGeometryComponent::GetMaterial, &GASS::PhysXSphereGeometryComponent::SetMaterial);
	}

	void PhysXSphereGeometryComponent::OnInitialize()
	{
		PhysXBaseGeometryComponent::OnInitialize();
	}

	physx::PxShape* PhysXSphereGeometryComponent::CreateShape()
	{
		if(!m_Body)
			return NULL;

		//Create shape
		if(m_SizeFromMesh)
		{
			GeometryComponentPtr geom  = GetGeometry();
			if(geom)
			{
				Sphere sphere = geom->GetBoundingSphere();
				m_Radius = sphere.m_Radius;
			}
		}
		Float  rad = GetRadius();
	
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
		physx::PxMaterial* material = system->GetMaterial(m_Material);
		physx::PxShape* shape = m_Body->GetPxRigidDynamic()->createShape(physx::PxSphereGeometry(rad), *material,physx::PxTransform(physx::PxVec3(0,0,0)));
	
		return shape;
	}

	
	void PhysXSphereGeometryComponent::SetRadius(Float rad)
	{
		if(rad > 0)
		{
			m_Radius = rad;
		}
	}

	Float PhysXSphereGeometryComponent::GetRadius() const
	{
		return m_Radius;
	}

	void PhysXSphereGeometryComponent::UpdateDebug()
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
