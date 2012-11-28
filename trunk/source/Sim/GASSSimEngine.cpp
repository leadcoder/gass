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
#include "Sim/Interface/GASSIInputSystem.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSTaskNode.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"

#include <tinyxml.h>

namespace GASS
{
	SimEngine::SimEngine(): m_CurrentTime(0), m_MaxUpdateFreq(0)
	{
		m_PluginManager = PluginManagerPtr(new PluginManager());
		m_SystemManager = SimSystemManagerPtr(new SimSystemManager());
		m_SceneObjectTemplateManager = BaseComponentContainerTemplateManagerPtr(new BaseComponentContainerTemplateManager());
		m_RTC = RunTimeControllerPtr(new RunTimeController());
		m_ScenePath.SetPath("%GASS_DATA_PATH%/sceneries/ogre/");
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
		// Create log manager
		if(LogManager::getSingletonPtr() == 0)
		{
			LogManager* log_man = new LogManager();
			log_man->createLog("GASS.log", true, true);
		}
	    LogManager::getSingleton().stream() << "SimEngine Initialization Started";
		m_PluginManager->LoadFromFile(configuration.GetFullPath());
		
		m_SystemManager->Load(configuration.GetFullPath());
		
		LoadSettings(configuration);
		
		//Initialize systems
		m_SystemManager->Init();

//		

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

		TiXmlElement *xml_scene_path = xml_settings->FirstChildElement("ScenePath");
		if(xml_scene_path)
			m_ScenePath.SetPath(Misc::ReadString((TiXmlElement *)xml_settings,"ScenePath"));

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

			TiXmlElement *xml_load = xml_sotm->FirstChildElement("Load");
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
			}
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
				if(delta_time > 1.0/60.0)
				{
					prev_time = current_time;
					Tick(1.0/60.0);
				}
			}
			else
			{
				//return and give application more idel time 
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

	SceneWeakPtr SimEngine::LoadScene(const std::string &name)
	{
		if(m_Scene)
			m_Scene->Unload();
		m_Scene.reset();
		m_Scene = ScenePtr(new Scene());
		m_Scene->Create();
		FilePath path(m_ScenePath.GetFullPath() + "/"  + name);
		m_Scene->Load(path);
		return m_Scene;
	}

	void SimEngine::SaveScene(const std::string &name)
	{
		FilePath path(m_ScenePath.GetFullPath() + "/"  +  name);

		boost::filesystem::path boost_path(path.GetFullPath()); 
		if(!boost::filesystem::exists(boost_path))  
		{
			//try
			boost::filesystem::create_directory(boost_path);
		}
		else if (!boost::filesystem::is_directory( boost_path) )
		{
			return;
		}

		if(m_Scene)
			m_Scene->Save(path);
	}

	SceneWeakPtr SimEngine::NewScene()
	{
		if(m_Scene)
			m_Scene->Unload();
		m_Scene.reset();
		m_Scene = ScenePtr(new Scene());
		m_Scene->Create();
		m_Scene->Load();
		return m_Scene;
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
						//m_Scenes.push_back(iter->path().string());
					}
				}
			}
		}
		return scene_names;
	}

	void SimEngine::UnloadScene()
	{
		//m_Scene.release();
	}

	SceneObjectPtr SimEngine::CreateObjectFromTemplate(const std::string &template_name) const
	{
		ComponentContainerPtr cc  = m_SceneObjectTemplateManager->CreateFromTemplate(template_name);
		SceneObjectPtr so = boost::shared_static_cast<SceneObject>(cc);
		return so;
	}

	bool SimEngine::Shutdown()
	{
		return true;
	}

/*	SimEngine::SceneIterator SimEngine::GetScenes()
	{
		return SceneIterator(m_Scenes.begin(),m_Scenes.end());
	}

	SimEngine::ConstSceneIterator SimEngine::GetScenes() const
	{
		return ConstSceneIterator(m_Scenes.begin(),m_Scenes.end());
	}*/

}
