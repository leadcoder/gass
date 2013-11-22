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
#include "Core/Utils/GASSSingleton.h"
#include "Core/Utils/GASSFilePath.h"
#include "Core/Utils/GASSIterators.h"
//#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include <boost/shared_ptr.hpp>

namespace GASS
{
	FDECL(PluginManager)
	FDECL(ResourceManager)
	FDECL(SimSystemManager)
	FDECL(RunTimeController)
	FDECL(BaseComponentContainerTemplateManager)
	FDECL(SceneObject)
	FDECL(Scene)
	FDECL(ScriptManager)
	class TaskNode;

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
		all scene information is capsulated by the Scene class and should 
		be created by the application after SimEngine has been intialized. 
	*/

	class GASSExport SimEngine  : public Singleton<SimEngine>
	{
	public:
		typedef std::vector<ScenePtr> SceneVector;
		typedef VectorIterator<SceneVector> SceneIterator;
		typedef ConstVectorIterator<SceneVector> ConstSceneIterator;

	public:
		SimEngine();
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
		void Init(const FilePath &configuration_file = FilePath("gass.xml"), const FilePath &log_folder = FilePath(""));

		/**
		Main update for GASS.
		@remarks
			The application is responsible for calling this function each frame
			SimEngine will take care of delta time calulations if you use this function
		@ note This cunction will call void Tick(double delta_time) 
		*/
		void Update();

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
		Get the sim system mananger. 
		This is the first step to get hold of a sim system,
		see SystemManager for more details of how to find a 
		certain system.
		*/

		SimSystemManagerPtr GetSimSystemManager() const {return m_SystemManager;}
		
		/**
		Get the object template manager. 
		The oject template manager holds templates for sim objects that can be created.
		See BaseComponentContainerTemplateManager inside the component system from more info.
		*/
		BaseComponentContainerTemplateManagerPtr GetSceneObjectTemplateManager() const {return m_SceneObjectTemplateManager;}

		/**
			Get the runtime control manager. See RuntimeController class for more information
		*/
		RunTimeControllerPtr GetRuntimeController() const {return m_RTC;}

		/**
		Return elapsed time
		*/
		double GetTime() const {return m_CurrentTime;}
		
		/**
			Convience function to create new objects from templates
		*/
		SceneObjectPtr CreateObjectFromTemplate(const std::string &template_name) const;

		/**
			Reload all templates in resource group GASS_TEMPLATES
		*/
		void ReloadTemplates();

		void DestroyScene(SceneWeakPtr scene);
		SceneWeakPtr CreateScene(const std::string &name);
		ConstSceneIterator GetScenes() const;
		SceneIterator GetScenes();
		FilePath GetLogFolder() const {return m_LogFolder;}
		void SetLogFolder(const FilePath &path) {m_LogFolder = path;}
		FilePath GetScenePath() const {return m_ScenePath;}
		void SetScenePath(const FilePath &path) {m_ScenePath = path;}
		std::vector<std::string> GetSavedScenes() const;
		void SyncMessages(double delta_time);
		ResourceManagerPtr GetResourceManager() const {return m_ResourceManager;}
		ScriptManagerPtr GetScriptManager() const {return m_ScriptManager;}
	private:
		void LoadSettings(const FilePath &configuration_file);
		PluginManagerPtr m_PluginManager;
		ScriptManagerPtr m_ScriptManager;
		SimSystemManagerPtr m_SystemManager;
		BaseComponentContainerTemplateManagerPtr m_SceneObjectTemplateManager;
		RunTimeControllerPtr m_RTC;
		ResourceManagerPtr m_ResourceManager;
		SceneVector m_Scenes;
		double m_CurrentTime;
		double m_MaxUpdateFreq;
		FilePath m_ScenePath;
		FilePath m_LogFolder;
	};
}

