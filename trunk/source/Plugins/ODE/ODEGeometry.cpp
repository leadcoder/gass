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
#include "Plugins/ODE/ODEBodyComponent.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"

#include "Core/MessageSystem/MessageManager.h"
#include "Core/Math/AABox.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/SceneObjectTemplate.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/SimEngine.h"
#include <boost/bind.hpp>

namespace GASS
{
	ODEGeometry::ODEGeometry():
		m_ODESpaceID (NULL),
		m_Body (NULL),
		m_Friction(1),
		m_Offset(0,0,0),
		m_BBOffset(0,0,0),
		m_BBoxSize(0,0,0),
		m_Slip(0),
		m_CollisionGeomScale(1,1,1),
		m_CollisionCategory(1),
		m_CollisionBits(1),
		m_GeomID(0),
		m_TransformGeomID(NULL),
	{

	}

	ODEGeometry::~ODEGeometry()
	{
		Reset();
	}

	void ODEGeometry::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsGeometryComponent",new Creator<ODEGeometry, IComponent>);

		RegisterProperty<Vec3>("Offset", &GASS::ODEGeometry::GetOffset, &GASS::ODEGeometry::SetOffset);
		RegisterProperty<float>("Friction", &GASS::ODEGeometry::GetFriction, &GASS::ODEGeometry::SetFriction);
		RegisterProperty<float>("Slip", &GASS::ODEGeometry::GetSlip, &GASS::ODEGeometry::SetSlip);
		RegisterProperty<long int>("CollisionBits", &GASS::ODEGeometry::GetCollisionBits, &GASS::ODEGeometry::SetCollisionBits);
		RegisterProperty<long int>("CollisionCategory", &GASS::ODEGeometry::GetCollisionCategory, &GASS::ODEGeometry::SetCollisionCategory);
		RegisterProperty<Vec3>("BaseScale", &GASS::ODEGeometry::GetBaseScale, &GASS::ODEGeometry::SetBaseScale);
		RegisterProperty<std::string>("GeometryType", &GASS::ODEGeometry::GetGeometryType, &GASS::ODEGeometry::SetGeometryType);
	}

	void ODEGeometry::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEGeometry::OnLoad,LoadPhysicsComponentsMessage ,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEGeometry::OnTransformationChanged,TransformationNotifyMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEGeometry::OnCollisionSettings,CollisionSettingsMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEGeometry::OnGeometryChanged,GeometryChangedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEGeometry::OnPhysicsDebug,PhysicsDebugMessage,0));
	}

	void ODEGeometry::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		Vec3 pos = message->GetPosition();
		SetPosition(pos);
		Quaternion rot = message->GetRotation();
		SetRotation(rot);
		SetScale(message->GetScale());
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
		ODEPhysicsSceneManagerPtr scene_manager = boost::shared_static_cast<ODEPhysicsSceneManager> (message->GetPhysicsSceneManager());
		assert(scene_manager);
		m_SceneManager = scene_manager;
		m_Body = GetSceneObject()->GetFirstComponent<ODEBodyComponent>().get();
	}

	dSpaceID ODEGeometry::GetStaticSpace()
	{
		if(m_ODESpaceID == NULL)
		{
			m_ODESpaceID = dSimpleSpaceCreate(ODEPhysicsSceneManagerPtr(m_SceneManager)->GetPhysicsSpace());
		}
		return m_ODESpaceID;
	}
	
	void ODEGeometry::CreateODEGeomFromGeom(IGeometryComponent* geom,
		dSpaceID space,
		dGeomID &geom_id,
		dGeomID &trans_geom_id,
		ODEBodyComponent* body)
	{
		AABox box = geom->GetBoundingBox();
		Vec3 bb_size = (box.m_Max - box.m_Min)*m_CollisionGeomScale;
		m_BBoxSize = bb_size;
		m_BSphereSize = sphere.m_Radius;
		Vec3 geom_offset(0,0,0);

		geom_offset = box.m_Max + box.m_Min;
		geom_offset = geom_offset*0.5f;
		geom_id= dCreateBox(0, bb_size.x, bb_size.y, bb_size.z);
		
		m_BBOffset = geom_offset;
		Vec3 temp_offset = m_Offset + geom_offset;
		
		trans_geom_id = dCreateGeomTransform(space);
		dGeomTransformSetCleanup(trans_geom_id, 1 );
		dGeomTransformSetGeom(trans_geom_id,geom_id);
		dGeomSetPosition(geom_id, temp_offset.x, temp_offset.y, temp_offset.z);
		
		if(body)
		{
			dGeomSetBody(trans_geom_id, body->GetODEBodyComponent());
		}
		else
		{
			dGeomSetBody(trans_geom_id, NULL);
		}
		dGeomSetData(trans_geom_id, (void*)this);
	}

	void ODEGeometry::SetPosition(const Vec3 &pos)
	{
		if(m_Body == NULL && m_TransformGeomID && m_GeometryType != PGT_PLANE)
		{
			dGeomSetPosition(m_TransformGeomID, pos.x, pos.y, pos.z);
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
		}
	}

	void ODEGeometry::Disable()
	{
		if(m_TransformGeomID) dGeomDisable(m_TransformGeomID);
	}

	void ODEGeometry::Enable()
	{
		if(m_TransformGeomID) dGeomEnable(m_TransformGeomID);
	}

	void ODEGeometry::Reset()
	{
		if(m_TransformGeomID) dGeomDestroy(m_TransformGeomID);
		if(m_ODESpaceID) dSpaceDestroy(m_ODESpaceID);
		m_TransformGeomID = NULL;
		m_ODESpaceID = NULL;
	}


	void ODEGeometry::OnGeometryChanged(GeometryChangedMessagePtr message)
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
				if (m_Body->GetMassRepresentation() == ODEBodyComponent::MR_GEOMETRY)
					CreateODEMassFromGeom(geom.get(),m_Body);

			}
			else
			{
				CreateODEGeomFromGeom(geom.get(),GetStaticSpace(),m_GeomID,m_TransformGeomID,NULL);
			}
			SetCollisionBits(m_CollisionBits);
			SetCollisionCategory(m_CollisionCategory);
		}
	}

	void ODEGeometry::SetSizeFromGeom(dGeomID id, GeometryComponentPtr geom)
	{
		//we we only resize/rescale box,sphere and cylinder right now
		Vec3 bbsize = geom->GetBoundingBox().m_Max - geom->GetBoundingBox().m_Min;
		dGeomBoxSetLengths(id,bbsize.x,bbsize.y,bbsize.z);
	}

	void ODEGeometry::SetScale(const Vec3 &value, dGeomID id)
	{
		Vec3 bbsize = m_BBoxSize*value;
		if(m_Body == NULL && m_TransformGeomID)
		{
			Vec3 offset = m_Offset + m_BBOffset*value;
			if(bbsize.Length() > 0)
			{
				dGeomBoxSetLengths(id,bbsize.x,bbsize.y,bbsize.z);
				dGeomSetPosition(id, offset.x, offset.y, offset.z);
			}
		}
	}

	void ODEGeometry::SetScale(const Vec3 &value)
	{
		SetScale(value,m_GeomID);
	}

	void ODEGeometry::CreateODEMassFromGeom(IGeometryComponent* geom,ODEBodyComponent* body)
	{
		AABox box = geom->GetBoundingBox();
		Vec3 bb_size = (box.m_Max - box.m_Min)*m_CollisionGeomScale;
		dMass ode_mass;
		dMassSetBoxTotal(&ode_mass, body->GetMass(), bb_size.x, bb_size.y, bb_size.z);
		body->SetODEMass(ode_mass);
	}


	long int ODEGeometry::GetCollisionBits() const 
	{
		return m_CollisionBits;
	}

	void ODEGeometry::SetCollisionBits(long int value)
	{
		m_CollisionBits = value;

		if(m_GeomID)
		{
			dGeomSetCollideBits (m_GeomID,m_CollisionBits);
			dGeomSetCollideBits (m_TransformGeomID, m_CollisionBits);
		}
	}

	long int ODEGeometry::GetCollisionCategory() const 
	{
		return m_CollisionCategory;
	}

	void ODEGeometry::SetCollisionCategory(long int value)
	{
		m_CollisionCategory =value;
		if(m_GeomID)
		{

			dGeomSetCategoryBits(m_GeomID, m_CollisionCategory );
			dGeomSetCategoryBits(m_TransformGeomID, m_CollisionCategory );
		}
	}

	void ODEGeometry::CreateDebugBox(const Vec3 &size,const Vec3 &offset)
	{
		ManualMeshDataPtr mesh_data(new ManualMeshData);
		MeshVertex vertex;
		mesh_data->Material = "WhiteTransparentNoLighting";

		vertex.TexCoord.Set(0,0);
		vertex.Color = Vec4(1,1,1,1);
		mesh_data->Type = LINE_LIST;
		std::vector<Vec3> conrners;

		conrners.push_back(Vec3( size.x/2.0 ,size.y/2.0 , size.z/2.0));
		conrners.push_back(Vec3(-size.x/2.0 ,size.y/2.0 , size.z/2.0));
		conrners.push_back(Vec3(-size.x/2.0 ,size.y/2.0 ,-size.z/2.0));
		conrners.push_back(Vec3( size.x/2.0 ,size.y/2.0 ,-size.z/2.0));

		conrners.push_back(Vec3( size.x/2.0 ,-size.y/2.0 , size.z/2.0));
		conrners.push_back(Vec3(-size.x/2.0 ,-size.y/2.0 , size.z/2.0));
		conrners.push_back(Vec3(-size.x/2.0 ,-size.y/2.0 ,-size.z/2.0));
		conrners.push_back(Vec3( size.x/2.0 ,-size.y/2.0 ,-size.z/2.0));


		for(int i = 0; i < 4; i++)
		{
			vertex.Pos = conrners[i];
			mesh_data->VertexVector.push_back(vertex);
			vertex.Pos = conrners[(i+1)%4];
			mesh_data->VertexVector.push_back(vertex);

			vertex.Pos = conrners[i];
			mesh_data->VertexVector.push_back(vertex);
			vertex.Pos = conrners[i+4];
			mesh_data->VertexVector.push_back(vertex);
		}

		for(int i = 0; i < 4; i++)
		{
			vertex.Pos = conrners[4 + i];
			mesh_data->VertexVector.push_back(vertex);
			vertex.Pos = conrners[4 + ((i+1)%4)];
			mesh_data->VertexVector.push_back(vertex);
		}

		SceneObjectPtr scene_object = GetDebugObject();
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		scene_object->PostMessage(mesh_message);

		//Vec3 pos  = m_Offset + offset;
		//scene_object->GetFirstComponent<ILocationComponent>()->SetPosition(pos);
		scene_object->PostMessage(MessagePtr(new PositionMessage(offset)));
	}


	

	void ODEGeometry::UpdateDebug(bool enable)
	{

		if(m_GeomID && enable)
		{
			dVector3 temp_size;
			dGeomBoxGetLengths (m_GeomID, temp_size);
			Vec3 size(temp_size[0],temp_size[1],temp_size[2]);
			const dReal* pos =  dGeomGetPosition(m_GeomID);
			CreateDebugBox(size,Vec3(pos[0],pos[1],pos[2]));
		}
		if(!enable)
		{
			SceneObjectPtr obj = GetDebugObject();
			GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->PostMessage(MessagePtr(new RemoveSceneObjectMessage(obj)));
		}
	}


	SceneObjectPtr ODEGeometry::GetDebugObject() 
	{
		SceneObjectPtr scene_object;
		IComponentContainer::ComponentContainerIterator children = GetSceneObject()->GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(children.getNext());
			int pos = child->GetName().find("DebugPhysics");
			if(pos  >= 0)
			{
				scene_object = child;
			}
		}

		if(!scene_object)
		{
			scene_object = GetSceneObject()->GetSceneObjectManager()->LoadFromTemplate("DebugPhysics",GetSceneObject());
			//scene_object = boost::shared_static_cast<SceneObject>(SimEngine::Get().GetSimObjectManager()->CreateFromTemplate("DebugPhysics"));
			if(!scene_object)
			{
				SceneObjectTemplatePtr debug_template (new SceneObjectTemplate);
				debug_template->SetName("DebugPhysics");

				BaseComponentPtr location_comp = boost::shared_dynamic_cast<BaseComponent>(ComponentFactory::Get().Create("LocationComponent"));
				location_comp->SetName("LocationComp");
				location_comp->SetPropertyByType("AttachToParent",true);


				BaseComponentPtr mesh_comp = boost::shared_dynamic_cast<BaseComponent>(ComponentFactory::Get().Create("ManualMeshComponent"));
				mesh_comp->SetName("MeshComp");
				mesh_comp->SetPropertyByType("CastShadows",false);

				debug_template->AddComponent(location_comp);
				debug_template->AddComponent(mesh_comp );
				SimEngine::Get().GetSimObjectManager()->AddTemplate(debug_template);
				scene_object = GetSceneObject()->GetSceneObjectManager()->LoadFromTemplate("DebugPhysics",GetSceneObject());
				//scene_object = boost::shared_static_cast<SceneObject>(SimEngine::Get().GetSimObjectManager()->CreateFromTemplate("DebugPhysics"));
			}
			//GetSceneObject()->AddChild(scene_object);
		}
		return scene_object;
	}

	void ODEGeometry::OnPhysicsDebug(PhysicsDebugMessagePtr message)
	{
		UpdateDebug(message->DebugGeometry());
	}
}
