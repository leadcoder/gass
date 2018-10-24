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

#include "Plugins/PhysX3/PhysXMeshGeometryComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"
#include "Plugins/PhysX3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXVehicleSceneQuery.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplateManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneObjectTemplate.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSIResourceComponent.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"

using namespace physx;

namespace GASS
{
	PhysXMeshGeometryComponent::PhysXMeshGeometryComponent() : m_Actor(NULL),
		m_Shape(NULL)
	{

	}

	PhysXMeshGeometryComponent::~PhysXMeshGeometryComponent()
	{

	}

	void PhysXMeshGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsMeshGeometryComponent",new Creator<PhysXMeshGeometryComponent, Component>);
	}

	void PhysXMeshGeometryComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXMeshGeometryComponent::OnGeometryChanged,GeometryChangedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXMeshGeometryComponent::OnTransformationChanged,TransformationChangedEvent, 0));
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
	}

	void PhysXMeshGeometryComponent::OnDelete()
	{

	}

	void PhysXMeshGeometryComponent::OnGeometryChanged(GeometryChangedEventPtr message)
	{
		MeshComponentPtr geom = GetSceneObject()->GetFirstComponentByClass<IMeshComponent>();
		GASSAssert(geom,"PhysXMeshGeometryComponent::OnGeometryChanged");
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		m_SceneManager = scene_manager;
		GASSAssert(scene_manager,"PhysXMeshGeometryComponent::OnGeometryChanged");

		std::string col_mesh_id = GetSceneObject()->GetName();
		ResourceComponentPtr res  = GetSceneObject()->GetFirstComponentByClass<IResourceComponent>();
		if(res)
		{
			col_mesh_id = res->GetResource().Name();
		}
		m_TriangleMesh = scene_manager->CreateTriangleMesh(col_mesh_id,geom);

		const Vec3 position = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
		physx::PxTransform pose = physx::PxTransform::createIdentity();
		pose.p = scene_manager->WorldToLocal(position);
		
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
		
		m_Actor = system->GetPxSDK()->createRigidStatic(pose);

		physx::PxMaterial* material = system->GetDefaultMaterial();
		
		physx::PxTriangleMeshGeometry geometry = physx::PxTriangleMeshGeometry(m_TriangleMesh.m_TriangleMesh);
		m_Shape = m_Actor->createShape(geometry, *material);

		if(!m_Shape)
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"failed to create shape","PhysXMeshGeometryComponent::OnGeometryChanged");
		
		PxFilterData queryFilterData;
		VehicleSetupDrivableShapeQueryFilterData(&queryFilterData);
		m_Shape->setQueryFilterData(queryFilterData);
	
		physx::PxFilterData collFilterData;
		GeometryFlags against = GeometryFlagManager::GetMask(GEOMETRY_FLAG_GROUND);
		collFilterData.word0 = GEOMETRY_FLAG_GROUND;
		collFilterData.word1 = against;
		m_Shape->setSimulationFilterData(collFilterData);
		scene_manager->GetPxScene()->addActor(*m_Actor);
	}


	void PhysXMeshGeometryComponent::OnTransformationChanged(TransformationChangedEventPtr message)
	{
		Vec3 pos = message->GetPosition();
		SetPosition(pos);
		Quaternion rot = message->GetRotation();
		SetRotation(rot);
	}


	void PhysXMeshGeometryComponent::SetPosition(const Vec3 &pos)
	{
		if(m_Actor)
		{
			//Get offset
			const PhysXPhysicsSceneManagerPtr scene_manager = m_SceneManager.lock();
			m_Actor->setGlobalPose(physx::PxTransform(scene_manager->WorldToLocal(pos), m_Actor->getGlobalPose().q));
		}
	}

	void PhysXMeshGeometryComponent::SetRotation(const Quaternion &rot)
	{
		if(m_Actor)
		{
			m_Actor->setGlobalPose(physx::PxTransform(m_Actor->getGlobalPose().p,PxConvert::ToPx(rot)));
		}
	}

}

