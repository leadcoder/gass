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
//#include "Sim/Systems/Input/GASSIControlSettingsSystem.h"
#include <boost/shared_ptr.hpp>

namespace GASS
{
	class PluginManager;
	class SimSystemManager;
	class BaseComponentContainerTemplateManager;
	class RunTimeController;
	class SceneObject;
	class Scene;
	class TaskNode;
	
	typedef boost::shared_ptr<PluginManager> PluginManagerPtr;
	typedef boost::shared_ptr<SimSystemManager> SimSystemManagerPtr;
	typedef boost::shared_ptr<BaseComponentContainerTemplateManager> BaseComponentContainerTemplateManagerPtr;
	//typedef boost::shared_ptr<ControlSettingsManager> ControlSettingsManagerPtr;
	typedef boost::shared_ptr<RunTimeController> RuntimeControllerPtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::shared_ptr<Scene> ScenePtr;
	typedef boost::weak_ptr<Scene> SceneWeakPtr;

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
			This function will initialize the sim engine. When this function is called
			the plugin manager will first try to load all GASS plugins listed in the 
			plugin_file. After that the system manager try to load all systems
			listed in the system_file. 
			@param plugin_file xml-file listing all plugins to load.
			@param system_file xml-file listing all systems to load.
			@param num_rtc_threads Threads to use in the RTC, see RTCcontroller for more information.
		*/
		void Init(const FilePath &plugin_file = FilePath("gass.xml"));

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
		Get the control settings manager. 
		*/
		//TODO: This should be moved to it's own system
		//ControlSettingsManagerPtr GetControlSettingsManager() {return m_ControlSettingsManager;}

		/**
			Get the runtime control manager. See RuntimeController class for more information
		*/
		RuntimeControllerPtr GetRuntimeController(){return m_RTC;}


		/**
		Return elapsed time
		*/
		double GetTime() const {return m_CurrentTime;}
		
		
		/**
			Convience function to create new objects from templates
		*/
		SceneObjectPtr CreateObjectFromTemplate(const std::string &template_name) const;

		/**
			Load scene form file
		*/
		SceneWeakPtr LoadScene(const FilePath &path);
		void  UnloadScene(SceneWeakPtr scene);

		SceneIterator GetScenes();
		ConstSceneIterator GetScenes() const;
	private:
		void LoadSettings(const FilePath &configuration_file);
		PluginManagerPtr m_PluginManager;
		SimSystemManagerPtr m_SystemManager;
		BaseComponentContainerTemplateManagerPtr m_SceneObjectTemplateManager;
		//ControlSettingsManagerPtr m_ControlSettingsManager;
		RuntimeControllerPtr m_RTC;
		SceneVector m_Scenes;
		double m_CurrentTime;
		double m_MaxUpdateFreq;
	};
}

