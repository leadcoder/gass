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

#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/Physx3/PhysXPlaneGeometryComponent.h"
#include "Plugins/Physx3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"
#include "Plugins/PhysX3/PhysXVehicleSceneQuery.h"

namespace GASS
{
	PhysXPlaneGeometryComponent::PhysXPlaneGeometryComponent()
	{

	}

	PhysXPlaneGeometryComponent::~PhysXPlaneGeometryComponent()
	{
		
	}

	void PhysXPlaneGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsPlaneGeometryComponent",new Creator<PhysXPlaneGeometryComponent, IComponent>);
	}

	void PhysXPlaneGeometryComponent::OnInitialize()
	{
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<PhysXPhysicsSystem>();
		physx::PxMaterial* material = system->GetDefaultMaterial();
		
		physx::PxReal d = 0.0f;	 
		physx::PxTransform pose = physx::PxTransform(physx::PxVec3(0.0f, 0, 0.0f),physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f)));
		physx::PxRigidStatic* plane = system->GetPxSDK()->createRigidStatic(pose);
		physx::PxShape* shape = plane->createShape(physx::PxPlaneGeometry(), *system->GetDefaultMaterial());
		
		physx::PxFilterData collFilterData;
		collFilterData.word0=COLLISION_FLAG_GROUND;
		collFilterData.word1=COLLISION_FLAG_GROUND_AGAINST;
		shape->setSimulationFilterData(collFilterData);
		
		PxFilterData queryFilterData;
		VehicleSetupDrivableShapeQueryFilterData(&queryFilterData);
		shape->setQueryFilterData(queryFilterData);
	

		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		scene_manager->GetPxScene()->addActor(*plane);
	}
}
