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
#include "Core/Math/Vector.h"
#include "Core/Reflection/BaseReflectionObject.h"
#include "Sim/Scenario/Scene/Messages/CoreScenarioSceneMessages.h"
#include "Core/Utils/Iterators.h"

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



	/**
		A scenario in GASS consist of scenarios scenes. The scenario class is
		therefore only a container of scenario scenes and the actual scenario 
		functionality is capsulated in its scenario scenes.

		By dividing the scenario in scenes the user can have different 
		representations of the same scenario, for instance one visual representation
		and one infrared. In another application the user might want to have a 
		separate scene for the menu-system or divided the scenario in different zones
		each represented by it's own scenario-scene.
		See ScenarioScene class for more information about scenario scenes. 
	*/
	class GASSExport Scenario : public Reflection<Scenario, BaseReflectionObject>, public boost::enable_shared_from_this<Scenario> , public IMessageListener
	{
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

		Scenario();
		virtual ~Scenario();

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
		Register for scenario scene messages, see ScenarioSceneMessages.h for available messages
		*/

		int RegisterForMessage(const MessageType &type, MessageFuncPtr callback, int priority = 0);

		/**
		Unregister for scenario scene messages
		*/
		void UnregisterForMessage(const MessageType &type, MessageFuncPtr callback);
		void PostMessage(MessagePtr message);
		void SendImmediate(MessagePtr message);


		/**
		Load a new scenario from path
		return success if true or false if failure
		*/
		bool Load(const std::string &scenario_parh);

		/**
		Save scenario to path
		*/
		bool Save(const std::string &name);

		/**
		Should be called by creator.
		This function allocate the scene object manager which take the
		Scenario as constructor argument and therefore can not be
		allocated in the Scenario contructor where shared_from_this()
        is not excepted. Further this function allocate all registred scene managers,
		they also need a ScenarioScenePtr and therefore not possible to allocate
		this stuff in the constructor
		*/
		void OnCreate();

		std::string GetPath() const {return m_ScenarioPath;}

		void OnUpdate(double delta_time);
		


		double GetOrigoOffsetEast() const;
		double GetOrigoOffsetNorth() const;
		void SetOrigoOffsetEast(double value);
		void SetOrigoOffsetNorth(double value);
		std::string GetProjection() const;
		void SetProjection(const std::string &proj);

		SceneObjectManagerPtr GetObjectManager() {return m_ObjectManager;}
		SceneManagerPtr GetSceneManager(const std::string &type);

	protected:
        /**
          Load scenario  from xml,
          this method is called by the scenario LoadXML method in the scenario class
          scene_elem is the should point to the ScenarioScene tag
        */
		void LoadXML(TiXmlElement *scene_elem);

		/**
          Save scenario  from xml,
          this method is called by the scenario SaveXML method in the scenario class
          scene_elem is the should point to the ScenarioScene tag
        */
		void SaveXML(TiXmlElement *scene_elem);

		SceneObjectPtr LoadSceneObjectXML(TiXmlElement *so_elem);
		void OnSpawnSceneObjectFromTemplate(SpawnObjectFromTemplateMessagePtr message);
		void OnRemoveSceneObject(RemoveSceneObjectMessagePtr message);
		void OnLoad();

		//Helper function to LoadXML
		SceneManagerPtr LoadSceneManager(TiXmlElement *sm_elem);
		
		Vec3 m_StartPos;
		Vec3 m_StartRot;

		std::string m_ScenarioPath;
		
		SceneManagerVector m_SceneManagers;
		
		std::string m_Name;
		
		SceneObjectManagerPtr m_ObjectManager;

		
		MessageManagerPtr m_ScenarioMessageManager;
		
		/**
		Get scene manager iterator
		*/
		SceneManagerIterator GetSceneManagers();



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


		
		
	
		/**
			Get optional resource locations relative to scenario path
		*/
		std::vector<std::string> GetScenarioResourceFolders() const;
		/**
			Set optional resource locations relative to scenario path
		*/
		void SetScenarioResourceFolders(const std::vector<std::string> &folders);
		std::vector<std::string> m_ResourceFolders;
	};
	typedef boost::shared_ptr<Scenario> ScenarioPtr;
	typedef boost::weak_ptr<Scenario> ScenarioWeakPtr;
}

