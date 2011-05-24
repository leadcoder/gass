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

#pragma once

#include "Sim/Common.h"
#include "Core/Reflection/BaseReflectionObject.h"
#include "Core/ComponentSystem/BaseComponentContainer.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"


namespace GASS
{
	class MessageManager;
	class SceneObjectManager;
	class SceneObject;

	typedef boost::shared_ptr<SceneObjectManager> SceneObjectManagerPtr;
	typedef boost::weak_ptr<SceneObjectManager> SceneObjectManagerWeakPtr;

	typedef boost::shared_ptr<MessageManager> MessageManagerPtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;
	typedef std::vector<SceneObjectPtr> SceneObjectVector;

	/**
	This class is derived from the BaseComponentContainer class and extend
	the BaseComponentContainer with message functionality.
	To communicate with components owned by the SceneObject a message manager is used.
	Some basic object messages is enumerated below.

	As the name indicates a SceneObject is a object in a scenario scene.
	The SceneObject is owned by a SceneObjectManager which in turn is owned
	by a ScenarioScene.
	By design the SceneObject class is not intended to be derived from,
	Instead new functionality should be added through components
	*/
	class GASSExport SceneObject : public Reflection<SceneObject, BaseComponentContainer>
	{
	public:

		SceneObject();
		virtual ~SceneObject();
		static	void RegisterReflection();
		void SyncMessages(double delta_time, bool recursive = true);

		SceneObjectManagerPtr GetSceneObjectManager() {return SceneObjectManagerPtr(m_Manager,boost::detail::sp_nothrow_tag());}
		//MessageManager* GetMessageManager(){return m_MessageManager;}
		void OnCreate();


		//public for now, not possible to get derived manager to get hold of this otherwise
		void SetSceneObjectManager(SceneObjectManagerPtr manager);

		/**
		Get owner object that is direct under scene root
		*/
		SceneObjectPtr GetObjectUnderRoot();


		//convenience functions
		SceneObjectPtr GetParentSceneObject()
		{
			//no dynamic cast because we are sure that all objects are derived from the SceneObject
			return boost::shared_static_cast<SceneObject>(GetParent());
		}
		void GetComponentsByClass(ComponentVector &components, const std::string &class_name, bool recursive = true);
		ComponentPtr GetFirstComponentByClass(const std::string &class_name, bool recursive = true);

		template <class T>
		void GetComponentsByClass(ComponentVector &components, bool recursive = true)
		{
			for(int i = 0 ; i < m_ComponentVector.size(); i++)
			{
				boost::shared_ptr<T> ret = boost::shared_dynamic_cast<T>(m_ComponentVector[i]);
				if(ret)
					components.push_back(ret);
			}

			if(recursive)
			{
				IComponentContainer::ComponentContainerIterator cc_iter = GetChildren();
				while(cc_iter.hasMoreElements())
				{
					SceneObjectPtr child = boost::shared_static_cast<SceneObject>(cc_iter.getNext());
					GetComponentsByClass<T>(components,recursive);
				}
			}
		}

		template <class T>
		boost::shared_ptr<T> GetFirstComponentByClass(bool recursive = false)
		{
			boost::shared_ptr<T> ret;
			for(int i = 0 ; i < m_ComponentVector.size(); i++)
			{
				ret = boost::shared_dynamic_cast<T>(m_ComponentVector[i]);
				if(ret)
					return ret;
			}

			if(recursive)
			{
				IComponentContainer::ComponentContainerIterator cc_iter = GetChildren();
				while(cc_iter.hasMoreElements())
				{
					SceneObjectPtr child = boost::shared_static_cast<SceneObject>(cc_iter.getNext());
					ret = GetFirstComponentByClass<T>(recursive);
					if(ret)
						return ret;
				}
			}
			return ret;
		}

		template <class T>
		boost::shared_ptr<T> GetFirstParentComponentByClass()
		{
			boost::shared_ptr<T> ret;
			for(int i = 0 ; i < m_ComponentVector.size(); i++)
			{
				ret = boost::shared_dynamic_cast<T>(m_ComponentVector[i]);
				if(ret)
					return ret;
			}

			SceneObjectPtr parent = GetParentSceneObject();
			if(parent)
			{
				return parent->GetFirstParentComponentByClass<T>();
			}
			return ret;
		}

		//should we return result or pass it as ref arg?
		//SceneObjectVector GetObjectsByName(const std::string &name, bool exact_math = true);
		void GetChildrenByName(SceneObjectVector &objects, const std::string &name,bool exact_math = true, bool recursive = true);
		SceneObjectPtr GetFirstChildByName(const std::string &name,bool exact_math = true, bool recursive = true);

		int RegisterForMessage(const MessageType &type, MessageFuncPtr callback, int priority = 0);
		void UnregisterForMessage(const MessageType &type, MessageFuncPtr callback);
		void PostMessage(MessagePtr message);
		void SendImmediate(MessagePtr message);
		void OnChangeName(SceneObjectNameMessagePtr message);
	protected:
		SceneObjectManagerWeakPtr m_Manager;
		MessageManagerPtr m_MessageManager;

	};

}
