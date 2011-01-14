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

#include "Plugins/ODE/ODEBaseGeometryComponent.h"
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
	ODEBaseGeometryComponent::ODEBaseGeometryComponent():
		m_ODESpaceID (NULL),
		m_Body (NULL),
		m_Friction(1),
		m_Offset(0,0,0),
		m_CollisionCategory(1),
		m_CollisionBits(1),
		m_GeomID(0),
		m_TransformGeomID(0),
		m_SizeFromMesh(true),
		m_Debug(false)
	{

	}

	ODEBaseGeometryComponent::~ODEBaseGeometryComponent()
	{
		Reset();
	}

	void ODEBaseGeometryComponent::RegisterReflection()
	{
		RegisterProperty<Vec3>("Offset", &GASS::ODEBaseGeometryComponent::GetOffset, &GASS::ODEBaseGeometryComponent::SetOffset);
		RegisterProperty<float>("Friction", &GASS::ODEBaseGeometryComponent::GetFriction, &GASS::ODEBaseGeometryComponent::SetFriction);
		RegisterProperty<bool>("SizeFromMesh", &GASS::ODEBaseGeometryComponent::GetSizeFromMesh, &GASS::ODEBaseGeometryComponent::SetSizeFromMesh);
		RegisterProperty<long int>("CollisionBits", &GASS::ODEBaseGeometryComponent::GetCollisionBits, &GASS::ODEBaseGeometryComponent::SetCollisionBits);
		RegisterProperty<long int>("CollisionCategory", &GASS::ODEBaseGeometryComponent::GetCollisionCategory, &GASS::ODEBaseGeometryComponent::SetCollisionCategory);

		RegisterProperty<bool>("Debug", &GASS::ODEBaseGeometryComponent::GetDebug, &GASS::ODEBaseGeometryComponent::SetDebug);
	}

	void ODEBaseGeometryComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBaseGeometryComponent::OnLoad,LoadPhysicsComponentsMessage ,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBaseGeometryComponent::OnTransformationChanged,TransformationNotifyMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBaseGeometryComponent::OnCollisionSettings,CollisionSettingsMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBaseGeometryComponent::OnGeometryChanged,GeometryChangedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBaseGeometryComponent::OnPhysicsDebug,PhysicsDebugMessage,0));
	}

	void ODEBaseGeometryComponent::OnLoad(LoadPhysicsComponentsMessagePtr message)
	{
		ODEPhysicsSceneManagerPtr scene_manager = boost::shared_static_cast<ODEPhysicsSceneManager> (message->GetPhysicsSceneManager());
		assert(scene_manager);
		m_SceneManager = scene_manager;
		UpdateODEGeom();
	}

	void ODEBaseGeometryComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		if(m_Body == 0) //only update position for static geometry 
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
			Quaternion rot = message->GetRotation();
			SetRotation(rot);
			//Reflect scaling			
			SetSizeFromMesh(m_SizeFromMesh);
		}
	}

	bool  ODEBaseGeometryComponent::GetSizeFromMesh()const
	{
		return m_SizeFromMesh;
	}

	void ODEBaseGeometryComponent::UpdateODEGeom()
	{
		Reset();
		m_Body = GetSceneObject()->GetFirstComponent<ODEBodyComponent>().get();
		dSpaceID space = GetSpace();

		m_GeomID  = CreateODEGeom();

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
		UpdateBodyMass();
		SetCollisionBits(m_CollisionBits);
		SetCollisionCategory(m_CollisionCategory);

	}
	
	void ODEBaseGeometryComponent::OnCollisionSettings(CollisionSettingsMessagePtr message)
	{
		bool value = message->EnableCollision();
		if(value)
			Enable();
		else
			Disable();
	}

	void ODEBaseGeometryComponent::Reset()
	{
		if(m_TransformGeomID) dGeomDestroy(m_TransformGeomID);
		if(m_ODESpaceID) dSpaceDestroy(m_ODESpaceID);
		m_TransformGeomID = NULL;
		m_ODESpaceID = NULL;
	}

	void ODEBaseGeometryComponent::SetOffset(const Vec3 &value)
	{
		m_Offset = value;
		if(m_GeomID)
		{
			dGeomSetPosition(m_GeomID, m_Offset.x, m_Offset.y, m_Offset.z);
			UpdateDebug();
		}
	}

	void ODEBaseGeometryComponent::SetPosition(const Vec3 &pos)
	{
		if(m_Body == NULL)
		{
			dGeomSetPosition(m_TransformGeomID, pos.x, pos.y, pos.z);
		}
	}

	void ODEBaseGeometryComponent::SetRotation(const Quaternion &rot)
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

	void ODEBaseGeometryComponent::Disable()
	{
		if(m_TransformGeomID) dGeomDisable(m_TransformGeomID);
	}

	void ODEBaseGeometryComponent::Enable()
	{
		if(m_TransformGeomID) dGeomEnable(m_TransformGeomID);
	}

	GeometryComponentPtr ODEBaseGeometryComponent::GetGeometry() const 
	{
		GeometryComponentPtr geom;
		if(m_GeometryTemplate != "")
		{
			geom = boost::shared_dynamic_cast<IGeometryComponent>(GetSceneObject()->GetComponent(m_GeometryTemplate));
		}
		else geom = GetSceneObject()->GetFirstComponent<IGeometryComponent>();
		return geom;
	}

	void ODEBaseGeometryComponent::OnGeometryChanged(GeometryChangedMessagePtr message)
	{
		if(m_SizeFromMesh)
		{
			SetSizeFromMesh(true);
		}
	}

	long int ODEBaseGeometryComponent::GetCollisionBits() const 
	{
		return m_CollisionBits;
	}

	void ODEBaseGeometryComponent::SetCollisionBits(long int value)
	{
		m_CollisionBits = value;
		if(m_GeomID)
		{
			dGeomSetCollideBits (m_GeomID,m_CollisionBits);
			dGeomSetCollideBits (m_TransformGeomID, m_CollisionBits);
		}
	}

	bool ODEBaseGeometryComponent::IsInitialized() const
	{
		return (m_GeomID == 0) ? false:true;
	}

	long int ODEBaseGeometryComponent::GetCollisionCategory() const 
	{
		return m_CollisionCategory;
	}

	void ODEBaseGeometryComponent::SetCollisionCategory(long int value)
	{
		m_CollisionCategory =value;
		if(m_GeomID)
		{
			dGeomSetCategoryBits(m_GeomID, m_CollisionCategory );
			dGeomSetCategoryBits(m_TransformGeomID, m_CollisionCategory );
		}
	}

	dSpaceID ODEBaseGeometryComponent::GetSpace()
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


	void ODEBaseGeometryComponent::OnPhysicsDebug(PhysicsDebugMessagePtr message)
	{
		SetDebug(message->DebugGeometry());
	}

	void ODEBaseGeometryComponent::SetDebug(bool value)
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
				obj->UnregisterForMessage(UNREG_TMESS(ODEBaseGeometryComponent::OnDebugTransformation,TransformationNotifyMessage));
				GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->PostMessage(MessagePtr(new RemoveSceneObjectMessage(obj)));
			}
		}
	}

	bool ODEBaseGeometryComponent::GetDebug() const
	{
		return m_Debug;
	}


	SceneObjectPtr ODEBaseGeometryComponent::GetDebugObject() 
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
			scene_object->RegisterForMessage(REG_TMESS(ODEBaseGeometryComponent::OnDebugTransformation,TransformationNotifyMessage,0));
			//GetSceneObject()->AddChild(scene_object);
		}
		return scene_object;
	}


	void ODEBaseGeometryComponent::OnDebugTransformation(TransformationNotifyMessagePtr message)
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
}
