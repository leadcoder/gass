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

#pragma once

#include "Sim/GASSCommon.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/ComponentSystem/GASSComponentContainer.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/GASSScene.h"
#include "Sim/Utils/GASSGUID.h"

namespace GASS
{
	typedef std::string SceneObjectID;
	typedef GASS_GUID SceneObjectGUID;
	class MessageManager;
	class Scene;
	class SceneObject;
	class BaseSceneComponent;
	class SceneObjectVisitor;

	typedef GASS_SHARED_PTR<Scene> ScenePtr;
	typedef GASS_WEAK_PTR<Scene> SceneWeakPtr;
	typedef GASS_SHARED_PTR<SceneObjectVisitor> SceneObjectVisitorPtr;

	typedef GASS_SHARED_PTR<MessageManager> MessageManagerPtr;
	typedef GASS_SHARED_PTR<SceneObject> SceneObjectPtr;
	typedef GASS_WEAK_PTR<SceneObject> SceneObjectWeakPtr;
	typedef GASS_SHARED_PTR<BaseSceneComponent> BaseSceneComponentPtr;
	typedef std::vector<SceneObjectPtr> SceneObjectVector;

	/**
	This class is derived from the ComponentContainer class and extend
	the ComponentContainer with message functionality.
	To communicate with components owned by the SceneObject a message manager is used.
	Some basic object messages is found in SceneObjectNameMessage.h

	As the name indicates a SceneObject is a object in a scene.
	The SceneObject is owned by a SceneObjectManager which in turn is owned
	by a Scene.
	By design the SceneObject class is not intended to be derived from,
	Instead new functionality should be added through components
	*/
	class GASSExport SceneObject : public Reflection<SceneObject, ComponentContainer>
	{
		friend class Scene;
	public:

		SceneObject();
		virtual ~SceneObject();
		static void RegisterReflection();
		void SyncMessages(double delta_time, bool recursive = true) const;
		ScenePtr GetScene() const {return m_Scene.lock();}

		void RemoveChildSceneObject(SceneObjectPtr child);
		
		void SendRemoveRequest(float delay);

		void AddChildSceneObject(SceneObjectPtr child , bool load);

		/**
		Get owner object that is direct under scene root
		*/
		SceneObjectPtr GetObjectUnderRoot();


		/**Convenience function the get parent scene object
			to hide object casting*/
		SceneObjectPtr GetParentSceneObject() const
		{
			//no dynamic cast because we are sure that all objects are derived from the SceneObject
			return GASS_STATIC_PTR_CAST<SceneObject>(GetParent());
		}

		bool Accept(SceneObjectVisitorPtr visitor);

		/**Get all components of certain class. This function allow you to pass the class name as a string
			@components Return components that are found
			@class_name Name of the component class to search for, note that this is the c++ class name
						and not the one you have registered to the object factory
			@recursive Indicates if we should search for components in child scene objects
		*/
		void GetComponentsByClassName(ComponentVector &components, const std::string &class_name, bool recursive = true) const;

		/**Get first component of certain class. This function allow you to pass the class name as a string
			@class_name Name of the component class to search for, note that this is the c++ class name
						and not the one you have registered to the object factory
			@recursive Indicates if we should search for components in child scene objects
		*/
		ComponentPtr GetFirstComponentByClassName(const std::string &class_name, bool recursive = true) const;


		/**Get all components of certain class. This function allow you to pass the class as a template argument
			@components Return components that are found
			@recursive Indicates if we should search for components in child scene objects
		*/
		template <class T>
		void GetComponentsByClass(ComponentVector &components, bool recursive = true) const
		{
			for(size_t i = 0 ; i < m_ComponentVector.size(); i++)
			{
				GASS_SHARED_PTR<T> ret = GASS_DYNAMIC_PTR_CAST<T>(m_ComponentVector[i]);
				if(ret)
					components.push_back(m_ComponentVector[i]);
			}

			if(recursive)
			{
				ComponentContainer::ConstComponentContainerIterator cc_iter = GetChildren();
				while(cc_iter.hasMoreElements())
				{
					SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(cc_iter.getNext());
					child->GetComponentsByClass<T>(components,recursive);
				}
			}
		}

