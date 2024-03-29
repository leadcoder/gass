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


#include "Plugins/PhysX/PhysXBoxGeometryComponent.h"
#include "Plugins/PhysX/PhysXBodyComponent.h"
#include "Plugins/PhysX/PhysXPhysicsSystem.h"

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
		ComponentFactory::GetPtr()->Register<PhysXBoxGeometryComponent>("PhysicsBoxGeometryComponent");
		RegisterGetSet("Size", &GASS::PhysXBoxGeometryComponent::GetSize, &GASS::PhysXBoxGeometryComponent::SetSize);
	}

	void PhysXBoxGeometryComponent::OnInitialize()
	{
		PhysXBaseGeometryComponent::OnInitialize();
	}

	physx::PxShape* PhysXBoxGeometryComponent::CreateShape(physx::PxRigidActor& actor)
	{
		if(m_SizeFromMesh)
		{
			GeometryComponentPtr geom  = GetGeometry();

			if(geom)
			{
				AABox box = geom->GetBoundingBox();
				SetSize((box.Max - box.Min));
				//SetOffset((box.Max + box.Min)*0.5);
			}
			else
			{
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No GeometryComponent found, not possible to get size from geomtry","PhysXBoxGeometryComponent::CreateShape");
			}
		}
		const Vec3 size = GetSize();
		
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
		//physx::PxMaterial* material = system->GetDefaultMaterial();
		physx::PxMaterial* material = system->GetMaterial("DEFAULT");
		const physx::PxVec3 dims(static_cast<float>(size.x/2.0) , static_cast<float>(size.y/2.0) , static_cast<float>(size.z/2.0));
		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(actor, physx::PxBoxGeometry(dims), *material);
		if(shape)
			shape->setLocalPose(physx::PxTransform(physx::PxIdentity));
		return shape;
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
}
