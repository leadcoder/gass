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
#include "Core/Math/GASSVector.h"
#include "Core/Utils/GASSColorRGBA.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Interface/GASSISceneManager.h"
#include "Sim/GASSEulerRotation.h"
#include "Core/Utils/GASSIterators.h"
#include "Core/Utils/GASSException.h"

namespace tinyxml2
{
	class XMLElement;
}

namespace GASS
{
	GASS_FORWARD_DECL(ResourceGroup)
	GASS_FORWARD_DECL(ResourceLocation)
	GASS_FORWARD_DECL(MessageManager)
	GASS_FORWARD_DECL(SceneObject)
	GASS_FORWARD_DECL(SceneObjectManager)
	GASS_IFORWARD_DECL(SceneManager)

	typedef std::vector<SceneManagerPtr> SceneManagerVector;
	typedef VectorIterator<SceneManagerVector> SceneManagerIterator;

	/**
	A Scene in gass can be loaded from disc or created in runtime.
	The scene class will create instances of all registered scene managers and also create
	a scene object manager used as a container for scene objects
	*/

	class GASSExport Scene final : public Reflection<Scene, BaseReflectionObject>,
		public GASS_ENABLE_SHARED_FROM_THIS<Scene> ,
		public IMessageListener
	{
		friend class SceneObject;
		friend class SimEngine;
	public:
		Scene(const std::string &name);
		static void RegisterReflection();

		/**
		Get the start position for this scene,
		can be used to give cameras or players a initial position
		*/

		Vec3 GetStartPos()const {return m_StartPos;}

		/**
		Get the start rotation for this scene,
		can be used to give cameras or players a initial rotation
		*/
		EulerRotation GetStartRot()const {return m_StartRot;}

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
		void SetStartRot(const EulerRotation &value) { m_StartRot = value; }


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
			Get how many messages that is unprocessed
		*/
		size_t GetQueuedMessages() const;

		/**
		Load a new scene from path
		*/
		void Load(const std::string &name);

		/**
		Save scene to path
		*/
		void Save(const std::string &name);

		void Clear();

			/**
		Get scene scene name
		*/
		std::string GetName() const {return m_Name;}

		/**
		Set scene scene name
		*/
		//void SetName(const std::string &name) {m_Name = name;}

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
		GASS_SHARED_PTR<T> GetFirstSceneManagerByClass(bool no_throw=false) const
		{
			GASS_SHARED_PTR<T> ret;
			for(size_t i = 0 ; i < m_SceneManagers.size(); i++)
			{
				ret = GASS_DYNAMIC_PTR_CAST<T>(m_SceneManagers[i]);
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

		//remove this!
		SceneObjectPtr LoadObjectFromTemplate(const std::string &template_name, SceneObjectPtr parent) const;
		SceneObjectPtr GetRootSceneObject() const {return m_Root;}
		SceneObjectPtr GetSceneryRoot() const {return SceneObjectPtr(m_TerrainObjects);}

		/**
		Get all scenes from path. This function recursively search for scene.xml
		files and push that path to the return vector
		*/
		static std::vector<std::string> GetScenes(const FilePath &path);
		std::string GetResourceGroupName() const;

		bool GetGeocentric() const { return m_Geocentric; }
		void SetGeocentric(bool value) { m_Geocentric = value; }


		/**
			Connivence function to draw line for debug purpose (immediate mode)
		*/
		void DrawDebugLine(const Vec3 &start_point, const Vec3 &end_point, const ColorRGBA &start_color, const ColorRGBA &end_color) const;
protected:

	/**
		This function must be called by creator before using this class.
		This function allocate the scene object manager which take the
		Scene as constructor argument and therefore can not be
		allocated in the Scene contructor where shared_from_this()
		is not excepted. Further this function allocate all registred scene managers,
		they also need a SceneScenePtr and therefore not possible to allocate
		this stuff in the constructor
		*/
		void OnCreate();

		void OnUnload();

		/**
		Load scene  from xml,
		this method is called by the scene LoadXML method in the scene class
		scene_elem is the should point to the SceneScene tag
		*/
		void LoadXML(tinyxml2::XMLElement *scene_elem);

		/**
		Save scene  from xml,
		this method is called by the scene SaveXML method in the scene class
		scene_elem is the should point to the SceneScene tag
		*/
		void SaveXML(tinyxml2::XMLElement *scene_elem);

		void OnSpawnSceneObjectFromTemplate(SpawnObjectFromTemplateRequestPtr message);
		void OnRemoveSceneObject(RemoveSceneObjectRequestPtr message);

		//Helper function to LoadXML
		SceneManagerPtr LoadSceneManager(tinyxml2::XMLElement *sm_elem);

		Vec3 m_StartPos;
		EulerRotation m_StartRot;
		SceneManagerVector m_SceneManagers;
		std::string m_Name;
		std::string m_FolderName;
		MessageManagerPtr m_SceneMessageManager;

		//Scene root node
		SceneObjectPtr m_Root;
		SceneObjectWeakPtr m_TerrainObjects;
		ResourceGroupWeakPtr m_ResourceGroup;
		ResourceLocationWeakPtr m_ResourceLocation;
		bool m_Initlized;
		bool m_Geocentric;
	};
	GASS_PTR_DECL(Scene)
}

