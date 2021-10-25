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

#include "Sim/GASSSimEngineConfig.h"
#include "Sim/GASSSimSystemManager.h"
#include "Core/PluginSystem/GASSPluginManager.h"
#include "Core/Utils/GASSLogger.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSXMLUtils.h"
#include "Core/Utils/GASSFileUtils.h"
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{

	SimEngineConfig SimEngineConfig::Create(PhysicsOptions physics,
		SoundOptions sound,
		NetworkOptions network,
		InputOptions input)
	{
		SimEngineConfig conf;
		conf.Plugins = {"GASSPluginOSG",
						"GASSPluginInput",
						"GASSPluginBase",
						"GASSEditorModule" };
		if (input == InputOptions::OIS)
			conf.Plugins.emplace_back("GASSPluginOIS");
		if (sound == SoundOptions::OPENAL)
			conf.Plugins.emplace_back("GASSPluginOpenAL");
		if (physics != PhysicsOptions::NONE)
		{
			conf.Plugins.emplace_back(physics == PhysicsOptions::PHYSX ? "GASSPluginPhysX" : "GASSPluginODE");
			conf.Plugins.emplace_back("GASSPluginVehicle");
		}
		if (network == NetworkOptions::RAKNET)
			conf.Plugins.emplace_back("GASSPluginRaknet");

		std::vector<std::string> systems;
		
		systems.emplace_back("EditorSystem");
		systems.emplace_back(input == InputOptions::OSG ? "OSGInputSystem" : "OISInputSystem");
		systems.emplace_back("CoreSystem");
		systems.emplace_back("ControlSettingsSystem");
		systems.emplace_back("SimulationSystem");

		if (physics != PhysicsOptions::NONE)
		{
			systems.emplace_back("MaterialSystem"); //must be listed before physics system
			systems.emplace_back(physics == PhysicsOptions::PHYSX ? "PhysXPhysicsSystem" : "ODEPhysicsSystem");
		}
		
		if (network == NetworkOptions::RAKNET)
			systems.emplace_back("RakNetNetworkSystem");

		systems.emplace_back("OSGGraphicsSystem");
		systems.emplace_back("OSGCollisionSystem");


		for (auto system_name : systems)
		{
			GASS::SimSystemConfig sysc;
			sysc.Name = system_name;
			conf.SimSystemManager.Systems.push_back(sysc);
		}
		conf.DataPath = "../../data/";
		conf.ResourceConfig.ResourceLocations.emplace_back("GASS", "%GASS_DATA_HOME%/gfx", true);
		if (physics != PhysicsOptions::NONE)
		{
			if (physics == PhysicsOptions::PHYSX)
				conf.ResourceConfig.ResourceLocations.emplace_back("GASS_TEMPLATES", "%GASS_DATA_HOME%/templates/vehicles/physx", true);
			conf.ResourceConfig.ResourceLocations.emplace_back("GASS", "%GASS_DATA_HOME%/physics", true);

		}
		conf.ResourceConfig.ResourceLocations.emplace_back("GASS", "%GASS_DATA_HOME%/input", true);
		if(sound != SoundOptions::NONE)
			conf.ResourceConfig.ResourceLocations.emplace_back("GASS", "%GASS_DATA_HOME%/sounds", true);
		
		//templates
		conf.ResourceConfig.ResourceLocations.emplace_back("GASS_TEMPLATES", "%GASS_DATA_HOME%/templates/camera", true);
		return conf;
	}

	SimEngineConfig SimEngineConfig::LoadFromfile(const FilePath& filename)
	{
		SimEngineConfig config;
		tinyxml2::XMLDocument xml_doc;
		if (xml_doc.LoadFile(filename.GetFullPath().c_str()) != tinyxml2::XML_NO_ERROR)
		{
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE, "Couldn't load:" + filename.GetFullPath(), "SimEngineConfig::LoadSettings");
		}

		tinyxml2::XMLElement *gass_elem = xml_doc.FirstChildElement("GASS");
		if (!gass_elem)
		{
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE, "Failed to find GASS tag in:" + filename.GetFullPath(), "SimEngineConfig::LoadSettings");
		}

		const tinyxml2::XMLElement *data_path_elem = gass_elem->FirstChildElement("SetDataPath");
		if (data_path_elem)
		{
			config.DataPath = XMLUtils::ReadStringAttribute(data_path_elem, "value");
		}

		const tinyxml2::XMLElement *scene_path_elem = gass_elem->FirstChildElement("ScenePath");
		if (scene_path_elem)
			config.ScenePath = XMLUtils::ReadStringAttribute(scene_path_elem, "value");

		//read SceneObjectTemplateManager settings
		tinyxml2::XMLElement *sotm_elem = gass_elem->FirstChildElement("SceneObjectTemplateManager");
		if (sotm_elem)
		{
			config.AddObjectIDToName = XMLUtils::ReadBool(sotm_elem, "AddObjectIDToName");
			config.ObjectIDPrefix = XMLUtils::ReadString(sotm_elem, "ObjectIDPrefix");
			config.ObjectIDSufix = XMLUtils::ReadString(sotm_elem, "ObjectIDSufix");
		}

		{
			tinyxml2::XMLElement *plugins_elem = gass_elem->FirstChildElement("Plugins");
			if (plugins_elem)
			{
				tinyxml2::XMLElement *plugin_elem = plugins_elem->FirstChildElement();
				// Loop through each plugin
				while (plugin_elem)
				{
					if (std::string(plugin_elem->Value()) == "Plugin")
					{
						std::string name = plugin_elem->Attribute("PluginFile");
						config.Plugins.push_back(name);
					}
					else if (std::string(plugin_elem->Value()) == "Directory")
					{
						std::string path = plugin_elem->Attribute("Path");
						//config.PluginDir = path;
					}

					plugin_elem = plugin_elem->NextSiblingElement();
				}
			}
		}

		{
			tinyxml2::XMLElement *res_man_elem = gass_elem->FirstChildElement("ResourceManager");
			if (res_man_elem)
			{
				tinyxml2::XMLElement *group_elem = res_man_elem->FirstChildElement("ResourceGroup");

				while (group_elem)
				{
					const std::string group_name = XMLUtils::ReadStringAttribute(group_elem, "name");
					tinyxml2::XMLElement *location_elem = group_elem->FirstChildElement("ResourceLocation");
					while (location_elem)
					{
						ResourceLocationConfig res_conf;
						res_conf.Group = group_name;
						res_conf.Path = XMLUtils::ReadStringAttribute(location_elem, "path");
						const std::string type = XMLUtils::ReadStringAttribute(location_elem, "type");
						const std::string rec = XMLUtils::ReadStringAttribute(location_elem, "recursive");
						res_conf.Recursive = false;
						if (StringUtils::ToLower(rec) == "true")
							res_conf.Recursive = true;

						if (StringUtils::ToLower(type) == "filesystem")
							res_conf.Type = RLT_FILESYSTEM;
						else if (StringUtils::ToLower(type) == "zip")
							res_conf.Type = RLT_ZIP;

						config.ResourceConfig.ResourceLocations.push_back(res_conf);
						location_elem = location_elem->NextSiblingElement("ResourceLocation");
					}
					group_elem = group_elem->NextSiblingElement("ResourceGroup");
				}
			}
		}

		{
			if (tinyxml2::XMLElement *systems_elem = gass_elem->FirstChildElement("Systems"))
			{
				if (systems_elem->Attribute("MaxUpdateFrequency"))
				{
					systems_elem->QueryDoubleAttribute("MaxUpdateFrequency", &config.SimSystemManager.MaxUpdateFrequency);
				}

				if (systems_elem->Attribute("SimulationUpdateFrequency"))
				{
					systems_elem->QueryDoubleAttribute("SimulationUpdateFrequency", &config.SimSystemManager.SimulationUpdateFrequency);
				}

				if (systems_elem->Attribute("MaxSimulationSteps"))
				{
					systems_elem->QueryIntAttribute("MaxSimulationSteps", &config.SimSystemManager.MaxSimulationSteps);
				}

				tinyxml2::XMLElement *system_elem = systems_elem->FirstChildElement();
				//Load all systems tags
				while (system_elem)
				{
					SimSystemConfig sys_config;
					sys_config.Name = system_elem->Value();
					tinyxml2::XMLElement *sys_option_elem = system_elem->FirstChildElement();
					while (sys_option_elem)
					{
						SimSystemOption option;
						option.Name = sys_option_elem->Value();
						option.Value = XMLUtils::ReadStringAttribute(sys_option_elem, "value");
						sys_config.Settings.push_back(option);
						sys_option_elem = sys_option_elem->NextSiblingElement();
					}
					config.SimSystemManager.Systems.push_back(sys_config);
					system_elem = system_elem->NextSiblingElement();
				}
			}
		}
		return config;
	}
}
