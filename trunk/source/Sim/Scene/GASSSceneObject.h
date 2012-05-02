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

#include "Sim/GASSCommon.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/ComponentSystem/GASSBaseComponentContainer.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/Scene/GASSCoreSceneMessages.h"
#include "Sim/Scene/GASSCoreSceneObjectMessages.h"



namespace GASS
{
	typedef std::string SceneObjectID;
	class MessageManager;
	class Scene;
	class SceneObject;

	typedef boost::shared_ptr<Scene> ScenePtr;
	typedef boost::weak_ptr<Scene> SceneWeakPtr;

	typedef boost::shared_ptr<MessageManager> MessageManagerPtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;
	typedef std::vector<SceneObjectPtr> SceneObjectVector;

	/**
	This class is derived from the BaseComponentContainer class and extend
	the BaseComponentContainer with message functionality.
	To communicate with components owned by the SceneObject a message manager is used.
	Some basic object messages is found in SceneObjectNameMessage.h

	As the name indicates a SceneObject is a object in a scene.
	The SceneObject is owned by a SceneObjectManager which in turn is owned
	by a Scene.
	By design the SceneObject class is not intended to be derived from,
	Instead new functionality should be added through components
	*/
	class GASSExport SceneObject : public Reflection<SceneObject, BaseComponentContainer> 
	{
	public:

		SceneObject();
		virtual ~SceneObject();
		static void RegisterReflection();
		void SyncMessages(double delta_time, bool recursive = true);

		ScenePtr GetScene() const {return ScenePtr(m_Scene,boost::detail::sp_nothrow_tag());}
		//void SetScene(ScenePtr scene) {m_Scene = scene;}
		void Initialize(ScenePtr scene);
		//MessageManager* GetMessageManager(){return m_MessageManager;}
		
		//void OnCreate();
		virtual void RemoveChild(ComponentContainerPtr child);
		void AddChild(ComponentContainerPtr child);
	
		void OnDelete();

		//public for now, not possible to get derived manager to get hold of this otherwise
		//void SetSceneObjectManager(SceneObjectManagerPtr manager);

		/**
		Get owner object that is direct under scene root
		*/
		SceneObjectPtr GetObjectUnderRoot();


		/**Convenience function the get parent scene object
			to hide object casting*/
		SceneObjectPtr GetParentSceneObject() const
		{
			//no dynamic cast because we are sure that all objects are derived from the SceneObject
			return boost::shared_static_cast<SceneObject>(GetParent());
		}

		/**Get all components of certain class. This function allow you to pass the class name as a string
			@components Return componnets that are found
			@class_name Name of the component class to search for, note that this is the c++ class name 
						and not the one you have registred to the object factory
			@recursive Indicates if we should search for components in child scene objects
		*/
		void GetComponentsByClass(ComponentVector &components, const std::string &class_name, bool recursive = true);

		/**Get first component of certain class. This function allow you to pass the class name as a string
			@class_name Name of the component class to search for, note that this is the c++ class name 
						and not the one you have registred to the object factory
			@recursive Indicates if we should search for components in child scene objects
		*/
		ComponentPtr GetFirstComponentByClass(const std::string &class_name, bool recursive = true);


		/**Get all components of certain class. This function allow you to pass the class as a template argument
			@components Return componnets that are found
			@recursive Indicates if we should search for components in child scene objects
		*/
		template <class T>
		void GetComponentsByClass(ComponentVector &components, bool recursive = true) const
		{
			for(int i = 0 ; i < m_ComponentVector.size(); i++)
			{
				boost::shared_ptr<T> ret = boost::shared_dynamic_cast<T>(m_ComponentVector[i]);
				if(ret)
					components.push_back(ret);
			}

			if(recursive)
			{
				IComponentContainer::ConstComponentContainerIterator cc_iter = GetChildren();
				while(cc_iter.hasMoreElements())
				{
					SceneObjectPtr child = boost::shared_static_cast<SceneObject>(cc_iter.getNext());
					child->GetComponentsByClass<T>(components,recursive);
				}
			}
		}

		/**Get first component of certain class. This function allow you to pass the class as a template argument
			@recursive Indicates if we should search for component in child scene objects
		*/
		template <class T>
		boost::shared_ptr<T> GetFirstComponentByClass(bool recursive = false) const
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
				IComponentContainer::ConstComponentContainerIterator cc_iter = GetChildren();
				while(cc_iter.hasMoreElements())
				{
					SceneObjectPtr child = boost::shared_static_cast<SceneObject>(cc_iter.getNext());
					ret = child->GetFirstComponentByClass<T>(recursive);
					if(ret)
						return ret;
				}
			}
			return ret;
		}


		/**Get first component of certain class from parent scene node. 
			This function allow you to pass the class as a template argument
		*/
		template <class T>
		boost::shared_ptr<T> GetFirstParentComponentByClass() const
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

		/** Get children scene objects that match name. 
			@objects Return scene objects that match name
			@name The object name to search for
			@exact_math Should the name be a exact match or should we allow 
				that the name argument is found in the scene object name string, this
				can be usefull in case that every object has a unique id as part of 
				the name wich is not known in compiletime.
		*/
		void GetChildrenByName(SceneObjectVector &objects, const std::string &name,bool exact_math = true, bool recursive = true) const;

		/** Get first child scene objects that match name. 
			@name The object name to search for
			@exact_math Should the name be a exact match or should we allow 
				that the name argument is found in the scene object name string, this
				can be usefull in case that every object has a unique id as part of 
				the name wich is not known in compiletime.
		*/
		SceneObjectPtr GetFirstChildByName(const std::string &name,bool exact_math = true, bool recursive = true) const;

		/** Get first child scene objects that match id.
			@id The object id to search for
		*/
		SceneObjectPtr GetChildByID(const SceneObjectID &id) const;
	
		int RegisterForMessage(const MessageType &type, MessageFuncPtr callback, int priority = 0);
		void UnregisterForMessage(const MessageType &type, MessageFuncPtr callback);
		
		void PostMessage(MessagePtr message);
		void SendImmediate(MessagePtr message);
		void OnChangeName(SceneObjectNameMessagePtr message);

		void SetID(const SceneObjectID &id){m_ID = id;}
		SceneObjectID GetID() const {return m_ID;}
		void LoadFromFile(const std::string &filename);
	protected:
		ComponentContainerPtr CreateComponentContainer(TiXmlElement *cc_elem) const;
		SceneWeakPtr m_Scene;
		MessageManagerPtr m_MessageManager;
		SceneObjectID m_ID;

	};

}
