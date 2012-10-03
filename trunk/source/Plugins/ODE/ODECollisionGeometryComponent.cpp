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

#include "Plugins/ODE/ODECollisionGeometryComponent.h"
#include "Plugins/ODE/ODECollisionSystem.h"
#include "Plugins/ODE/ODEBodyComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"

#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Math/GASSAABox.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Scene/GASSSceneObjectTemplate.h"
#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSIMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSITerrainComponent.h"
#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include <boost/bind.hpp>

namespace GASS
{
	ODECollisionGeometryComponent::ODECollisionGeometryComponent():
		m_CollisionCategory(1),
		m_CollisionBits(1),
		m_GeomID(0),
		m_Type(CGT_NONE),
		m_Offset(0,0,0)
	{

	}

	ODECollisionGeometryComponent::~ODECollisionGeometryComponent()
	{
		
	}

	void ODECollisionGeometryComponent::RegisterReflection()
	{
		RegisterProperty<unsigned long>("CollisionBits", &GASS::ODECollisionGeometryComponent::GetCollisionBits, &GASS::ODECollisionGeometryComponent::SetCollisionBits);
		RegisterProperty<unsigned long>("CollisionCategory", &GASS::ODECollisionGeometryComponent::GetCollisionCategory, &GASS::ODECollisionGeometryComponent::SetCollisionCategory);
	}

	void ODECollisionGeometryComponent::OnInitialize()
	{
		//GetSceneObject()->RegisterForMessage(REG_TMESS(ODECollisionGeometryComponent::OnUnload,UnloadComponentsMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODECollisionGeometryComponent::OnTransformationChanged,TransformationNotifyMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODECollisionGeometryComponent::OnCollisionSettings,CollisionSettingsMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODECollisionGeometryComponent::OnGeometryChanged,GeometryChangedMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODECollisionGeometryComponent::OnGeometryScale,GeometryScaleMessage ,0));
		
		
	}
	
	void ODECollisionGeometryComponent::OnGeometryChanged(GeometryChangedMessagePtr message)
	{
		CreateGeometry();
	}


