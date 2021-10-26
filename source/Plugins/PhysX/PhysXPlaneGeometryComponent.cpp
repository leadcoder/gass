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

#include "Plugins/PhysX/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX/PhysXPlaneGeometryComponent.h"
#include "Plugins/PhysX/PhysXPhysicsSystem.h"
#include "Plugins/PhysX/PhysXVehicleSceneQuery.h"

namespace GASS
{
	PhysXPlaneGeometryComponent::PhysXPlaneGeometryComponent() : m_Material("DEFAULT")
		
	{

	}

	PhysXPlaneGeometryComponent::~PhysXPlaneGeometryComponent()
	{

	}

	void PhysXPlaneGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<PhysXPlaneGeometryComponent>("PhysicsPlaneGeometryComponent");
		RegisterMember("Material", &GASS::PhysXPlaneGeometryComponent::m_Material);
	}

	void PhysXPlaneGeometryComponent::OnInitialize()
	{
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();

		//physx::PxReal d = 0.0f;
		physx::PxTransform pose = physx::PxTransform(physx::PxVec3(0.0f, 0, 0.0f), physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f)));
		m_Actor = system->GetPxSDK()->createRigidStatic(pose);
		physx::PxMaterial* material = system->GetMaterial(m_Material);
		m_Shape = physx::PxRigidActorExt::createExclusiveShape(*m_Actor, physx::PxPlaneGeometry(), *material);

		PxFilterData query_filter_data;
		VehicleSetupDrivableShapeQueryFilterData(&query_filter_data);
		m_Shape->setQueryFilterData(query_filter_data);

		physx::PxFilterData coll_filter_data;
		GeometryFlags against = GeometryFlagManager::GetMask(GEOMETRY_FLAG_GROUND);
		coll_filter_data.word0 = GEOMETRY_FLAG_GROUND;
		coll_filter_data.word1 = against;
		m_Shape->setSimulationFilterData(coll_filter_data);

		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		scene_manager->GetPxScene()->addActor(*m_Actor);
	}

	void PhysXPlaneGeometryComponent::OnDelete()
	{
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		if (m_Actor)
		{
			scene_manager->GetPxScene()->removeActor(*m_Actor);
			m_Actor->release();
			m_Actor = nullptr;
		}
	}
}
