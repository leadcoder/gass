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
#include "Core/Math/GASSVector.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Core/Utils/GASSIterators.h"
#include "Core/Utils/GASSException.h"

class TiXmlElement;

namespace GASS
{
	class MessageManager;
	class ISceneManager;
	class SceneObject;
	class SceneObjectManager;

	typedef boost::shared_ptr<SceneObjectManager> SceneObjectManagerPtr;
	typedef boost::shared_ptr<ISceneManager> SceneManagerPtr;
	typedef std::vector<SceneManagerPtr> SceneManagerVector;
	typedef boost::shared_ptr<MessageManager> MessageManagerPtr;
	typedef VectorIterator<SceneManagerVector> SceneManagerIterator;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr; 

	/**
	A Scene in gass can be loaded from disc or created in runtime.  
	The scene class will create instances of all registered scene managers and also create 
	a scene object manager used as a container for scene objects
	*/

	class GASSExport Scene : public Reflection<Scene, BaseReflectionObject>, 
		public boost::enable_shared_from_this<Scene> , 
		public IMessageListener
	{
		friend class SceneObject;
	public:
		enum
		{
			GFX_COMPONENT_LOAD_PRIORITY = 0,
			PHYSICS_COMPONENT_LOAD_PRIORITY = 1,
			SOUND_COMPONENT_LOAD_PRIORITY = 2,
			CORE_COMPONENT_LOAD_PRIORITY = 3,
			USER_COMPONENT_LOAD_PRIORITY = 4,

			GFX_SYSTEM_LOAD_PRIORITY = 0,
			PHYSICS_SYSTEM_LOAD_PRIORITY = 1,
			SOUND_SYSTEM_LOAD_PRIORITY = 2,
			CORE_SYSTEM_LOAD_PRIORITY = 3,
			USER_SYSTEM_LOAD_PRIORITY = 4,

		};
		Scene(const std::string &name = std::string(""));
		virtual ~Scene();
		static void RegisterReflection();

		/**
		This function must be called by creator before using this class.
		This function allocate the scene object manager which take the
		Scene as constructor argument and therefore can not be
		allocated in the Scene contructor where shared_from_this()
		is not excepted. Further this function allocate all registred scene managers,
		they also need a SceneScenePtr and therefore not possible to allocate
		this stuff in the constructor
		*/
		void Create();

		/**
		Get the start position for this scene,
		can be used to give cameras or players a initial position
		*/

		Vec3 GetStartPos()const {return m_StartPos;}

		/**
		Get the start rotation for this scene,
		can be used to give cameras or players a initial rotation
		*/
		Vec3 GetStartRot()const {return m_StartRot;}

		/**
		Set the start position for this scene,
		this attribute should be specified by scene.xml
		but can be replaced by using this method
		*/
		void SetStartPos(const Vec3 &value) {m_StartPos = value;}

		/**
		Set the start rotation for this scene,
		this attribute should be specified by scene.xml
		but can be replaced by using this method
		*/
		void SetStartRot(const Vec3 &value) {m_StartRot= value;}


		/**
		Register for scene scene messages, see SceneSceneMessages.h for available messages
		*/

		int RegisterForMessage(const MessageType &type, MessageFuncPtr callback, int priority = 0);

		/**
		Unregister for scene scene messages
		*/
		void UnregisterForMessage(const MessageType &type, MessageFuncPtr callback);
		void PostMessage(SceneMessagePtr message);
		void SendImmediate(SceneMessagePtr message);

		/**
		Load a new scene from path
		*/
		void Load(const std::string &name);

		/**
		Save scene to path
		*/
		void Save(const std::string &name);

			/**
		Get scene scene name
		*/
		std::string GetName() const {return m_Name;}

		/**
		Set scene scene name
		*/
		void SetName(const std::string &name) {m_Name = name;}

		/**
		Register for scene scene messages, see SceneSceneMessages.h for available messages
		*/

		/**
		Get scene manager iterator
		*/
		SceneManagerIterator GetSceneManagers();

		
		FilePath GetSceneFolder() const;

		void SyncMessages(double delta_time) const;

		SceneManagerPtr GetSceneManagerByName(const std::string &name) const;
		
		/**Get first SceneManager of certain class. This function allow you to pass the class as a template 
		*/
		template <class T>
		boost::shared_ptr<T> GetFirstSceneManagerByClass(bool no_throw=false) const
		{
			boost::shared_ptr<T> ret;
			for(int i = 0 ; i < m_SceneManagers.size(); i++)
			{
				ret = boost::shared_dynamic_cast<T>(m_SceneManagers[i]);
				if(ret)
					return ret;
			}
			if(!no_throw)
			{
				std::string sm_name = typeid(T).name();
				GASS_EXCEPT(GASS::Exception::ERR_ITEM_NOT_FOUND,"Scene Manager not found:" + sm_name, "Scene::GetFirstSceneManagerByClass");
			}
			return ret;
		}

		SceneObjectPtr LoadObjectFromTemplate(const std::string &template_name, SceneObjectPtr parent);
		SceneObjectPtr GetRootSceneObject() const {return m_Root;}
		SceneObjectPtr GetSceneryRoot() const {return SceneObjectPtr(m_TerrainObjects);}
		
		/**
			Check if scene has been loaded
		*/
		bool IsSceneLoaded() const {return m_SceneLoaded;}

		/**
			Get how many messages that is unprocessed
		*/
		size_t GetQueuedMessages() const;

		void Load();
		/**
		Get all scenes from path. This function recursively search for scene.xml 
		files and push that path to the return vector
		*/
		static std::vector<std::string> GetScenes(const FilePath &path);
		void Unload();

		std::string GetResourceGroupName() const;
protected:
		/**
		Load scene  from xml,
		this method is called by the scene LoadXML method in the scene class
		scene_elem is the should point to the SceneScene tag
		*/
		void LoadXML(TiXmlElement *scene_elem);

		/**
		Save scene  from xml,
		this method is called by the scene SaveXML method in the scene class
		scene_elem is the should point to the SceneScene tag
		*/
		void SaveXML(TiXmlElement *scene_elem);

		SceneObjectPtr LoadSceneObjectXML(TiXmlElement *so_elem);
		void OnSpawnSceneObjectFromTemplate(SpawnObjectFromTemplateRequestPtr message);
		void OnRemoveSceneObject(RemoveSceneObjectRequestPtr message);

		
		
		
		//Helper function to LoadXML
		SceneManagerPtr LoadSceneManager(TiXmlElement *sm_elem);

		Vec3 m_StartPos;
		Vec3 m_StartRot;

		SceneManagerVector m_SceneManagers;

		std::string m_Name;

		MessageManagerPtr m_SceneMessageManager;

		//Scene root node
		SceneObjectPtr m_Root;
		SceneObjectWeakPtr m_TerrainObjects;

		bool m_SceneLoaded;
		bool m_CreateCalled;
	};
	typedef boost::shared_ptr<Scene> ScenePtr;
	typedef boost::weak_ptr<Scene> SceneWeakPtr;
}

