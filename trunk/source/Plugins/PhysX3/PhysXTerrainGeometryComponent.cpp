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

#include "Plugins/PhysX3/PhysXTerrainGeometryComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"
#include "Plugins/PhysX3/PhysXBodyComponent.h"
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
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"
#include <boost/bind.hpp>

namespace GASS
{
	PhysXTerrainGeometryComponent::PhysXTerrainGeometryComponent(): m_Friction(1),
		m_CollisionCategory(1),
		m_CollisionBits(1),
		m_Debug(false)
	{

	}

	PhysXTerrainGeometryComponent::~PhysXTerrainGeometryComponent()
	{

	}

	void PhysXTerrainGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsTerrainGeometryComponent",new Creator<PhysXTerrainGeometryComponent, IComponent>);
		RegisterProperty<unsigned long>("CollisionBits", &GASS::PhysXTerrainGeometryComponent::GetCollisionBits, &GASS::PhysXTerrainGeometryComponent::SetCollisionBits);
		RegisterProperty<unsigned long>("CollisionCategory", &GASS::PhysXTerrainGeometryComponent::GetCollisionCategory, &GASS::PhysXTerrainGeometryComponent::SetCollisionCategory);
	}

	void PhysXTerrainGeometryComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXTerrainGeometryComponent::OnCollisionSettings,CollisionSettingsMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXTerrainGeometryComponent::OnGeometryChanged,GeometryChangedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXTerrainGeometryComponent::OnPhysicsDebug,PhysicsDebugMessage,0));
	
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		assert(scene_manager);
		m_SceneManager = scene_manager;
	}

	void PhysXTerrainGeometryComponent::OnDelete()
	{
		Reset();
	}

	void PhysXTerrainGeometryComponent::OnGeometryChanged(GeometryChangedMessagePtr message)
	{
		Reset();
		m_Shape = CreateTerrain();
		SetCollisionBits(m_CollisionBits);
		SetCollisionCategory(m_CollisionCategory);
	}

	HeightmapTerrainComponentPtr PhysXTerrainGeometryComponent::GetTerrainComponent() const 
	{
		HeightmapTerrainComponentPtr geom;
		if(m_GeometryTemplate != "")
		{
			geom = boost::shared_dynamic_cast<IHeightmapTerrainComponent>(GetSceneObject()->GetComponent(m_GeometryTemplate));
		}
		else geom = GetSceneObject()->GetFirstComponentByClass<IHeightmapTerrainComponent>();
		return geom;
	}

	physx::PxShape* PhysXTerrainGeometryComponent::CreateTerrain()
	{
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();

		HeightmapTerrainComponentPtr terrain = GetTerrainComponent();
		GeometryComponentPtr geom = boost::shared_dynamic_cast<IGeometryComponent>(terrain);

		//save raw point for fast height access, not thread safe!!
		m_TerrainGeom = terrain.get();

		physx::PxShape* shape = NULL;	

		if(terrain)
		{
			m_TerrainBounds = geom->GetBoundingBox();
			int samples_x = terrain->GetSamples();
			int samples_z = terrain->GetSamples();
			Float size_x = m_TerrainBounds.m_Max.x - m_TerrainBounds.m_Min.x;
			Float size_z = m_TerrainBounds.m_Max.z - m_TerrainBounds.m_Min.z;
			m_SampleWidth = size_x/(samples_x-1);
			m_SampleHeight = size_z/(samples_z-1);

			Float scale_x = size_x/(Float) samples_x;
			Float scale_z = size_z/(Float) samples_z;
			//physx::PxHeightFieldSample* samples = (physx::PxHeightFieldSample*) system->GetAllocator()->allocate(sizeof(physx::PxHeightFieldSample)*samples_x*samples_z,0,__FILE__, __LINE__);
			physx::PxHeightFieldSample* samples = (physx::PxHeightFieldSample*) new physx::PxHeightFieldSample[samples_x*samples_z];//(sizeof(physx::PxHeightFieldSample)*(samples_x*samples_z));

			const physx::PxReal heightScale = 0.01f;
			memset(samples,0,samples_x*samples_z*sizeof(physx::PxHeightFieldSample));

			for(physx::PxU32 x = 0; x < samples_x; x++)
			{
				for(physx::PxU32 z = 0; z < samples_z; z++)
				{
					Float world_x = x * m_SampleWidth + m_TerrainBounds.m_Min.x;
					Float world_z = z * m_SampleWidth + m_TerrainBounds.m_Min.z;
					Float height = m_TerrainGeom->GetHeightAtWorldLocation(world_x,world_z);

					samples[z+x*samples_x].height = (physx::PxI16)(height/heightScale);
					//samples[x+z*samples_x].setTessFlag();
					samples[z+x*samples_x].materialIndex0=0;
					samples[z+x*samples_x].materialIndex1=0;
				}
			}

			physx::PxHeightFieldDesc hfDesc;

			hfDesc.format             = physx::PxHeightFieldFormat::eS16_TM;
			hfDesc.nbColumns          = samples_x;
			hfDesc.nbRows             = samples_z;
			hfDesc.samples.data       = samples;
			hfDesc.samples.stride     = sizeof(physx::PxHeightFieldSample);
			//hfDesc.thickness = 0.1;
			//hfDesc.convexEdgeThreshold = 0;
			//hfDesc.flags = 0;

			physx::PxHeightField* heightField = system->GetPxSDK()->createHeightField(hfDesc);

			physx::PxTransform pose = physx::PxTransform::createIdentity();

			Vec3 center_position;
			center_position.x = m_TerrainBounds.m_Min.x;
			center_position.z = m_TerrainBounds.m_Min.z;
		//	center_position.x = m_TerrainBounds.m_Min.x + (m_TerrainBounds.m_Max.x - m_TerrainBounds.m_Min.x)*0.5;
		//	center_position.z = m_TerrainBounds.m_Min.z + (m_TerrainBounds.m_Max.z - m_TerrainBounds.m_Min.z)*0.5;
			
			center_position.y = 0;

			pose.p = physx::PxVec3(center_position.x,center_position.y,center_position.z);

			physx::PxRigidStatic* hfActor = system->GetPxSDK()->createRigidStatic(pose);

			physx::PxHeightFieldGeometry hfGeom(heightField, physx::PxMeshGeometryFlags(), heightScale, scale_x, scale_z);
			shape = hfActor->createShape(hfGeom, *system->GetDefaultMaterial());

			physx::PxFilterData collFilterData;
			collFilterData.word0=COLLISION_FLAG_GROUND;
			collFilterData.word1=COLLISION_FLAG_GROUND_AGAINST;
			shape->setSimulationFilterData(collFilterData);

			PhysXPhysicsSceneManagerPtr sm = PhysXPhysicsSceneManagerPtr(m_SceneManager);
			sm->GetPxScene()->addActor(*hfActor);
		}
		return shape;
	}

	unsigned long PhysXTerrainGeometryComponent::GetCollisionBits() const 
	{
		return m_CollisionBits;
	}

	void PhysXTerrainGeometryComponent::SetCollisionBits(unsigned long value)
	{
		m_CollisionBits = value;
	}

	void PhysXTerrainGeometryComponent::OnCollisionSettings(CollisionSettingsMessagePtr message)
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

	unsigned long PhysXTerrainGeometryComponent::GetCollisionCategory() const 
	{
		return m_CollisionCategory;
	}

	void PhysXTerrainGeometryComponent::SetCollisionCategory(unsigned long value)
	{
		m_CollisionCategory =value;

	}

	void PhysXTerrainGeometryComponent::OnPhysicsDebug(PhysicsDebugMessagePtr message)
	{
		//SetDebug(message->DebugGeometry());
	}
}

