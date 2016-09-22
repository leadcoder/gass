/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/										*
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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
#include "Core/PluginSystem/GASSPluginManager.h"
#include "Sim/Messages/GASSPlatformMessages.h"

#include <stdio.h>
#include <iostream>

#ifdef WIN32
	#include <conio.h>
#endif


#ifndef WIN32
	#include <stdio.h>
	#include <termios.h>
	#include <unistd.h>
	int _getch( ) {
		struct termios oldt, newt;
		int ch;
		tcgetattr( STDIN_FILENO, &oldt );
		newt = oldt;
		newt.c_lflag &= ~( ICANON | ECHO );
		tcsetattr( STDIN_FILENO, TCSANOW, &newt );
		ch = getchar();
		tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
		return ch;	
	}
#endif

int start(int argc, char* argv[])
{
	(void) argc;
	(void) argv;
	//Create engine instance and initialize with config file
	GASS::SimEngine* engine = new GASS::SimEngine();
	engine->Init(GASS::FilePath("SampleSimVehicle.xml"));
	
	//Get graphic system and create one main rendering window
	GASS::GraphicsSystemPtr gfx_sys = engine->GetSimSystemManager()->GetFirstSystemByClass<GASS::IGraphicsSystem>();
	GASS::RenderWindowPtr win = gfx_sys->CreateRenderWindow("MainWindow",800,600);

	//Create viewport in main window
	win->CreateViewport("MainViewport", 0, 0, 1, 1);
	
	//Give input system window handle
	GASS::InputSystemPtr input_system = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::IInputSystem>();
	input_system->SetMainWindowHandle(win->GetHWND());
	
	//Create the scene
	GASS::ScenePtr scene = GASS::ScenePtr(GASS::SimEngine::Get().CreateScene("new_demo_scene"));
	
	//Load pre-build scene from data folder
	scene->Load("new_terrain");
	
	//create free camera and add it to the scene under the root node
	GASS::SceneObjectPtr camera_obj = engine->CreateObjectFromTemplate("FreeCameraObject");
	scene->GetRootSceneObject()->AddChildSceneObject(camera_obj, true);
	
	//Set camera position
	GASS::PositionRequestPtr pos_msg(new GASS::PositionRequest(scene->GetStartPos()));
	camera_obj->PostRequest(pos_msg);

	//Make this the primary camera
	GASS::SystemMessagePtr camera_msg(new GASS::ChangeCameraRequest(camera_obj->GetFirstComponentByClass<GASS::ICameraComponent>()));
	engine->GetSimSystemManager()->PostMessage(camera_msg);

	//Create vehicle and add it to the root node of the scene
	GASS::SceneObjectPtr vehicle_obj = engine->CreateObjectFromTemplate("PXTank");
	scene->GetRootSceneObject()->AddChildSceneObject(vehicle_obj, true);

	//Set start position
	GASS::Vec3 pos = scene->GetStartPos();
	vehicle_obj->SendImmediateRequest(GASS::WorldPositionRequestPtr(new GASS::WorldPositionRequest(pos)));

	//Update the engine forever
	bool done = false;
	while(!done)
	{
		engine->Update();
	}
	return 0;
}

int main(int argc, char* argv[])
{
	int ret =0;
	try
	{
		ret = start(argc, argv);
	}
	catch(std::exception& e) 
	{
		std::cout << "Exception:" << e.what() << std::endl;
		_getch();
	}
	return ret;
}


