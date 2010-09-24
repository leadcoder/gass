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

#include "Plugins/ODE/ODEGeometryComponent.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Plugins/ODE/ODEBodyComponent.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/Math/AABox.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include <boost/bind.hpp>

namespace GASS
{
	ODEGeometryComponent::ODEGeometryComponent()
		:m_ODESecondarySpaceID(NULL),
		m_ODESpaceID (NULL),
		m_Body (NULL),
		m_Friction(1),
		m_Offset(0,0,0),
		m_BBOffset(0,0,0),
		m_Slip(0),
		m_GeometryType(PGT_BOX),
		m_CollisionGeomScale(1,1,1),
		m_CollisionCategory(1),
		m_CollisionBits(1),
		m_GeomID(0),
		m_SecondGeomID(0),
		m_TransformGeomID(NULL),
		m_SecondTransformGeomID(NULL)
	{

	}

	ODEGeometryComponent::~ODEGeometryComponent()
	{
		Reset();
	}

	void ODEGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsGeometryComponent",new Creator<ODEGeometryComponent, IComponent>);

		RegisterProperty<Vec3>("Offset", &GASS::ODEGeometryComponent::GetOffset, &GASS::ODEGeometryComponent::SetOffset);
		RegisterProperty<float>("Friction", &GASS::ODEGeometryComponent::GetFriction, &GASS::ODEGeometryComponent::SetFriction);
		RegisterProperty<float>("Slip", &GASS::ODEGeometryComponent::GetSlip, &GASS::ODEGeometryComponent::SetSlip);
		RegisterProperty<long int>("CollisionBits", &GASS::ODEGeometryComponent::GetCollisionBits, &GASS::ODEGeometryComponent::SetCollisionBits);
		RegisterProperty<long int>("CollisionCategory", &GASS::ODEGeometryComponent::GetCollisionCategory, &GASS::ODEGeometryComponent::SetCollisionCategory);
		//RegisterVectorProperty<std::string>("CollisionBits", &GASS::ODEGeometryComponent::GetCollisionBits, &GASS::ODEGeometryComponent::SetCollisionBits);
		//RegisterVectorProperty<std::string>("CollisionCategories", &GASS::ODEGeometryComponent::GetCollisionCategory, &GASS::ODEGeometryComponent::SetCollisionCategory);
		RegisterProperty<std::string>("GeometryType", &GASS::ODEGeometryComponent::GetGeometryType, &GASS::ODEGeometryComponent::SetGeometryType);
	}

	void ODEGeometryComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEGeometryComponent::OnLoad,LoadPhysicsComponentsMessage ,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEGeometryComponent::OnTransformationChanged,TransformationNotifyMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEGeometryComponent::OnCollisionSettings,CollisionSettingsMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEGeometryComponent::OnGeometryChanged,GeometryChangedMessage,0));
		
	}

	void ODEGeometryComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		Vec3 pos = message->GetPosition();
		SetPosition(pos);

		Quaternion rot = message->GetRotation();
		SetRotation(rot);

		SetScale(message->GetScale());
		
	}

	void ODEGeometryComponent::OnCollisionSettings(CollisionSettingsMessagePtr message)
	{
		bool value = message->EnableCollision();
		if(value)
			Enable();
		else
			Disable();
	}

	void ODEGeometryComponent::OnLoad(LoadPhysicsComponentsMessagePtr message)
	{
		ODEPhysicsSceneManagerPtr scene_manager = boost::shared_static_cast<ODEPhysicsSceneManager> (message->GetPhysicsSceneManager());
		assert(scene_manager);
		m_SceneManager = scene_manager;

		m_Body = GetSceneObject()->GetFirstComponent<ODEBodyComponent>().get();
	}

	dSpaceID ODEGeometryComponent::GetStaticSpace()
	{
		if(m_ODESpaceID == NULL)
		{
			m_ODESpaceID = dSimpleSpaceCreate(ODEPhysicsSceneManagerPtr(m_SceneManager)->GetPhysicsSpace());
		}
		return m_ODESpaceID;
	}

	dSpaceID ODEGeometryComponent::GetSecondaryStaticSpace()
	{
		if(m_ODESecondarySpaceID == 0)
		{
			m_ODESecondarySpaceID = dSimpleSpaceCreate(ODEPhysicsSceneManagerPtr(m_SceneManager)->GetCollisionSpace());
		}
		return m_ODESecondarySpaceID;
	}

	void ODEGeometryComponent::CreateODEGeomFromGeom(IGeometryComponent* geom,
		dSpaceID space,
		dGeomID &geom_id,
		dGeomID &trans_geom_id,
		ODEBodyComponent* body)
	{

		AABox box = geom->GetBoundingBox();
		//Vec3 maxVec3 = box.m_Max - m_Owner->GetPosition();
		//Vec3 min = box.m_Min - m_Owner->GetPosition();
		Vec3 bb_size = (box.m_Max - box.m_Min)*m_CollisionGeomScale;
		//bb_size = bb_size*m_Owner->GetScale();

		if(bb_size.x <= 0) //Set default size
			bb_size.x = 0.01;
		if(bb_size.y <= 0)
		  bb_size.y = 0.01;
		if(bb_size.z <= 0)
			bb_size.z = 0.01;

		Sphere sphere = geom->GetBoundingSphere();
		sphere.m_Radius *= m_CollisionGeomScale.x;//*m_Owner->GetScale().x;

		if(sphere.m_Radius <= 0) //Set default size
		{
			sphere.m_Radius = 1;
		}

		
		m_BBSize = bb_size;
		m_BSSize = sphere.m_Radius;


		
		Vec3 geom_offset(0,0,0);


		switch(m_GeometryType)
		{
		case PGT_PLANE:
			{
				Vec3 plane_normal = GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->GetSceneUp();
				geom_id = dCreatePlane(space, plane_normal.x, plane_normal.y, plane_normal.z,1);
				//geom_id = dCreatePlane(space, 0, 0, 1,1);
			}
			break;
		case PGT_BOX:
			{
				geom_offset = box.m_Max + box.m_Min;
				geom_offset = geom_offset*0.5f;
				geom_id= dCreateBox(0, bb_size.x, bb_size.y, bb_size.z);
				//Log::Print("BBsize:%f,%f,%f",bb_size.x, bb_size.y, bb_size.z);
			}
			break;
		case PGT_CYLINDER:
			{

				geom_offset = box.m_Max + box.m_Min;
				geom_offset = geom_offset*0.5f;

				float radius=std::max(bb_size.x/2.f,bb_size.y/2.f);
				float length=bb_size.z-radius;
				geom_id = dCreateCCylinder (0, radius, length);

			}
			break;
		case PGT_SPHERE:
			{
				geom_offset = box.m_Max + box.m_Min;
				geom_offset = geom_offset*0.5f;
				geom_id  = dCreateSphere(0, sphere.m_Radius);
			}
			break;
		case PGT_MESH:
			{
				IMeshComponent* mesh  = dynamic_cast<IMeshComponent*>(geom);

				if(mesh)
				{
					ODECollisionMesh col_mesh = ODEPhysicsSceneManagerPtr(m_SceneManager)->CreateCollisionMesh(mesh);
					geom_id = dCreateTriMesh(0, col_mesh.ID, 0, 0, 0);

				}
			}
			break;
		case PGT_TERRAIN:
				geom_id = CreateTerrain(geom,  0);
			break;

		}

		
		m_BBOffset = geom_offset;
		Vec3 temp_offset = m_Offset + geom_offset;
		//Set the clean-up mode of geometry transform. If the clean-up mode is 1,
		//then the encapsulated object will be destroyed when the geometry transform is destroyed.
		//If the clean-up mode is 0 this does not happen. The default clean-up mode is 0.

		if(m_GeometryType != PGT_PLANE)
		{
			trans_geom_id = dCreateGeomTransform(space);
			dGeomTransformSetCleanup(trans_geom_id, 1 );
			dGeomTransformSetGeom(trans_geom_id,geom_id);
			dGeomSetPosition(geom_id, temp_offset.x, temp_offset.y, temp_offset.z);
		}
		else
			trans_geom_id = geom_id;

		if(body)
		{
			dGeomSetBody(trans_geom_id, body->GetODEBodyComponent());
		}
		else
		{
			//SetGeomTransformation(trans_geom_id,m_Owner);
			dGeomSetBody(trans_geom_id, NULL);
		}
		dGeomSetData(trans_geom_id, (void*)this);
	}

	void ODEGeometryComponent::SetPosition(const Vec3 &pos)
	{
		if(m_Body == NULL && m_TransformGeomID && m_GeometryType != PGT_PLANE)
		{
			dGeomSetPosition(m_TransformGeomID, pos.x, pos.y, pos.z);
			dGeomSetPosition(m_SecondTransformGeomID, pos.x, pos.y, pos.z);
		}
	}

	void ODEGeometryComponent::SetRotation(const Quaternion &rot)
	{
		if(m_Body == NULL && m_TransformGeomID && m_GeometryType != PGT_PLANE)
		{
			dReal ode_rot_mat[12];
			Mat4 rot_mat;
			rot_mat.Identity();
			rot.ToRotationMatrix(rot_mat);
			ODEPhysicsSceneManager::CreateODERotationMatrix(rot_mat,ode_rot_mat);
			dGeomSetRotation(m_TransformGeomID, ode_rot_mat);
			dGeomSetRotation(m_SecondTransformGeomID, ode_rot_mat);
		}
	}

	void ODEGeometryComponent::Disable()
	{
		if(m_TransformGeomID) dGeomDisable(m_TransformGeomID);
		if(m_SecondTransformGeomID) dGeomDisable(m_SecondTransformGeomID);
	}

	void ODEGeometryComponent::Enable()
	{
		if(m_TransformGeomID) dGeomEnable(m_TransformGeomID);
		if(m_SecondTransformGeomID) dGeomEnable(m_SecondTransformGeomID);
	}

	void ODEGeometryComponent::Reset()
	{
		if(m_TransformGeomID) dGeomDestroy(m_TransformGeomID);
		if(m_SecondTransformGeomID) dGeomDestroy(m_SecondTransformGeomID);
		if(m_ODESpaceID) dSpaceDestroy(m_ODESpaceID);
		if(m_ODESecondarySpaceID) dSpaceDestroy(m_ODESecondarySpaceID);
		m_TransformGeomID = NULL;
		m_SecondTransformGeomID = NULL;
		m_ODESpaceID = NULL;
		m_ODESecondarySpaceID = NULL;
	}


	void ODEGeometryComponent::OnGeometryChanged(GeometryChangedMessagePtr message)
	{
		Reset();
		GeometryComponentPtr geom;
		if(m_GeometryTemplate != "")
		{
			geom = boost::shared_dynamic_cast<IGeometryComponent>(GetSceneObject()->GetComponent(m_GeometryTemplate));
		}
		else geom = GetSceneObject()->GetFirstComponent<IGeometryComponent>();
		if(geom)
		{
			if(m_Body)
			{
				CreateODEGeomFromGeom(geom.get(),m_Body->GetSpace(),m_GeomID,m_TransformGeomID,m_Body);
				CreateODEGeomFromGeom(geom.get(),m_Body->GetSecondarySpace(),m_SecondGeomID,m_SecondTransformGeomID,m_Body);
				if (m_Body->GetMassRepresentation() == ODEBodyComponent::MR_GEOMETRY)
					CreateODEMassFromGeom(geom.get(),m_Body);
				
			}
			else
			{
				CreateODEGeomFromGeom(geom.get(),GetStaticSpace(),m_GeomID,m_TransformGeomID,NULL);
				CreateODEGeomFromGeom(geom.get(),GetSecondaryStaticSpace(),m_SecondGeomID,m_SecondTransformGeomID,NULL);
				
				
			}
			SetCollisionBits(m_CollisionBits);
			SetCollisionCategory(m_CollisionCategory);
		}
	}

	void ODEGeometryComponent::SetSizeFromGeom(dGeomID id, GeometryComponentPtr geom)
	{
		//we we only resize/rescale box,sphere and cylinder right now
		float radius = geom->GetBoundingSphere().m_Radius;
		Vec3 bbsize = geom->GetBoundingBox().m_Max - geom->GetBoundingBox().m_Min;
		switch(m_GeometryType)
		{
		case PGT_BOX:
			dGeomBoxSetLengths(id,bbsize.x,bbsize.y,bbsize.z);
			break;
		case PGT_CYLINDER:
			dGeomCCylinderSetParams(id,bbsize.x/2.f, bbsize.y);
			break;
		case PGT_SPHERE:
			dGeomSphereSetRadius(id,radius);
			break;
		case PGT_PLANE:
			break;
		}
	}

	void ODEGeometryComponent::SetScale(const Vec3 &value, dGeomID id)
	{
		//we we only resize/rescale box,sphere and cylinder right now
		float radius = m_BSSize*value.x;
		Vec3 bbsize = m_BBSize*value;

		Vec3 offset = m_Offset + m_BBOffset*value;
		switch(m_GeometryType)
		{
		case PGT_BOX:
			dGeomBoxSetLengths(id,bbsize.x,bbsize.y,bbsize.z);
			dGeomSetPosition(id, offset.x, offset.y, offset.z);
			break;
		case PGT_CYLINDER:
			dGeomCCylinderSetParams(id,bbsize.x/2.f, bbsize.y);
			dGeomSetPosition(id, offset.x, offset.y, offset.z);
			break;
		case PGT_SPHERE:
			dGeomSphereSetRadius(id,radius);
			dGeomSetPosition(id, offset.x, offset.y, offset.z);
			break;
		case PGT_PLANE:
			break;
		}
	}

	void ODEGeometryComponent::SetScale(const Vec3 &value)
	{
		SetScale(value,m_GeomID);
		SetScale(value,m_SecondGeomID);
	}

	void ODEGeometryComponent::SetGeometryType(const std::string &geom_type)
	{
		std::string geometryTypeNameLC = Misc::ToLower(geom_type);
		if(geometryTypeNameLC.compare("mesh")==0)
		{
			m_GeometryType = PGT_MESH;
		}
		else if(geometryTypeNameLC.compare("box")==0)
		{
			m_GeometryType = PGT_BOX;
		}
		else if(geometryTypeNameLC.compare("terrain")==0)
		{
			m_GeometryType = PGT_TERRAIN;
		}
		else if(geometryTypeNameLC.compare("cylinder")==0)
		{
			m_GeometryType = PGT_CYLINDER;
		}
		else if(geometryTypeNameLC.compare("sphere")==0)
		{
			m_GeometryType = PGT_SPHERE;
		}
		else if(geometryTypeNameLC.compare("plane")==0)
		{
			m_GeometryType = PGT_PLANE;
		}
		else
		{
			Log::Error("Unknown geometry type %s",geometryTypeNameLC.c_str());
		}
	}

	std::string ODEGeometryComponent::GetGeometryType() const
	{
		switch(m_GeometryType)
		{
		case PGT_MESH:
			return "mesh";
		case PGT_BOX:
			return "box";
			break;
		case PGT_CYLINDER:
			return "cylinder";
		case PGT_SPHERE:
			return "sphere";
		case PGT_TERRAIN:
			return "terrain";
		case PGT_PLANE:
			return "plane";
		default:
			return "unknown";
		}
	}

	void ODEGeometryComponent::CreateODEMassFromGeom(IGeometryComponent* geom,ODEBodyComponent* body)
	{
		AABox box = geom->GetBoundingBox();
		//Vec3 max = box.m_Max - m_Owner->GetPosition();
		//Vec3 min = box.m_Min - m_Owner->GetPosition();
		Vec3 bb_size = (box.m_Max - box.m_Min)*m_CollisionGeomScale;
		dMass ode_mass;
		switch(m_GeometryType)
		{
		//case PGT_PLANE:
		case PGT_MESH:
		case PGT_BOX:
			{
				dMassSetBoxTotal(&ode_mass, body->GetMass(), bb_size.x, bb_size.y, bb_size.z);
			}
			break;
		case PGT_CYLINDER:
			{
				dMassSetCylinderTotal(&ode_mass, body->GetMass(),3, bb_size.x/2.f, bb_size.y);
			}
			break;

		case PGT_SPHERE:
			{
				Sphere sphere = geom->GetBoundingSphere();
				sphere.m_Radius *= m_CollisionGeomScale.x;
				dMassSetSphereTotal(&ode_mass, body->GetMass(), sphere.m_Radius);
				break;
			}
		}
		body->SetODEMass(ode_mass);
	}

	dGeomID ODEGeometryComponent::CreateTerrain(IGeometryComponent* geom,  dSpaceID space)
	{
		m_TerrainGeom = dynamic_cast<ITerrainComponent*>(geom);
		dGeomID geom_id = 0;

		if(m_TerrainGeom)
		{

			int samples_x;
			int samples_z;
			Float size_x;
			Float size_z;
			Vec3 center_pos;

			m_TerrainGeom->GetBounds(m_TerrainBounds.m_Min,m_TerrainBounds.m_Max);
			samples_x = m_TerrainGeom->GetSamplesX();
			samples_z = m_TerrainGeom->GetSamplesZ();
			size_x = m_TerrainBounds.m_Max.x - m_TerrainBounds.m_Min.x;
			size_z = m_TerrainBounds.m_Max.z - m_TerrainBounds.m_Min.z;
			m_SampleWidth = size_x/(samples_x-1);
			m_SampleHeight = size_z/(samples_z-1);

			//Log::Print("Terrain  samples_x:%d samples_y:%d size_x:%f size_y:%f",samples_x,samples_z,size_x,size_z);

			float thickness = 1;//m_TerrainBounds.m_Max.y - m_TerrainBounds.m_Min.y;
			//Log::Print("thickness %f",thickness );

			dHeightfieldDataID heightid = dGeomHeightfieldDataCreate();
			dGeomHeightfieldDataBuildCallback(	heightid, //getSpaceID(space),
				this, // pUserData ?
				ODEGeometryComponent::TerrainHeightCallback,
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
			geom_id = dCreateHeightfield( space, heightid, 1 );

			Vec3 center_position;
			center_position.x = m_TerrainBounds.m_Min.x + (m_TerrainBounds.m_Max.x - m_TerrainBounds.m_Min.x)*0.5;
			center_position.z = m_TerrainBounds.m_Min.z + (m_TerrainBounds.m_Max.z - m_TerrainBounds.m_Min.z)*0.5;
			center_position.y = 0;
			m_Offset = center_position;
			//dGeomSetPosition(geom_id,center_position.x,center_position.y,center_position.z);
		}
		return geom_id;
	}

	dReal ODEGeometryComponent::TerrainHeightCallback(void* data,int x,int z)
	{
		ODEGeometryComponent* ode_terrain = (ODEGeometryComponent*)data;
		return ode_terrain->GetTerrainHeight(x,z);
	}

	Float ODEGeometryComponent::GetTerrainHeight(unsigned int x,unsigned int z)
	{
		Float world_x = x * m_SampleWidth + m_TerrainBounds.m_Min.x;
		Float world_z = z * m_SampleWidth + m_TerrainBounds.m_Min.z;
		return m_TerrainGeom->GetHeight(world_x,world_z);
	}


	long int ODEGeometryComponent::GetCollisionBits() const 
	{
		return m_CollisionBits;
	}

	void ODEGeometryComponent::SetCollisionBits(long int value)
	{
		m_CollisionBits = value;

		if(m_GeomID)
		{
			dGeomSetCollideBits (m_GeomID,m_CollisionBits);
			dGeomSetCollideBits (m_TransformGeomID, m_CollisionBits);
			dGeomSetCollideBits (m_SecondGeomID,m_CollisionBits);
			dGeomSetCollideBits (m_SecondTransformGeomID, m_CollisionBits);
		}
	}

	long int ODEGeometryComponent::GetCollisionCategory() const 
	{
		return m_CollisionCategory;
	}

	void ODEGeometryComponent::SetCollisionCategory(long int value)
	{
		m_CollisionCategory =value;
		if(m_GeomID)
		{

			dGeomSetCategoryBits(m_GeomID, m_CollisionCategory );
			dGeomSetCategoryBits(m_TransformGeomID, m_CollisionCategory );
			
			dGeomSetCategoryBits(m_SecondGeomID, m_CollisionCategory);
			dGeomSetCategoryBits(m_SecondTransformGeomID, m_CollisionCategory);
		}
	}

	


}
