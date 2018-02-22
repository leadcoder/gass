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

#include "Plugins/PhysX3/PhysXConvexGeometryComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"
#include "Plugins/PhysX3/PhysXBodyComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplateManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneObjectTemplate.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSIResourceComponent.h"
#include "Sim/GASSSimEngine.h"

using namespace physx;

namespace GASS
{
	PhysXConvexGeometryComponent::PhysXConvexGeometryComponent() : m_Shape(0),
		m_SimulationCollision(true)
	{

	}

	PhysXConvexGeometryComponent::~PhysXConvexGeometryComponent()
	{
		
	}

	void PhysXConvexGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsConvexGeometryComponent",new Creator<PhysXConvexGeometryComponent, Component>);
		RegisterProperty<bool>("SimulationCollision", &GASS::PhysXConvexGeometryComponent::GetSimulationCollision, &GASS::PhysXConvexGeometryComponent::SetSimulationCollision);

	}

	void PhysXConvexGeometryComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXConvexGeometryComponent::OnGeometryChanged,GeometryChangedEvent,0));
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		
	}

	void PhysXConvexGeometryComponent::OnDelete()
	{
		//deleted when body is deleted?
		/*if(m_Shape) 
			m_Shape->release();
		m_Shape = NULL;*/
	}

	void PhysXConvexGeometryComponent::OnGeometryChanged(GeometryChangedEventPtr message)
	{
		MeshComponentPtr mesh = GetSceneObject()->GetFirstComponentByClass<IMeshComponent>();
		GASSAssert(mesh,"PhysXConvexGeometryComponent::OnGeometryChanged");
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		m_SceneManager = scene_manager;
		GASSAssert(scene_manager,"PhysXConvexGeometryComponent::OnGeometryChanged");

		std::string col_mesh_id = GetSceneObject()->GetName();
		ResourceComponentPtr res  = GetSceneObject()->GetFirstComponentByClass<IResourceComponent>();
		if(res)
		{
			col_mesh_id = res->GetResource().Name();
		}
		m_ConvexMesh = scene_manager->CreateConvexMesh(col_mesh_id, mesh);


		PhysXBodyComponentPtr body = GetSceneObject()->GetFirstComponentByClass<PhysXBodyComponent>();
		if(body)
		{
			if(m_Shape)
				m_Shape->release();
			PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
			physx::PxMaterial* material = system->GetDefaultMaterial();


			PxConvexMeshGeometry convex_geom(m_ConvexMesh.m_ConvexMesh);
			m_Shape = body->GetPxRigidDynamic()->createShape(convex_geom, *material);
			//update collision flags
			GeometryComponentPtr geom  = GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();
			physx::PxFilterData collFilterData;
			if(geom)
			{
				GeometryFlags against = GeometryFlagManager::GetMask(geom->GetGeometryFlags());
				collFilterData.word0 = geom->GetGeometryFlags();
				collFilterData.word1 = against;
				m_Shape->setSimulationFilterData(collFilterData);
			}
			m_Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE,m_SimulationCollision);
			if(body)
			{
				physx::PxReal mass = body->GetMass();
				const physx::PxVec3 localPos = physx::PxVec3(0,0,0);
				physx::PxRigidBodyExt::setMassAndUpdateInertia(*body->GetPxRigidDynamic(), mass,&localPos);

				//density!!!
				//physx::PxRigidBodyExt::updateMassAndInertia(*body->GetPxRigidDynamic(), mass,&localPos);
			}
		}
	}
}

