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

#ifndef PLUGINMANAGER_HH
#define PLUGINMANAGER_HH

#include "Core/Common.h"
#include <string>
#include <vector>

namespace GASS
{
	class DynamicModule;

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Plugin
	*  @{
	*/

	/**
		Manager used to load and hold plugins. 
		A GASS plugin is the same as a regular 
		dynamic library (dll in windows, so linux etc.)
		The plugin manager only give the user some help
		to load dynamic libraries in a platform independent
		manner and doing this by loading a configuration file 
		that hold a list of plugins to load.

	*/
	class GASSCoreExport PluginManager
	{
	public:
		PluginManager();
		virtual ~PluginManager();
		/**
			Load plugin-configuration file.
			
			Bellow is a configuration file example that 
			load 3 plugins (TestPlugin1,TestPlugin2,TestPlugin3). 
			Note that  the filename extension is removed. 
			
			<?xml version="1.0" encoding="utf-8"?>
			<Plugins>
				<Plugin PluginFile = "TestPlugin1"/>
				<Plugin PluginFile = "TestPlugin2"/>
				<Plugin PluginFile = "TestPlugin3"/>
			</Plugins>
		*/
		void LoadFromFile(const std::string &filename);

		/*
			Load single plugin
		*/
		void LoadPlugin(const std::string &name);

		/*
			Unload single plugin
		*/
		void UnloadPlugin(const std::string &name);

		/**
			Reload all loaded plugins
		*/
		void ReloadAll();
		
		/**
			Free all loaded plugins
		*/
		void Shutdown();

		/**
			Load all plugins from directory
			@param directory Folder to serach for plugins in
			@param extention_filter Only files with this extention are loaded, if not provided 
									plugins are supposed to have dll extention on windows and so in linux.

		*/
		void LoadPluginsFromDirectory(const std::string &directory,const std::string &extention_filter = "");
	protected:
		std::vector<DynamicModule*> m_Plugins;
	};
}

#endif // #ifndef PLUGINMANAGER_HH

