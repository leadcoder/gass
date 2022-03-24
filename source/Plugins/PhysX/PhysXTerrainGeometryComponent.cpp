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


#include "Plugins/PhysX/PhysXTerrainGeometryComponent.h"
#include "Plugins/PhysX/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX/PhysXPhysicsSystem.h"
#include "Plugins/PhysX/PhysXVehicleSceneQuery.h"
#include "Sim/GASSComponentFactory.h"
#include "Sim/GASSSceneObjectTemplateManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Math/GASSAABox.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneObjectTemplate.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/GASSSimEngine.h"

namespace GASS
{
	PhysXTerrainGeometryComponent::PhysXTerrainGeometryComponent()
		

	{

	}

	PhysXTerrainGeometryComponent::~PhysXTerrainGeometryComponent()
	{

	}

	void PhysXTerrainGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<PhysXTerrainGeometryComponent>("PhysicsTerrainGeometryComponent");
	}

	void PhysXTerrainGeometryComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXTerrainGeometryComponent::OnCollisionSettings,CollisionSettingsRequest ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXTerrainGeometryComponent::OnGeometryChanged,GeometryChangedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXTerrainGeometryComponent::OnTransformationChanged, TransformationChangedEvent, 0));

		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		assert(scene_manager);
		m_SceneManager = scene_manager;
	}

	void PhysXTerrainGeometryComponent::OnGeometryChanged(GeometryChangedEventPtr message)
	{
		Reset();
		CreateTerrain();
	}

	void PhysXTerrainGeometryComponent::OnTransformationChanged(TransformationChangedEventPtr message)
	{
			Vec3 pos = message->GetPosition();
			Quaternion rot = message->GetRotation();

			Vec3 position;
			position.x = m_TerrainBounds.Min.x;
			position.z = m_TerrainBounds.Min.z;
			position.y = 0;
			const Mat4 transform(rot, pos);
			position = transform * position;
			SetPosition(position);
			SetRotation(rot);
	}

	void PhysXTerrainGeometryComponent::SetPosition(const Vec3& pos)
	{
		if (m_Actor)
		{
			//Get offset
			const PhysXPhysicsSceneManagerPtr scene_manager = m_SceneManager.lock();
			m_Actor->setGlobalPose(physx::PxTransform(scene_manager->WorldToLocal(pos), m_Actor->getGlobalPose().q));
		}
	}

	void PhysXTerrainGeometryComponent::SetRotation(const Quaternion& rot)
	{
		if (m_Actor)
		{
			m_Actor->setGlobalPose(physx::PxTransform(m_Actor->getGlobalPose().p, PxConvert::ToPx(rot)));
		}
	}

	HeightmapTerrainComponentPtr PhysXTerrainGeometryComponent::GetTerrainComponent() const 
	{
		HeightmapTerrainComponentPtr geom;
		if(m_GeometryTemplate != "")
		{
			geom = GASS_DYNAMIC_PTR_CAST<IHeightmapTerrainComponent>(GetSceneObject()->GetComponent(m_GeometryTemplate));
		}
		else geom = GetSceneObject()->GetFirstComponentByClass<IHeightmapTerrainComponent>();
		return geom;
	}

	void PhysXTerrainGeometryComponent::CreateTerrain()
	{
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
		HeightmapTerrainComponentPtr terrain = GetTerrainComponent();
		//save raw point for fast height access, not thread safe!!
		m_TerrainGeom = terrain.get();
		//physx::PxShape* shape = NULL;

		if(terrain)
		{
			
			int samples_x = terrain->GetNumSamplesW();
			int samples_z = terrain->GetNumSamplesH();
			if (samples_x < 2 || samples_z < 2)
			{
				return;
			}

			Release();

			m_TerrainBounds = terrain->GetBoundingBox();
			Float size_x = m_TerrainBounds.Max.x - m_TerrainBounds.Min.x;
			Float size_z = m_TerrainBounds.Max.z - m_TerrainBounds.Min.z;
			
			Float scale_x = size_x/((Float) samples_x-1);
			Float scale_z = size_z/((Float) samples_z-1);
			//physx::PxHeightFieldSample* samples = (physx::PxHeightFieldSample*) system->GetAllocator()->allocate(sizeof(physx::PxHeightFieldSample)*samples_x*samples_z,0,__FILE__, __LINE__);
			auto* samples = (physx::PxHeightFieldSample*) new physx::PxHeightFieldSample[samples_x*samples_z];//(sizeof(physx::PxHeightFieldSample)*(samples_x*samples_z));

			const physx::PxReal height_scale = 0.1f;
			memset(samples,0, samples_x * samples_z * sizeof(physx::PxHeightFieldSample));

			for(int x = 0; x < samples_x; x++)
			{
				for(int z = 0; z < samples_z; z++)
				{
					//Float world_x = x * m_SampleWidth + m_TerrainBounds.Min.x;
					//Float world_z = z * m_SampleWidth + m_TerrainBounds.Min.z;
					//Float height = m_TerrainGeom->GetHeightAtWorldLocation(world_x,world_z);
					Float height = m_TerrainGeom->GetHeightAtSample(x,z);
					samples[z+x*samples_z].height = (physx::PxI16)(height/height_scale);
					//samples[x+z*samples_x].setTessFlag();
					samples[z+x*samples_z].materialIndex0=0;
					samples[z+x*samples_z].materialIndex1=0;
				}
			}

			physx::PxHeightFieldDesc hf_desc;

			hf_desc.format             = physx::PxHeightFieldFormat::eS16_TM;
			hf_desc.nbColumns          = samples_z;
			hf_desc.nbRows             = samples_x;
			hf_desc.samples.data       = samples;
			hf_desc.samples.stride     = sizeof(physx::PxHeightFieldSample);
			//hfDesc.thickness = 0.1;
			//hfDesc.convexEdgeThreshold = 0;
			//hfDesc.flags = 0;

			physx::PxHeightField* height_field = system->GetPxCooking()->createHeightField(hf_desc, system->GetPxSDK()->getPhysicsInsertionCallback());// system->GetPxSDK()->createHeightField();
			physx::PxTransform pose = physx::PxTransform(PxIdentity);

			Vec3 position;
			position.x = m_TerrainBounds.Min.x;
			position.z = m_TerrainBounds.Min.z;
			position.y = 0;

			
			Vec3 pos(0,0,0);
			Quaternion rot = Quaternion::IDENTITY;
			auto lc = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
			if (lc)
			{
				pos = lc->GetWorldPosition();
				rot = lc->GetWorldRotation();
			}
			
			const Mat4 transform(rot,pos);
			position = transform * position;

			pose.p = scene_manager->WorldToLocal(position);
			pose.q = PxConvert::ToPx(rot);

			m_Actor = system->GetPxSDK()->createRigidStatic(pose);

			physx::PxHeightFieldGeometry hf_geom(height_field, physx::PxMeshGeometryFlags(), static_cast<float>(height_scale), static_cast<float>(scale_x), static_cast<float>(scale_z));
			m_Shape = PxRigidActorExt::createExclusiveShape(*m_Actor,hf_geom, *system->GetDefaultMaterial());

			physx::PxFilterData coll_filter_data;
			GeometryFlags against = GeometryFlagManager::GetMask(GEOMETRY_FLAG_GROUND);
			coll_filter_data.word0 = GEOMETRY_FLAG_GROUND;
			coll_filter_data.word1 = against;
			m_Shape->setSimulationFilterData(coll_filter_data);

			PxFilterData query_filter_data;
			VehicleSetupDrivableShapeQueryFilterData(&query_filter_data);
			m_Shape->setQueryFilterData(query_filter_data);

			PhysXPhysicsSceneManagerPtr sm = PhysXPhysicsSceneManagerPtr(m_SceneManager);
			sm->GetPxScene()->addActor(*m_Actor);
		}
	}

	void PhysXTerrainGeometryComponent::Release()
	{
		PhysXPhysicsSceneManagerPtr sm = PhysXPhysicsSceneManagerPtr(m_SceneManager); 
		if (sm && m_Actor)
		{
			sm->GetPxScene()->removeActor(*m_Actor);
			m_Actor->release();
			m_Actor = nullptr;
		}
	}

	void PhysXTerrainGeometryComponent::OnDelete()
	{
		Release();
	}
	
	
	void PhysXTerrainGeometryComponent::OnCollisionSettings(CollisionSettingsRequestPtr message)
	{
		bool value = message->EnableCollision();
		if(value)
			Enable();
		else
			Disable();
	}

	void PhysXTerrainGeometryComponent::Reset()
	{

	}

	void PhysXTerrainGeometryComponent::Disable()
	{
	}

	void PhysXTerrainGeometryComponent::Enable()
	{
	}
}
