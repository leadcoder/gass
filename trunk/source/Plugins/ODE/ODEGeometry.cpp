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

#include "Plugins/ODE/ODEGeometry.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Plugins/ODE/ODEBody.h"
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
#include <boost/bind.hpp>


namespace GASS
{
	ODEGeometry::ODEGeometry()
		:m_ODESecondarySpaceID(NULL),
		m_ODESpaceID (NULL),
		m_Body (NULL),
		m_SceneManager(NULL),
		m_TransformGeomID(NULL),
		m_SecondTransformGeomID(NULL),
		m_Friction(1),
		m_Offset(0,0,0),
		m_Slip(0),
		m_GeometryType(PGT_BOX),
		m_CollisionGeomScale(1,1,1)
	{

	}

	ODEGeometry::~ODEGeometry()
	{
		if(m_TransformGeomID) dGeomDestroy(m_TransformGeomID);
		if(m_SecondTransformGeomID) dGeomDestroy(m_SecondTransformGeomID);
		if(m_ODESpaceID) dSpaceDestroy(m_ODESpaceID);
		if(m_ODESecondarySpaceID) dSpaceDestroy(m_ODESecondarySpaceID);
	}

	void ODEGeometry::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("ODEGeometry",new Creator<ODEGeometry, IComponent>);

		RegisterProperty<Vec3>("Offset", &GASS::ODEGeometry::GetOffset, &GASS::ODEGeometry::SetOffset);
		//RegisterProperty<std::string>("GeometryComponent", &SetGeometryComponent, &GetGeometryComponent);
		RegisterProperty<float>("Friction", &GASS::ODEGeometry::GetFriction, &GASS::ODEGeometry::SetFriction);
		RegisterProperty<float>("Slip", &GASS::ODEGeometry::GetSlip, &GASS::ODEGeometry::SetSlip);
		RegisterProperty<std::string>("GeometryType", &GASS::ODEGeometry::GetGeometryType, &GASS::ODEGeometry::SetGeometryType);

