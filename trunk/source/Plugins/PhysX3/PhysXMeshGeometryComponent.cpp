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

#include "Plugins/PhysX3/PhysXMeshGeometryComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"
#include "Plugins/PhysX3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXVehicleSceneQuery.h"
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
#include "Sim/Interface/GASSIResourceComponent.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"
#include <boost/bind.hpp>

using namespace physx;

namespace GASS
{
	PhysXMeshGeometryComponent::PhysXMeshGeometryComponent()
	{

	}

	PhysXMeshGeometryComponent::~PhysXMeshGeometryComponent()
	{

	}

	void PhysXMeshGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsMeshGeometryComponent",new Creator<PhysXMeshGeometryComponent, IComponent>);
	}

	void PhysXMeshGeometryComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXMeshGeometryComponent::OnGeometryChanged,GeometryChangedMessage,0));
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
	}

	void PhysXMeshGeometryComponent::OnDelete()
	{

	}

	void PhysXMeshGeometryComponent::OnGeometryChanged(GeometryChangedMessagePtr message)
	{
		MeshComponentPtr geom = GetSceneObject()->GetFirstComponentByClass<IMeshComponent>();
		GASSAssert(geom,"PhysXMeshGeometryComponent::OnGeometryChanged");
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		GASSAssert(scene_manager,"PhysXMeshGeometryComponent::OnGeometryChanged");

		std::string col_mesh_id = GetSceneObject()->GetName();
		ResourceComponentPtr res  = GetSceneObject()->GetFirstComponentByClass<IResourceComponent>();
		if(res)
		{
			col_mesh_id = res->GetResource().Name();
		}
		m_TriangleMesh = scene_manager->CreateTriangleMesh(col_mesh_id,geom);

		Vec3 position = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
		physx::PxTransform pose = physx::PxTransform::createIdentity();
		pose.p = physx::PxVec3(position.x,position.y,position.z);
		
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<PhysXPhysicsSystem>();
		
		physx::PxRigidStatic* pxActor = system->GetPxSDK()->createRigidStatic(pose);

		physx::PxMaterial* material = system->GetDefaultMaterial();
		
		physx::PxTriangleMeshGeometry geometry = physx::PxTriangleMeshGeometry(m_TriangleMesh.m_TriangleMesh);
		physx::PxShape* shape = pxActor->createShape(geometry, *material);
		
		physx::PxFilterData collFilterData;
		collFilterData.word0=COLLISION_FLAG_DRIVABLE_OBSTACLE;
		collFilterData.word1=COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST;
		shape->setSimulationFilterData(collFilterData);
		
		PxFilterData queryFilterData;
		VehicleSetupDrivableShapeQueryFilterData(&queryFilterData);
		shape->setQueryFilterData(queryFilterData);
		scene_manager->GetPxScene()->addActor(*pxActor);
	}
}

