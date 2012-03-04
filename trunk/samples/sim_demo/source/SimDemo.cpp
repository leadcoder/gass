/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
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
/*#include <boost/bind.hpp>

#include "Core/ComponentSystem/IComponent.h"
#include "Core/Math/Vector.h"
#include "Core/Utils/Factory.h"
#include "Core/Utils/Log.h"
#include "Core/Serialize/Serialize.h"
#include "Core/Utils/Timer.h"
#include "Core/Utils/Log.h"
#include "Core/ComponentSystem/ComponentContainerFactory.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/BaseComponent.h"
#include "Core/ComponentSystem/BaseComponentContainer.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplate.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/PluginSystem/PluginManager.h"

#include "Sim/SimEngine.h"
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/SceneObject.h"

#include "Sim/Systems/Collision/ICollisionSystem.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Components/Graphics/Geometry/ILineComponent.h"*/
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
	GASS::SimEngine::Get().GetSimObjectManager()->AddTemplate(container);
}

void TestCollision(GASS::ScenarioPtr scenario)
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
	request.Scenario = scenario;
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

GASS::ScenarioPtr scenario(new GASS::Scenario());


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


	std::cout << "----------------Update-------------------\n";
	while(app->Update())
	{
		
	}
	
	exit(0);


/*	SimServer server;
	SimClient client;

	std::string plugin_file = "../Configuration/plugins.xml";
	std::string sys_conf_file ="../Configuration/systems.xml";
	std::string ctrl_conf_file ="../Configuration/control_settings.xml";
	std::string scenario_path ="../data/scenarios/ogre_demo_scenario";


	int index = 1;
	bool is_server = false;
	//check if arguments are provided
	//Example:  --Plugins configurations/osg/plugins.xml --SystemConfiguration configurations/osg/systems.xml --Scenario ../data/scenarios/osg_demo_scenario
	//Example:  --Plugins configurations/ogre/plugins.xml --SystemConfiguration configurations/ogre/systems.xml --Scenario ../data/scenarios/ogre_demo_scenario
	while(index < argc)
	{
		char* arg = argv[index];
		if(_strcmpi(arg, "--Plugins") == 0)
		{
			plugin_file = argv[index+1];
		}
		else if(_strcmpi(arg, "--SystemConfiguration") == 0)
		{
			sys_conf_file = argv[index+1];
		}
		else if(_strcmpi(arg, "--Scenario") == 0)
		{
			scenario_path = argv[index+1];
		}
		else if(_strcmpi(arg, "--IsServer") == 0)
		{
			is_server = atoi(argv[index+1]);
		}
		index += 2;
	}
	GASS::SimEngine* engine = new GASS::SimEngine();
	engine->Init(plugin_file,sys_conf_file,ctrl_conf_file);
	engine->GetSimObjectManager()->Load("..\\data\\templates\\vehicles\\jim_tank.xml");


	//network settings
	if(is_server)
	{
		engine->GetSimSystemManager()->SendImmediate(GASS::MessagePtr(new GASS::StartServerMessage("SimDemoServer",2001)));
		GASS::MessageFuncPtr callback(new GASS::MessageFunc<GASS::IMessage>(boost::bind( &SimServer::OnClientConnected, &server, _1 ),&server));
		engine->GetSimSystemManager()->RegisterForMessage(typeid(GASS::ClientConnectedMessage),callback,0);

		//CreateManualObject();
		scenario->Load(scenario_path);






		for(int i = 0; i < 1; i++)
		{
			GASS::SceneObjectPtr scene_object = scenario->GetScenarioScenes().at(0)->GetObjectManager()->LoadFromTemplate("JimTank");
			if(scene_object)
			{

				GASS::Vec3 pos = scenario->GetScenarioScenes().front()->GetStartPos();
				pos.x = pos.x + i*7;
				pos.z = pos.z - 2;
				//pos.x = 0;
				//pos.y = 10;
				//pos.z = 0;
				boost::shared_ptr<GASS::IMessage> pos_msg(new GASS::PositionMessage(pos));
				scene_object->SendImmediate(pos_msg);

				if(i==0)
				{
					/*GASS::MessagePtr enter_msg(new GASS::EnterVehicleMessage());
					scene_object->PostMessage(enter_msg);

					GASS::SceneObjectVector objs = scene_object->GetObjectsByName("Turret", false);
					if(objs.size() > 0)
						objs.front()->PostMessage(enter_msg);*/
		/*		}
			}
			//Sleep(1000);
		}

	}
	else
	{
		GASS::MessageFuncPtr resp_callback(new GASS::MessageFunc<GASS::IMessage>(boost::bind( &SimClient::OnServerResponse, &client, _1 ),&client));
		engine->GetSimSystemManager()->RegisterForMessage(typeid(GASS::ServerResponseMessage),resp_callback,0);
		GASS::MessageFuncPtr load_callback(new GASS::MessageFunc<GASS::IMessage>(boost::bind( &SimClient::OnLoadScenario, &client, _1 ),&client));
		engine->GetSimSystemManager()->RegisterForMessage(typeid(GASS::StartSceanrioRequestMessage),load_callback,0);
		engine->GetSimSystemManager()->SendImmediate(GASS::MessagePtr(new GASS::StartClientMessage("SimDemoClient",2002,2001)));



		printf("\n\nWaiting for server");
		float update_time = 0;
		while(!client.IsConnected())
		{
			engine->Update(update_time); //update engine (need to process messages)
			engine->GetSimSystemManager()->PostMessage(GASS::MessagePtr(new GASS::PingRequestMessage(2001)));
			update_time += 1.0;
			Sleep(1000);
			printf(".");
			//send ping request
		}
	}




	//scenario->Load("../../../data/scenarios/camp_genesis");
	//scenario->Load("../../../data/advantage_scenario");

	/*for(int i = 0; i < 2; i++)
	{
		for(int j = 0; j < 2; j++)
		{
			//GASS::SceneObject* scene_object = scenario->GetScene(0)->GetObjectManager()->LoadFromTemplate("ContainerObject");
			GASS::SceneObjectPtr scene_object = scenario->GetScene(0)->GetObjectManager()->LoadFromTemplate("ContainerTemplate");

			int from_id = i*40 + j;
			GASS::MessagePtr pos_msg(new GASS::Message(GASS::ScenarioScene::OBJECT_RM_POSITION,from_id));
			GASS::Vec3 pos(1705.18 + i*10 ,100.3, 2808+10*j);
			pos_msg->SetData("Position",pos);
			scene_object->GetMessageManager()->SendImmediate(pos_msg);
		}
	}*/



	/*GASS::Timer timer;
	timer.Reset();
	double prev = 0;
	double temp_t = 0;
	bool check_reset = true;
	double update_time = 1.0/60.0;

	while(timer.GetTime() < 1200)
	{
		double time = timer.GetTime();
		temp_t = time;
		if(time - prev > update_time)
		{
			engine->Update(update_time);
			scenario->OnUpdate(update_time);
			//std::cout << "Time is:" << time << std::endl;
			//std::cout << "FPS:" << 1.0/(time - prev) << std::endl;
			static bool once = true;
			prev = time;
		}*/

	/*	delete scenario;
		scenario = new GASS::Scenario();

		//CreateManualObject();

		scenario->Load("../data/scenarios/ogre_demo_scenario");*/

		/*if(check_reset && time > 5.0)
		{
			check_reset = false;
			timer.Reset();
			time = timer.GetTime();
			std::cout << "Reset timer after 5 sec , current time is:" << time << std::endl;
			prev = time;
		}*/
	/*}*/

	/////////////////TEST plugin manager/////////////
	return 0;
}


