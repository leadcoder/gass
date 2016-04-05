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



#include "Plugins/ODE/Collision/ODECollisionGeometryComponent.h"
#include "Plugins/ODE/Collision/ODECollisionSystem.h"
#include "Plugins/ODE/Collision/ODECollisionSceneManager.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplateManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Math/GASSAABox.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneObjectTemplate.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIResourceComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/GASSPhysicsMesh.h"
#ifdef _MSC_VER
#define NOMINMAX
#include <algorithm>
#endif

namespace GASS
{
	ODECollisionGeometryComponent::ODECollisionGeometryComponent():
		m_GeomID(0),
		m_OffsetGeomID(0),
		m_Type(CGT_NONE),
		m_Offset(0,0,0),
		m_TerrainData(NULL)
	{
		m_ColMeshInfo.ID = 0;
	}

	ODECollisionGeometryComponent::~ODECollisionGeometryComponent()
	{
		delete m_TerrainData;
		if(m_ColMeshInfo.ID)
		{
			dGeomTriMeshDataDestroy(m_ColMeshInfo.ID);
		}
	}

	void ODECollisionGeometryComponent::RegisterReflection()
	{
		RegisterProperty<std::string>("Type", &GASS::ODECollisionGeometryComponent::GetTypeByName, &GASS::ODECollisionGeometryComponent::SetTypeByName);
	}

