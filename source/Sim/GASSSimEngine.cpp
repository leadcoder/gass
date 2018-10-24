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

#include "Sim/GASSSimEngine.h"
#include "Sim/Utils/GASSProfiler.h"
#include "Sim/Utils/GASSProfileRuntimeHandler.h"
#include "Sim/GASSSimSystemManager.h"
#include "Core/PluginSystem/GASSPluginManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplateManager.h"
#include "Core/Utils/GASSLogger.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSXMLUtils.h"
#include "Core/Utils/GASSFileUtils.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSResourceManager.h"
#include "Sim/GASSResourceGroup.h"
#include "Sim/GASSResourceLocation.h"
#include "Sim/GASSResource.h"
#include "Sim/GASSScriptManager.h"
#include "Sim/GASSResourceHandle.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/GASSGeometryFlags.h"
#include "Sim/Interface/GASSIGraphicsSystem.h"
#include <stdlib.h>
#include <stdio.h>
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{
	SimEngine::SimEngine(const FilePath &log_folder) : m_CurrentTime(0),
		m_PluginManager(new PluginManager()),
		m_ScriptManager(new ScriptManager()),
		m_ResourceManager(new ResourceManager()),
		m_SystemManager(new SimSystemManager()),
		m_SceneObjectTemplateManager(new ComponentContainerTemplateManager()),
		m_LogFolder(log_folder)
	{
		//Always log to file if sim is created?
		if (!Logger::IsInitialize())
		{
			const std::string log_file = log_folder.GetFullPath() + "GASS.log";
			GASS_LOG_INITIALIZE(log_file);
		}
		m_ScriptManager->Init();
	}

		SimEngine::~SimEngine()
	{

	}

	template<> SimEngine* Singleton<SimEngine>::m_Instance = 0;
	SimEngine* SimEngine::GetPtr(void)
	{
		assert(m_Instance);
		return m_Instance;
	}

	SimEngine& SimEngine::Get(void)
	{
		assert(m_Instance);
		return *m_Instance;
	}

	void SimEngine::Init(const FilePath &configuration)
	{
		GASS_LOG(LINFO) << "SimEngine Initialization Started";

		if (configuration.GetFullPath() != "")
		{ 
			LoadSettings(configuration);
			m_PluginManager->LoadFromFile(configuration.GetFullPath());
			LoadResources(configuration);
			m_SystemManager->Load(configuration.GetFullPath());
		}

		//Initialize systems
		m_SystemManager->Init();

		//load collision masks...move this else where
		ResourceHandle res("collision_settings.xml");
		try
		{
			GeometryFlagManager::LoadGeometryFlagsFile(res.GetResource()->Path().GetFullPath());
		}
		catch (...)
		{

		}
		//Load templates
		ReloadTemplates();

		//initialize profiler
		ProfileSample::m_OutputHandler = new ProfileRuntimeHandler();
		ProfileSample::ResetAll();
		GASS_LOG(LINFO) << "SimEngine Initialization Completed";
	}

	void SimEngine::ReloadTemplates()
	{
		//load all templates
		ResourceGroupVector groups = m_ResourceManager->GetResourceGroups();

		//remove previous templates
		GetSceneObjectTemplateManager()->Clear();
		for (size_t i = 0; i < groups.size(); i++)
		{
			ResourceLocationVector locations = groups[i]->GetResourceLocations();
			for (size_t j = 0; j < locations.size(); j++)
			{
				ResourceLocation::ResourceMap resources = locations[j]->GetResources();
				FileResourcePtr res_ptr;
				ResourceLocation::ResourceMap::const_iterator iter = resources.begin();
				while (iter != resources.end())
				{
					res_ptr = iter->second;
					const FilePath file_path = res_ptr->Path();
					if (StringUtils::ToLower(file_path.GetExtension()) == "template")
					{
						GASS_LOG(LINFO) << "Start loading template:" << file_path.GetFullPath();
						GetSceneObjectTemplateManager()->Load(file_path.GetFullPath());
					}
					++iter;
				}
			}
		}
	}

	void SimEngine::LoadSettings(const FilePath &configuration_file)
	{
		GASS_LOG(LINFO) << "Start loading SimEngine settings from " << configuration_file;
		tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
		if (xmlDoc->LoadFile(configuration_file.GetFullPath().c_str()) != tinyxml2::XML_NO_ERROR)
		{
			delete xmlDoc;
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE, "Couldn't load:" + configuration_file.GetFullPath(), "SimEngine::LoadSettings");
		}

		tinyxml2::XMLElement *xml_settings = xmlDoc->FirstChildElement("GASS");
		if (!xml_settings)
		{
			delete xmlDoc;
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE, "Failed to find GASS tag in:" + configuration_file.GetFullPath(), "SimEngine::LoadSettings");
		}

		tinyxml2::XMLElement *xml_data_path = xml_settings->FirstChildElement("SetDataPath");
		if (xml_data_path)
		{
			const std::string data_path = XMLUtils::ReadString(dynamic_cast<tinyxml2::XMLElement *>(xml_settings), "SetDataPath");
			SetDataPath(data_path);
		}
		m_ScenePath.SetPath("%GASS_DATA_HOME%/sceneries/");

		tinyxml2::XMLElement *xml_scene_path = xml_settings->FirstChildElement("ScenePath");
		if (xml_scene_path)
			m_ScenePath.SetPath(XMLUtils::ReadString(dynamic_cast<tinyxml2::XMLElement *>(xml_settings), "ScenePath"));

		//read SceneObjectTemplateManager settings
		tinyxml2::XMLElement *xml_sotm = xml_settings->FirstChildElement("SceneObjectTemplateManager");
		if (xml_sotm)
		{
			bool add_object_id = XMLUtils::ReadBool(xml_sotm, "AddObjectIDToName");
			GetSceneObjectTemplateManager()->SetAddObjectIDToName(add_object_id);

			std::string prefix = XMLUtils::ReadString(xml_sotm, "ObjectIDPrefix");
			GetSceneObjectTemplateManager()->SetObjectIDPrefix(prefix);

			std::string sufix = XMLUtils::ReadString(xml_sotm, "ObjectIDSufix");
			GetSceneObjectTemplateManager()->SetObjectIDSuffix(sufix);
		}
		delete xmlDoc;
	}

	void SimEngine::PutEnv(const std::string &value)
	{
#ifdef WIN32
		int ret = _putenv(value.c_str());
		if (ret == -1)
			GASS_EXCEPT(Exception::ERR_INVALID_STATE, "Failed to set env var:" + value, "SimEngine::PutEnv");
#else
		char * writable = new char[value.size() + 1];
		std::copy(value.begin(), value.end(), writable);
		writable[value.size()] = '\0'; // don't forget the terminating 0
		putenv(writable);
		//delete[] writable;
#endif

	}

	void SimEngine::SetDataPath(const FilePath &data_path)
	{
		m_DataPath = data_path;
		if (data_path.Exist())
		{
			const std::string env_str = "GASS_DATA_HOME=" + data_path.GetFullPath();
			PutEnv(env_str);
		}
	}

	FilePath SimEngine::GetDataPath() const
	{
		return m_DataPath;
	}

	void SimEngine::LoadResources(const FilePath &configuration_file)
	{
		GASS_LOG(LINFO) << "Start loading SimEngine settings from " << configuration_file;
		tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
		if (xmlDoc->LoadFile(configuration_file.GetFullPath().c_str()) != tinyxml2::XML_NO_ERROR)
		{
			delete xmlDoc;
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE, "Couldn't load:" + configuration_file.GetFullPath(), "SimEngine::LoadSettings");
		}

		tinyxml2::XMLElement *xml_settings = xmlDoc->FirstChildElement("GASS");
		if (!xml_settings)
		{
			delete xmlDoc;
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE, "Failed to find GASS tag in:" + configuration_file.GetFullPath(), "SimEngine::LoadSettings");
		}

		tinyxml2::XMLElement *xml_res_man = xml_settings->FirstChildElement("ResourceManager");
		if (xml_res_man)
			m_ResourceManager->LoadXML(xml_res_man);

		delete xmlDoc;
	}

	bool SimEngine::Update()
	{
		static GASS::Timer  timer;
		static double prev_time = 0;
		double current_time = timer.GetTime();
		double delta_time = current_time - prev_time;

		//clamp delta time
		delta_time = std::max<double>(delta_time, 0);
		delta_time = std::min<double>(delta_time, 10.0);

		//just tick engine with delta time
		Tick(delta_time);
		prev_time = current_time;
		return true;
	}

	void SimEngine::DebugPrint(const std::string &message)
	{
		GetSimSystemManager()->GetFirstSystemByClass<IGraphicsSystem>()->PrintDebugText(message);
	}
	
	void SimEngine::Tick(double delta_time)
	{
		//ProfileSample::ResetAll();
		{
			PROFILE("SimEngine::Update")

			GetSimSystemManager()->OnUpdate(delta_time);
			m_CurrentTime += delta_time;
			//GASS_PRINT(" TICK:" << delta_time << " Time:" << m_CurrentTime);
		}
#ifdef PROFILER
		ProfileSample::Output();
#endif
	}

	void SimEngine::DestroyScene(SceneWeakPtr scene)
	{
		ScenePtr the_scene = scene.lock();
		if (!the_scene)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Scene not valid", "SimEngine::DestroyScene");
		the_scene->OnUnload();
		SceneVector::iterator iter = m_Scenes.begin();
		while (iter != m_Scenes.end())
		{
			if (the_scene == *iter)
			{
				m_Scenes.erase(iter);
				return;
			}
			++iter;
		}
		GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed find scene", "SimEngine::DestroyScene");
	}

	SceneWeakPtr SimEngine::CreateScene(const std::string &name)
	{
		ScenePtr scene(new Scene(name));
		scene->OnCreate();
		m_Scenes.push_back(scene);
		return scene;
	}

	std::vector<std::string> SimEngine::GetSavedScenes() const
	{
		std::vector<std::string> scene_names;
		if (m_ScenePath.Exist())
		{
			std::vector<FilePath> folders;
			FilePath::GetFoldersFromPath(folders, m_ScenePath, false);
			for (size_t i = 0; i < folders.size(); ++i)
			{
				if (FileUtils::FileExist(folders[i].GetFullPath() + "scene.xml"))
				{
					std::cout << folders[i] << "\n";
					std::string scene_name = folders[i].GetLastFolder();
					scene_names.push_back(scene_name);
				}
			}
		}
		return scene_names;
	}

	SceneObjectPtr SimEngine::CreateObjectFromTemplate(const std::string &template_name) const
	{
		ComponentContainerPtr cc = m_SceneObjectTemplateManager->CreateFromTemplate(template_name);
		SceneObjectPtr so = GASS_STATIC_PTR_CAST<SceneObject>(cc);
		if (!so)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to create object from template:" + template_name, "SimEngine::CreateObjectFromTemplate");
		so->GenerateGUID(true);
		so->ResolveTemplateReferences(so);
		return so;
	}

	bool SimEngine::Shutdown()
	{
		return true;
	}

	void SimEngine::SyncMessages(double delta_time)
	{
		m_SystemManager->SyncMessages(delta_time);
		for (size_t i = 0; i < m_Scenes.size(); i++)
		{
			m_Scenes[i]->SyncMessages(delta_time);
		}
	}

	SimEngine::SceneIterator SimEngine::GetScenes()
	{
		return SceneIterator(m_Scenes.begin(), m_Scenes.end());
	}

	SimEngine::ConstSceneIterator SimEngine::GetScenes() const
	{
		return ConstSceneIterator(m_Scenes.begin(), m_Scenes.end());
	}

	double SimEngine::GetTime() const
	{
		return m_SystemManager->GetTime();
	}

	double SimEngine::GetSimulationTime() const
	{
		return m_SystemManager->GetSimulationTime();
	}
}
