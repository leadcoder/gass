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


#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Plugins/Game/GameMessages.h"

namespace GASS
{

	class SceneObject;
	typedef SPTR<SceneObject> SceneObjectPtr;
	typedef WPTR<SceneObject> SceneObjectWeakPtr;

	class BoneModifierComponent :  public Reflection<BoneModifierComponent,BaseSceneComponent>
	{
	public:
		BoneModifierComponent();
		virtual ~BoneModifierComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
	private:
		void OnLODChange(LODMessagePtr message);
		void OnTransformation(TransformationNotifyMessagePtr message);
		void OnDriveWheelPhysicsMessage(VelocityNotifyMessagePtr message);
		
		std::string GetBoneName() const;
		void SetBoneName(const std::string &name);
		std::string GetMeshObject() const;
		void SetMeshObject(const std::string &name);
		
		std::string m_BoneName;
		bool m_Active;
		ADD_PROPERTY(SceneObjectRef, SourceObject);
	};
}
#endif
