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
#include "Core/Utils/GASSLogger.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSXMLUtils.h"
#include "Core/Utils/GASSFileUtils.h"
#include "Core/Utils/GASSSystem.h"
#include "Sim/GASSSceneObjectTemplateManager.h"
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
	SimEngine::SimEngine(const FilePath &log_folder) : 
		m_PluginManager(new PluginManager()),
		m_ScriptManager(new ScriptManager()),
		m_ResourceManager(new ResourceManager()),
		m_SystemManager(new SimSystemManager()),
		m_SceneObjectTemplateManager(new SceneObjectTemplateManager()),
		m_LogFolder(log_folder)
	{
		m_Instance = this;
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

	SimEngine* SimEngine::m_Instance = nullptr;
	SimEngine* SimEngine::GetPtr()
	{
		//assert(m_Instance);
		if (m_Instance == nullptr)
			m_Instance = new SimEngine();
		return m_Instance;
	}
	SimEngine& SimEngine::Get()
	{
		//assert(m_Instance);
		if (m_Instance == nullptr)
			m_Instance = new SimEngine();
		return *m_Instance;
	}

	void SimEngine::Init(const FilePath &config_file)
	{
		SimEngineConfig config = SimEngineConfig::LoadFromfile(config_file);
		Init(config);
	}

	void SimEngine::Init(const SimEngineConfig &config)
	{
		GASS_LOG(LINFO) << "SimEngine Initialization Started";
		SetDataPath(FilePath(config.DataPath));
		SetScenePath(FilePath(config.ScenePath));
		//GetSceneObjectTemplateManager()->SetAddObjectIDToName(config.AddObjectIDToName);
		//GetSceneObjectTemplateManager()->SetObjectIDPrefix(config.ObjectIDPrefix);
		//GetSceneObjectTemplateManager()->SetObjectIDSuffix(config.ObjectIDSufix);
		
		m_PluginManager->LoadPlugins(config.Plugins);
		m_ResourceManager->Load(config.ResourceConfig);
		m_SystemManager->Load(config.SimSystemManager);
	
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

	void SimEngine::SetDataPath(const FilePath &data_path)
	{
		m_DataPath = data_path;
		if (data_path.Exist())
		{
			const std::string env_str = "GASS_DATA_HOME=" + data_path.GetFullPath();
			if(!System::SetEnvVar(env_str))
				GASS_EXCEPT(Exception::ERR_INVALID_STATE, "Failed to GASS_DATA_HOME env var:" + data_path.GetFullPath(), "SimEngine::SetDataPath");
		}
	}

	FilePath SimEngine::GetDataPath() const
	{
		return m_DataPath;
	}

	bool SimEngine::Update()
	{
		static GASS::Timer  timer;
		static double prev_time = 0;
		const double current_time = timer.GetTime();
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
		auto iter = m_Scenes.begin();
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
					//std::cout << folders[i] << "\n";
					std::string scene_name = folders[i].GetLastFolder();
					scene_names.push_back(scene_name);
				}
			}
		}
		return scene_names;
	}

	SceneObjectPtr SimEngine::CreateObjectFromTemplate(const std::string &template_name) const
	{
		SceneObjectPtr so = m_SceneObjectTemplateManager->CreateFromTemplate(template_name);
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
