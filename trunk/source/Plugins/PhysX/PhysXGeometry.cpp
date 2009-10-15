/****************************************************************************	
*                                                                           *
* HiFiEngine                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: hifiengine@gmail.com                                               *
* Web page: http://n00b.dyndns.org/HiFiEngine                               *
*                                                                           *
* HiFiEngine is only used with knowledge from the author. This software     *
* is not al
lowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the HiFiEngine license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 
#include "Plugins/PhysX/PhysXGeometry.h"
#include "Plugins/PhysX/PhysXBody.h"
#include "Plugins/PhysX/PhysXPhysicsSystem.h"
#include "Plugins/PhysX/PhysXPhysicsSceneManager.h"

#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/Math/AABox.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"


#include <boost/bind.hpp>


namespace GASS
{

	PhysXGeometry::PhysXGeometry() :m_StaticActor(NULL)
	{
		m_Offset.Set(0,0,0);
		m_CollisionGeomScale = Vec3(1,1,1);
	}

	PhysXGeometry::~PhysXGeometry()
	{
		
	}


	void PhysXGeometry::Enable()
	{
		
	}

	void PhysXGeometry::Disable()
	{
		
	}

	void PhysXGeometry::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysXGeometry",new Creator<PhysXGeometry, IComponent>);
		RegisterProperty<Vec3>("Offset", &GASS::PhysXGeometry::GetOffset, &GASS::PhysXGeometry::SetOffset);
		RegisterProperty<float>("Friction", &GASS::PhysXGeometry::GetFriction, &GASS::PhysXGeometry::SetFriction);
		RegisterProperty<std::string>("GeometryType", &GASS::PhysXGeometry::GetGeometryType, &GASS::PhysXGeometry::SetGeometryType);
	}

	void PhysXGeometry::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(OBJECT_RM_LOAD_PHYSICS_COMPONENTS,  TYPED_MESSAGE_FUNC(PhysXGeometry::OnLoad, LoadPhysicsComponentsMessage),0);
		GetSceneObject()->RegisterForMessage(OBJECT_NM_TRANSFORMATION_CHANGED,  TYPED_MESSAGE_FUNC(PhysXGeometry::OnTransformationChanged, TransformationNotifyMessage));
		GetSceneObject()->RegisterForMessage(OBJECT_RM_COLLISION_SETTINGS,  TYPED_MESSAGE_FUNC(PhysXGeometry::OnCollisionSettings,CollisionSettingsMessage ));
	}

	void PhysXGeometry::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		Vec3 pos = message->GetPosition();
		SetPosition(pos);

		Quaternion rot = message->GetRotation();
		SetRotation(rot);
	}

	void PhysXGeometry::OnCollisionSettings(CollisionSettingsMessagePtr message)
	{
		bool value = message->EnableCollision();
		if(value)
			Enable();
		else
			Disable();
	}

	void PhysXGeometry::OnLoad(LoadPhysicsComponentsMessagePtr message)
	{
		m_SceneManager = static_cast<PhysXPhysicsSceneManager*>(message->GetPhysicsSceneManager());
		assert(m_SceneManager);

		boost::shared_ptr<IGeometryComponent> geom;
		if(m_GeometryTemplate != "")
		{
			geom = boost::shared_dynamic_cast<IGeometryComponent>(GetSceneObject()->GetComponent(m_GeometryTemplate));
		}
		else geom = GetSceneObject()->GetFirstComponent<IGeometryComponent>();

		m_Body  = GetSceneObject()->GetFirstComponent<PhysXBody>();
		if(geom)
		{
			CreateShape(geom,m_Body);
			if(m_Body)
			{
				//CreateODEGeomFromGeom(geom.get(),m_Body->GetSpace(),m_GeomID,m_TransformGeomID,m_Body);
				//CreateODEGeomFromGeom(geom.get(),m_Body->GetSecondarySpace(),m_SecondGeomID,m_SecondTransformGeomID,m_Body);
				//if (m_Body->GetMassRepresentation() == ODEBody::MR_GEOMETRY)
				//	CreateODEMassFromGeom(geom.get(),m_Body);
				
			}
			else
			{
				//CreateODEGeomFromGeom(geom.get(),GetStaticSpace(),m_GeomID,m_TransformGeomID,NULL);
				//CreateODEGeomFromGeom(geom.get(),GetSecondaryStaticSpace(),m_SecondGeomID,m_SecondTransformGeomID,NULL);
			}
		}
	}

	void PhysXGeometry::CreateShape(GeometryComponentPtr geom, PhysXBodyPtr body)
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
		
		NxShapeDesc* shape;
		switch(m_GeometryType)
		{
		case PGT_BOX:
			{
				NxBoxShapeDesc* boxDesc = new NxBoxShapeDesc();
				boxDesc->dimensions = NxVec3(bb_size.x, bb_size.y, bb_size.z);
				shape = boxDesc;
				//actorDesc.shapes.pushBack(&boxDesc);
				//actorDesc.body			= NULL;//&bodyDesc;
				//actorDesc.density		= 10.0f;
				//actorDesc.globalPose.t  =  NxVec3(0,0,0);
				//NxActor *actor = m_SceneManager->GetNxScene()->createActor(actorDesc);
			}
			break;
		case PGT_CYLINDER:
			{
				NxCapsuleShapeDesc capsuleDesc;
	
				NxCapsuleShapeDesc* capDesc = new NxCapsuleShapeDesc();
				capDesc->radius = max(bb_size.x/2.f,bb_size.y/2.f);
				capDesc->height = bb_size.z-capDesc->radius;
				
				geom_offset = box.m_Max + box.m_Min;
				geom_offset = geom_offset*0.5f;

				//float radius=std::max(bb_size.x/2.f,bb_size.y/2.f);
				//float length=bb_size.z-radius;
				//geom_id = dCreateCCylinder (0, radius, length);

			}
			break;
		case PGT_SPHERE:
			{
				NxSphereShapeDesc* sphereDesc = new NxSphereShapeDesc();
				sphereDesc->radius = sphere.m_Radius;
				shape = sphereDesc;
				//geom_id  = dCreateSphere(0, sphere.m_Radius);
			}
			break;
		case PGT_TERRAIN:
		case PGT_MESH:
			{
				MeshComponentPtr mesh  = boost::shared_dynamic_cast<IMeshComponent>(geom);

				if(mesh)
				{
					PhysXPhysicsSystemPtr sys = SimEngine::GetPtr()->GetSystemManager()->GetFirstSystem<PhysXPhysicsSystem>();
					NxCollisionMesh col_mesh = sys->CreateCollisionMesh(mesh.get());
					if(col_mesh.NxMesh)
					{
						NxTriangleMeshShapeDesc* meshShapeDesc = new NxTriangleMeshShapeDesc();    
						meshShapeDesc->meshData = col_mesh.NxMesh;
						meshShapeDesc->meshFlags = NX_MESH_DOUBLE_SIDED;
						//meshShapeDesc->localPose.t	= NxVec3(0, 0, 0);
						
						//meshShapeDesc->meshPagingMode = NX_MESH_PAGING_AUTO;
						shape = meshShapeDesc;
						
					}
				}
			}
			break;
		}

		if(shape)
		{
			if(body)
				body->AddShape(shape);
			else // static geometry
			{
				NxActorDesc adesc;
				//adesc.body		= NULL;
				//adesc.globalPose.t = NxVec3(0,0,0);
				adesc.shapes.pushBack(shape);
				m_StaticActor = m_SceneManager->GetNxScene()->createActor(adesc);


				NxPlaneShapeDesc planeDesc;
			    NxActorDesc actorDesc;
				actorDesc.shapes.pushBack(&planeDesc);
				m_SceneManager->GetNxScene()->createActor(actorDesc);

			}
		}
		
	}

	void PhysXGeometry::SetPosition(const Vec3 &pos)
	{
		if(m_Body == NULL)
		{
			//dGeomSetPosition(m_TransformGeomID, pos.x, pos.y, pos.z);
			//dGeomSetPosition(m_SecondTransformGeomID, pos.x, pos.y, pos.z);
		}
	}

	void PhysXGeometry::SetRotation(const Quaternion &rot)
	{
		if(m_Body == NULL)
		{
			Mat4 rot_mat;
			rot_mat.Identity();
			rot.ToRotationMatrix(rot_mat);
			//ODEPhysicsSceneManager::CreateODERotationMatrix(rot_mat,ode_rot_mat);
			//dGeomSetRotation(m_TransformGeomID, ode_rot_mat);
			//dGeomSetRotation(m_SecondTransformGeomID, ode_rot_mat);
		}
	}
	
	void PhysXGeometry::SetScale(const Vec3 &value)
	{
		//SetScale(value,m_GeomID);
		//SetScale(value,m_SecondGeomID);
	}

	void PhysXGeometry::SetGeometryType(const std::string &geom_type)
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
		else
		{
			Log::Error("Unknown geometry type %s",geometryTypeNameLC.c_str());
		}
	}

	std::string PhysXGeometry::GetGeometryType() const
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
		default:
			return "unknown";
		}
	}

}
