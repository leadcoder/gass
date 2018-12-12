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

#include "Core/PluginSystem/GASSPluginManager.h"
#include "Core/PluginSystem/GASSDynamicModule.h"
#include "Core/Utils/GASSLogger.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSFileUtils.h"
#include "Core/Serialize/tinyxml2.h"
#include <cassert>

namespace GASS
{
	PluginManager::~PluginManager()
	{
		Shutdown();
	}

	void PluginManager::LoadFromFile(const std::string &filename)
	{

		if(filename =="")
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS,"No File name provided", "PluginManager::LoadFromFile");

		GASS_LOG(LINFO) << "Start loading plugins from " << filename;
		auto *xmlDoc = new tinyxml2::XMLDocument();
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
		for(auto & plugin : m_Plugins)
		{
			plugin->Unload();
			delete plugin;
		}
		m_Plugins.clear();
	}

	void PluginManager::UnloadPlugin(const std::string &name)
	{
		for(auto & plugin : m_Plugins)
		{
			if(plugin->GetModuleName() == name)
				plugin->Unload();
		}
	}

	void PluginManager::ReloadAll()
	{
		for(auto & plugin : m_Plugins)
		{
			plugin->Unload();
			plugin->Load();
		}
	}

	void PluginManager::CallFunction(const std::string &func_name, void* arg1)
	{
		for(auto & plugin : m_Plugins)
		{
			plugin->CallFunction(func_name,arg1);
		}
	}

	void PluginManager::LoadPlugin(const std::string &name)
	{
		std::string file_name = name;
		if(std::string::npos == file_name.find("."))
		{
#ifndef NDEBUG 
			file_name += "_d";
#endif

#ifndef WIN32
			file_name += ".so";
#else
			file_name += ".dll";
#endif
		}
		auto* module = new DynamicModule(file_name);
		module->Load();
		GASS_LOG(LINFO) << file_name << " loaded";
		m_Plugins.push_back(module);
	}

	void PluginManager::LoadPluginsFromDirectory(const std::string &directory, const std::string &extention_filter)
	{
		std::string ext = extention_filter;
		if(ext == "")
		{
#ifndef WIN32
			ext = "so";
#else
			ext = "dll";
#endif
		}
		std::vector<std::string> files;

		FileUtils::GetFilesFromPath(files, directory, false, false);
		std::vector<std::string> plugins;

		for (const auto & file : files)
		{
			std::string extension = FileUtils::GetExtension(file);
			std::string filename = FileUtils::GetFilename(file);
			//filter debug plugins
#ifndef NDEBUG
				if (filename.find("_d") == std::string::npos)
					continue;
#else
				if (filename.find("_d") != std::string::npos)
					continue;
#endif

				if (extension == ext)
				{
					plugins.push_back(filename);
				}
		}

		std::string saved_path = FileUtils::GetCurrentDir();
		FileUtils::SetCurrentDir(directory);
		for (const auto & plugin : plugins)
		{
			auto* module = new DynamicModule(plugin);
			module->Load();
			GASS_LOG(LINFO) << plugin << " loaded";
			m_Plugins.push_back(module);
		}
		FileUtils::SetCurrentDir(saved_path);
	}
}
