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
#include "Plugins/Game/GameMessages.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Sim/GASSBaseSceneManager.h"
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

int run(int argc, char* argv[])
{
	GASS::SimEngine* m_Engine = new GASS::SimEngine();
	m_Engine->Init(GASS::FilePath("GUISample.xml"));
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
	light_obj->SendImmediateRequest(GASS::RotationRequestPtr(new GASS::RotationRequest(GASS::Vec3(40,32,0))));
	
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
		getch();
	}
	return ret;
}