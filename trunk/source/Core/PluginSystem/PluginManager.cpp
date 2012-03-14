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

#include "Core/PluginSystem/PluginManager.h"
#include "Core/PluginSystem/DynamicModule.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "tinyxml.h"
#include <assert.h>
#include <boost/filesystem.hpp>

namespace GASS
{
	PluginManager::PluginManager()
	{
	}

	PluginManager::~PluginManager()
	{
		Shutdown();
	}

	void PluginManager::LoadFromFile(const std::string &filename)
	{
		
		if(filename =="")
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS,"No File name provided", "PluginManager::LoadFromFile");
		
		LogManager::getSingleton().stream() << "Start loading plugins from " << filename;
		TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());
		if (!xmlDoc->LoadFile())
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load:" + filename, "PluginManager::LoadFromFile");
		
		TiXmlElement *plugins = xmlDoc->FirstChildElement("Plugins");

		if(plugins)
		{
			plugins = plugins->FirstChildElement();
			// Loop through each plugin
			while(plugins)
			{
				if(std::string(plugins->Value()) == "Plugin")
				{
					std::string name = plugins->Attribute("PluginFile");
#ifdef DEBUG
					name += "_d";
#endif

#ifndef WIN32
					name += ".so";
#else
					name += ".dll";
#endif

					DynamicModule* module = new DynamicModule(name);
					module->Load();
					LogManager::getSingleton().stream() << name << " loaded";
					m_Plugins.push_back(module);
				}
				else if(std::string(plugins->Value()) == "Directory")
				{
					std::string path = plugins->Attribute("Path");
					LoadPluginsFromDirectory(path);
				}
				plugins = plugins->NextSiblingElement();
			}
		}
		xmlDoc->Clear();
		// Delete our allocated document and return success ;)
		delete xmlDoc;
	}
	void PluginManager::Shutdown()
	{
		for(size_t i = 0 ; i < m_Plugins.size(); i++)
		{
			m_Plugins[i]->Unload();
			delete m_Plugins[i];
		}
		m_Plugins.clear();
	}

	void PluginManager::Reload()
	{
		for(size_t i = 0 ; i < m_Plugins.size(); i++)
		{
			m_Plugins[i]->Unload();
			m_Plugins[i]->Load();
		}
	}


	void PluginManager::LoadPluginsFromDirectory(const std::string &directory)
	{
#ifndef WIN32
		const std::string ext = ".so";
#else
		const std::string ext = ".dll";
#endif
		boost::filesystem::path path(directory); 
		
		std::vector<std::string> plugins;
		if( boost::filesystem::exists( path) )  
		{
			
			boost::filesystem::directory_iterator end ;    
			for( boost::filesystem::directory_iterator iter(path) ; iter != end ; ++iter )      
			{
				if ( !boost::filesystem::is_directory( *iter ) )      
				{   
					std::string exstension = iter->path().extension().generic_string();
					std::string filename = iter->path().filename().generic_string();

					if(exstension == ext)
					{
						plugins.push_back(filename );
					}
				}     
			}
			boost::filesystem::path saved_path = boost::filesystem::current_path();
			boost::filesystem::current_path(path);
			for(size_t i = 0 ; i < plugins.size() ; i++)
			{
				DynamicModule* module = new DynamicModule(plugins[i]);
				module->Load();
				LogManager::getSingleton().stream() << plugins[i] << " loaded";
				m_Plugins.push_back(module);
			}
			boost::filesystem::current_path(saved_path);
		}
	}
}