	void ODECollisionGeometryComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		Reset();
	}

	void ODECollisionGeometryComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		if(m_Type == CGT_TERRAIN)
			return;

		Vec3 pos = message->GetPosition();
		SetPosition(pos);
		Quaternion rot = message->GetRotation();
		SetRotation(rot);

		if(m_Type ==CGT_BOX)
			SetScale(message->GetScale());
		
	}

	void ODECollisionGeometryComponent::CreateGeometry()
	{
		Reset();
		//dSpaceID space = GetCollisionSystem()->GetSpace();
		//Check if we have mesh data????
		switch(m_Type)
		{
		case CGT_MESH:
			m_GeomID = CreateMeshGeometry();
			break;
		case CGT_BOX:
			m_GeomID = CreateBoxGeometry();
			break;
		case CGT_TERRAIN:
			m_GeomID = CreateTerrainGeometry();
			break;
		}
		dGeomSetBody(m_GeomID , NULL);
		dGeomSetData(m_GeomID , (void*)this);
		SetCollisionBits(m_CollisionBits);
		SetCollisionCategory(m_CollisionCategory);
	}
	
	void ODECollisionGeometryComponent::OnCollisionSettings(CollisionSettingsMessagePtr message)
	{
		bool value = message->EnableCollision();
		if(value)
			Enable();
		else
			Disable();
	}

	void ODECollisionGeometryComponent::Reset()
	{
		if(m_GeomID)
			dGeomDestroy(m_GeomID);
	}

	void ODECollisionGeometryComponent::SetPosition(const Vec3 &pos)
	{
		if(m_GeomID)
		{
			dGeomSetPosition(m_GeomID, pos.x, pos.y, pos.z);
		}
	}

	void ODECollisionGeometryComponent::SetRotation(const Quaternion &rot)
	{
		if(m_GeomID)
		{
			dReal ode_rot_mat[12];
			Mat4 rot_mat;
			rot_mat.Identity();
			rot.ToRotationMatrix(rot_mat);
			ODEPhysicsSceneManager::CreateODERotationMatrix(rot_mat,ode_rot_mat);
			dGeomSetRotation(m_GeomID, ode_rot_mat);
		}
	}

	void ODECollisionGeometryComponent::Disable()
	{
		if(m_GeomID)
			dGeomDisable(m_GeomID);
	}

	void ODECollisionGeometryComponent::Enable()
	{
		if(m_GeomID)
			dGeomEnable(m_GeomID);
	}


	ODECollisionSystemPtr ODECollisionGeometryComponent::GetCollisionSystem() const
	{
		ODECollisionSystemPtr system =  SimEngine::Get().GetSimSystemManager()->GetFirstSystem<ODECollisionSystem>();
		if(!system)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"ODECollisionSystem not found", "ODECollisionGeometryComponent::GetCollisionSystem");
		return system;
	}


	dGeomID ODECollisionGeometryComponent::CreateBoxGeometry()
	{
		GeometryComponentPtr geom = GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();
		if(!geom)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No GeometryComponent found while collision shape type is CST_BOX", "ODECollisionGeometryComponent::CreateBoxGeometry");
		}
		AABox box = geom->GetBoundingBox();
		Vec3 size = box.m_Max - box.m_Min;
		return dCreateBox(GetCollisionSystem()->GetSpace(), size.x, size.y, size.z); 
	}

	dGeomID ODECollisionGeometryComponent::CreateMeshGeometry()
	{
		dGeomID geom_id = 0;
		MeshComponentPtr mesh = GetSceneObject()->GetFirstComponentByClass<IMeshComponent>();
		if(mesh)
		{
			ODECollisionMeshInfo col_mesh = GetCollisionSystem()->CreateCollisionMesh(mesh);
			geom_id = dCreateTriMesh(GetCollisionSystem()->GetSpace(), col_mesh.ID, 0, 0, 0);
		}
		else
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No MeshComponent found while collision shape type is CST_MESH", "ODECollisionGeometryComponent::CreateMeshGeometry");
		}
		return geom_id; 
	}

	void ODECollisionGeometryComponent::OnGeometryScale(GeometryScaleMessagePtr message)
	{
		SetScale(message->GetScale());
	}

	void ODECollisionGeometryComponent::SetScale(const Vec3 &scale)
	{
		if(m_GeomID)
		{
			if(m_Type == CGT_BOX) //can only handle boxes right now
			{
				GeometryComponentPtr geom = GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();
				if(!geom)
				{
					GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No GeometryComponent found while collision shape type is CST_BOX", "ODECollisionGeometryComponent::OnGeometryScale");
				}
				//Get bounding box
				AABox box = geom->GetBoundingBox();
				Vec3 size = (box.m_Max - box.m_Min)*scale;
				dGeomBoxSetLengths(m_GeomID, size.x, size.y, size.z);
				LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
				if(location)
				{
					Vec3 offset = (box.m_Max + box.m_Min)*scale*0.5;
					Vec3 new_pos = location->GetWorldPosition() + offset;
					SetPosition(new_pos);
				}
			}
		}
	}


	unsigned long ODECollisionGeometryComponent::GetCollisionBits() const 
	{
		return m_CollisionBits;
	}

	void ODECollisionGeometryComponent::SetCollisionBits(unsigned long value)
	{
		m_CollisionBits = value;
		if(m_GeomID)
		{
			dGeomSetCollideBits (m_GeomID,m_CollisionBits);
		}
	}

	bool ODECollisionGeometryComponent::IsInitialized() const
	{
		return (m_GeomID == 0) ? false:true;
	}

	unsigned long  ODECollisionGeometryComponent::GetCollisionCategory() const 
	{
		return m_CollisionCategory;
	}

	void ODECollisionGeometryComponent::SetCollisionCategory(unsigned long value)
	{
		m_CollisionCategory =value;
		if(m_GeomID)
		{
			dGeomSetCategoryBits(m_GeomID, m_CollisionCategory );
		}
	}

	dGeomID ODECollisionGeometryComponent::CreateTerrainGeometry()
	{
		TerrainComponentPtr terrain= GetSceneObject()->GetFirstComponentByClass<ITerrainComponent>();

		if(!terrain)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No TerrainComponent found while collision shape type is CST_TERRAIN", "ODECollisionGeometryComponent::CreateTerrainGeometry");
		}
		
		GeometryComponentPtr geom = boost::shared_dynamic_cast<IGeometryComponent>(terrain);

		//save raw point for fast height access, not thread safe!!
		m_TerrainData.m_TerrainGeom = terrain.get();

		dGeomID geom_id = 0;

		if(terrain)
		{
			m_TerrainData.m_TerrainBounds = geom->GetBoundingBox();
			int samples_x = terrain->GetSamplesX();
			int samples_z = terrain->GetSamplesZ();
			Float size_x = m_TerrainData.m_TerrainBounds.m_Max.x - m_TerrainData.m_TerrainBounds.m_Min.x;
			Float size_z = m_TerrainData.m_TerrainBounds.m_Max.z - m_TerrainData.m_TerrainBounds.m_Min.z;
			m_TerrainData.m_SampleWidth = size_x/(samples_x-1);
			m_TerrainData.m_SampleHeight = size_z/(samples_z-1);

			//FileLog::Print("Terrain  samples_x:%d samples_y:%d size_x:%f size_y:%f",samples_x,samples_z,size_x,size_z);
			float thickness = 1;//m_TerrainBounds.m_Max.y - m_TerrainBounds.m_Min.y;
			//FileLog::Print("thickness %f",thickness );
			dHeightfieldDataID heightid = dGeomHeightfieldDataCreate();
			dGeomHeightfieldDataBuildCallback(	heightid, //getSpaceID(space),
				this, // pUserData ?
				ODECollisionGeometryComponent::TerrainHeightCallback,
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
			dGeomHeightfieldDataSetBounds( heightid, m_TerrainData.m_TerrainBounds.m_Min.y,  m_TerrainData.m_TerrainBounds.m_Max.y);
			geom_id = dCreateHeightfield( GetCollisionSystem()->GetSpace(), heightid, 1 );

			Vec3 center_position;
			center_position.x = m_TerrainData.m_TerrainBounds.m_Min.x + (m_TerrainData.m_TerrainBounds.m_Max.x - m_TerrainData.m_TerrainBounds.m_Min.x)*0.5;
			center_position.z = m_TerrainData.m_TerrainBounds.m_Min.z + (m_TerrainData.m_TerrainBounds.m_Max.z - m_TerrainData.m_TerrainBounds.m_Min.z)*0.5;
			center_position.y = 0;
			dGeomSetPosition(geom_id, center_position.x, center_position.y, center_position.z);
		}
		return geom_id;
	}
	
	dReal ODECollisionGeometryComponent::TerrainHeightCallback(void* data,int x,int z)
	{
		ODECollisionGeometryComponent* ode_terrain = (ODECollisionGeometryComponent*)data;
		return ode_terrain->GetTerrainHeight(x,z);
	}

	Float ODECollisionGeometryComponent::GetTerrainHeight(unsigned int x,unsigned int z)
	{
		Float world_x = x * m_TerrainData.m_SampleWidth + m_TerrainData.m_TerrainBounds.m_Min.x;
		Float world_z = z * m_TerrainData.m_SampleWidth + m_TerrainData.m_TerrainBounds.m_Min.z;
		Float h = m_TerrainData.m_TerrainGeom->GetHeight(world_x,world_z);
		return h;
	}
	ODECollisionGeometryComponent::TerrainData ODECollisionGeometryComponent::m_TerrainData = ODECollisionGeometryComponent::TerrainData();



}