		/**Get first component of certain class. This function allow you to pass the class as a template argument
			@recursive Indicates if we should search for component in child scene objects
		*/
		template <class T>
		GASS_SHARED_PTR<T> GetFirstComponentByClass(bool recursive = false) const
		{
			GASS_SHARED_PTR<T> ret;
			for(size_t i = 0 ; i < m_ComponentVector.size(); i++)
			{
				ret = GASS_DYNAMIC_PTR_CAST<T>(m_ComponentVector[i]);
				if(ret)
					return ret;
			}

			if(recursive)
			{
				ComponentContainer::ConstComponentContainerIterator cc_iter = GetChildren();
				while(cc_iter.hasMoreElements())
				{
					SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(cc_iter.getNext());
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
		GASS_SHARED_PTR<T> GetFirstParentComponentByClass() const
		{
			GASS_SHARED_PTR<T> ret;
			for(size_t i = 0 ; i < m_ComponentVector.size(); i++)
			{
				ret = GASS_DYNAMIC_PTR_CAST<T>(m_ComponentVector[i]);
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


		template <class T>
		GASS_SHARED_PTR<T> GetTopParentComponentByClass() const
		{
			GASS_SHARED_PTR<T> ret;
			SceneObjectPtr parent = GetParentSceneObject();
			if(parent)
			{
				ret =  parent->GetTopParentComponentByClass<T>();
				if(ret)
					return ret;
			}
			for(int i = 0 ; i < m_ComponentVector.size(); i++)
			{
				ret = GASS_DYNAMIC_PTR_CAST<T>(m_ComponentVector[i]);
				if(ret)
					return ret;
			}
			return ret;
		}

		/** Get children scene objects that match name.
			@objects Return scene objects that match name
			@name The object name to search for
			@exact_math Should the name be a exact match or should we allow
				that the name argument is found in the scene object name string.
		*/
		void GetChildrenByName(SceneObjectVector &objects, const std::string &name,bool exact_math = true, bool recursive = true) const;

		/** Get first child scene objects that match name.
			@name The object name to search for
			@exact_math Should the name be a exact match or should we allow
				that the name argument is found in the scene object name string.
		*/
		SceneObjectPtr GetFirstChildByName(const std::string &name,bool exact_math = true, bool recursive = true) const;


		/** Get child scene objects that match GUID.
			@name The object GUID to search for
		*/
		SceneObjectPtr GetChildByGUID(const SceneObjectGUID &guid) const;


		/** Get first child scene objects that match id.
			@id The object id to search for
		*/
		SceneObjectPtr GetChildByID(const SceneObjectID &id) const;


		/** Get children scene objects that match ID.
			@objects Return scene objects that match ID
			@name The object ID to search for
			@exact_math Should the ID be a exact match or should we allow
				that the name argument is found in the scene object name string.
		*/
		void GetChildrenByID(SceneObjectVector &objects, const SceneObjectID &id,bool exact_math = true, bool recursive = true) const;


		int RegisterForMessage(const MessageType &type, MessageFuncPtr callback, int priority = 0);
		void UnregisterForMessage(const MessageType &type, MessageFuncPtr callback);

		void OnChangeName(SceneObjectNameMessagePtr message);

		void SetID(const SceneObjectID &id){m_ID = id;}
		SceneObjectID GetID() const {return m_ID;}

		void SaveToFile(const std::string &filename);
		size_t GetQueuedMessages() const;
		void ClearMessages() const;

		/**Create a copy of this scene object include all components and optional ascendants.
			The copy will not be initialized or added to the scene, its up to the user to add
			this copy new host SceneObject. The copy will only reflect registred properties.

		   @copy_children Indicates if child SceneObjects also should be copied
		*/
		SceneObjectPtr CreateCopy(bool copy_children_recursively = true) const;
		SceneObjectGUID GetGUID() const {return m_GUID;}
		void SetGUID(const SceneObjectGUID& value) {m_GUID=value;}
		void GenerateNewGUID(bool recursively);
		void GenerateGUID(bool recursive);
		void ResolveTemplateReferences(SceneObjectPtr template_root);

		/**
		Convinces function for BaseSceneComponent's that call GetComponent on ComponentContainer
		*/
		BaseSceneComponentPtr GetBaseSceneComponent(const std::string &comp_name) const;

		/**
		Function used by scripts to get components by class name, note that this will return a raw pointer!
		*/
		BaseSceneComponent* GetComponentByClassName(const std::string &comp_name) const;
		SceneObject* GetSceneObjectByName(const std::string &name) const;
		SceneObject* GetSceneObjectByID(const std::string &id) const;
		//void LoadFromFile(const std::string &filename);
		static SceneObjectPtr LoadFromXML(const std::string &filename);

		void PostRequest(SceneObjectRequestMessagePtr message);
		void SendImmediateRequest(SceneObjectRequestMessagePtr message);

		void PostEvent(SceneObjectEventMessagePtr message);
		void SendImmediateEvent(SceneObjectEventMessagePtr message);

protected:

		void InitializePointers();
		void Initialize(ScenePtr scene);
		void OnDelete();

		SceneObjectPtr CreateCopyRec(bool copy_children_recursively) const;
		void RemapRefRec(std::map<SceneObjectGUID,SceneObjectGUID> &ref_map);
		void GenerateNewGUIDRec(std::map<SceneObjectGUID,SceneObjectGUID> &ref_map, bool recursively);

		ComponentContainerPtr CreateComponentContainerXML(tinyxml2::XMLElement *cc_elem) const;
		SceneWeakPtr m_Scene;
		MessageManagerPtr m_MessageManager;
		SceneObjectID m_ID;
		SceneObjectGUID m_GUID;
	};


	class ISceneObjectEnumerationPropertyMetaData
	{
	public:
		virtual std::vector<SceneObjectPtr> GetEnumeration(BaseReflectionObjectPtr object) const = 0;
	private:
	};
	typedef GASS_SHARED_PTR<ISceneObjectEnumerationPropertyMetaData> SceneObjectEnumerationPropertyMetaDataPtr;


	typedef std::vector<SceneObjectPtr> SceneObjectEnumerationFunc(BaseReflectionObjectPtr obj);
	class SceneObjectEnumerationProxyPropertyMetaData : public BasePropertyMetaData, public ISceneObjectEnumerationPropertyMetaData
	{
	public:
		SceneObjectEnumerationProxyPropertyMetaData(const std::string &annotation, PropertyFlags flags,SceneObjectEnumerationFunc *enumeration_func ,bool /*multi_select*/): BasePropertyMetaData(annotation,flags),
			m_EnumFunc(enumeration_func)
		{

		}
		virtual std::vector<SceneObjectPtr> GetEnumeration(BaseReflectionObjectPtr object) const {return m_EnumFunc(object);}
	private:
		SceneObjectEnumerationFunc* m_EnumFunc;
	};
	typedef GASS_SHARED_PTR<SceneObjectEnumerationProxyPropertyMetaData > SceneObjectEnumerationProxyPropertyMetaDataPtr;

}
