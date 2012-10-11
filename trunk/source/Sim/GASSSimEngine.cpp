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
#include "Sim/Scheduling/GASSTBBRuntimeController.h"
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
#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Sim/Components/Graphics/GASSICameraComponent.h"
#include "Sim/Components/Graphics/GASSILightComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSITerrainComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSIMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSIShape.h"
#include "Sim/Components/Physics/GASSIPhysicsGeometryComponent.h"
#include "Sim/Components/Network/GASSINetworkComponent.h"
#include "Sim/Components/Common/GASSIWaypointListComponent.h"
#include "Sim/Systems/GASSIPhysicsSystem.h"
#include "Sim/Systems/Collision/GASSICollisionSystem.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Systems/Input/GASSIInputSystem.h"
#include "Sim/Systems/Input/GASSIControlSettingsSystem.h"
#include "Sim/Systems/Messages/GASSCoreSystemMessages.h"
#include "Sim/Scheduling/GASSTaskNode.h"

#include "Sim/Scene/GASSSceneObject.h"
#include <tinyxml.h>

namespace GASS
{
	SimEngine::SimEngine(): m_CurrentTime(0), m_MaxUpdateFreq(0)
	{
		m_PluginManager = PluginManagerPtr(new PluginManager());
		m_SystemManager = SimSystemManagerPtr(new SimSystemManager());
		m_SceneObjectTemplateManager = BaseComponentContainerTemplateManagerPtr(new BaseComponentContainerTemplateManager());
		m_RTC = RuntimeControllerPtr(new TBBRuntimeController());
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
			bool external_update = Misc::ReadBool(xml_rtc,"ExternalSimulationUpdate");
			GetSimSystemManager()->SetPauseSimulation(external_update);

			TiXmlElement *xml_tn = xml_rtc->FirstChildElement("TaskNode");
			if(xml_tn)
			{
				m_SimulationTaskNode = new TaskNode();
				m_SimulationTaskNode->LoadXML(xml_tn);
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
				prev_time = current_time;
				Tick(delta_time);
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
		GetSimSystemManager()->Update(delta_time);

		//test, update rtc
		m_SimulationTaskNode->Update(delta_time,NULL);

		m_CurrentTime += delta_time;
		}
#ifdef PROFILER
		ProfileSample::Output();
#endif
	}

	SceneWeakPtr SimEngine::LoadScene(const FilePath &path)
	{
		ScenePtr scene = ScenePtr(new Scene());
		scene->Create();
		scene->Load(path);
		m_Scenes.push_back(scene);
		return scene;
	}

	void SimEngine::UnloadScene(SceneWeakPtr scene)
	{

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

	SimEngine::SceneIterator SimEngine::GetScenes()
	{
		return SceneIterator(m_Scenes.begin(),m_Scenes.end());
	}

	SimEngine::ConstSceneIterator SimEngine::GetScenes() const
	{
		return ConstSceneIterator(m_Scenes.begin(),m_Scenes.end());
	}

}
