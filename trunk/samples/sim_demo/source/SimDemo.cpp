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

#include "Client.h"
#include "Server.h"

#include <stdio.h>
#include <iostream>
#include <fstream>

#ifdef WIN32
#include <conio.h>
#endif
GASS::MessageManager mm;

void CreateManualObject()
{
	GASS::BaseComponentContainerTemplatePtr container( new GASS::BaseComponentContainerTemplate()); //boost::shared_static_cast<GASS::BaseComponentContainer>( GASS::ComponentContainerTemplateFactory::Get().Create("BaseComponentContainer"));
	container->SetName("ContainerTemplate");
	GASS::BaseComponentPtr lc = boost::shared_static_cast<GASS::BaseComponent>(GASS::ComponentFactory::Get().Create("LocationComponent"));
	lc->SetName("ContainerLocation");
	lc->SetPropertyByType("Position",GASS::Vec3(0,0,0));
	lc->SetPropertyByType("Rotation",GASS::Vec3(45,45,45));
	container->AddComponent(lc);

	GASS::BaseComponentPtr mc = boost::shared_static_cast<GASS::BaseComponent>(GASS::ComponentFactory::Get().Create("MeshComponent"));
	mc->SetName("ContainerMesh");
	mc->SetPropertyByType("Filename",std::string("container_01.mesh"));
	mc->SetPropertyByType("CastShadow",true);
	container->AddComponent(mc);

	GASS::BaseComponentPtr geom_comp = boost::shared_static_cast<GASS::BaseComponent>(GASS::ComponentFactory::Get().Create("ODEGeometry"));
	geom_comp->SetName("ContainerPhysicsGeom");
	geom_comp->SetPropertyByType("GeometryType",std::string("box"));
	container->AddComponent(geom_comp);

	GASS::BaseComponentPtr body_comp = boost::shared_static_cast<GASS::BaseComponent>(GASS::ComponentFactory::Get().Create("ODEBody"));
	body_comp->SetName("ContainerPhysicsBody");
	//body_comp->SetPropertyByValue("GeometryType","box");
	container->AddComponent(body_comp);
	GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(container);
}

void TestCollision(GASS::ScenePtr scene)
{
	GASS::CollisionSystemPtr col_sys = GASS::SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<GASS::ICollisionSystem>();

	GASS::CollisionHandle handle;
	GASS::CollisionRequest request;
	GASS::CollisionResult result;

	//request.LineStart = GASS::Vec3(1720.49,1000, 2875.78);
	//request.LineEnd = GASS::Vec3(1720.49,-1000, 2875.78);

	request.LineStart = GASS::Vec3(100.49,1000, 100.78);
	request.LineEnd = GASS::Vec3(100.49,-1000, 100.78);
//	request.Type = GASS::CollisionType::COL_LINE;
	request.Scene = scene;
	request.ReturnFirstCollisionPoint = 0;

	handle = col_sys->Request(request);
	if(col_sys->Check(handle,result))
	{
		if(result.Coll)
		{
			std::cout << "Collision at:" << result.CollPosition.x << " " << result.CollPosition.y << " " << result.CollPosition.z << std::endl;
		}
	}
	//col_sys
}

GASS::ScenePtr scene(new GASS::Scene());


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

typedef boost::shared_ptr<SimApplication> SimApplicationPtr;

int main(int argc, char* argv[])
{
	SimApplicationPtr app;
	bool is_server = false;
	std::string config = "../Configuration/app_config.xml";
	int index = 1;
	while(index < argc)
	{
		char* arg = argv[index];
		if(strcmp(arg, "--IsServer") == 0)
		{
			is_server = atoi(argv[index+1]);
		}
		else if(strcmp(arg, "--Config") == 0)
		{
			config = argv[index+1];
		}
		index += 2;
	}

	std::cout << "Server, client or standalone? Press [S] ,[C] or [A]:";
	char key = _getch();
	int app_mode = 2;
	if(key == 'c' || key == 'C')
		app_mode = 0;
	else if(key == 's' || key == 'S')
		app_mode = 1;
	else if(key == 'a' || key == 'A')
		app_mode = 2;

	if(app_mode == 0)
		app = SimApplicationPtr(new SimClient(config));
	else if(app_mode == 1)
		app = SimApplicationPtr(new SimServer(config));
	else if(app_mode == 2)
		app = SimApplicationPtr(new SimApplication(config));

	app->Init();

	while(app->Update())
	{
		
	}
	return 0;
}


