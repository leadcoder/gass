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


#include "Plugins/PhysX/PhysXSphereGeometryComponent.h"
#include "Plugins/PhysX/PhysXBodyComponent.h"
#include "Plugins/PhysX/PhysXPhysicsSystem.h"

namespace GASS
{
	PhysXSphereGeometryComponent::PhysXSphereGeometryComponent():
		
		m_Material("DEFAULT")
	{

	}

	PhysXSphereGeometryComponent::~PhysXSphereGeometryComponent()
	{

	}

	void PhysXSphereGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<PhysXSphereGeometryComponent>("PhysicsSphereGeometryComponent");
		RegisterGetSet("Radius", &GASS::PhysXSphereGeometryComponent::GetRadius, &GASS::PhysXSphereGeometryComponent::SetRadius);
		RegisterMember("Material", &GASS::PhysXSphereGeometryComponent::m_Material);
	}

	void PhysXSphereGeometryComponent::OnInitialize()
	{
		PhysXBaseGeometryComponent::OnInitialize();
	}

	physx::PxShape* PhysXSphereGeometryComponent::CreateShape(physx::PxRigidActor& actor)
	{
		//Create shape
		if(m_SizeFromMesh)
		{
			GeometryComponentPtr geom  = GetGeometry();
			if(geom)
			{
				//Sphere sphere = geom->GetBoundingSphere();
				//m_Radius = sphere.m_Radius;
				auto size = geom->GetBoundingBox().GetSize();
				m_Radius = std::max(size.z, std::max(size.x, size.y))/2.0;
				//m_Radius = sphere.m_Radius;
			}
		}
		Float  rad = GetRadius();

		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
		physx::PxMaterial* material = system->GetMaterial(m_Material);
		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(actor, physx::PxSphereGeometry(static_cast<float>(rad)), *material);
		if (shape)
			shape->setLocalPose(physx::PxTransform(physx::PxVec3(0, 0, 0)));
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