		//REGISTER_PROP(String,IPhysicsGeometry,m_AddToBody,"AddToBody",CProperty::STREAM|CProperty::READONLY,"");
		//REGISTER_PROP(String,IPhysicsGeometry,m_GeometryTemplate,"GeometryTemplate",CProperty::STREAM|CProperty::READONLY,"");
		//REGISTER_PROP(String,IPhysicsGeometry,m_GeometryTypeName,"GeometryType",CProperty::STREAM|CProperty::READONLY,"");
		//REGISTER_PROP(Vect3D,IPhysicsGeometry,m_Offset,"Offset",CProperty::STREAM|CProperty::READONLY,"");
		//REGISTER_PROP(Vect3D,IPhysicsGeometry,m_CollisionGeomScale,"Scale",CProperty::STREAM|CProperty::READONLY,"");
	}

	void ODEGeometry::OnCreate()
	{
		REGISTER_OBJECT_MESSAGE_CLASS(ODEGeometry::OnLoad,LoadPhysicsComponentsMessage ,1);
		REGISTER_OBJECT_MESSAGE_CLASS(ODEGeometry::OnTransformationChanged,TransformationNotifyMessage ,0);
		REGISTER_OBJECT_MESSAGE_CLASS(ODEGeometry::OnCollisionSettings,CollisionSettingsMessage ,0);
	}

	void ODEGeometry::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		Vec3 pos = message->GetPosition();
		SetPosition(pos);

		Quaternion rot = message->GetRotation();
		SetRotation(rot);
	}

	void ODEGeometry::OnCollisionSettings(CollisionSettingsMessagePtr message)
	{
		bool value = message->EnableCollision();
		if(value)
			Enable();
		else
			Disable();
	}

	void ODEGeometry::OnLoad(LoadPhysicsComponentsMessagePtr message)
	{
		m_SceneManager = static_cast<ODEPhysicsSceneManager*>(message->GetPhysicsSceneManager());
		assert(m_SceneManager);

		m_Body = GetSceneObject()->GetFirstComponent<ODEBody>().get();

		boost::shared_ptr<IGeometryComponent> geom;
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
				if (m_Body->GetMassRepresentation() == ODEBody::MR_GEOMETRY)
					CreateODEMassFromGeom(geom.get(),m_Body);
			}
			else
			{
				CreateODEGeomFromGeom(geom.get(),GetStaticSpace(),m_GeomID,m_TransformGeomID,NULL);
				CreateODEGeomFromGeom(geom.get(),GetSecondaryStaticSpace(),m_SecondGeomID,m_SecondTransformGeomID,NULL);
			}
		}
	}

	dSpaceID ODEGeometry::GetStaticSpace()
	{
		if(m_ODESpaceID == NULL)
		{
			m_ODESpaceID = dSimpleSpaceCreate(m_SceneManager->GetPhysicsSpace());
		}
		return m_ODESpaceID;
	}

	dSpaceID ODEGeometry::GetSecondaryStaticSpace()
	{
		if(m_ODESecondarySpaceID == 0)
		{
			m_ODESecondarySpaceID = dSimpleSpaceCreate(m_SceneManager->GetCollisionSpace());
		}
		return m_ODESecondarySpaceID;
	}

	void ODEGeometry::CreateODEGeomFromGeom(IGeometryComponent* geom,
		dSpaceID space,
		dGeomID &geom_id,
		dGeomID &trans_geom_id,
		ODEBody* body)
	{

		AABox box = geom->GetBoundingBox();
		//Vec3 maxVec3 = box.m_Max - m_Owner->GetPosition();
		//Vec3 min = box.m_Min - m_Owner->GetPosition();
		Vec3 bb_size = (box.m_Max - box.m_Min)*m_CollisionGeomScale;
		//bb_size = bb_size*m_Owner->GetScale();

		Sphere sphere = geom->GetBoundingSphere();
		sphere.m_Radius *= m_CollisionGeomScale.x;//*m_Owner->GetScale().x;
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
					ODECollisionMesh col_mesh = m_SceneManager->CreateCollisionMesh(mesh);
					geom_id = dCreateTriMesh(0, col_mesh.ID, 0, 0, 0);

				}
			}
			break;
		case PGT_TERRAIN:
				geom_id = CreateTerrain(geom,  0);
			break;

		}

		m_Offset = m_Offset + geom_offset;
		//Set the clean-up mode of geometry transform. If the clean-up mode is 1,
		//then the encapsulated object will be destroyed when the geometry transform is destroyed.
		//If the clean-up mode is 0 this does not happen. The default clean-up mode is 0.

		if(m_GeometryType != PGT_PLANE)
		{
			trans_geom_id = dCreateGeomTransform(space);
			dGeomTransformSetCleanup(trans_geom_id, 1 );
			dGeomTransformSetGeom(trans_geom_id,geom_id);
			dGeomSetPosition(geom_id, m_Offset.x, m_Offset.y, m_Offset.z);
		}
		else
			trans_geom_id = geom_id;
		

		if(body)
		{
			dGeomSetBody(trans_geom_id, body->GetODEBody());
		}
		else
		{
			//SetGeomTransformation(trans_geom_id,m_Owner);
			dGeomSetBody(trans_geom_id, NULL);
		}
		dGeomSetData(trans_geom_id, (void*)this);
	}

	void ODEGeometry::SetPosition(const Vec3 &pos)
	{
		if(m_Body == NULL && m_TransformGeomID && m_GeometryType != PGT_PLANE)
		{
			dGeomSetPosition(m_TransformGeomID, pos.x, pos.y, pos.z);
			dGeomSetPosition(m_SecondTransformGeomID, pos.x, pos.y, pos.z);
		}
	}

	void ODEGeometry::SetRotation(const Quaternion &rot)
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

	void ODEGeometry::Disable()
	{
		dGeomDisable(m_TransformGeomID);
		dGeomDisable(m_SecondTransformGeomID);
	}

	void ODEGeometry::Enable()
	{
		dGeomEnable(m_TransformGeomID);
		dGeomEnable(m_SecondTransformGeomID);
	}

	void ODEGeometry::SetScale(const Vec3 &value, dGeomID id)
	{
		//we we only resize/rescale box,sphere and cylinder right now
		float radius = m_BSSize*value.x;
		Vec3 bbsize = m_BBSize*value;
		switch(m_GeometryType)
		{
		case PGT_BOX:
			dGeomBoxSetLengths(id,bbsize.x,bbsize.y,bbsize.z);
			dGeomSetPosition(id, m_Offset.x*value.x, m_Offset.y*value.y, m_Offset.z*value.z);
			break;
		case PGT_CYLINDER:
			dGeomCCylinderSetParams(id,bbsize.x/2.f, bbsize.y);
			dGeomSetPosition(id, m_Offset.x*value.x, m_Offset.y*value.y, m_Offset.z*value.z);
			break;
		case PGT_SPHERE:
			dGeomSphereSetRadius(id,radius);
			dGeomSetPosition(id, m_Offset.x*value.x, m_Offset.y*value.y, m_Offset.z*value.z);
			break;
		case PGT_PLANE:
			break;
		}
	}

	void ODEGeometry::SetScale(const Vec3 &value)
	{
		SetScale(value,m_GeomID);
		SetScale(value,m_SecondGeomID);
	}

	void ODEGeometry::SetGeometryType(const std::string &geom_type)
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

	std::string ODEGeometry::GetGeometryType() const
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

	void ODEGeometry::CreateODEMassFromGeom(IGeometryComponent* geom,ODEBody* body)
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

	dGeomID ODEGeometry::CreateTerrain(IGeometryComponent* geom,  dSpaceID space)
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

			float thickness = 1;//m_TerrainBounds.m_Max.y - m_TerrainBounds.m_Min.y;
			//Log::Print("thickness %f",thickness );

			dHeightfieldDataID heightid = dGeomHeightfieldDataCreate();
			dGeomHeightfieldDataBuildCallback(	heightid, //getSpaceID(space),
				this, // pUserData ?
				ODEGeometry::TerrainHeightCallback,
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

	dReal ODEGeometry::TerrainHeightCallback(void* data,int x,int z)
	{
		ODEGeometry* ode_terrain = (ODEGeometry*)data;
		return ode_terrain->GetTerrainHeight(x,z);
	}

	Float ODEGeometry::GetTerrainHeight(unsigned int x,unsigned int z)
	{
		Float world_x = x * m_SampleWidth + m_TerrainBounds.m_Min.x;
		Float world_z = z * m_SampleWidth + m_TerrainBounds.m_Min.x;
		return m_TerrainGeom->GetHeight(world_x,world_z);
	}

	/*	bool ODEGeometry::WantsContact( dContact & contact, IPhysicsObject * other, dGeomID you, dGeomID him, bool firstTest )
	{
	if(m_Slip < 0)
	return true;

	//Calcaulte slip param
	float inv = 1;
	dBodyID bid = dGeomGetBody( you );
	bool makeNoise = true;
	float noiseVolume = 0;
	dReal const * vel = dBodyGetLinearVel( bid );
	//float colVel = ((Vector3 const &)*vel) / (Vector3 &)contact.geom.normal;
	float colVel = Math::Dot(((Vec3 const &)*vel) , (Vec3 &)contact.geom.normal);

	Vec3 front;
	Vec3 up;
	//Compute fDir1 if a wheel
	dReal const * R = dBodyGetRotation(bid);
	front = Vec3( R[2] * inv, R[6] * inv, R[10] * inv );
	up = Vec3( R[1] * inv, R[5] * inv, R[9] * inv );
	//  Set Slip2
	contact.surface.mode |= dContactSlip2 | dContactFDir1;
	float v = sqrtf( vel[0]*vel[0] + vel[1]*vel[1] + vel[2]*vel[2] );
	contact.surface.slip2 = m_Slip * v;
	//  Turn on Approx1
	contact.surface.mode |= dContactApprox1;
	//  re-tweak mu and mu2
	contact.surface.mu = contact.surface.mu2 = m_Friction;
	//  The theory is that it doesn't matter if "front" points "up"
	//  because we want fdir2 to be orthogonal to "front" and contact
	//  normal. Of course, if "front" points in the direction of the
	//  contact, this can be a problem. In that case, choose "up".
	if( fabsf( dDot( contact.geom.normal, &front.x, 3 ) ) > 0.5f )
	{
	((Vec3 &)contact.fdir1) = up;
	}
	else
	{
	((Vec3 &)contact.fdir1) = front;
	}
	return true;
	}*/


}
