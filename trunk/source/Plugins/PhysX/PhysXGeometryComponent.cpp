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

#include "Plugins/PhysX/PhysXGeometryComponent.h"
#include "Plugins/PhysX/PhysXBodyComponent.h"
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

	PhysXGeometryComponent::PhysXGeometryComponent() :m_StaticActor(NULL)
	{
		m_Offset.Set(0,0,0);
		m_CollisionGeomScale = Vec3(1,1,1);
	}

	PhysXGeometryComponent::~PhysXGeometryComponent()
	{

	}


	void PhysXGeometryComponent::Enable()
	{

	}

	void PhysXGeometryComponent::Disable()
	{

	}

	void PhysXGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsGeometryComponent",new Creator<PhysXGeometryComponent, IComponent>);
		RegisterProperty<Vec3>("Offset", &GASS::PhysXGeometryComponent::GetOffset, &GASS::PhysXGeometryComponent::SetOffset);
		RegisterProperty<float>("Friction", &GASS::PhysXGeometryComponent::GetFriction, &GASS::PhysXGeometryComponent::SetFriction);
		RegisterProperty<std::string>("GeometryType", &GASS::PhysXGeometryComponent::GetGeometryType, &GASS::PhysXGeometryComponent::SetGeometryType);
	}

	void PhysXGeometryComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(OBJECT_RM_LOAD_PHYSICS_COMPONENTS,  TYPED_MESSAGE_FUNC(PhysXGeometryComponent::OnLoad, LoadPhysicsComponentsMessage),0);
		GetSceneObject()->RegisterForMessage(OBJECT_NM_TRANSFORMATION_CHANGED,  TYPED_MESSAGE_FUNC(PhysXGeometryComponent::OnTransformationChanged, TransformationNotifyMessage));
		GetSceneObject()->RegisterForMessage(OBJECT_RM_COLLISION_SETTINGS,  TYPED_MESSAGE_FUNC(PhysXGeometryComponent::OnCollisionSettings,CollisionSettingsMessage ));
	}

	void PhysXGeometryComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		Vec3 pos = message->GetPosition();
		SetPosition(pos);

		Quaternion rot = message->GetRotation();
		SetRotation(rot);
	}

	void PhysXGeometryComponent::OnCollisionSettings(CollisionSettingsMessagePtr message)
	{
		bool value = message->EnableCollision();
		if(value)
			Enable();
		else
			Disable();
	}

	void PhysXGeometryComponent::OnLoad(LoadPhysicsComponentsMessagePtr message)
	{
		m_SceneManager = boost::shared_dynamic_cast<PhysXPhysicsSceneManager>(message->GetPhysicsSceneManager());
		assert(m_SceneManager);

		boost::shared_ptr<IGeometryComponent> geom;
		if(m_GeometryTemplate != "")
		{
			geom = boost::shared_dynamic_cast<IGeometryComponent>(GetSceneObject()->GetComponent(m_GeometryTemplate));
		}
		else geom = GetSceneObject()->GetFirstComponent<IGeometryComponent>();

		m_Body  = GetSceneObject()->GetFirstComponent<PhysXBodyComponent>();
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

	void PhysXGeometryComponent::CreateShape(GeometryComponentPtr geom, PhysXBodyComponentPtr body)
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
				boxDesc->setToDefault();
				boxDesc->dimensions = NxVec3(bb_size.x/2.0f, bb_size.y/2.0f, bb_size.z/2.0f);
				shape = boxDesc;

				geom_offset = box.m_Max + box.m_Min;
				geom_offset = geom_offset*0.5f;

				shape->localPose.t  =  NxVec3(geom_offset.x,geom_offset.y,geom_offset.z);
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
				capDesc->setToDefault();
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
				sphereDesc->setToDefault();
				sphereDesc->radius = sphere.m_Radius;
				shape = sphereDesc;
				//geom_id  = dCreateSphere(0, sphere.m_Radius);
			}
			break;
		case PGT_TERRAIN:
			{

			TerrainComponentPtr terrain  = boost::shared_dynamic_cast<ITerrainComponent>(geom);
			
			if(terrain)
			{
				
				NxReal sixtyFourKb = 65536.0f;
				NxReal thirtyTwoKb = 32767.5f;

				NxU32 nbRows = terrain->GetSamplesZ();
				NxU32 nbColumns =terrain->GetSamplesX();

				Vec3 b_min, b_max;
				terrain->GetBounds(b_min,b_max);
				float h_span = b_max.y - b_min.y;

				NxHeightFieldDesc heightFieldDesc;
				heightFieldDesc.setToDefault();
				heightFieldDesc.nbColumns		= nbColumns;
				heightFieldDesc.nbRows			= nbRows;
				heightFieldDesc.convexEdgeThreshold = 0;

				// allocate storage for samples
				heightFieldDesc.samples	= new NxU32[heightFieldDesc.nbColumns*heightFieldDesc.nbRows];
				heightFieldDesc.sampleStride	= sizeof(NxU32);

				char* currentByte = (char*)heightFieldDesc.samples;
				for (NxU32 row = 0; row < nbRows; row++)
				{
					for (NxU32 column = 0; column < nbColumns; column++)
					{
						//NxReal s = NxReal(row) / NxReal(nbRows);
						//NxReal t = NxReal(column) / NxReal(nbColumns);
						float posx = terrain->GetSizeX()*((float)column/(float)nbColumns);
						float posz = terrain->GetSizeZ()*((float)row/(float)nbRows);

						//float h = terrain->GetHeight(posx,posz);
						//NxI16 height = (NxI16)(thirtyTwoKb * terrain->GetHeight(posz,posx)/h_span);
						NxI16 height = (NxI16)(thirtyTwoKb * terrain->GetHeightData()[nbRows*column + row]/h_span);

						//Log::Print("%d %f %f",height,posx,posz);
						//NxI16 height = (NxI32)(sixtyFourKb * 10/1000.f);
						
							//NxI16 height = - (nbRows / 2 - row) * (nbRows / 2 - row) - (nbColumns / 2 - column) * (nbColumns / 2 - column);

						NxHeightFieldSample* currentSample = (NxHeightFieldSample*)currentByte;
						currentSample->height = height;
						currentSample->materialIndex0 = 1;
						currentSample->materialIndex1 = 1;
						currentSample->tessFlag = 0;
						//currentSample->unused = 0;
						currentByte += heightFieldDesc.sampleStride;
					}
				}

				PhysXPhysicsSystemPtr sys = SimEngine::GetPtr()->GetSystemManager()->GetFirstSystem<PhysXPhysicsSystem>();
				NxHeightField* heightField = sys->GetNxSDK()->createHeightField(heightFieldDesc);

				// data has been copied, we can free our buffer
				delete [] heightFieldDesc.samples;

			
				
				NxHeightFieldShapeDesc* heightFieldShapeDesc = new NxHeightFieldShapeDesc();
				heightFieldShapeDesc->setToDefault();
				heightFieldShapeDesc->heightField	= heightField;
				heightFieldShapeDesc->shapeFlags		= NX_SF_FEATURE_INDICES | NX_SF_VISUALIZATION;
				
				heightFieldShapeDesc->heightScale	= h_span / thirtyTwoKb;
				heightFieldShapeDesc->rowScale		= terrain->GetSizeZ() / NxReal(nbRows-1);
				heightFieldShapeDesc->columnScale	= terrain->GetSizeX() / NxReal(nbColumns-1);
				
				shape =heightFieldShapeDesc;
				//heightFieldShapeDesc.meshFlags	= NX_MESH_SMOOTH_SPHERE_COLLISIONS;
				//heightFieldShapeDesc.materialIndexHighBits = 0;
				//heightFieldShapeDesc.holeMaterial = 2;

				//NxActorDesc actorDesc;
				//actorDesc.shapes.pushBack(&heightFieldShapeDesc);
				//actorDesc.body		   = NULL;
				//actorDesc.globalPose.t = pos;
				//NxActor* newActor = m_SceneManager->GetNxScene()->createActor(actorDesc);
			}
			}
			break;
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
				adesc.setToDefault();
				adesc.body		= NULL;
				adesc.globalPose.t = NxVec3(0,0,0);
				adesc.shapes.pushBack(shape);
				m_StaticActor = PhysXPhysicsSceneManagerPtr(m_SceneManager)->GetNxScene()->createActor(adesc);

					


			/*	NxPlaneShapeDesc planeDesc;
				NxActorDesc actorDesc;
				planeDesc.d = 5;
				actorDesc.shapes.pushBack(&planeDesc);
				m_SceneManager->GetNxScene()->createActor(actorDesc);
*/
			}
		}

	}

	void PhysXGeometryComponent::SetPosition(const Vec3 &pos)
	{
		if(m_Body == NULL)
		{
			//dGeomSetPosition(m_TransformGeomID, pos.x, pos.y, pos.z);
			//dGeomSetPosition(m_SecondTransformGeomID, pos.x, pos.y, pos.z);
		}
	}

	void PhysXGeometryComponent::SetRotation(const Quaternion &rot)
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

	void PhysXGeometryComponent::SetScale(const Vec3 &value)
	{
		//SetScale(value,m_GeomID);
		//SetScale(value,m_SecondGeomID);
	}

	void PhysXGeometryComponent::SetGeometryType(const std::string &geom_type)
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

	std::string PhysXGeometryComponent::GetGeometryType() const
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