	void ODECollisionGeometryComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODECollisionGeometryComponent::OnTransformationChanged,TransformationChangedEvent ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODECollisionGeometryComponent::OnCollisionSettings,CollisionSettingsRequest ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODECollisionGeometryComponent::OnGeometryFlagsChanged,GeometryFlagsChangedEvent ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODECollisionGeometryComponent::OnGeometryChanged,GeometryChangedEvent ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODECollisionGeometryComponent::OnGeometryScale,GeometryScaleRequest ,0));
		m_CollisionSceneManager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<ODECollisionSceneManager>();
	}

	std::string ODECollisionGeometryComponent::GetTypeByName() const
	{
		switch(m_Type)
		{
			case CGT_MESH:
				return "MESH";
			case CGT_TERRAIN:
				return "TERRAIN";
			case CGT_BOX:
				return "BOX";
			case CGT_PLANE:
				return "PLANE";
			case CGT_NONE:
				return "NONE";
			default:
				return "NONE";
		}
	}

	void ODECollisionGeometryComponent::SetTypeByName(const std::string &type)
	{
		if(type == "MESH")
			m_Type =  CGT_MESH;
		else if(type == "TERRAIN")
			m_Type = CGT_TERRAIN;
		else if(type == "BOX")
			m_Type = CGT_BOX;
		else if(type == "PLANE")
			m_Type = CGT_PLANE;
		else if(type == "NONE")
			m_Type = CGT_NONE;
		else
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS,"Unkown type:" + type, " ODECollisionGeometryComponent::SetTypeByName");
			
	}
	
	void ODECollisionGeometryComponent::OnGeometryChanged(GeometryChangedEventPtr message)
	{
		CreateGeometry();
	}

	void ODECollisionGeometryComponent::OnDelete()
	{
	 
		Reset();
	}

	void ODECollisionGeometryComponent::OnTransformationChanged(TransformationChangedEventPtr message)
	{
		if(m_Type == CGT_TERRAIN)
			return;

		if(m_Type == CGT_PLANE)
		{
			if(m_GeomID == NULL)
			{
				CreateGeometry();
				SetFlags(GEOMETRY_FLAG_UNKNOWN);
			}
			return;
		}

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
		case CGT_PLANE:
			m_GeomID = CreatePlaneGeometry();
		}

		if(m_GeomID)
		{
			GASS_MUTEX_LOCK(GetCollisionSceneManager()->GetMutex());
			dGeomSetBody(m_GeomID , NULL);
			dGeomSetData(m_GeomID , (void*)this);
		}

		GeometryComponentPtr geom = GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();
		if(geom)
		{
			SetFlags(geom->GetGeometryFlags());
		}
	}

	void ODECollisionGeometryComponent::OnGeometryFlagsChanged(GeometryFlagsChangedEventPtr message)
	{
		SetFlags(message->GetGeometryFlags());
	}
	
	void ODECollisionGeometryComponent::OnCollisionSettings(CollisionSettingsRequestPtr message)
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
		{
			GASS_MUTEX_LOCK(GetCollisionSceneManager()->GetMutex());
			dGeomDestroy(m_GeomID);
			m_GeomID = 0;
		}
	}

	void ODECollisionGeometryComponent::SetPosition(const Vec3 &pos)
	{
		if(m_GeomID)
		{
			GASS_MUTEX_LOCK(GetCollisionSceneManager()->GetMutex());
			dGeomSetPosition(m_GeomID, pos.x, pos.y, pos.z);
		}
	}

	void ODECollisionGeometryComponent::SetRotation(const Quaternion &rot)
	{
		if(m_GeomID)
		{
			GASS_MUTEX_LOCK(GetCollisionSceneManager()->GetMutex());
			dReal ode_rot_mat[12];
			Mat4 rot_mat;
			rot_mat.Identity();
			rot.ToRotationMatrix(rot_mat);
			CreateODERotationMatrix(rot_mat,ode_rot_mat);
			dGeomSetRotation(m_GeomID, ode_rot_mat);
		}
	}

	void ODECollisionGeometryComponent::Disable()
	{
		if(m_GeomID)
		{
			GASS_MUTEX_LOCK(GetCollisionSceneManager()->GetMutex());
			dGeomDisable(m_GeomID);
		}
	}

	void ODECollisionGeometryComponent::Enable()
	{
		if(m_GeomID)
		{
			GASS_MUTEX_LOCK(GetCollisionSceneManager()->GetMutex());
			dGeomEnable(m_GeomID);
		}
	}

	ODECollisionSceneManagerPtr ODECollisionGeometryComponent::GetCollisionSceneManager() const
	{
		return m_CollisionSceneManager;
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
		
		m_OffsetGeomID = dCreateBox(0, size.x, size.y, size.z);

		Vec3 offset = (box.m_Max + box.m_Min)*0.5;
		GASS_MUTEX_LOCK(GetCollisionSceneManager()->GetMutex());
		dGeomSetPosition(m_OffsetGeomID, offset.x, offset.y, offset.z);
		dGeomID gid = dCreateGeomTransform(GetCollisionSceneManager()->GetSpace());
		dGeomTransformSetCleanup(gid, 1 );
		dGeomTransformSetGeom(gid,m_OffsetGeomID);
		return gid;
	}

	dGeomID ODECollisionGeometryComponent::CreatePlaneGeometry()
	{
		
		LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		if(!location)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No LocationComponent found while collision shape type is CST_PLANE", "ODECollisionGeometryComponent::CreatePlaneGeometry");
		}
		Mat4 rot_mat;
		rot_mat.Identity();
		location->GetWorldRotation().ToRotationMatrix(rot_mat);
		Vec3 pos = location->GetWorldPosition();
		Vec3 normal = rot_mat.GetYAxis();

		GASS_MUTEX_LOCK(GetCollisionSceneManager()->GetMutex());

		dGeomID geom_id = dCreatePlane(GetCollisionSceneManager()->GetSpace(), normal.x, normal.y, normal.z, pos.x+pos.y+pos.z);
		return geom_id;
		//dGeomPlaneSetParams(m_GeomID, normal.x, normal.y, normal.z, pos - );
	}

	dGeomID ODECollisionGeometryComponent::CreateMeshGeometry()
	{
		
		dGeomID geom_id = 0;
		MeshComponentPtr mesh = GetSceneObject()->GetFirstComponentByClass<IMeshComponent>();
		if(mesh)
		{
			//first check cache!
			ResourceComponentPtr res  = GetSceneObject()->GetFirstComponentByClass<IResourceComponent>();
			ODECollisionMeshInfo col_mesh;
			bool has_col_mesh = false;
			std::string col_mesh_id; 
			if(res)
			{
				std::string col_mesh_id = res->GetResource().Name();
				GASS_MUTEX_LOCK(GetCollisionSceneManager()->GetMutex());

				if(GetCollisionSceneManager()->HasCollisionMesh(col_mesh_id)) //check cache
				{
					col_mesh = GetCollisionSceneManager()->GetCollisionMesh(col_mesh_id);
					has_col_mesh = true;
				}
				else
				{
					GraphicsMesh gfx_mesh_data = mesh->GetMeshData();
					PhysicsMeshPtr physics_mesh(new PhysicsMesh(gfx_mesh_data));
					col_mesh = GetCollisionSceneManager()->CreateCollisionMeshAndCache(col_mesh_id,physics_mesh);
				}
				geom_id = dCreateTriMesh(GetCollisionSceneManager()->GetSpace(), col_mesh.ID, 0, 0, 0);
			}
			else //probably manual mesh
			{
				GraphicsMesh gfx_mesh_data = mesh->GetMeshData();
				PhysicsMeshPtr physics_mesh(new PhysicsMesh(gfx_mesh_data));
				if(physics_mesh->PositionVector.size() > 0 )
				{
					if(m_ColMeshInfo.ID)
					{
						dGeomTriMeshDataDestroy(m_ColMeshInfo.ID);
					}
					m_ColMeshInfo = GetCollisionSceneManager()->_CreateCollisionMesh(physics_mesh);
					geom_id = dCreateTriMesh(GetCollisionSceneManager()->GetSpace(), m_ColMeshInfo.ID, 0, 0, 0);
				}
			}
		}
		else
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No MeshComponent found while collision shape type is CST_MESH", "ODECollisionGeometryComponent::CreateMeshGeometry");
		}
		return geom_id; 
	}

	void ODECollisionGeometryComponent::OnGeometryScale(GeometryScaleRequestPtr message)
	{
		//update scale
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
				//Get bounding box,  note that scale already applied in bounding box!
				AABox box = geom->GetBoundingBox();
				Vec3 size = box.m_Max - box.m_Min;

				GASS_MUTEX_LOCK(GetCollisionSceneManager()->GetMutex());

				dGeomBoxSetLengths(m_OffsetGeomID, size.x, size.y, size.z);
				Vec3 offset = (box.m_Max + box.m_Min)*0.5;
				dGeomSetPosition(m_OffsetGeomID, offset.x, offset.y, offset.z);

				//also reset position, know why but offset change is not reflected otherwise
				const dReal* pos = dGeomGetPosition(m_GeomID);
				dGeomSetPosition(m_GeomID, pos[0], pos[1], pos[2]);
			}
		}
	}

	bool ODECollisionGeometryComponent::IsInitialized() const
	{
		return (m_GeomID == 0) ? false:true;
	}

	unsigned long  ODECollisionGeometryComponent::GetFlags() const 
	{
		if(m_GeomID)
		{
			GASS_MUTEX_LOCK(GetCollisionSceneManager()->GetMutex());
			return  dGeomGetCategoryBits(m_GeomID);
		}
		return 0;
	}

	void ODECollisionGeometryComponent::SetFlags(unsigned long value)
	{
		if(m_GeomID)
		{
			GASS_MUTEX_LOCK(GetCollisionSceneManager()->GetMutex());
			dGeomSetCollideBits(m_GeomID,0);
			dGeomSetCategoryBits(m_GeomID, value);
		}
	}

	dGeomID ODECollisionGeometryComponent::CreateTerrainGeometry()
	{
		
		HeightmapTerrainComponentPtr terrain= GetSceneObject()->GetFirstComponentByClass<IHeightmapTerrainComponent>();

		if(!terrain)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No TerrainComponent found while collision shape type is CST_TERRAIN", "ODECollisionGeometryComponent::CreateTerrainGeometry");
		}
		
		GeometryComponentPtr geom = GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(terrain);

		//save raw point for fast height access, not thread safe!!
	

		dGeomID geom_id = 0;

		if(terrain)
		{
			m_TerrainData = new TerrainData;
			m_TerrainData->m_TerrainGeom = terrain.get();
			m_TerrainData->m_TerrainBounds = geom->GetBoundingBox();
			int samples_x = terrain->GetNumSamplesW();
			int samples_z = terrain->GetNumSamplesH();
			m_TerrainData->m_Samples = samples_x;
			
			Float size_x = m_TerrainData->m_TerrainBounds.m_Max.x - m_TerrainData->m_TerrainBounds.m_Min.x;
			Float size_z = m_TerrainData->m_TerrainBounds.m_Max.z - m_TerrainData->m_TerrainBounds.m_Min.z;
			m_TerrainData->m_SampleWidth = size_x/(samples_x-1);
			m_TerrainData->m_SampleHeight = size_z/(samples_z-1);

			//FileLog::Print("Terrain  samples_x:%d samples_y:%d size_x:%f size_y:%f",samples_x,samples_z,size_x,size_z);
			float thickness = 1;//m_TerrainBounds.m_Max.y - m_TerrainBounds.m_Min.y;
			
			GASS_MUTEX_LOCK(GetCollisionSceneManager()->GetMutex());

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
			//dGeomHeightfieldDataSetBounds( heightid, m_TerrainData->m_TerrainBounds.m_Min.y,  m_TerrainData->m_TerrainBounds.m_Max.y);
			//if we support dynamic terrains (ie terrain editor) min,max is unkonwn att start so just set some values predefined values
			dGeomHeightfieldDataSetBounds( heightid, 0, 2000);
			geom_id = dCreateHeightfield( GetCollisionSceneManager()->GetSpace(), heightid, 1 );

			Vec3 center_position;
			center_position.x = m_TerrainData->m_TerrainBounds.m_Min.x + (m_TerrainData->m_TerrainBounds.m_Max.x - m_TerrainData->m_TerrainBounds.m_Min.x)*0.5;
			center_position.z = m_TerrainData->m_TerrainBounds.m_Min.z + (m_TerrainData->m_TerrainBounds.m_Max.z - m_TerrainData->m_TerrainBounds.m_Min.z)*0.5;
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
		return m_TerrainData->m_TerrainGeom->GetHeightAtSample(x,z);
		//return m_TerrainData->m_TerrainGeom->GetHeightAtPoint(x,z);
	}

	void ODECollisionGeometryComponent::CreateODERotationMatrix(const Mat4 &m, dReal *ode_mat)
	{
		//Make ODE rotation matrix

		ode_mat[0] = m.m_Data2[0];
		ode_mat[1] = m.m_Data2[1];
		ode_mat[2] = m.m_Data2[2];
		ode_mat[3] = 0;
		ode_mat[4] = m.m_Data2[4];
		ode_mat[5] = m.m_Data2[5];
		ode_mat[6] = m.m_Data2[6];
		ode_mat[7] = 0;
		ode_mat[8] = m.m_Data2[8];
		ode_mat[9] = m.m_Data2[9];
		ode_mat[10]= m.m_Data2[10];
		ode_mat[11] = 0;
	}

	void ODECollisionGeometryComponent::CreateGASSRotationMatrix(const dReal *R, Mat4 &m)
	{
		m.m_Data2[0] = R[0];
		m.m_Data2[1] = R[1];
		m.m_Data2[2] = R[2];
		m.m_Data2[3] = 0;
		m.m_Data2[4] = R[4];
		m.m_Data2[5] = R[5];
		m.m_Data2[6]= R[6];
		m.m_Data2[7]= 0;
		m.m_Data2[8] = R[8];
		m.m_Data2[9] = R[9];
		m.m_Data2[10] = R[10];
		m.m_Data2[11] = 0;
		m.m_Data2[12] = 0;
		m.m_Data2[13] = 0;
		m.m_Data2[14] = 0;
		m.m_Data2[15] = 1;

	}
}