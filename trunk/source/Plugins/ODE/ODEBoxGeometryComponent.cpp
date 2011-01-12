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

#include "Plugins/ODE/ODEBoxGeometryComponent.h"
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
	ODEBoxGeometryComponent::ODEBoxGeometryComponent():
		m_ODESpaceID (NULL),
		m_Body (NULL),
		m_Friction(1),
		m_Offset(0,0,0),
		m_Size(1,1,1),
		m_Slip(0),
		m_CollisionGeomScale(1,1,1),
		m_CollisionCategory(1),
		m_CollisionBits(1),
		m_GeomID(0),
		m_TransformGeomID(NULL),
		m_SizeFromMesh(true),
		m_Debug(false)
	{

	}

	ODEBoxGeometryComponent::~ODEBoxGeometryComponent()
	{
		Reset();
	}

	void ODEBoxGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsBoxGeometryComponent",new Creator<ODEBoxGeometryComponent, IComponent>);

		RegisterProperty<Vec3>("Size", &GASS::ODEBoxGeometryComponent::GetSize, &GASS::ODEBoxGeometryComponent::SetSize);
		RegisterProperty<Vec3>("Offset", &GASS::ODEBoxGeometryComponent::GetOffset, &GASS::ODEBoxGeometryComponent::SetOffset);
		RegisterProperty<float>("Friction", &GASS::ODEBoxGeometryComponent::GetFriction, &GASS::ODEBoxGeometryComponent::SetFriction);
		//RegisterProperty<float>("Slip", &GASS::ODEBoxGeometryComponent::GetSlip, &GASS::ODEBoxGeometryComponent::SetSlip);
		RegisterProperty<bool>("SizeFromMesh", &GASS::ODEBoxGeometryComponent::GetSizeFromMesh, &GASS::ODEBoxGeometryComponent::SetSizeFromMesh);
		RegisterProperty<long int>("CollisionBits", &GASS::ODEBoxGeometryComponent::GetCollisionBits, &GASS::ODEBoxGeometryComponent::SetCollisionBits);
		RegisterProperty<long int>("CollisionCategory", &GASS::ODEBoxGeometryComponent::GetCollisionCategory, &GASS::ODEBoxGeometryComponent::SetCollisionCategory);
		RegisterProperty<bool>("Debug", &GASS::ODEBoxGeometryComponent::GetDebug, &GASS::ODEBoxGeometryComponent::SetDebug);
	}

	void ODEBoxGeometryComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBoxGeometryComponent::OnLoad,LoadPhysicsComponentsMessage ,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBoxGeometryComponent::OnTransformationChanged,TransformationNotifyMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBoxGeometryComponent::OnCollisionSettings,CollisionSettingsMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBoxGeometryComponent::OnGeometryChanged,GeometryChangedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBoxGeometryComponent::OnPhysicsDebug,PhysicsDebugMessage,0));
	}

	void ODEBoxGeometryComponent::OnLoad(LoadPhysicsComponentsMessagePtr message)
	{
		ODEPhysicsSceneManagerPtr scene_manager = boost::shared_static_cast<ODEPhysicsSceneManager> (message->GetPhysicsSceneManager());
		assert(scene_manager);
		m_SceneManager = scene_manager;
		UpdateODEGeom();
	}

	void ODEBoxGeometryComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		Vec3 pos = message->GetPosition();
		SetPosition(pos);
		Quaternion rot = message->GetRotation();
		SetRotation(rot);
		//SetScale(message->GetScale());
		//Update scaling
		SetSizeFromMesh(m_SizeFromMesh);
	}

	bool  ODEBoxGeometryComponent::GetSizeFromMesh()const
	{
		return m_SizeFromMesh;
	}

	void ODEBoxGeometryComponent::SetSizeFromMesh(bool value)
	{
		m_SizeFromMesh = value;
		if(m_SizeFromMesh && IsInitialized())
		{
			GeometryComponentPtr geom  = GetGeometry();
			if(geom)
			{
				AABox box = geom->GetBoundingBox();
				SetSize((box.m_Max - box.m_Min)*m_CollisionGeomScale);
				SetOffset((box.m_Max + box.m_Min)*0.5);
			}
		}
	}

	void ODEBoxGeometryComponent::UpdateODEGeom()
	{
		Reset();
		m_Body = GetSceneObject()->GetFirstComponent<ODEBodyComponent>().get();
		dSpaceID space = GetSpace();

		m_GeomID = dCreateBox(0, m_Size.x, m_Size.y, m_Size.z); 

		m_TransformGeomID = dCreateGeomTransform(space);
		dGeomTransformSetCleanup(m_TransformGeomID, 1 );
		dGeomTransformSetGeom(m_TransformGeomID,m_GeomID);
		dGeomSetPosition(m_GeomID, m_Offset.x, m_Offset.y, m_Offset.z);
		if(m_Body)
		{
			dGeomSetBody(m_TransformGeomID, m_Body->GetODEBodyComponent());
		}
		else
		{
			dGeomSetBody(m_TransformGeomID, NULL);
		}
		dGeomSetData(m_TransformGeomID, (void*)this);

		//if (m_Body && m_Body->GetMassRepresentation() == ODEBodyComponent::MR_GEOMETRY)
		UpdateBodyMass();
		SetCollisionBits(m_CollisionBits);
		SetCollisionCategory(m_CollisionCategory);
	}

	void ODEBoxGeometryComponent::SetSize(const Vec3 &size)
	{
		m_Size = size;
		if(m_GeomID)
		{
			dGeomBoxSetLengths(m_GeomID, m_Size.x, m_Size.y, m_Size.z);
			UpdateBodyMass();
			UpdateDebug();
		}
	}

	Vec3 ODEBoxGeometryComponent::GetSize() const
	{
		return m_Size;
	}

	void ODEBoxGeometryComponent::UpdateBodyMass()
	{
		if(m_Body && m_Body->GetMassRepresentation() == ODEBodyComponent::MR_GEOMETRY)
		{
			dMass ode_mass;
			dMassSetBoxTotal(&ode_mass, m_Body->GetMass(), m_Size.x, m_Size.y, m_Size.z);
			m_Body->SetODEMass(ode_mass);
		}
	}

	void ODEBoxGeometryComponent::OnCollisionSettings(CollisionSettingsMessagePtr message)
	{
		bool value = message->EnableCollision();
		if(value)
			Enable();
		else
			Disable();
	}

	void ODEBoxGeometryComponent::Reset()
	{
		if(m_TransformGeomID) dGeomDestroy(m_TransformGeomID);
		if(m_ODESpaceID) dSpaceDestroy(m_ODESpaceID);
		m_TransformGeomID = NULL;
		m_ODESpaceID = NULL;
	}

	void ODEBoxGeometryComponent::SetOffset(const Vec3 &value)
	{
		m_Offset = value;
		if(m_GeomID)
		{
			dGeomSetPosition(m_GeomID, m_Offset.x, m_Offset.y, m_Offset.z);
			UpdateDebug();
		}
	}

	void ODEBoxGeometryComponent::SetPosition(const Vec3 &pos)
	{
		if(m_Body == NULL)
		{
			dGeomSetPosition(m_TransformGeomID, pos.x, pos.y, pos.z);
		}
	}

	void ODEBoxGeometryComponent::SetRotation(const Quaternion &rot)
	{
		if(m_Body == NULL)
		{
			dReal ode_rot_mat[12];
			Mat4 rot_mat;
			rot_mat.Identity();
			rot.ToRotationMatrix(rot_mat);
			ODEPhysicsSceneManager::CreateODERotationMatrix(rot_mat,ode_rot_mat);
			dGeomSetRotation(m_TransformGeomID, ode_rot_mat);
		}
	}

	void ODEBoxGeometryComponent::Disable()
	{
		if(m_TransformGeomID) dGeomDisable(m_TransformGeomID);
	}

	void ODEBoxGeometryComponent::Enable()
	{
		if(m_TransformGeomID) dGeomEnable(m_TransformGeomID);
	}

	GeometryComponentPtr ODEBoxGeometryComponent::GetGeometry() const 
	{
		GeometryComponentPtr geom;
		if(m_GeometryTemplate != "")
		{
			geom = boost::shared_dynamic_cast<IGeometryComponent>(GetSceneObject()->GetComponent(m_GeometryTemplate));
		}
		else geom = GetSceneObject()->GetFirstComponent<IGeometryComponent>();
		return geom;
	}

	void ODEBoxGeometryComponent::OnGeometryChanged(GeometryChangedMessagePtr message)
	{
		if(m_SizeFromMesh)
		{
			SetSizeFromMesh(true);
		}
	}

	long int ODEBoxGeometryComponent::GetCollisionBits() const 
	{
		return m_CollisionBits;
	}

	void ODEBoxGeometryComponent::SetCollisionBits(long int value)
	{
		m_CollisionBits = value;
		if(m_GeomID)
		{
			dGeomSetCollideBits (m_GeomID,m_CollisionBits);
			dGeomSetCollideBits (m_TransformGeomID, m_CollisionBits);
		}
	}

	bool ODEBoxGeometryComponent::IsInitialized() const
	{
		return (m_GeomID == 0) ? false:true;
	}

	long int ODEBoxGeometryComponent::GetCollisionCategory() const 
	{
		return m_CollisionCategory;
	}

	void ODEBoxGeometryComponent::SetCollisionCategory(long int value)
	{
		m_CollisionCategory =value;
		if(m_GeomID)
		{
			dGeomSetCategoryBits(m_GeomID, m_CollisionCategory );
			dGeomSetCategoryBits(m_TransformGeomID, m_CollisionCategory );
		}
	}

	dSpaceID ODEBoxGeometryComponent::GetSpace()
	{
		if(m_Body)
		{
			return m_Body->GetSpace();
		}
		else
		{
			if(m_ODESpaceID == NULL)
			{
				m_ODESpaceID = dSimpleSpaceCreate(ODEPhysicsSceneManagerPtr(m_SceneManager)->GetPhysicsSpace());
			}
			return m_ODESpaceID;
		}
	}

	void ODEBoxGeometryComponent::CreateDebugBox(const Vec3 &size,const Vec3 &offset)
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





	

	void ODEBoxGeometryComponent::UpdateDebug()
	{

		if(m_Debug)
		{
			if(m_GeomID)
			{
				dVector3 temp_size;
				dGeomBoxGetLengths (m_GeomID, temp_size);
				Vec3 size(temp_size[0],temp_size[1],temp_size[2]);
				const dReal* pos =  dGeomGetPosition(m_GeomID);
				CreateDebugBox(size,Vec3(pos[0],pos[1],pos[2]));
			}
		}
		
	}


	SceneObjectPtr ODEBoxGeometryComponent::GetDebugObject() 
	{
		SceneObjectPtr scene_object;
		IComponentContainer::ComponentContainerIterator children = GetSceneObject()->GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(children.getNext());
			int pos = child->GetName().find(GetName() + "DebugPhysics");
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
			scene_object->SetName(GetName() + scene_object->GetName());
			scene_object->RegisterForMessage(REG_TMESS(ODEBoxGeometryComponent::OnDebugTransformation,TransformationNotifyMessage,0));
			//GetSceneObject()->AddChild(scene_object);
		}
		return scene_object;
	}

	void ODEBoxGeometryComponent::OnDebugTransformation(TransformationNotifyMessagePtr message)
	{
		SceneObjectPtr obj = GetDebugObject();
		Vec3 pos  =obj->GetFirstComponent<ILocationComponent>()->GetPosition();
		if(pos != m_Offset)
		{
			m_Offset = pos;
			if(m_GeomID)
			{
				dGeomSetPosition(m_GeomID, m_Offset.x, m_Offset.y, m_Offset.z);
			}
		}
	}

	void ODEBoxGeometryComponent::OnPhysicsDebug(PhysicsDebugMessagePtr message)
	{
		SetDebug(message->DebugGeometry());
	}

	void ODEBoxGeometryComponent::SetDebug(bool value)
	{
		m_Debug = value;
		if(IsInitialized())
		{
			if(m_Debug)
			{
				UpdateDebug();
			}
			else
			{
				SceneObjectPtr obj = GetDebugObject();
				obj->UnregisterForMessage(UNREG_TMESS(ODEBoxGeometryComponent::OnDebugTransformation,TransformationNotifyMessage));
				GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->PostMessage(MessagePtr(new RemoveSceneObjectMessage(obj)));
			}
		}
	}

	bool ODEBoxGeometryComponent::GetDebug() const
	{
		return m_Debug;
	}
}
