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
#include "Core/Utils/Singleton.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include <boost/shared_ptr.hpp>

namespace GASS
{
	class PluginManager;
	class SimSystemManager;
	class BaseComponentContainerTemplateManager;
	class IRuntimeController;
	
	typedef boost::shared_ptr<PluginManager> PluginManagerPtr;
	typedef boost::shared_ptr<SimSystemManager> SystemManagerPtr;
	typedef boost::shared_ptr<BaseComponentContainerTemplateManager> BaseComponentContainerTemplateManagerPtr;
	typedef boost::shared_ptr<ControlSettingsManager> ControlSettingsManagerPtr;
	typedef boost::shared_ptr<IRuntimeController> RuntimeControllerPtr;

	/**\
	* \brief Main class for GASS Sim.
		The SimEngine is a singleton class that hold some basic functionality
		of the Sim module like plugin management, runtime execution and system management.
		When using the GASS sim module this class is the first one that should be instantiated 
		and initialized. The user is responsible to call the update function of this 
		class each frame, this will ensure that all system get updated through the RTC scheduling
		class. 
		This class has no knowledge of scenarios and/or scenario scenes, this could be changed in the future
		but for now scenario information is capsulated in the Scenario class. 

	*/

	class GASSExport SimEngine  : public Singleton<SimEngine>
	{
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
		*/
		bool Init(const std::string &plugin_file = std::string("plugins.xml"), 
				  const std::string &system_file = std::string("systems.xml"),
				  const std::string &control_settings = std::string("control_settings.xml"));

		/**
		Main update for GASS.
		@remarks
			The application is responsible for calling this function each frame
		@param delta is the frame time.
		*/
		void Update(float delta_time);
		
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

		SystemManagerPtr GetSystemManager() {return m_SystemManager;}
		
		/**
		Get the object template manager. 
		The oject template manager holds templates for sim objects that can be created.
		See BaseComponentContainerTemplateManager inside the component system from more info.
		*/
		BaseComponentContainerTemplateManagerPtr GetSimObjectManager() {return m_SimObjectManager;}
		
		/**
		Get the control settings manager. 
		*/
		//TODO: This should be moved to it's own system
		ControlSettingsManagerPtr GetControlSettingsManager() {return m_ControlSettingsManager;}

		/**
			Get the runtime control manager. See RuntimeController class for more information
		*/
		RuntimeControllerPtr GetRuntimeController(){return m_RTC;}
	private:
		PluginManagerPtr m_PluginManager;
		SystemManagerPtr m_SystemManager;
		BaseComponentContainerTemplateManagerPtr m_SimObjectManager;
		ControlSettingsManagerPtr m_ControlSettingsManager;
		RuntimeControllerPtr m_RTC;
	};
}

