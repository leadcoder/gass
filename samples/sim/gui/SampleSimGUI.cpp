/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/										*
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


#include "Sim/GASS.h"
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Core/PluginSystem/GASSPluginManager.h"
#include "SampleDialog.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

#ifdef WIN32
#include <conio.h>
#endif

#ifndef WIN32
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int _getch( ) {
	struct termios oldt,
		newt;
	int            ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;

}
#endif


class MyMenu  : public GASS::IMessageListener, public GASS_ENABLE_SHARED_FROM_THIS<MyMenu>
{
	public:

	MyMenu()
	{

	}
	
	~MyMenu()
	{

	}
	
	void Init()
	{
		GASS::SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(MyMenu::OnGUILoaded, GASS::GUILoadedEvent ,0));
	}

	void OnGUILoaded(GASS::GUILoadedEventPtr message)
	{
		GASS::SampleDialog* dialog = new GASS::SampleDialog(NULL);
		
		dialog->setVisible(true);
	}
};

int run(int argc, char* argv[])
{

	std::cout << "Select render system, press [1] for Ogre , [2] for OSG";
	char key = static_cast<char>(_getch());
	std::string gfx_system_name = "OgreGraphicsSystem";
	std::string gui_gfx_system_name = "MyGUIOgreSystem";
	std::string gfx_plugin = "GASSPluginOgre";
	
	if(key == '1')
	{
		gfx_system_name = "OgreGraphicsSystem";
		gui_gfx_system_name = "MyGUIOgreSystem";
		gfx_plugin = "GASSPluginOgre";
	}
	else if(key == '2')
	{
		gfx_system_name = "OSGGraphicsSystem";
		gui_gfx_system_name = "MyGUIOSGSystem";
		gfx_plugin = "GASSPluginOSG";
	}

	GASS::SimEngine* m_Engine = new GASS::SimEngine();
	m_Engine->Init(GASS::FilePath("SampleSimGUI.xml"));

	//load gfx plugin
	m_Engine->GetPluginManager()->LoadPlugin(gfx_plugin);
	//Create systems
	GASS::SimSystemPtr gfx_system = GASS::SystemFactory::Get().Create(gfx_system_name);
	gfx_system->OnCreate(m_Engine->GetSimSystemManager());
	
	GASS::SimSystemPtr gui_system = GASS::SystemFactory::Get().Create(gui_gfx_system_name);
	gui_system->OnCreate(m_Engine->GetSimSystemManager());
	gui_system->Init();
	gui_system->RegisterForUpdate();
	m_Engine->GetSimSystemManager()->AddSystem(gui_system);
	
	gfx_system->Init();
	gfx_system->RegisterForUpdate();

	m_Engine->GetSimSystemManager()->AddSystem(gfx_system);
	//GASS::TaskNode2* node = m_Engine->GetRootTaskNode()->GetChildByID(gfx_system->GetUpdateGroup().GetValue());
	//node->Register(gfx_system);


	GASS_SHARED_PTR<MyMenu> menu (new MyMenu());
	menu->Init();

	//reload templates
	m_Engine->ReloadTemplates();

	GASS::GraphicsSystemPtr gfx_sys = m_Engine->GetSimSystemManager()->GetFirstSystemByClass<GASS::IGraphicsSystem>();
	GASS::RenderWindowPtr win = gfx_sys->CreateRenderWindow("MainWindow",800,600);
	win->CreateViewport("MainViewport", 0, 0, 1, 1);
	GASS::InputSystemPtr input_system = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::IInputSystem>();
	input_system->SetMainWindowHandle(win->GetHWND());
	GASS::ScenePtr scene(m_Engine->CreateScene("NewScene"));
	{
		GASS::SceneObjectTemplatePtr plane_template (new GASS::SceneObjectTemplate);
		plane_template->SetName("PlaneObject");
		plane_template->AddBaseSceneComponent("LocationComponent");
		plane_template->AddBaseSceneComponent("ManualMeshComponent");
		//plane_template->AddBaseSceneComponent("PhysicsPlaneGeometryComponent");
		GASS::ComponentPtr plane_comp  = plane_template->AddBaseSceneComponent("PlaneGeometryComponent");
		plane_comp->SetPropertyByType("Size",GASS::Vec2(100,100));
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(plane_template);
	}
	{
		GASS::SceneObjectTemplatePtr light_template (new GASS::SceneObjectTemplate);
		light_template->SetName("LightObject");
		light_template->AddBaseSceneComponent("LocationComponent");
		GASS::BaseSceneComponentPtr light_comp = light_template->AddBaseSceneComponent("LightComponent");
		light_comp->SetPropertyByString("DiffuseColor","0.5 0.5 0.5");
		//light_comp->SetPropertyByString("AmbientColor","0.5 0.5 0.5");
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(light_template);
	}

	GASS::SceneObjectPtr terrain_obj = scene->LoadObjectFromTemplate("PlaneObject",scene->GetRootSceneObject());
	GASS::SceneObjectPtr light_obj = scene->LoadObjectFromTemplate("LightObject",scene->GetRootSceneObject());
	light_obj->SendImmediateRequest(GASS::RotationRequestPtr(new GASS::RotationRequest(GASS::EulerRotation(40, 32, 0).GetQuaternion())));
	
	//create free camera and set start pos
	GASS::SceneObjectPtr free_obj = scene->LoadObjectFromTemplate("FreeCameraObject",scene->GetRootSceneObject());
	GASS::PositionRequestPtr pos_msg(new GASS::PositionRequest(GASS::Vec3(0,2,0)));
	if(free_obj)
	{
		free_obj->SendImmediateRequest(pos_msg);
		GASS::SystemMessagePtr camera_msg(new GASS::ChangeCameraRequest(free_obj->GetFirstComponentByClass<GASS::ICameraComponent>()));
		m_Engine->GetSimSystemManager()->PostMessage(camera_msg);
	}

	
	while(true)
	{
		m_Engine->Update();
	}
	return 0;
}

int main(int argc, char* argv[])
{
	int ret =0;
	try
	{
		ret = run(argc,argv);
	}
	catch(std::exception& e) 
	{
		std::cout << "Exception:" << e.what() << std::endl;
		_getch();
	}
	return ret;
}