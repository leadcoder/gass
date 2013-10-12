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

#ifndef GASS_BASE_SCENE_COMPONENT_H
#define GASS_BASE_SCENE_COMPONENT_H

#include "Sim/GASSCommon.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSISceneManager.h"
#include "Sim/GASSTaskNode.h"
#include "Core/ComponentSystem/GASSBaseComponent.h"
#include "Core/MessageSystem/GASSIMessage.h"
namespace GASS
{
	
	/**
	Base class for all gass components
	*/

	class GASSExport BaseSceneComponent : public Reflection<BaseSceneComponent, BaseComponent> , public SHARE_CLASS<BaseSceneComponent>, public IMessageListener, public ISceneManagerListener, public ITaskNodeListener
	{
		friend class SceneObject;
	public:
		BaseSceneComponent();
		virtual ~BaseSceneComponent();
		SceneObjectPtr GetSceneObject() const;
		virtual void OnInitialize();
		virtual void OnDelete(){};
		virtual void SceneManagerTick(double delta) {(void)delta;}
		virtual void Update(double delta) {(void)delta;};
	protected:
		void RemapReferences(const std::map<SceneObjectGUID,SceneObjectGUID> &ref_map);
		void InitializePointers();
		void InitializeSceneObjectRef();
		//called by sceneobject to resolve SceneObjectRef 
		void ResolveTemplateReferences(SceneObjectPtr template_root);
	};
	typedef SPTR<BaseSceneComponent> BaseSceneComponentPtr;
	typedef WPTR<BaseSceneComponent> BaseSceneComponentWeakPtr;
}
#endif 
