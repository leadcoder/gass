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
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
//include to ensure interface export
#include "Sim/Interface/GASSIGraphicsSceneManager.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/Interface/GASSILightComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSIShape.h"
#include "Sim/Interface/GASSIPhysicsGeometryComponent.h"
#include "Sim/Interface/GASSINetworkComponent.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"
#include "Sim/Interface/GASSIPhysicsSystem.h"
#include "Sim/Interface/GASSICollisionSystem.h"
#include "Sim/Interface/GASSICollisionSceneManager.h"
#include "Sim/Interface/GASSIInputSystem.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSIResourceComponent.h"
#include "Sim/Interface/GASSIRenderWindow.h"
#include "Sim/Interface/GASSIViewport.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSTaskNode.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSRunTimeController.h"
#include "Sim/GASSResourceManager.h"
#include "Sim/GASSResourceGroup.h"
#include "Sim/GASSResourceLocation.h"
#include "Sim/GASSResource.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include <stdlib.h>
#include <stdio.h>
#include <tinyxml.h>


namespace GASS
{
	SimEngine::SimEngine() : m_CurrentTime(0), 
		m_MaxUpdateFreq(0), 
		m_PluginManager(new PluginManager()),
		m_ResourceManager(new ResourceManager()),
		m_SystemManager(new SimSystemManager()),
		m_SceneObjectTemplateManager(new BaseComponentContainerTemplateManager()),
		m_RTC(new RunTimeController()),
		m_LogFolder("")
	{
		
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

	void SimEngine::Init(const FilePath &configuration, const FilePath &log_folder)
	{
		m_LogFolder = log_folder;
		// Create log manager
		if(LogManager::getSingletonPtr() == 0)
		{
			LogManager* log_man = new LogManager();

			const std::string log_file = log_folder.GetFullPath() + "GASS.log";
			log_man->createLog(log_file, true, true);
		}
	    LogManager::getSingleton().stream() << "SimEngine Initialization Started";
		m_PluginManager->LoadFromFile(configuration.GetFullPath());
		
		LoadSettings(configuration);
		
		//Initialize systems
		m_SystemManager->Init();

		//load all templates
		ResourceGroupVector groups = m_ResourceManager->GetResourceGroups();
		for(size_t i = 0; i < groups.size(); i++)
		{
			ResourceLocationVector locations = groups[i]->GetResourceLocations();
			for(size_t j = 0; j < locations.size(); j++)
			{
				ResourceLocation::ResourceMap resources = locations[j]->GetResources();
				FileResourcePtr res;
				ResourceLocation::ResourceMap::const_iterator iter = resources.begin();
				while(iter != resources.end())
				{
					res = iter->second;
					const FilePath file_path = res->Path();
					if(Misc::ToLower(file_path.GetExtension()) == "template")
					{
						LogManager::getSingleton().stream() << "Start loading template:" << file_path.GetFullPath();
						GetSceneObjectTemplateManager()->Load(file_path.GetFullPath());
					}
					iter++;
				}
			}
		}

		//Create scene object		
		//m_Scene->Create();

		//intilize profiler
		ProfileSample::m_OutputHandler = new ProfileRuntimeHandler();
		ProfileSample::ResetAll();

		LogManager::getSingleton().stream() << "SimEngine Initialization Completed";
	}

	void SimEngine::LoadSettings(const FilePath &configuration_file)
	{
		LogManager::getSingleton().stream() << "Start loading SimEngine settings from " << configuration_file;
		TiXmlDocument *xmlDoc = new TiXmlDocument(configuration_file.GetFullPath().c_str());
		if (!xmlDoc->LoadFile())
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load:" + configuration_file.GetFullPath(), "SimEngine::LoadSettings");
		
		//add top tag!
		TiXmlDocument *xml_settings = xmlDoc;

		TiXmlElement *xml_data_path = xml_settings->FirstChildElement("SetDataPath");
		if(xml_data_path)
		{
			std::string env_data_path = Misc::ReadString((TiXmlElement *)xml_settings,"SetDataPath");
			if(boost::filesystem::exists(boost::filesystem::path(env_data_path)))
			{
				env_data_path = "GASS_DATA_HOME=" + env_data_path;
				_putenv(env_data_path.c_str()); 
			}
		}
		m_ScenePath.SetPath("%GASS_DATA_HOME%/sceneries/");

		TiXmlElement *xml_scene_path = xml_settings->FirstChildElement("ScenePath");
		if(xml_scene_path)
			m_ScenePath.SetPath(Misc::ReadString((TiXmlElement *)xml_settings,"ScenePath"));


		TiXmlElement *xml_res_man= xml_settings->FirstChildElement("ResourceManager");
		if(xml_res_man)
			m_ResourceManager->LoadXML(xml_res_man);

		m_SystemManager->Load(configuration_file.GetFullPath());
	
		//read SceneObjectTemplateManager settings
		TiXmlElement *xml_sotm = xml_settings->FirstChildElement("SceneObjectTemplateManager");
		if(xml_sotm)
		{
			bool add_object_id = Misc::ReadBool(xml_sotm,"AddObjectIDToName");
			GetSceneObjectTemplateManager()->SetAddObjectIDToName(add_object_id);

			std::string prefix = Misc::ReadString(xml_sotm,"ObjectIDPrefix");
			GetSceneObjectTemplateManager()->SetObjectIDPrefix(prefix);
			
			std::string sufix = Misc::ReadString(xml_sotm,"ObjectIDSufix");
			GetSceneObjectTemplateManager()->SetObjectIDSuffix(sufix);

			/*TiXmlElement *xml_load = xml_sotm->FirstChildElement("Load");
			if(xml_load)
			{
				TiXmlElement *xml_temp = xml_load->FirstChildElement("Template");
				while(xml_temp)
				{
					std::string file_path = xml_temp->Attribute("value");
					GASS::FilePath fp;
					fp.SetPath(file_path);
					file_path = fp.GetFullPath();
					if(Misc::GetExtension(file_path) == "xml")
						GetSceneObjectTemplateManager()->Load(file_path);
					else
						GetSceneObjectTemplateManager()->LoadFromPath(file_path);

					xml_temp = xml_temp->NextSiblingElement("Template");
				}
			}*/
		}

		TiXmlElement *xml_rtc = xml_settings->FirstChildElement("RTC");
		if(xml_rtc)
		{
			int num_threads = Misc::ReadInt(xml_rtc,"NumberOfThreads");
			m_RTC->Init(num_threads);
			int update_freq = Misc::ReadInt(xml_rtc,"MaxUpdateFreqency");
			m_MaxUpdateFreq = static_cast<double>(update_freq);
			//bool external_update = Misc::ReadBool(xml_rtc,"ExternalSimulationUpdate");
			//GetSimSystemManager()->SetPauseSimulation(external_update);

			TiXmlElement *xml_tn = xml_rtc->FirstChildElement("TaskNode");
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
				if(delta_time > 1.0/600.0)
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

		//GetSimSystemManager()->Update(delta_time);
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
