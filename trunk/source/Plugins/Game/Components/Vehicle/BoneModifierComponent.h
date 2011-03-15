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

#ifndef BONE_MODIFIER_COMPONENT_H
#define BONE_MODIFIER_COMPONENT_H

#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Sim/Common.h"
#include "Core/MessageSystem/IMessage.h"
#include "Plugins/Game/GameMessages.h"

namespace GASS
{

	class SceneObject;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class BoneModifierComponent :  public Reflection<BoneModifierComponent,BaseSceneComponent>
	{
	public:
		BoneModifierComponent();
		virtual ~BoneModifierComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	private:
		std::string GetBoneName() const;
		void SetBoneName(const std::string &name);
		std::string GetSourceObject() const;
		void SetSourceObject(const std::string &name);
		std::string GetMeshObject() const;
		void SetMeshObject(const std::string &name);

		void OnLoad(LoadGameComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnLODChange(LODMessagePtr message);

		void OnTransformation(TransformationNotifyMessagePtr message);
		void OnDriveWheelPhysicsMessage(VelocityNotifyMessagePtr message);
		SceneObjectWeakPtr m_SourceObject;

		std::string m_BoneName;
		std::string m_MeshObjectName;
		std::string m_SourceObjectName;
		bool m_Active;


	};
}
#endif
