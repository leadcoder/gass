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
#include "Core/Utils/Log.h"
#include "tinyxml.h"
#include <assert.h>

namespace GASS
{
	PluginManager::PluginManager()
	{
	}

	PluginManager::~PluginManager()
	{
		Shutdown();
	}

	bool PluginManager::LoadFromFile(const std::string &filename)
	{
		if(filename =="") return false;
		TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());
		if (!xmlDoc->LoadFile())
		{
			// Fatal error, cannot load
			Log::Warning("PluginManager::Load() - Couldn't load: %s", filename.c_str());
			return false;
		}
		TiXmlElement *plugins = xmlDoc->FirstChildElement("Plugins");

		if(plugins)
		{
			plugins = plugins->FirstChildElement();
			// Loop through each plugin
			while(plugins)
			{
				std::string name = plugins->Attribute("PluginFile");
				DynamicModule* module = new DynamicModule(name);
				if(module->Load())
					m_Plugins.push_back(module);
				else delete module;
				plugins = plugins->NextSiblingElement();

			}
		}
		xmlDoc->Clear();
		// Delete our allocated document and return success ;)
		delete xmlDoc;
		return true;
	}
	void PluginManager::Shutdown()
	{
		for(int i = 0 ; i < m_Plugins.size(); i++)
		{
			m_Plugins[i]->Unload();
			delete m_Plugins[i];
		}
		m_Plugins.clear();
	}

	void PluginManager::Reload()
	{
		for(int i = 0 ; i < m_Plugins.size(); i++)
		{
			m_Plugins[i]->Unload();
			m_Plugins[i]->Load();
		}
	}
}
