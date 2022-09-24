// CoreTest.cpp : Defines the entry point for the console application.
//

#ifndef SIM_APPLICATION_H
#define SIM_APPLICATION_H


#include "Sim/GASS.h"
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Core/Serialize/tinyxml2.h"


#include <stdio.h>


class SimApplication : public GASS::IMessageListener, public GASS_ENABLE_SHARED_FROM_THIS<SimApplication>
{
protected:
	std::string m_SceneName;
	GASS::SimEngine* m_Engine;
	GASS::SceneWeakPtr m_Scene;
	GASS::Timer* m_Timer;
	double m_UpdateFreq;
	int m_ServerPort;
	int m_ClientPort;
public:
	SimApplication() :
	  m_SceneName("%GASS_DATA_HOME%/sceneries/osg_demo.scene"),
		  m_Timer(new GASS::Timer()),
		  m_UpdateFreq(60),
		  m_ServerPort(2005),
		  m_ClientPort(2006),
		  m_Engine(NULL)
	  {

	  }
	  virtual ~SimApplication()
	  {
		  delete m_Engine;
	  }

	  void _LoadScene(const std::string &scene_name)
	  {
		  m_Scene = GASS::SimEngine::Get().CreateScene(scene_name);
		  GASS::ScenePtr scene = GASS::ScenePtr(m_Scene);
		  scene->Load(GASS::FilePath(scene_name));
		  GASS_LOG(LINFO) << "SimApplication::Init -- Scene Loaded:" << m_SceneName;
		  scene->GetOrCreateCamera();
	  }

	  //Helper function to create main window
	  void _CreateMainWindow()
	  {
		  m_Engine = new GASS::SimEngine();
		  m_Engine->Init(GASS::FilePath("SampleSimNetwork.xml"));

		  GASS::GraphicsSystemPtr gfx_sys = m_Engine->GetSimSystemManager()->GetFirstSystemByClass<GASS::IGraphicsSystem>();

		  GASS::RenderWindowPtr win = gfx_sys->CreateRenderWindow("MainWindow",800,600);
		  win->CreateViewport("MainViewport", 0, 0, 1, 1);

		  GASS::InputSystemPtr input_system = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::IInputSystem>();
		  input_system->SetMainWindowHandle(win->GetHWND());
	  }

	  virtual bool Init()
	  {
		  _CreateMainWindow();
		  
		  GASS_LOG(LINFO) << "SimApplication::Init -- Start Loading Scene:" << m_SceneName;
		  m_Scene = m_Engine->CreateScene();
		  auto scene = m_Scene.lock();
		  scene->Load(GASS::FilePath(m_SceneName));
		  GASS_LOG(LINFO) << "SimApplication::Init -- Scene Loaded:" << m_SceneName;
		  scene->GetOrCreateCamera();
		
		  GASS::SceneObjectPtr object  = scene->LoadObjectFromTemplate("JimTank",scene->GetRootSceneObject());
		  
		  if(object)
			  object->GetFirstComponentByClass<GASS::ILocationComponent>()->SetWorldPosition(scene->GetStartPos());
		  return true;
	  }

	  virtual bool Update()
	  {
		  m_Engine->Update();
		  return true;
	  }
};
#endif




