/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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

#include "Core/PluginSystem/GASSPluginManager.h"
#include "Core/PluginSystem/GASSDynamicModule.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "tinyxml2.h"
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
		tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
		if (xmlDoc->LoadFile(filename.c_str()) != tinyxml2::XML_NO_ERROR)
		{
			delete xmlDoc;
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load:" + filename, "PluginManager::LoadFromFile");
		}


		tinyxml2::XMLElement *plugins = xmlDoc->FirstChildElement("GASS");
		if (!plugins)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Couldn't find GASS tag in:" + filename, "PluginManager::LoadFromFile");

		plugins = plugins->FirstChildElement("Plugins");

		if(plugins)
		{
			plugins = plugins->FirstChildElement();
			// Loop through each plugin
			while(plugins)
			{
				if(std::string(plugins->Value()) == "Plugin")
				{
					std::string name = plugins->Attribute("PluginFile");
					LoadPlugin(name);
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

	void PluginManager::UnloadPlugin(const std::string &name)
	{
		for(size_t i = 0 ; i < m_Plugins.size(); i++)
		{
			if(m_Plugins[i]->GetModuleName() == name)
				m_Plugins[i]->Unload();
		}
	}

	void PluginManager::ReloadAll()
	{
		for(size_t i = 0 ; i < m_Plugins.size(); i++)
		{
			m_Plugins[i]->Unload();
			m_Plugins[i]->Load();
		}
	}

	void PluginManager::CallFunction(const std::string &func_name, void* arg1)
	{
		for(size_t i = 0 ; i < m_Plugins.size(); i++)
		{
			m_Plugins[i]->CallFunction(func_name,arg1);
		}
	}

	void PluginManager::LoadPlugin(const std::string &name)
	{
		std::string file_name = name;
		if(std::string::npos == file_name.find("."))
		{
#ifdef DEBUG
			file_name += "_d";
#endif

#ifndef WIN32
			file_name += ".so";
#else
			file_name += ".dll";
#endif
		}
		DynamicModule* module = new DynamicModule(file_name);
		module->Load();
		LogManager::getSingleton().stream() << file_name << " loaded";
		m_Plugins.push_back(module);
	}

	void PluginManager::LoadPluginsFromDirectory(const std::string &directory, const std::string &extention_filter)
	{
		std::string ext = extention_filter;
		if(ext == "")
		{
#ifndef WIN32
			ext = ".so";
#else
			ext = ".dll";
#endif
		}
		boost::filesystem::path path(directory); 

		std::vector<std::string> plugins;
		if( boost::filesystem::exists( path) )  
		{

			boost::filesystem::directory_iterator end ;    
			for( boost::filesystem::directory_iterator iter(path) ; iter != end ; ++iter )      
			{
				if ( !boost::filesystem::is_directory( *iter ) )      
				{   
					std::string extension = iter->path().extension().generic_string();
					std::string filename = iter->path().filename().generic_string();

					//filter debug plugins
					#ifdef DEBUG
						if(filename.find("_d") == std::string::npos)
							continue;
					#else
						if(filename.find("_d") != std::string::npos)
							continue;
					#endif
				
					if(extension == ext)
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
