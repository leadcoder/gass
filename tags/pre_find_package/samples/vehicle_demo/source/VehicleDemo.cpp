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
#include "Core/Utils/GASSHeightmap.h"
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
	/*int hm_size = 10000;
	GASS::Heightmap* hm = new GASS::Heightmap(GASS::Vec3(0,0,0),GASS::Vec3(200,0,200),hm_size,hm_size);
	for(unsigned int i = 0; i < hm_size; i++)
	{
		for(unsigned int j = 0; j < hm_size; j++)
			hm->SetHeight(i,j, i*0.1);
	}
	//hm->ImportRAWFile("c:/temp/TerrainHeightmap.raw", 10, 200);
	float height = hm->GetInterpolatedHeight(100, 100);

	hm->Save("c:/temp/dump.bin");
	hm->Load("c:/temp/dump.bin");
	
	height = hm->GetInterpolatedHeight(100, 100);

	for(int i = 0; i < 5000; i++)
	{
		float height = hm->GetInterpolatedHeight(100, i* 200.0/5000.0);
		std::cout << height <<  "  ";
	}*/

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

	GASS::SimEngine* m_Engine = new GASS::SimEngine();
	m_Engine->Init(GASS::FilePath("GASSPhysicsOSGDemo.xml"));
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
		plane_template->AddBaseSceneComponent("PhysicsPlaneGeometryComponent");
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
		light_comp->SetPropertyByString("AmbientColor","0.5 0.5 0.5");
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(light_template);
	}
	
	{
		GASS::SceneObjectTemplatePtr box_template (new GASS::SceneObjectTemplate);
		box_template->SetName("BoxObject");
		box_template->AddBaseSceneComponent("LocationComponent");
		GASS::BaseSceneComponentPtr mmc  = box_template->AddBaseSceneComponent("ManualMeshComponent");
		mmc->SetPropertyByType("CastShadow",true);
		box_template->AddBaseSceneComponent("PhysicsBoxGeometryComponent");
		box_template->AddBaseSceneComponent("PhysicsBodyComponent");
		GASS::BaseSceneComponentPtr box_comp  = box_template->AddBaseSceneComponent("BoxGeometryComponent");
		box_comp->SetPropertyByType("Size",GASS::Vec3(1,1,1));
		box_comp->SetPropertyByType("Lines",false);
		
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(box_template);
	}

	{
		GASS::SceneObjectTemplatePtr mesh_template (new GASS::SceneObjectTemplate);
		mesh_template->SetName("MeshObject");
		mesh_template->AddBaseSceneComponent("LocationComponent");
		mesh_template->AddBaseSceneComponent("MeshComponent");
		mesh_template->AddBaseSceneComponent("PhysicsBoxGeometryComponent");
		mesh_template->AddBaseSceneComponent("PhysicsBodyComponent");
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(mesh_template);
	}

	GASS::SceneObjectPtr terrain_obj = scene->LoadObjectFromTemplate("PlaneObject",scene->GetRootSceneObject());
	GASS::SceneObjectPtr light_obj = scene->LoadObjectFromTemplate("LightObject",scene->GetRootSceneObject());
	light_obj->SendImmediateRequest(GASS::RotationRequestPtr(new GASS::RotationRequest(GASS::Vec3(40,32,0))));
	
	//GASS::SceneObjectPtr tlc = scene->LoadObjectFromTemplate("ToyotaLandcruiser",scene->GetRootSceneObject());
	//tlc->SendImmediate(GASS::MessagePtr(new GASS::PositionRequest(GASS::Vec3(0,4,-13))));
	
	GASS::SceneObjectPtr tlc = scene->LoadObjectFromTemplate("Liebherr",scene->GetRootSceneObject());
	tlc->SendImmediateRequest(GASS::PositionRequestPtr(new GASS::PositionRequest(GASS::Vec3(0,4,-13))));
	
	
	/*GASS::SceneObjectPtr group = scene->LoadObjectFromTemplate("AIVehicleGroup",scene->GetRootSceneObject());
	
	GASS::SceneObjectPtr wp = scene->LoadObjectFromTemplate("BehaviorWaypoint",group->GetChildByID("BEHAVIOR_WPL"));
	wp->SendImmediate(GASS::MessagePtr(new GASS::PositionRequest(GASS::Vec3(0,0,-100))));
	wp = scene->LoadObjectFromTemplate("BehaviorWaypoint",group->GetChildByID("BEHAVIOR_WPL"));
	wp->SendImmediate(GASS::MessagePtr(new GASS::PositionRequest(GASS::Vec3(40,0,-100))));
	wp = scene->LoadObjectFromTemplate("BehaviorWaypoint",group->GetChildByID("BEHAVIOR_WPL"));
	wp->SendImmediate(GASS::MessagePtr(new GASS::PositionRequest(GASS::Vec3(40,0,-1000))));

	GASS::SceneObjectPtr trigger = scene->LoadObjectFromTemplate("VehicleTrigger",scene->GetRootSceneObject());
	trigger->SendImmediate(GASS::MessagePtr(new GASS::PositionRequest(GASS::Vec3(0,0,-80))));
	GASS::BaseSceneComponentPtr wp_comp = wp->GetBaseSceneComponent("VehicleBehaviorComponent");
	GASS::SceneObjectRef t_ref(trigger->GetGUID());
	std::vector<GASS::SceneObjectRef> t_vec;
	t_vec.push_back(t_ref);
	wp_comp->SetPropertyByType("Triggers",t_vec);
	


	GASS::SceneObjectPtr patria = scene->LoadObjectFromTemplate("AIVehicle",group);
	patria = scene->LoadObjectFromTemplate("AIVehicle",group);
	patria->SendImmediate(GASS::MessagePtr(new GASS::PositionRequest(GASS::Vec3(0,0,10))));

	patria = scene->LoadObjectFromTemplate("AIVehicle",group);
	patria->SendImmediate(GASS::MessagePtr(new GASS::PositionRequest(GASS::Vec3(0,0,20))));

	
	GASS::BaseSceneComponentPtr trigger_comp = trigger->GetBaseSceneComponent("VehicleTriggerComponent");
	GASS::SceneObjectRef c_ref(patria->GetGUID());
	std::vector<GASS::SceneObjectRef> c_vec;
	c_vec.push_back(c_ref);
	trigger_comp->SetPropertyByType("ActivationControllers",c_vec);
	*/

	
	//create free camera and set start pos
	GASS::SceneObjectPtr free_obj = scene->LoadObjectFromTemplate("FreeCameraObject",scene->GetRootSceneObject());
	GASS::PositionRequestPtr pos_msg(new GASS::PositionRequest(GASS::Vec3(0,2,0)));
	if(free_obj)
	{
		free_obj->SendImmediateRequest(pos_msg);
		GASS::SystemMessagePtr camera_msg(new GASS::ChangeCameraRequest(free_obj->GetFirstComponentByClass<GASS::ICameraComponent>()));
		m_Engine->GetSimSystemManager()->PostMessage(camera_msg);
	}
	static float wheel_vel = 0;
	static float steer_vel = 0;

	GASS::BaseSceneManagerPtr ogre_sm = DYNAMIC_PTR_CAST<GASS::BaseSceneManager>(scene->GetSceneManagerByName("OgreGraphicsSceneManager"));
	if(ogre_sm)
	{
		/*ogre_sm->SetPropertyByString("ShadowMode","SHADOWS_DISABLED");
		ogre_sm->SetPropertyByString("ShadowMode","TEXTURE_SHADOWS_MODULATIVE");

		//ogre_sm->SetPropertyByString("ShadowMode","TEXTURE_SHADOWS_ADDITIVE");
		ogre_sm->SetPropertyByString("TextureShadowProjection","UNIFORM_FOCUSED");
		ogre_sm->SetPropertyByType("SelfShadowing",false);*/
	}

	while(true)
	{
		m_Engine->Update();
		static bool key_down=false;
		if(GetAsyncKeyState(VK_SPACE))
		{
			if(!key_down)
			{
				key_down = true;
				GASS::Vec3 pos = free_obj->GetFirstComponentByClass<GASS::ILocationComponent>()->GetPosition();
				GASS::Quaternion rot = free_obj->GetFirstComponentByClass<GASS::ILocationComponent>()->GetRotation();
				GASS::Mat4 rot_mat;
				rot_mat.Identity();
				rot.ToRotationMatrix(rot_mat);
				GASS::Vec3 vel = rot_mat.GetZAxis()*-2500;
				GASS::Vec3 torq(0,0,2000);
				torq = rot_mat * torq;
				GASS::SceneObjectPtr box_obj = scene->LoadObjectFromTemplate("BoxObject",scene->GetRootSceneObject());
				box_obj->SendImmediateRequest(GASS::PositionRequestPtr(new GASS::PositionRequest(pos)));
				box_obj->SendImmediateRequest(GASS::RotationRequestPtr(new GASS::RotationRequest(rot)));
				box_obj->SendImmediateRequest(GASS::PhysicsBodyAddForceRequestPtr(new GASS::PhysicsBodyAddForceRequest(vel)));
				box_obj->SendImmediateRequest(GASS::PhysicsBodyAddTorqueRequestPtr(new GASS::PhysicsBodyAddTorqueRequest(torq)));
			}
		}
		else if(GetAsyncKeyState(VK_DELETE))
		{
		}
		else if(GetAsyncKeyState(VK_F1))
		{
			if(!key_down)
			{
				GASS::SimEngine::Get().GetSimSystemManager()->PostMessage(GASS::SystemRequestMessagePtr(new GASS::ScenarioStateRequest(GASS::SS_PLAY)));
				key_down = true;
				//tp.x += 1;
				//tlc->PostMessage(GASS::MessagePtr(new GASS::PositionRequest(tp)));
//				mesh_obj->SendImmediate(GASS::MessagePtr(new GASS::MeshFileRequest("wheel.3ds")));
			}
		}
		else if(GetAsyncKeyState(VK_F2))
		{
			if(!key_down)
			{
				GASS::SimEngine::Get().GetSimSystemManager()->PostMessage(GASS::SystemRequestMessagePtr(new GASS::ScenarioStateRequest(GASS::SS_STOP)));
				
				//tp.y += 0.2;
//				tlc->PostMessage(GASS::MessagePtr(new GASS::PositionRequest(tp)));

				key_down = true;
	//			mesh_obj->SendImmediate(GASS::MessagePtr(new GASS::MeshFileRequest("car.3ds")));
			}
		}
		else if(GetAsyncKeyState(VK_F3))
		{
			if(!key_down)
			{
				key_down = true;
				//bdrige_seg_obj2->PostMessage(GASS::MessagePtr(new GASS::MaterialMessage(GASS::Vec4(1,1,1,1),GASS::Vec3(1,1,1))));
			}
		}
		else if(GetAsyncKeyState(VK_F4))
		{
			if(!key_down)
			{
				key_down = true;
			}
		}

		else if(GetAsyncKeyState(VK_DOWN))
		{
			key_down = true;
			wheel_vel -= 2;
		}
		else if(GetAsyncKeyState(VK_UP))
		{
			key_down = true;
			wheel_vel += 2;
		}
		else if(GetAsyncKeyState(VK_LEFT))
		{
			key_down = true;
			steer_vel = 2;
		}
		else if(GetAsyncKeyState(VK_RIGHT))
		{
			key_down = true;
			steer_vel = -2;
		}
		else
		{
			steer_vel = 0;
			key_down = false;
		}
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