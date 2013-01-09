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
		m_Engine->Init(GASS::FilePath("../Configuration/GASS.xml"));

		//load keyboard config!
		GASS::ControlSettingsSystemPtr css = m_Engine->GetSimSystemManager()->GetFirstSystem<GASS::IControlSettingsSystem>();
		if(css)
		{
			css->Load("../Configuration/control_settings.xml");
		}


		//m_Engine->GetSimSystemManager()->SetPauseSimulation(false);
		
		GASS::GraphicsSystemPtr gfx_sys = m_Engine->GetSimSystemManager()->GetFirstSystem<GASS::IGraphicsSystem>();
		gfx_sys->CreateViewport("MainViewport", "MainWindow", 0,0,1, 1);

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
			GASS::SceneMessagePtr camera_msg(new GASS::ChangeCameraRequest(free_obj,"ALL"));
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
		m_Engine->GetSimSystemManager()->PostMessage(GASS::SystemMessagePtr(new GASS::GUIScriptRequest("GUI.xml")));
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
			m_ControlSettings = app_settings->Attribute("ControlSettings");
			m_SceneName = app_settings->Attribute("Scene");
			GASS::FilePath full_path(m_SceneName);
			m_SceneName = full_path.GetFullPath();
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
			m_Engine->Update();
		return true;
	}

};
#endif




