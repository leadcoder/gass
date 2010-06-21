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
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/ScenarioSceneMessages.h"
#include "Core/Math/Vector.h"
#include "Core/Reflection/BaseReflectionObject.h"
#include "Core/MessageSystem/IMessage.h"


class TiXmlElement;

namespace GASS
{
	class ISceneManager;
	class MessageManager;
	class SceneObject;
	class SceneObjectManager;

	typedef boost::shared_ptr<SceneObjectManager> SceneObjectManagerPtr;
	typedef boost::shared_ptr<ISceneManager> SceneManagerPtr;
	typedef boost::shared_ptr<MessageManager> MessageManagerPtr;


	/**
		A scenario-scene is a step to divide a scenario into convenient parts.
		The owner of a scenario-scene is the scenario and the scenario-scene in its turn
		is the owner of the scenario-scene-objects. By dividing a scenario
		into scenario-scenes the user can have different representations of the same
		scenario, for instance one visual representation and one infrared.
		In another application the user might want to have a separate scene for the
		menu-system or divided the scenario in different zones,
		each represented by it's own scenario scene.
	*/

	class GASSExport ScenarioScene : public Reflection<ScenarioScene, BaseReflectionObject>, public boost::enable_shared_from_this<ScenarioScene>
	{
	public:
		// Default priorities for component loading
		//TODO: move this to better location
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
		friend class Scenario;
	public:
        /**
            Contructor that take the owner scenario as parameter
        */
		ScenarioScene(ScenarioPtr scenario);
		ScenarioScene(){}
		virtual ~ScenarioScene();
		/**
            Reflection function
        */
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
		Vec3 GetStartRot()const {return m_StartRot;}

		/**
            Set the start position for this scene,
            this attribute should be specified by scenario.xml
            but can be replaced by using this method
        */
		void SetStartPos(const Vec3 &value) {m_StartPos = value;}

		/**
            Set the start rotation for this scene,
            this attribute should be specified by scenario.xml
            but can be replaced by using this method
        */
		void SetStartRot(const Vec3 &value) {m_StartRot= value;}
		/**
          Deprecated
        */
		Vec2 GetRT90Origo() const {return m_RT90Origo;}

		/**
          Deprecated
        */
		void SetRT90Origo(const Vec2& origo) {m_RT90Origo=origo;}

		//Remove this
		/**
          Deprecated
        */
		Vec3 RT90ToGASSPosition(const Vec3 &pos);

		/**
          Deprecated
        */
		Vec3 GASSToRT90Position(const Vec3 &pos);

		/**
		Should be called by creator.
		This function is allocate the scene object manager which take the
		ScenarioScene as constructor argument and therefore can not be
		allocated in the ScenarioScene contructor where shared_from_this()
        is not excepted. Further this function allocate all registred scene managers,
		they also need a ScenarioScenePtr and therefore not possible to allocate
		this stuff in the constructor
		*/
		void OnCreate();

		/**
		Get scenario scene name
		*/
		std::string GetName() const {return m_Name;}

		/**
		Set scenario scene name
		*/
		void SetName(const std::string &name) {m_Name = name;}

		/**
		Register for scenario scene messages, see ScenarioSceneMessages.h for available messages
		*/

		int RegisterForMessage(const MessageType &type, MessageFuncPtr callback, int priority = 0);

		/**
		Unregister for scenario scene messages
		*/
		void UnregisterForMessage(const MessageType &type, MessageFuncPtr callback);
		void PostMessage(MessagePtr message);
		void SendImmediate(MessagePtr message);

		SceneObjectManagerPtr GetObjectManager() {return m_ObjectManager;}
		SceneManagerPtr GetSceneManager(const std::string &type);
		Vec3 GetSceneUp() {return m_Up;}
		Vec3 GetSceneEast() {return m_East;}
		Vec3 GetSceneNorth() {return m_North;}

		void CreateRegistredSceneManagers();
	protected:
        /**
          Load scenario scene from xml,
          this method is called by the scenario LoadXML method in the scenario class
          scene_elem is the should point to the ScenarioScene tag
        */
		void LoadXML(TiXmlElement *scene_elem);

		/**
          Save scenario scene from xml,
          this method is called by the scenario SaveXML method in the scenario class
          scene_elem is the should point to the ScenarioScene tag
        */
		void SaveXML(TiXmlElement *scene_elem);

		SceneObjectPtr LoadSceneObjectXML(TiXmlElement *so_elem);
		void OnSpawnSceneObjectFromTemplate(SpawnObjectFromTemplateMessagePtr message);
		void OnRemoveSceneObject(RemoveSceneObjectMessagePtr message);
		void Shutdown();

		void SetInstancesFile(const std::string &value);
		std::string GetInstancesFile() const;
		void SetUpVector(const std::string &value);
		std::string GetUpVector() const;
		void SetEastVector(const std::string &value);
		std::string GetEastVector() const;
		void SetNorthVector(const std::string &value);
		std::string GetNorthVector() const;

		//helpers
		Vec3 GetVector(const std::string &str) const;
		std::string  GetVector(const Vec3 &vec) const;


		void OnUpdate(double delta_time);
		void OnLoad();

		//Helper function to LoadXML
		SceneManagerPtr LoadSceneManager(TiXmlElement *sm_elem);
		Vec3 m_StartPos;
		Vec3 m_StartRot;
		Vec2 m_RT90Origo;
		std::vector<SceneManagerPtr> m_SceneManagers;
		MessageManagerPtr m_SceneMessageManager;
		ScenarioWeakPtr m_Scenario;
		std::string m_Name;
		SceneObjectManagerPtr m_ObjectManager;

		Vec3 m_Up;
		Vec3 m_North;
		Vec3 m_East;

		std::string m_InstancesFile;

	};

	typedef boost::shared_ptr<ScenarioScene> ScenarioScenePtr;
}

