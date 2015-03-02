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


#include "Plugins/ODE/ODETerrainGeometryComponent.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Plugins/ODE/ODEBodyComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplateManager.h"

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


namespace GASS
{
	ODETerrainGeometryComponent::ODETerrainGeometryComponent():
		m_SpaceID (NULL),
		m_Friction(1),
		m_CollisionCategory(1),
		m_CollisionBits(1),
		m_GeomID(0),
		m_Debug(false)
	{

	}

	ODETerrainGeometryComponent::~ODETerrainGeometryComponent()
	{
		//Reset();
	}

	void ODETerrainGeometryComponent::RegisterReflection()
	{
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("ODETerrainGeometryComponent", OF_VISIBLE)));

		RegisterProperty<unsigned long>("CollisionBits", &GASS::ODETerrainGeometryComponent::GetCollisionBits, &GASS::ODETerrainGeometryComponent::SetCollisionBits);
		RegisterProperty<unsigned long>("CollisionCategory", &GASS::ODETerrainGeometryComponent::GetCollisionCategory, &GASS::ODETerrainGeometryComponent::SetCollisionCategory);
	}

	void ODETerrainGeometryComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODETerrainGeometryComponent::OnCollisionSettings,CollisionSettingsRequest ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODETerrainGeometryComponent::OnGeometryChanged,GeometryChangedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODETerrainGeometryComponent::OnPhysicsDebug,PhysicsDebugRequest,0));
	
		ODEPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<ODEPhysicsSceneManager>();
		assert(scene_manager);
		m_SceneManager = scene_manager;
	}

	void ODETerrainGeometryComponent::OnDelete()
	{
		Reset();
	}

	void ODETerrainGeometryComponent::OnGeometryChanged(GeometryChangedEventPtr message)
	{
		Reset();
		m_GeomID = CreateTerrain();
		dGeomSetData(m_GeomID, (void*)this);
		SetCollisionBits(m_CollisionBits);
		SetCollisionCategory(m_CollisionCategory);
	}

	HeightmapTerrainComponentPtr ODETerrainGeometryComponent::GetTerrainComponent() const 
	{
		HeightmapTerrainComponentPtr terrain;
		if(m_GeometryTemplate != "")
		{
			terrain = DYNAMIC_PTR_CAST<IHeightmapTerrainComponent>(GetSceneObject()->GetComponent(m_GeometryTemplate));
		}
		else terrain = GetSceneObject()->GetFirstComponentByClass<IHeightmapTerrainComponent>();
		return terrain;
	}

	dGeomID ODETerrainGeometryComponent::CreateTerrain()
	{
		HeightmapTerrainComponentPtr terrain = GetTerrainComponent();
		GeometryComponentPtr geom = DYNAMIC_PTR_CAST<IGeometryComponent>(terrain);

		//save raw point for fast height access, not thread safe!!
		m_TerrainGeom = terrain.get();

		dGeomID geom_id = 0;

		if(terrain)
		{
			m_TerrainBounds = geom->GetBoundingBox();
			//m_Samples = terrain->GetWidth();
			int samples_x = terrain->GetNumSamplesW();
			int samples_z = terrain->GetNumSamplesH();
			Float size_x = m_TerrainBounds.m_Max.x - m_TerrainBounds.m_Min.x;
			Float size_z = m_TerrainBounds.m_Max.z - m_TerrainBounds.m_Min.z;
			m_SampleWidth = size_x/(samples_x-1);
			m_SampleHeight = size_z/(samples_z-1);

			//FileLog::Print("Terrain  samples_x:%d samples_y:%d size_x:%f size_y:%f",samples_x,samples_z,size_x,size_z);
			float thickness = 1;//m_TerrainBounds.m_Max.y - m_TerrainBounds.m_Min.y;
			//FileLog::Print("thickness %f",thickness );
			dHeightfieldDataID heightid = dGeomHeightfieldDataCreate();
			dGeomHeightfieldDataBuildCallback(	heightid, //getSpaceID(space),
				this, // pUserData ?
				ODETerrainGeometryComponent::TerrainHeightCallback,
				size_x, //X
				size_z, //Z
				samples_x, // w // Vertex count along edge >= 2
				samples_z, // h // Vertex count along edge >= 2
				1.0,     //vScale
				0.0,	// vOffset
				thickness,	// vThickness
				0); // nWrapMode
			/*dGeomHeightfieldDataBuildSingle( heightid,
			m_TerrainGeom->GetHeightData(), 1,
			size_x, size_z, samples_x, samples_z,
			1, 0, thickness, 0);*/

			// Give some very bounds which, while conservative,
			// makes AABB computation more accurate than +/-INF.
			dGeomHeightfieldDataSetBounds( heightid, m_TerrainBounds.m_Min.y,  m_TerrainBounds.m_Max.y);
			geom_id = dCreateHeightfield( GetSpace(), heightid, 1 );

			Vec3 center_position;
			center_position.x = m_TerrainBounds.m_Min.x + (m_TerrainBounds.m_Max.x - m_TerrainBounds.m_Min.x)*0.5;
			center_position.z = m_TerrainBounds.m_Min.z + (m_TerrainBounds.m_Max.z - m_TerrainBounds.m_Min.z)*0.5;
			center_position.y = 0;
			dGeomSetPosition(geom_id, center_position.x, center_position.y, center_position.z);

			//FileLog::Print("Terrain  pos:%f %f %f",center_position.x, center_position.y, center_position.z);
			//m_Offset = center_position;
			//dGeomSetPosition(geom_id,center_position.x,center_position.y,center_position.z);
		}
		return geom_id;
	}


	//Not thread safe!!! TODO:Keep a copy of the terrain height map instead
	dReal ODETerrainGeometryComponent::TerrainHeightCallback(void* data,int x,int z)
	{
		ODETerrainGeometryComponent* ode_terrain = (ODETerrainGeometryComponent*)data;
		return ode_terrain->GetTerrainHeight(x,z);
	}

	Float ODETerrainGeometryComponent::GetTerrainHeight(unsigned int x,unsigned int z)
	{
		//return m_TerrainGeom->GetHeightAtPoint(x,m_Samples-1-z);
		return m_TerrainGeom->GetHeightAtSample(x,z);
		/*Float world_x = x * m_SampleWidth + m_TerrainBounds.m_Min.x;
		Float world_z = z * m_SampleWidth + m_TerrainBounds.m_Min.z;
		Float h = m_TerrainGeom->GetHeightAtWorldLocation(world_x,world_z);
		*/
	}

	unsigned long ODETerrainGeometryComponent::GetCollisionBits() const 
	{
		return m_CollisionBits;
	}

	void ODETerrainGeometryComponent::SetCollisionBits(unsigned long value)
	{
		m_CollisionBits = value;
		if(m_GeomID)
		{
			dGeomSetCollideBits (m_GeomID,m_CollisionBits);
		}
	}

	void ODETerrainGeometryComponent::OnCollisionSettings(CollisionSettingsRequestPtr message)
	{
		bool value = message->EnableCollision();
		if(value)
			Enable();
		else
			Disable();
	}

	void ODETerrainGeometryComponent::Reset()
	{
		if(m_SpaceID) dSpaceDestroy(m_SpaceID);
		m_SpaceID = NULL;
	}

	void ODETerrainGeometryComponent::Disable()
	{
		if(m_GeomID) dGeomDisable(m_GeomID);
	}

	void ODETerrainGeometryComponent::Enable()
	{
		if(m_GeomID) dGeomEnable(m_GeomID);
	}

	unsigned long ODETerrainGeometryComponent::GetCollisionCategory() const 
	{
		return m_CollisionCategory;
	}

	void ODETerrainGeometryComponent::SetCollisionCategory(unsigned long value)
	{
		m_CollisionCategory =value;
		if(m_GeomID)
		{
			dGeomSetCategoryBits(m_GeomID, m_CollisionCategory );
		}
	}

	dSpaceID ODETerrainGeometryComponent::GetSpace()
	{
		if(m_SpaceID == NULL)
		{
			m_SpaceID = dSimpleSpaceCreate(ODEPhysicsSceneManagerPtr(m_SceneManager)->GetPhysicsSpace());
		}
		return m_SpaceID;
	}

	void ODETerrainGeometryComponent::OnPhysicsDebug(PhysicsDebugRequestPtr message)
	{
		//SetDebug(message->DebugGeometry());
	}
}

