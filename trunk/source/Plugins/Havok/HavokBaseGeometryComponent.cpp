/****************************************************************************
* This file is part of GASS.                                                *
* See http://cHavok.google.com/p/gass/                                 *
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

#include "Plugins/Havok/HavokBaseGeometryComponent.h"
#include "Plugins/Havok/HavokPhysicsSceneManager.h"
#include "Plugins/Havok/HavokBodyComponent.h"
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
	HavokBaseGeometryComponent::HavokBaseGeometryComponent():
		m_Body (NULL),
		m_Friction(1),
		m_Offset(0,0,0),
		m_CollisionCategory(1),
		m_CollisionBits(1),
		m_SizeFromMesh(true),
		m_Debug(false)
	{

	}

	HavokBaseGeometryComponent::~HavokBaseGeometryComponent()
	{
		Reset();
	}

	void HavokBaseGeometryComponent::RegisterReflection()
	{
		RegisterProperty<Vec3>("Offset", &GASS::HavokBaseGeometryComponent::GetOffset, &GASS::HavokBaseGeometryComponent::SetOffset);
		RegisterProperty<float>("Friction", &GASS::HavokBaseGeometryComponent::GetFriction, &GASS::HavokBaseGeometryComponent::SetFriction);
		RegisterProperty<bool>("SizeFromMesh", &GASS::HavokBaseGeometryComponent::GetSizeFromMesh, &GASS::HavokBaseGeometryComponent::SetSizeFromMesh);
		RegisterProperty<long int>("CollisionBits", &GASS::HavokBaseGeometryComponent::GetCollisionBits, &GASS::HavokBaseGeometryComponent::SetCollisionBits);
		RegisterProperty<long int>("CollisionCategory", &GASS::HavokBaseGeometryComponent::GetCollisionCategory, &GASS::HavokBaseGeometryComponent::SetCollisionCategory);
		RegisterProperty<bool>("Debug", &GASS::HavokBaseGeometryComponent::GetDebug, &GASS::HavokBaseGeometryComponent::SetDebug);
	}

	void HavokBaseGeometryComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokBaseGeometryComponent::OnLoad,LoadPhysicsComponentsMessage ,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokBaseGeometryComponent::OnTransformationChanged,TransformationNotifyMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokBaseGeometryComponent::OnCollisionSettings,CollisionSettingsMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokBaseGeometryComponent::OnGeometryChanged,GeometryChangedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokBaseGeometryComponent::OnPhysicsDebug,PhysicsDebugMessage,0));
	}

	void HavokBaseGeometryComponent::OnLoad(LoadPhysicsComponentsMessagePtr message)
	{
		HavokPhysicsSceneManagerPtr scene_manager = boost::shared_static_cast<HavokPhysicsSceneManager> (message->GetPhysicsSceneManager());
		assert(scene_manager);
		m_SceneManager = scene_manager;
		UpdateHavokGeom();
		if(m_Debug) 
			SetDebug(true);
	}

	void HavokBaseGeometryComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
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

	bool  HavokBaseGeometryComponent::GetSizeFromMesh() const
	{
		return m_SizeFromMesh;
	}

	void HavokBaseGeometryComponent::UpdateHavokGeom()
	{
		m_Body = GetSceneObject()->GetFirstComponentByClass<HavokBodyComponent>().get();
		UpdateBodyMass();
		SetCollisionBits(m_CollisionBits);
		SetCollisionCategory(m_CollisionCategory);
		SetOffset(m_Offset);
	}
	
	void HavokBaseGeometryComponent::OnCollisionSettings(CollisionSettingsMessagePtr message)
	{
		bool value = message->EnableCollision();
		if(value)
			Enable();
		else
			Disable();
	}

	

	void HavokBaseGeometryComponent::SetOffset(const Vec3 &value)
	{
		m_Offset = value;
		//if(m_GeomID)
		{
			UpdateDebug();
		}
	}

	void HavokBaseGeometryComponent::SetPosition(const Vec3 &pos)
	{
		if(m_Body == NULL)
		{

		}
	}

	void HavokBaseGeometryComponent::SetRotation(const Quaternion &rot)
	{
		if(m_Body == NULL)
		{

		}
	}

	void HavokBaseGeometryComponent::Disable()
	{

	}

	void HavokBaseGeometryComponent::Enable()
	{

	}

	GeometryComponentPtr HavokBaseGeometryComponent::GetGeometry() const 
	{
		GeometryComponentPtr geom;
		if(m_GeometryTemplate != "")
		{
			geom = boost::shared_dynamic_cast<IGeometryComponent>(GetSceneObject()->GetComponent(m_GeometryTemplate));
		}
		else geom = GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();
		return geom;
	}

	void HavokBaseGeometryComponent::OnGeometryChanged(GeometryChangedMessagePtr message)
	{
		if(m_SizeFromMesh)
		{
			SetSizeFromMesh(true);
		}
	}

	long int HavokBaseGeometryComponent::GetCollisionBits() const 
	{
		return m_CollisionBits;
	}

	void HavokBaseGeometryComponent::SetCollisionBits(long int value)
	{
		m_CollisionBits = value;
	}

	long int HavokBaseGeometryComponent::GetCollisionCategory() const 
	{
		return m_CollisionCategory;
	}

	void HavokBaseGeometryComponent::SetCollisionCategory(long int value)
	{
		m_CollisionCategory =value;
	
	}

	void HavokBaseGeometryComponent::OnPhysicsDebug(PhysicsDebugMessagePtr message)
	{
		SetDebug(message->DebugGeometry());
	}

	void HavokBaseGeometryComponent::SetDebug(bool value)
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
				obj->UnregisterForMessage(UNREG_TMESS(HavokBaseGeometryComponent::OnDebugTransformation,TransformationNotifyMessage));
				GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->PostMessage(MessagePtr(new RemoveSceneObjectMessage(obj)));
			}
		}
	}

	bool HavokBaseGeometryComponent::GetDebug() const
	{
		return m_Debug;
	}


	SceneObjectPtr HavokBaseGeometryComponent::GetDebugObject()
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
			//scene_object = GetSceneObject()->GetSceneObjectManager()->LoadFromTemplate("DebugPhysics",GetSceneObject());
			scene_object = boost::shared_static_cast<SceneObject>(SimEngine::Get().GetSimObjectManager()->CreateFromTemplate("DebugPhysics"));
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
				//scene_object = GetSceneObject()->GetSceneObjectManager()->LoadFromTemplate("DebugPhysics",GetSceneObject());
				scene_object = boost::shared_static_cast<SceneObject>(SimEngine::Get().GetSimObjectManager()->CreateFromTemplate("DebugPhysics"));
			}
			scene_object->SetName(GetName() + scene_object->GetName());
			scene_object->RegisterForMessage(REG_TMESS(HavokBaseGeometryComponent::OnDebugTransformation,TransformationNotifyMessage,0));
			GetSceneObject()->AddChild(scene_object);
		}
		return scene_object;
	}


	void HavokBaseGeometryComponent::OnDebugTransformation(TransformationNotifyMessagePtr message)
	{
		SceneObjectPtr obj = GetDebugObject();
		Vec3 pos  =obj->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
		if(pos != m_Offset)
		{
			m_Offset = pos;
			/*if(m_GeomID)
			{
				dGeomSetPosition(m_GeomID, m_Offset.x, m_Offset.y, m_Offset.z);
			}*/
		}
	}
}
