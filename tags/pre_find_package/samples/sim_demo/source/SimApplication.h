// CoreTest.cpp : Defines the entry point for the console application.
//

#ifndef SIM_APPLICATION_H
#define SIM_APPLICATION_H


#include "Sim/GASS.h"
#include "Plugins/Game/GameMessages.h"
#include "tinyxml2.h"


#include <stdio.h>
#include <iostream>
#include <fstream>

class SimApplication : public GASS::IMessageListener, public boost::enable_shared_from_this<SimApplication> 
{
protected:
	std::string m_SceneName;
	GASS::SimEngine* m_Engine;
	GASS::SceneWeakPtr m_Scene;
	GASS::Timer* m_Timer;
	double m_UpdateFreq;
public:
	SimApplication() :
	  m_SceneName("new_terrain"),
		  m_Timer(new GASS::Timer()),
		  m_UpdateFreq(60)

	  {


	  }
	  virtual ~SimApplication()
	  {
		  delete m_Engine;
	  }

	  virtual bool Init()
	  {
		  m_Engine = new GASS::SimEngine();
		  m_Engine->Init(GASS::FilePath("GASSSimDemo.xml"));
		  
		  GASS::GraphicsSystemPtr gfx_sys = m_Engine->GetSimSystemManager()->GetFirstSystemByClass<GASS::IGraphicsSystem>();

		  GASS::RenderWindowPtr win = gfx_sys->CreateRenderWindow("MainWindow",800,600);
		  win->CreateViewport("MainViewport", 0, 0, 1, 1);

		  GASS::InputSystemPtr input_system = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::IInputSystem>();
		  input_system->SetMainWindowHandle(win->GetHWND());

		  GASS::LogManager::getSingleton().stream() << "SimApplication::Init -- Start Loading Scene:" <<  m_SceneName;
		  m_Scene = m_Engine->CreateScene("NewScene");
		  //m_Scene = GASS::SimEngine::Get().CreateScene("NewScene");
		  GASS::ScenePtr scene = GASS::ScenePtr(m_Scene);
		  scene->Load(m_SceneName);
		  //m_Scene->Load(m_SceneName);
		  GASS::LogManager::getSingleton().stream() << "SimApplication::Init -- Scene Loaded:" << m_SceneName;
		  //create free camera and set start pos
		  GASS::SceneObjectPtr free_obj = scene->LoadObjectFromTemplate("FreeCameraObject",scene->GetRootSceneObject());
		  GASS::PositionRequestPtr pos_msg(new GASS::PositionRequest(scene->GetStartPos()));
		  if(free_obj)
		  {
			  free_obj->SendImmediateRequest(pos_msg);

			  GASS::SystemMessagePtr camera_msg(new GASS::ChangeCameraRequest(free_obj->GetFirstComponentByClass<GASS::ICameraComponent>()));
			  m_Engine->GetSimSystemManager()->PostMessage(camera_msg);
		  }
		 

		  GASS::SceneObjectPtr object  = scene->LoadObjectFromTemplate("JimTank",scene->GetRootSceneObject());
		  GASS::Vec3 pos = scene->GetStartPos();
		  
		  if(object)
				object->SendImmediateRequest(GASS::WorldPositionRequestPtr(new GASS::WorldPositionRequest(pos)));
				
		  /*for(int i = 0; i <  m_Objects.size();i++)
		  {
		  GASS::SceneObjectPtr object  = scene->LoadObjectFromTemplate(m_Objects[i],scene->GetRootSceneObject());
		  GASS::Vec3 pos = scene->GetStartPos();
		  pos.z -= 10*(i+1);

		  GASS::MessagePtr pos_msg(new GASS::WorldPositionRequest(pos));
		  if(object)
		  object->SendImmediate(pos_msg);
		  }*/

		  m_Engine->GetSimSystemManager()->PostMessage(GASS::SystemMessagePtr(new GASS::GUIScriptRequest("GUI.xml")));
		  return true;
	  }


	  /*virtual bool LoadConfig(const std::string filename)
	  {
	  if(filename =="") return false;
	  tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
	  if (xmlDoc->LoadFile(filename.c_str()) != tinyxml2::XML_NO_ERROR)
	  {
	  // Fatal error, cannot load
	  GASS::LogManager::getSingleton().stream() << "WARNING: SimApplication::LoadConfig() - Couldn't load xmlfile: " << filename;
	  return 0;
	  }
	  tinyxml2::XMLElement *app_settings = xmlDoc->FirstChildElement("SimApplication");

	  if(app_settings)
	  {
	  m_ControlSettings = app_settings->Attribute("ControlSettings");
	  m_SceneName = app_settings->Attribute("Scene");
	  GASS::FilePath full_path(m_SceneName);
	  m_SceneName = full_path.GetFullPath();
	  tinyxml2::XMLElement *object_elem = app_settings->FirstChildElement("Objects");
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
	  //tinyxml2::XMLElement *sys_config =  app_settings->FirstChildElement("SystemConfig();
	  }*/

	  virtual bool Update()
	  {
		  m_Engine->Update();
		  return true;
	  }

};
#endif




