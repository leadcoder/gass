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
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"

#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"

#include "Sim/Interface/GASSILocationComponent.h"


namespace GASS
{
	BoneModifierComponent::BoneModifierComponent() : m_Active(true)
	{
	}

	BoneModifierComponent::~BoneModifierComponent()
	{

	}

	void BoneModifierComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("BoneModifierComponent",new Creator<BoneModifierComponent, IComponent>);
		RegisterProperty<std::string>("BoneName", &BoneModifierComponent::GetBoneName, &BoneModifierComponent::SetBoneName);
		RegisterProperty<SceneObjectRef>("SourceObject", &BoneModifierComponent::GetSourceObject, &BoneModifierComponent::SetSourceObject);
		//RegisterProperty<std::string>("MeshObject", &BoneModifierComponent::GetMeshObject, &BoneModifierComponent::SetMeshObject);
	}

	void BoneModifierComponent::OnInitialize()
	{
		BaseSceneComponent::OnInitialize();
		GetSceneObject()->RegisterForMessage(REG_TMESS(BoneModifierComponent::OnLODChange,LODMessage,0));
	}

	void BoneModifierComponent::OnDelete()
	{
		GetSceneObject()->UnregisterForMessage(UNREG_TMESS(BoneModifierComponent::OnTransformation,TransformationNotifyMessage));
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

	

	void BoneModifierComponent::OnTransformation(TransformationNotifyMessagePtr message)
	{
		//if(!m_Active)
		//	return;

		//SceneObjectPtr so = ;
		if(m_SourceObject.IsValid())
		{
				//GetSceneObject()->GetParentSceneObject()->PostMessage(MessagePtr(new BoneTransformationMessage(m_BoneName, message->GetPosition(),message->GetRotation())));
				LocationComponentPtr location1 = m_SourceObject->GetFirstComponentByClass<ILocationComponent>();
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
				GetSceneObject()->PostRequest(BoneTransformationMessagePtr(new BoneTransformationMessage(m_BoneName,offset,Quaternion::IDENTITY)));
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
}
