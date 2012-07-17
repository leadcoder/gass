// CoreTest.cpp : Defines the entry point for the console application.
//

#ifndef SIM_APPLICATION_H
#define SIM_APPLICATION_H
#include <boost/bind.hpp>

#include "Sim/GASS.h"
#include "Plugins/Game/GameMessages.h"
#include "tinyxml.h"

#include <stdio.h>
#include <iostream>
#include <fstream>

class SimApplication : public GASS::IMessageListener, public boost::enable_shared_from_this<SimApplication> 
{
protected:
	std::string m_SystemConfig;
	std::string m_ControlSettings;
	std::string m_Plugins;
	std::string m_SceneName;
	std::string m_Instances;
	std::vector<std::string> m_Templates;
	std::vector<std::string> m_Objects;
	GASS::SimEngine* m_Engine;
	GASS::SceneWeakPtr m_Scene;
	GASS::Timer* m_Timer;
	double m_UpdateFreq;
public:
	SimApplication(const std::string configuration)
	{
		LoadConfig(configuration);
		m_Timer =  new GASS::Timer();

	}
	virtual ~SimApplication()
	{
		delete m_Engine;
	}

	virtual bool Init()
	{
		m_Engine = new GASS::SimEngine();
		m_Engine->Init(m_Plugins,m_SystemConfig,m_ControlSettings);

		GASS::GraphicsSystemPtr gfx_sys = m_Engine->GetSimSystemManager()->GetFirstSystem<GASS::IGraphicsSystem>();
		gfx_sys->CreateViewport("MainViewport", "MainWindow", 0,0,1, 1);

		//GASS::ScenePtr scene (new GASS::Scene());
		//scene->Create();
		for(int i = 0; i <  m_Templates.size();i++)
		{
			m_Engine->GetSceneObjectTemplateManager()->Load(m_Templates[i]);
		}

		//m_Scene = scene;
		//scene->Create();
		GASS::LogManager::getSingleton().stream() << "SimApplication::Init -- Start Loading Scene:" <<  m_SceneName;


		m_Scene = m_Engine->LoadScene(m_SceneName);
		GASS::ScenePtr scene = GASS::ScenePtr(m_Scene);
		//m_Scene->Load(m_SceneName);

		GASS::LogManager::getSingleton().stream() << "SimApplication::Init -- Scene Loaded:" << m_SceneName;
		//create free camera and set start pos
		GASS::SceneObjectPtr free_obj = scene->LoadObjectFromTemplate("FreeCameraObject",scene->GetRootSceneObject());
		GASS::MessagePtr pos_msg(new GASS::PositionMessage(scene->GetStartPos()));
		if(free_obj)
		{
			free_obj->SendImmediate(pos_msg);
			GASS::MessagePtr camera_msg(new GASS::ChangeCameraMessage(free_obj,"ALL"));
			scene->PostMessage(camera_msg);
		}

		for(int i = 0; i <  m_Objects.size();i++)
		{
			GASS::SceneObjectPtr object  = scene->LoadObjectFromTemplate(m_Objects[i],scene->GetRootSceneObject());
			GASS::Vec3 pos = scene->GetStartPos();
			pos.x += 10*i;

			GASS::MessagePtr pos_msg(new GASS::WorldPositionMessage(pos));
			if(object)
				object->SendImmediate(pos_msg);
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
			GASS::LogManager::getSingleton().stream() << "WARNING: SimApplication::LoadConfig() - Couldn't load xmlfile: " << filename;
			return 0;
		}
		TiXmlElement *app_settings = xmlDoc->FirstChildElement("SimApplication");

		if(app_settings)
		{
			m_SystemConfig = app_settings->Attribute("SystemConfig");
			m_Plugins = app_settings->Attribute("Plugins");
			m_ControlSettings = app_settings->Attribute("ControlSettings");
			m_SceneName = app_settings->Attribute("Scene");

			GASS::FilePath full_path(m_SceneName);
			m_SceneName = full_path.GetFullPath();

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
					name = full_path.GetFullPath();

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
		if(prev > 0)
			m_Engine->Update(time - prev);
		prev = time;

		//double update_time = 1.0/m_UpdateFreq;
		/*if(time - prev > update_time)
		{
			m_Engine->Update(update_time);//time - prev);
			//m_Scene->SyncMessages(update_time);//time - prev);
			prev = time;
			Sleep(0);
		}*/
		//else 
		//	Sleep(30);


		//		if(GetAsyncKeyState(VK_ESCAPE))
		//			return false;
		return true;
	}

};
#endif




