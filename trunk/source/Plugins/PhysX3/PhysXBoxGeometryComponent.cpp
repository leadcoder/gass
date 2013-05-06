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
		if(m_SizeFromMesh)
		{
			GeometryComponentPtr geom  = GetGeometry();
		
			if(geom)
			{
				AABox box = geom->GetBoundingBox();
				SetSize((box.m_Max - box.m_Min));
				//SetOffset((box.m_Max + box.m_Min)*0.5);
			}
			else
			{
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No GeometryComponent found, not possible to get size from geomtry","PhysXBoxGeometryComponent::CreateShape");
			}
		}
		Vec3 size = GetSize();
		physx::PxTransform offset = physx::PxTransform::createIdentity();
		//offset.p = PxConvert::ToPx(GetOffset());

		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
		physx::PxMaterial* material = system->GetDefaultMaterial();
		
		physx::PxVec3 dims(size.x/2.0,size.y/2.0,size.z/2.0);
		physx::PxShape* shape = m_Body->GetPxActor()->createShape(physx::PxBoxGeometry(dims), *material,offset);
		
	
		//physx::PxReal mass = m_Body->GetMass();
		//physx::PxRigidBodyExt::setMassAndUpdateInertia(*m_Body->GetPxActor(), mass);

		/*float mass = 1;
		physx::PxVec3 chassisDims(15,1,15);
		physx::PxVec3 chassisMOI
			((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*mass/12.0f,
			(chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*mass/12.0f,
			(chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*mass/12.0f);
		
		m_Body->GetPxActor()->setMass(mass);
		m_Body->GetPxActor()->setMassSpaceInertiaTensor(chassisMOI);
		m_Body->GetPxActor()->setCMassLocalPose(physx::PxTransform(physx::PxVec3(0,-0.5,0),physx::PxQuat::createIdentity()));
		*/
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
