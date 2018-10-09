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
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Core/Utils/GASSSingleton.h"
#include "Core/Utils/GASSFilePath.h"
#include "Core/Utils/GASSIterators.h"
#include "Core/Utils/GASSEnumBinder.h"
#include "Core/MessageSystem/GASSStaticMessageListener.h"

//debug stuff
#define GASS_PRINT(message){std::stringstream ss; ss << message; SimEngine::Get().GetSimSystemManager()->SendImmediate(SystemMessagePtr(new DebugPrintRequest(ss.str())));}
#define GASS_DRAW_LINE(start,end,color) GetSceneObject()->GetScene()->PostMessage(SceneMessagePtr(new DrawLineRequest(start,end,color,color)));

namespace GASS
{
	GASS_FORWARD_DECL(PluginManager)
	GASS_FORWARD_DECL(ResourceManager)
	GASS_FORWARD_DECL(SimSystemManager)
	GASS_FORWARD_DECL(TaskNode)
	GASS_FORWARD_DECL(SystemStepper)
	GASS_FORWARD_DECL(ComponentContainerTemplateManager)
	GASS_FORWARD_DECL(SceneObject)
	GASS_FORWARD_DECL(Scene)
	GASS_FORWARD_DECL(ScriptManager)
	class OldTaskNode;

	enum UpdateGroupID
	{
		UGID_NO_UPDATE,
		UGID_PRE_SIM,
		UGID_SIM,
		UGID_POST_SIM,
		UGID_LAST
	};

	START_ENUM_BINDER(UpdateGroupID,UpdateGroupIDBinder)
		BIND(UGID_NO_UPDATE)
		BIND(UGID_PRE_SIM)
		BIND(UGID_SIM)
		BIND(UGID_POST_SIM)
	END_ENUM_BINDER(UpdateGroupID,UpdateGroupIDBinder)


	/** \addtogroup GASSSim
	*  @{
	*/
	

	/**\
	* \brief Main class for GASS Sim.
		The SimEngine is a singleton class that hold some basic functionality
		of the Sim module like plugin management, runtime execution and system management.
		When using the GASS sim module this class is the first one that should be instantiated 
		and initialized. The user is responsible to call the update function of this 
		class each frame, this will ensure that all system get updated through the RTC scheduling
		class. 
		NOTE: This class has no knowledge of scenes, 
		all scene information is encapsulated by the Scene class and should 
		be created by the application after SimEngine has been initialized. 
	*/

	class GASSExport SimEngine : public Singleton<SimEngine> , StaticMessageListener
	{
	public:
		typedef std::vector<ScenePtr> SceneVector;
		typedef VectorIterator<SceneVector> SceneIterator;
		typedef ConstVectorIterator<SceneVector> ConstSceneIterator;
	public:
		SimEngine(const FilePath &log_folder = FilePath("./"));
		virtual ~SimEngine();
		static SimEngine* GetPtr();
		static SimEngine& Get();

		/**
			This function will initialize the GASS simulation engine. When this function is called
			all GASS plugins listed in the configuration_file are loaded. After that the system manager try to load all systems
			listed in the configuration_file. 
			@param configuration_file xml-file listing all plugins to load.
			@param log_folder Alternative File path for log files. 
		*/
		void Init(const FilePath &configuration_file = FilePath("gass.xml"));

		/**
		Main update for GASS.
		@remarks
			The application is responsible for calling this function each frame
			SimEngine will take care of delta time calculations if you use this function
			return true if simulation was updated else false
		@ note This function will call void Tick(double delta_time) 

		*/
		bool Update();

		/**
		The application can call this function instead of Update() to feed custom delta_time
		@param delta_time is the time step.
		*/
		void Tick(double delta_time);
		
		/**
		Shutdown GASS.
		*/
		bool Shutdown();

		/**
		Get the sim system manager. 
		This is the first step to get hold of a sim system,
		see SystemManager for more details of how to find a 
		certain system.
		*/

		SimSystemManagerPtr GetSimSystemManager() const {return m_SystemManager;}
		
		/**
		Get the object template manager. 
		The object template manager holds templates for sim objects that can be created.
		See ComponentContainerTemplateManager inside the component system from more info.
		*/
		ComponentContainerTemplateManagerPtr GetSceneObjectTemplateManager() const {return m_SceneObjectTemplateManager;}

		/**
			Get root task node
		*/
		//TaskNode2Ptr GetRootTaskNode() const;

		/**
		Return elapsed time
		*/
		//double GetTime() const {return m_CurrentTime;}
		
		/**
			Convince function to create new objects from templates
		*/
		SceneObjectPtr CreateObjectFromTemplate(const std::string &template_name) const;

		/**
			Reload all templates in resource group GASS_TEMPLATES
		*/
		void ReloadTemplates();

		/**
			Remove scene 
		*/
		void DestroyScene(SceneWeakPtr scene);

		/**
			Create new scene 
			@param Unique name for this scene (will be used if the scene is saved)
		*/
		SceneWeakPtr CreateScene(const std::string &name);

		/**
			Get const iterator to all active scenes
		*/
		ConstSceneIterator GetScenes() const;

		/**
			Get iterator to all active scenes
		*/
		SceneIterator GetScenes();

		/**
			Get folder for log files
		*/
		FilePath GetLogFolder() const {return m_LogFolder;}

		/**
			Set folder for log files
		*/
		void SetLogFolder(const FilePath &path) {m_LogFolder = path;}

		/**
			Get file path where scene are stored
		*/
		FilePath GetScenePath() const {return m_ScenePath;}

		/**
			Set file path where scene are stored
		*/
		void SetScenePath(const FilePath &path) {m_ScenePath = path;}

		/**
			Get all saved scene from ScenePath
		*/
		std::vector<std::string> GetSavedScenes() const;

		/*
			Sync messages for all systems, scenes and sceneobjects
		*/
		void SyncMessages(double delta_time);

		ResourceManagerPtr GetResourceManager() const {return m_ResourceManager;}
		ScriptManagerPtr GetScriptManager() const {return m_ScriptManager;}
		PluginManagerPtr GetPluginManager() const {return m_PluginManager;}
		//SimSystemManagerPtr GetRunTimeController() const {return m_SystemManager;}

		/**
			Enable/disable external updates
		*/
		//void SetUpdateSimOnRequest(bool value);
		//bool GetUpdateSimOnRequest() const;
		double GetMaxUpdateFreq() const {return m_MaxUpdateFreq;}
		double GetTime() const;
		double GetSimulationTime() const;
	private:
		void LoadSettings(const FilePath &configuration_file);
		void LoadResources(const FilePath &configuration_file);
		PluginManagerPtr m_PluginManager;
		ScriptManagerPtr m_ScriptManager;
		SimSystemManagerPtr m_SystemManager;
		ComponentContainerTemplateManagerPtr m_SceneObjectTemplateManager;
		SystemStepperPtr m_SystemStepper;
		
		ResourceManagerPtr m_ResourceManager;
		SceneVector m_Scenes;
		double m_CurrentTime;
		double m_MaxUpdateFreq;
		FilePath m_ScenePath;
		FilePath m_LogFolder;
		
		int m_NumThreads;

		
	};
}

