/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/GASSComponent.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{
	
	/**
	Base class for all sim components
	*/

	class GASSExport BaseSceneComponent : public Reflection<BaseSceneComponent, Component> , public GASS_ENABLE_SHARED_FROM_THIS<BaseSceneComponent>, public IMessageListener, public ISceneManagerListener
	{
		friend class SceneObject;
	public:
		BaseSceneComponent();
		
		/**
			Get owner scene object. 
			Same as get owner in component 
		*/
		SceneObjectPtr GetSceneObject() const;
		/**
			Called by SceneObject during initialization. 
		*/
		virtual void OnInitialize();

		/**
			Called by SceneObject when ditto deleted.
			default implementation to minimize concrete components implementation
		*/
		virtual void OnDelete(){};


		//ISceneManagerListener
		/**
			Implements the ISceneManagerListener interface
			Called by responsible SceneManager if components is registered as listener.
		*/
		void SceneManagerTick(double delta) override {(void)delta;}

		/*template <typename Message>
		int RegisterForMessage(GASS_FUNCTION message_callback, int priority = 0)
		{
			GASS::MessageFuncPtr mess_func(new GASS::MessageFunc<Message>(GASS_BIND(message_callback, this, GASS_PLACEHOLDERS::_1), GASSMessageHasher(#FUNCTION), shared_from_this()));
			RegisterForMessage(typeid(Message), mess_func);
		}*/

	protected:
		template <class T>
		void RegisterForPreUpdate()
		{
			GASS_SHARED_PTR<BaseSceneManager> base_sm = GASS_DYNAMIC_PTR_CAST<BaseSceneManager>(GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<T>());
			if (!base_sm)
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find SceneManager", "BaseSceneManager::RegisterForPreUpdate");
			SceneManagerListenerPtr listener = shared_from_this();
			base_sm->RegisterPreUpdate(listener);
		}
		template <class T>
		void RegisterForPostUpdate() 
		{

			GASS_SHARED_PTR<BaseSceneManager> base_sm = GASS_DYNAMIC_PTR_CAST<BaseSceneManager>(GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<T>());
			if (!base_sm)
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find SceneManager", "BaseSceneManager::RegisterForPostUpdate");
			SceneManagerListenerPtr listener = shared_from_this();
			base_sm->RegisterPostUpdate(listener);
		}

		void RemapReferences(const std::map<SceneObjectGUID,SceneObjectGUID> &ref_map);
		void InitializePointers();
		void InitializeSceneObjectRef();
		//called by scene object to resolve SceneObjectRef 
		void ResolveTemplateReferences(SceneObjectPtr template_root);
	};
	using BaseSceneComponentPtr = std::shared_ptr<BaseSceneComponent>;
	using BaseSceneComponentWeakPtr = std::weak_ptr<BaseSceneComponent>;
}
#endif 
