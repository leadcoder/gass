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

#include "BoneModifierComponent.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Components/Graphics/ILocationComponent.h"


namespace GASS
{
	BoneModifierComponent::BoneModifierComponent() : m_Active(false)
	{
	}

	BoneModifierComponent::~BoneModifierComponent()
	{

	}

	void BoneModifierComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("BoneModifierComponent",new Creator<BoneModifierComponent, IComponent>);
		RegisterProperty<std::string>("BoneName", &BoneModifierComponent::GetBoneName, &BoneModifierComponent::SetBoneName);
		RegisterProperty<std::string>("SourceObject", &BoneModifierComponent::GetSourceObject, &BoneModifierComponent::SetSourceObject);
		//RegisterProperty<std::string>("MeshObject", &BoneModifierComponent::GetMeshObject, &BoneModifierComponent::SetMeshObject);
	}

	void BoneModifierComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(BoneModifierComponent::OnLoad,LoadGameComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(BoneModifierComponent::OnLODChange,LODMessage,0));
		
	}

	void BoneModifierComponent::OnLODChange(LODMessagePtr message)
	{
		if(message->GetLevel() == LODMessage::LOD_LOW)
		{
			m_Active = false;
		}
		else
		{
			m_Active = true; 
		}
	}

	void BoneModifierComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		//SetBoneName(m_BoneName);
		//IMeshComponentPtr mesh = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<IMeshComponent>();
		GetSceneObject()->RegisterForMessage(REG_TMESS(BoneModifierComponent::OnTransformation,TransformationNotifyMessage,0));

		if(m_SourceObjectName != "")
		{
			m_SourceObject = GetSceneObject()->GetObjectUnderRoot()->GetFirstChildByName(m_SourceObjectName,false);
			if(!SceneObjectPtr(m_SourceObject))
				Log::Warning("Failed to find source %s for bone modifier %s",m_SourceObjectName.c_str(),m_BoneName.c_str());
		}
		else
			m_SourceObject  = GetSceneObject();
	}

	void BoneModifierComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		GetSceneObject()->UnregisterForMessage(UNREG_TMESS(BoneModifierComponent::OnTransformation,TransformationNotifyMessage));
	}

	void BoneModifierComponent::OnTransformation(TransformationNotifyMessagePtr message)
	{
		if(!m_Active)
			return;


		SceneObjectPtr so(m_SourceObject,boost::detail::sp_nothrow_tag());
		if(so)
		{
				//GetSceneObject()->GetParentSceneObject()->PostMessage(MessagePtr(new BoneTransformationMessage(m_BoneName, message->GetPosition(),message->GetRotation())));
				LocationComponentPtr location1 = so->GetFirstComponentByClass<ILocationComponent>();
				LocationComponentPtr location2 = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
				//LocationComponentPtr location2 = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<ILocationComponent>();

				Mat4 trans1,trans2;

				trans1.Identity();
				Quaternion rot1 = location1->GetWorldRotation();
				rot1.ToRotationMatrix(trans1);
				Vec3 pos1 = location1->GetWorldPosition();
				trans1.SetTranslation(pos1.x,pos1.y,pos1.z);
				
				
				trans2.Identity();
				Quaternion rot2 = location2->GetWorldRotation();
				rot2.ToRotationMatrix(trans2);
				Vec3 pos2 = location2->GetWorldPosition();
				trans2.SetTranslation(pos2.x,pos2.y,pos2.z);
				trans2 = trans2.Invert2();
				
				Mat4 mat_rel = trans1*trans2;
				Vec3 offset(0,0,0);
				offset = mat_rel*offset;
				//offset.z = -offset.z;

				GetSceneObject()->PostMessage(MessagePtr(new BoneTransformationMessage(m_BoneName,offset,Quaternion::IDENTITY)));
				//objects.front()->PostMessage(MessagePtr(new BoneTransformationMessage(m_BoneName,pos1,Quaternion::IDENTITY)));
				//std::cout << "send bone" << std::endl;
			
		}
	}

	std::string BoneModifierComponent::GetBoneName() const
	{
		return m_BoneName;
	}

	void BoneModifierComponent::SetBoneName(const std::string &name)
	{
		m_BoneName = name;
	}

	std::string BoneModifierComponent::GetSourceObject() const
	{
		return m_SourceObjectName;
	}

	void BoneModifierComponent::SetSourceObject(const std::string &name)
	{
		m_SourceObjectName = name;
	}

	std::string BoneModifierComponent::GetMeshObject() const
	{
		return m_MeshObjectName;
	}

	void BoneModifierComponent::SetMeshObject(const std::string &name)
	{
		m_MeshObjectName = name;
	}
}
