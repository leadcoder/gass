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
	GASS::SimEngineConfig config = GASS::SimEngineConfig::Create(GASS::PhysicsOptions::PHYSX,
		GASS::SoundOptions::NONE,
		GASS::NetworkOptions::NONE);
	config.ResourceConfig.ResourceLocations.emplace_back("SAMPLE_DATA", "%GASS_DATA_HOME%/sample_data", true);
	engine->Init(config);
	
	//Get graphic system and create one main rendering window
	GASS::GraphicsSystemPtr gfx_sys = engine->GetSimSystemManager()->GetFirstSystemByClass<GASS::IGraphicsSystem>();
	GASS::RenderWindowPtr win = gfx_sys->CreateRenderWindow("MainWindow",800,600);

	//Create viewport in main window
	win->CreateViewport("MainViewport", 0, 0, 1, 1);
	
	//Give input system window handle
	GASS::InputSystemPtr input_system = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::IInputSystem>();
	input_system->SetMainWindowHandle(win->GetHWND());
	
	//Create the scene
	auto scene = GASS::SimEngine::Get().CreateScene().lock();
	
	//Load pre-build scene from data folder
	scene->Load(GASS::FilePath("%GASS_DATA_HOME%/sample_data/sceneries/osg_demo.scene"));
	scene->GetOrCreateCamera();

	//Create vehicle and add it to the root node of the scene
	GASS::SceneObjectPtr vehicle_obj = engine->CreateObjectFromTemplate("PXTank");
	scene->GetRootSceneObject()->AddChildSceneObject(vehicle_obj, true);

	//Set start position
	vehicle_obj->GetFirstComponentByClass<GASS::ILocationComponent>()->SetWorldPosition(scene->GetStartPos());

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


