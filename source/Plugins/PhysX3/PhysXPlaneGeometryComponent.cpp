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

#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPlaneGeometryComponent.h"
#include "Plugins/PhysX3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"
#include "Plugins/PhysX3/PhysXVehicleSceneQuery.h"

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
		ComponentFactory::GetPtr()->Register("PhysicsPlaneGeometryComponent",new Creator<PhysXPlaneGeometryComponent, Component>);
		RegisterProperty<std::string>("Material", &GASS::PhysXPlaneGeometryComponent::GetMaterial, &GASS::PhysXPlaneGeometryComponent::SetMaterial);
	}

	void PhysXPlaneGeometryComponent::OnInitialize()
	{
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();

		//physx::PxReal d = 0.0f;
		physx::PxTransform pose = physx::PxTransform(physx::PxVec3(0.0f, 0, 0.0f),physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f)));
		physx::PxRigidStatic* plane = system->GetPxSDK()->createRigidStatic(pose);
		physx::PxMaterial* material = system->GetMaterial(m_Material);
		physx::PxShape* shape = plane->createShape(physx::PxPlaneGeometry(), *material);

		PxFilterData queryFilterData;
		VehicleSetupDrivableShapeQueryFilterData(&queryFilterData);
		shape->setQueryFilterData(queryFilterData);

		physx::PxFilterData collFilterData;
		GeometryFlags against = GeometryFlagManager::GetMask(GEOMETRY_FLAG_GROUND);
		collFilterData.word0 = GEOMETRY_FLAG_GROUND;
		collFilterData.word1 = against;
		shape->setSimulationFilterData(collFilterData);

		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		scene_manager->GetPxScene()->addActor(*plane);
	}
}
