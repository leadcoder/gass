// CoreTest.cpp : Defines the entry point for the console application.
//

#ifndef SIM_APPLICATION_H
#define SIM_APPLICATION_H
#include <boost/bind.hpp>

#include "Core/ComponentSystem/IComponent.h"
#include "Core/Math/Vector.h"
#include "Core/Utils/Factory.h"
#include "Core/Utils/Log.h"
#include "Core/Serialize/Serialize.h"
#include "Core/Utils/Timer.h"
#include "Core/Utils/Log.h"
#include "Core/ComponentSystem/BaseComponentContainer.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"

#include "Core/ComponentSystem/ComponentContainerFactory.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/BaseComponent.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/BaseMessage.h"
#include "Sim/Systems/SimSystemManager.h"

#include "Core/PluginSystem/PluginManager.h"
#include "Core/Utils/Log.h"

#include "Sim/SimEngine.h"
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Components/Graphics/ICameraComponent.h"

#include "Sim/Systems/Collision/ICollisionSystem.h"
#include "Sim/Components/Graphics/Geometry/ILineComponent.h"

#include "Sim/Systems/Input/IInputSystem.h"
#include "Plugins/Game/GameMessages.h"

#include "tinyxml.h"

#include <stdio.h>
#include <iostream>
#include <fstream>

class SimApplication
{
protected:
	std::string m_SystemConfig;
	std::string m_ControlSettings;
	std::string m_Plugins;
	std::string m_ScenarioName;
	std::string m_Instances;
	//std::string m_DataPath;
	std::vector<std::string> m_Templates;
	std::vector<std::string> m_Objects;
	GASS::SimEngine* m_Engine;
	GASS::ScenarioPtr m_Scenario;
	GASS::Timer* m_Timer;
	double m_UpdateFreq;
public:
	SimApplication(const std::string configuration)
	{
		LoadConfig(configuration);
		//Init();
		m_Timer =  new GASS::Timer();

	}
	virtual ~SimApplication()
	{
		m_Scenario.reset();
		delete m_Engine;
	}

	virtual bool Init()
	{
		m_Engine = new GASS::SimEngine();
		m_Engine->Init(m_Plugins,m_SystemConfig,m_ControlSettings);
		GASS::ScenarioPtr scenario (new GASS::Scenario());

		for(int i = 0; i <  m_Templates.size();i++)
		{
			m_Engine->GetSimObjectManager()->Load(m_Templates[i]);
		}

		m_Scenario = scenario;
		GASS::Log::Print("SimApplication::Init -- Start Loading Scenario: %s", m_ScenarioName.c_str());


		if(m_Scenario->Load(m_ScenarioName))
		{
			m_Scenario->Save("c:/temp/scenario_test");
			//if(m_Instances != "")
			//	scenario->GetScenarioScenes().at(0)->GetObjectManager()->LoadFromFile(m_Instances);

            GASS::Log::Print("SimApplication::Init -- Scenario Loaded:%s", m_ScenarioName.c_str());

			//only populate first scene
			GASS::ScenarioScenePtr scene = m_Scenario->GetScenarioScenes().getNext();

			for(int i = 0; i <  m_Objects.size();i++)
			{

				GASS::SceneObjectPtr object = scene->GetObjectManager()->LoadFromTemplate(m_Objects[i]);

				GASS::Vec3 pos = scene->GetStartPos();
				pos.x += 10*i;
				GASS::MessagePtr pos_msg(new GASS::PositionMessage(pos));
				if(object)
					object->SendImmediate(pos_msg);
			}
		}
		else
		{
			GASS::Log::Error("Failed to load scenario %s", m_ScenarioName.c_str());
			return false;
		}
		m_Timer->Reset();

		return true;
	}


	virtual bool LoadConfig(const std::string filename)
	{
		if(filename =="") return false;
		TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());
		if (!xmlDoc->LoadFile())
		{
			// Fatal error, cannot load
			GASS::Log::Warning("SimApplication::LoadConfig() - Couldn't load xmlfile: %s", filename.c_str());
			return 0;
		}
		TiXmlElement *app_settings = xmlDoc->FirstChildElement("SimApplication");

		if(app_settings)
		{
			m_SystemConfig = app_settings->Attribute("SystemConfig");
			m_Plugins = app_settings->Attribute("Plugins");
			m_ControlSettings = app_settings->Attribute("ControlSettings");
			m_ScenarioName = app_settings->Attribute("Scenario");

			GASS::FilePath full_path(m_ScenarioName);
			m_ScenarioName = full_path.GetPath();

			m_UpdateFreq = atoi(app_settings->Attribute("UpdateFrequency"));


			//m_Instances = app_settings->Attribute("Instances");
			TiXmlElement *temp_elem = app_settings->FirstChildElement("Templates");
			if(temp_elem)
			{
				temp_elem = temp_elem ->FirstChildElement("Template");
				while(temp_elem)
				{
					std::string name = temp_elem->Attribute("Name");
					GASS::FilePath full_path(name);
					name = full_path.GetPath();

					m_Templates.push_back(name);
					temp_elem = temp_elem->NextSiblingElement();
				}
			}


			TiXmlElement *object_elem = app_settings->FirstChildElement("Objects");
			if(object_elem)
			{
				object_elem = object_elem->FirstChildElement("Object");
				while(object_elem)
				{
					std::string name = object_elem->Attribute("Name");
					m_Objects.push_back(name);
					object_elem = object_elem->NextSiblingElement();
				}
			}
		}
		return true;
		//TiXmlElement *sys_config =  app_settings->FirstChildElement("SystemConfig();
	}

	virtual bool Update()
	{
		static double prev = -1;
		double time = m_Timer->GetTime();
		double update_time = 1.0/m_UpdateFreq;
		if(time - prev > update_time)
		{
			m_Engine->Update(time - prev);
			m_Scenario->OnUpdate(time - prev);
			prev = time;
		}




//		if(GetAsyncKeyState(VK_ESCAPE))
//			return false;

		return true;
	}

};
#endif




