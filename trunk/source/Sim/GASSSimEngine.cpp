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

#include "Sim/GASSSimEngine.h"
#include "Sim/GASSRunTimeController.h"
#include "Sim/Utils/GASSProfiler.h"
#include "Sim/Utils/GASSProfileRuntimeHandler.h"
#include "Core/PluginSystem/GASSPluginManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/System/GASSISystemManager.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplateManager.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSXMLUtils.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSTaskNode.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSRunTimeController.h"
#include "Sim/GASSResourceManager.h"
#include "Sim/GASSResourceGroup.h"
#include "Sim/GASSResourceLocation.h"
#include "Sim/GASSResource.h"
#include "Sim/GASSScriptManager.h"
#include "Sim/GASSResourceHandle.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/GASSGeometryFlags.h"
#include <stdlib.h>
#include <stdio.h>
#include <tinyxml2.h>


namespace GASS
{
	SimEngine::SimEngine(const FilePath &log_folder) : m_CurrentTime(0),
		m_MaxUpdateFreq(0),
		m_PluginManager(new PluginManager()),
		m_ScriptManager(new ScriptManager()),
		m_ResourceManager(new ResourceManager()),
		m_SystemManager(new SimSystemManager()),
		m_SceneObjectTemplateManager(new ComponentContainerTemplateManager()),
		m_RTC(new RunTimeController()),
		m_LogFolder(log_folder)
	{
		// Create log manager
		if(LogManager::getSingletonPtr() == 0)
		{
			LogManager* log_man = new LogManager();
			const std::string log_file = log_folder.GetFullPath() + "GASS.log";
			log_man->createLog(log_file, true, true);
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
		LogManager::getSingleton().stream() << "SimEngine Initialization Started";
		/*m_LogFolder = log_folder;
		// Create log manager
		if(LogManager::getSingletonPtr() == 0)
		{
			LogManager* log_man = new LogManager();

			const std::string log_file = log_folder.GetFullPath() + "GASS.log";
			log_man->createLog(log_file, true, true);
		}
	    LogManager::getSingleton().stream() << "SimEngine Initialization Started";*/
		//m_ScriptManager->Init();
		if(configuration.GetFullPath() != "")
			m_PluginManager->LoadFromFile(configuration.GetFullPath());

		if(configuration.GetFullPath() != "")
			LoadSettings(configuration);
		//Initialize systems
		m_SystemManager->Init();

		//load collision masks
		ResourceHandle res("collision_settings.xml");
		try
		{
			GeometryFlagManager::LoadGeometryFlagsFile(res.GetResource()->Path().GetFullPath());
		}
		catch(...)
		{

		}
		//Load templates
		ReloadTemplates();

		//intialize profiler
		ProfileSample::m_OutputHandler = new ProfileRuntimeHandler();
		ProfileSample::ResetAll();

		//Catch script events


		LogManager::getSingleton().stream() << "SimEngine Initialization Completed";
	}


	void SimEngine::ReloadTemplates()
	{
		//load all templates
		ResourceGroupVector groups = m_ResourceManager->GetResourceGroups();

		//remove previous templates
		GetSceneObjectTemplateManager()->Clear();
		for(size_t i = 0; i < groups.size(); i++)
		{
			ResourceLocationVector locations = groups[i]->GetResourceLocations();
			for(size_t j = 0; j < locations.size(); j++)
			{
				ResourceLocation::ResourceMap resources = locations[j]->GetResources();
				FileResourcePtr res_ptr;
				ResourceLocation::ResourceMap::const_iterator iter = resources.begin();
				while(iter != resources.end())
				{
					res_ptr = iter->second;
					const FilePath file_path = res_ptr->Path();
					if(StringUtils::ToLower(file_path.GetExtension()) == "template")
					{
						LogManager::getSingleton().stream() << "Start loading template:" << file_path.GetFullPath();
						GetSceneObjectTemplateManager()->Load(file_path.GetFullPath());
					}
					iter++;
				}
			}
		}
	}

	void SimEngine::LoadSettings(const FilePath &configuration_file)
	{
		LogManager::getSingleton().stream() << "Start loading SimEngine settings from " << configuration_file;
		tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
		if (xmlDoc->LoadFile(configuration_file.GetFullPath().c_str()) != tinyxml2::XML_NO_ERROR)
		{
			delete xmlDoc;
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load:" + configuration_file.GetFullPath(), "SimEngine::LoadSettings");
		}

		tinyxml2::XMLElement *xml_settings = xmlDoc->FirstChildElement("GASS");
		if (!xml_settings)
		{
			delete xmlDoc;
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Failed to find GASS tag in:" + configuration_file.GetFullPath(), "SimEngine::LoadSettings");
		}

		tinyxml2::XMLElement *xml_data_path = xml_settings->FirstChildElement("SetDataPath");
		if(xml_data_path)
		{
			std::string env_data_path = XMLUtils::ReadString((tinyxml2::XMLElement *)xml_settings,"SetDataPath");
			if(boost::filesystem::exists(boost::filesystem::path(env_data_path)))
			{
				env_data_path = "GASS_DATA_HOME=" + env_data_path;
#ifdef WIN32
				_putenv(env_data_path.c_str());
#else
                char * writable = new char[env_data_path.size() + 1];
                std::copy(env_data_path.begin(), env_data_path.end(), writable);
                writable[env_data_path.size()] = '\0'; // don't forget the terminating 0
                putenv(writable);
                delete[] writable;
#endif
			}
		}
		m_ScenePath.SetPath("%GASS_DATA_HOME%/sceneries/");

		tinyxml2::XMLElement *xml_scene_path = xml_settings->FirstChildElement("ScenePath");
		if(xml_scene_path)
			m_ScenePath.SetPath(XMLUtils::ReadString((tinyxml2::XMLElement *)xml_settings,"ScenePath"));


		tinyxml2::XMLElement *xml_res_man= xml_settings->FirstChildElement("ResourceManager");
		if(xml_res_man)
			m_ResourceManager->LoadXML(xml_res_man);

		m_SystemManager->Load(configuration_file.GetFullPath());

		//read SceneObjectTemplateManager settings
		tinyxml2::XMLElement *xml_sotm = xml_settings->FirstChildElement("SceneObjectTemplateManager");
		if(xml_sotm)
		{
			bool add_object_id = XMLUtils::ReadBool(xml_sotm,"AddObjectIDToName");
			GetSceneObjectTemplateManager()->SetAddObjectIDToName(add_object_id);

			std::string prefix = XMLUtils::ReadString(xml_sotm,"ObjectIDPrefix");
			GetSceneObjectTemplateManager()->SetObjectIDPrefix(prefix);

			std::string sufix = XMLUtils::ReadString(xml_sotm,"ObjectIDSufix");
			GetSceneObjectTemplateManager()->SetObjectIDSuffix(sufix);
		}

		tinyxml2::XMLElement *xml_rtc = xml_settings->FirstChildElement("RTC");
		if(xml_rtc)
		{
			int num_threads = XMLUtils::ReadInt(xml_rtc,"NumberOfThreads");
			m_RTC->Init(num_threads);
			int update_freq = XMLUtils::ReadInt(xml_rtc,"MaxUpdateFreqency");
			m_MaxUpdateFreq = static_cast<double>(update_freq);
			//bool external_update = XMLUtils::ReadBool(xml_rtc,"ExternalSimulationUpdate");
			//GetSimSystemManager()->SetPauseSimulation(external_update);

			tinyxml2::XMLElement *xml_tn = xml_rtc->FirstChildElement("TaskNode");
			if(xml_tn)
			{
				m_RTC->LoadXML(xml_tn);
			}
		}
		delete xmlDoc;
	}

	void SimEngine::Update()
	{
		static GASS::Timer  timer;
		static double prev_time = 0;
		double current_time = timer.GetTime();
		double delta_time = current_time - prev_time;

		//clamp delta time
		delta_time = std::max<double>(delta_time,0.00001);
		delta_time = std::min<double>(delta_time,10.0);

		if(m_MaxUpdateFreq > 0)
		{
			const double target_update_time = 1.0/m_MaxUpdateFreq;
			if(delta_time > target_update_time)
			{
				//if(delta_time > 1.0/600.0)
				{
					prev_time = current_time;
					Tick(delta_time);
				}
			}
			else
			{
				//return and give application more idle time
			}
		}
		else
		{
			//just tick engine with delta time
			Tick(delta_time);
			prev_time = current_time;
		}
	}

	void SimEngine::Tick(double delta_time)
	{
		//ProfileSample::ResetAll();
		{
		PROFILE("SimEngine::Update")
		//update systems

		m_RTC->Update(delta_time);

		m_CurrentTime += delta_time;
		}
#ifdef PROFILER
		ProfileSample::Output();
#endif
	}

	void SimEngine::DestroyScene(SceneWeakPtr scene)
	{
		ScenePtr the_scene = ScenePtr(scene,NO_THROW);
		if(!the_scene)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Scene no valid", "SimEngine::DestroyScene");
		the_scene->OnUnload();
		SceneVector::iterator iter = m_Scenes.begin();
		while(iter != m_Scenes.end())
		{
			if(the_scene == *iter)
			{
				m_Scenes.erase(iter);
				return;
			}
			iter++;
		}
		GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed find scene", "SimEngine::DestroyScene");
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
		boost::filesystem::path boost_path(m_ScenePath.GetFullPath());

		std::vector<std::string> scene_names;
		if(boost::filesystem::exists(boost_path))
		{
			boost::filesystem::directory_iterator end ;
			for( boost::filesystem::directory_iterator iter(boost_path) ; iter != end ; ++iter )
			{
				if (boost::filesystem::is_directory( *iter ) )
				{
					if(boost::filesystem::exists(boost::filesystem::path(iter->path().string() + "/scene.xml")))
					{
						std::string scene_name = iter->path().filename().generic_string();
						scene_names.push_back(scene_name);
					}
				}
			}
		}
		return scene_names;
	}

	SceneObjectPtr SimEngine::CreateObjectFromTemplate(const std::string &template_name) const
	{
		ComponentContainerPtr cc  = m_SceneObjectTemplateManager->CreateFromTemplate(template_name);
		SceneObjectPtr so = STATIC_PTR_CAST<SceneObject>(cc);
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
		for(size_t i = 0 ; i < m_Scenes.size(); i++)
		{
			m_Scenes[i]->SyncMessages(delta_time);
		}
	}

	SimEngine::SceneIterator SimEngine::GetScenes()
	{
		return SceneIterator(m_Scenes.begin(),m_Scenes.end());
	}

	SimEngine::ConstSceneIterator SimEngine::GetScenes() const
	{
		return ConstSceneIterator(m_Scenes.begin(),m_Scenes.end());
	}

}
