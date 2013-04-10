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


int main(int argc, char* argv[])
{
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

	/*std::cout << "Server, client or standalone? Press [S] ,[C] or [A]:";
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
	else if(app_mode == 2)*/
	//	app = SimApplicationPtr(new SimApplication(config));

	GASS::SimEngine* m_Engine = new GASS::SimEngine();
	m_Engine->Init(GASS::FilePath("../Configuration/GASS.xml"));

	//load keyboard config!
	GASS::ControlSettingsSystemPtr css = m_Engine->GetSimSystemManager()->GetFirstSystemByClass<GASS::IControlSettingsSystem>();
	if(css)
	{
		css->Load("../Configuration/control_settings.xml");
	}
	GASS::GraphicsSystemPtr gfx_sys = m_Engine->GetSimSystemManager()->GetFirstSystemByClass<GASS::IGraphicsSystem>();

	GASS::RenderWindowPtr win = gfx_sys->CreateRenderWindow("MainWindow",800,600);
	win->CreateViewport("MainViewport", 0, 0, 1, 1);

	GASS::InputSystemPtr input_system = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::IInputSystem>();
	input_system->SetMainWindowHandle(win->GetHWND());
	
	GASS::ScenePtr scene(m_Engine->CreateScene("NewScene"));

	{
		GASS::SceneObjectTemplatePtr plane_template (new GASS::SceneObjectTemplate);
		plane_template->SetName("PlaneObject");
		plane_template->AddComponent("LocationComponent");
		plane_template->AddComponent("ManualMeshComponent");
		plane_template->AddComponent("PhysicsPlaneGeometryComponent");
		GASS::BaseComponentPtr plane_comp2  = DYNAMIC_PTR_CAST<GASS::BaseComponent>(plane_template->AddComponent("PlaneGeometryComponent"));
		plane_comp2 ->SetPropertyByType("Size",GASS::Vec2(100,100));
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(plane_template );
	}
	{
		GASS::SceneObjectTemplatePtr light_template (new GASS::SceneObjectTemplate);
		light_template->SetName("LightObject");
		light_template->AddComponent("LocationComponent");
		light_template->AddComponent("LightComponent");
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(light_template);
	}
	{
		{
			GASS::SceneObjectTemplatePtr vehicle_template (new GASS::SceneObjectTemplate);
			vehicle_template->SetName("VehicleObject");
			vehicle_template->SetID("VEHICLE");
			vehicle_template->AddComponent("LocationComponent");
			vehicle_template->AddComponent("ManualMeshComponent");
			vehicle_template->AddComponent("PhysicsBodyComponent");
			vehicle_template->AddComponent("PhysicsBoxGeometryComponent");
			GASS::BaseComponentPtr box_comp  = DYNAMIC_PTR_CAST<GASS::BaseComponent>(vehicle_template->AddComponent("BoxGeometryComponent"));
			box_comp->SetPropertyByType("Size",GASS::Vec3(2,1,4));
			box_comp->SetPropertyByType("Lines",false);
			GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(vehicle_template);

			//add wheels
			GASS::SceneObjectTemplatePtr wheel_template;
			GASS::Float offset = -1;

			wheel_template =  GASS::SceneObjectTemplatePtr(new GASS::SceneObjectTemplate);
			wheel_template->SetName("FrontLeftWheel");
			wheel_template->SetID("FL_WHEEL");
			wheel_template->SetInheritance("WheelObject");
			GASS::ComponentPtr location_comp  = wheel_template->AddComponent("LocationComponent");
			DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(-1,offset,-1));
			vehicle_template->AddChild(wheel_template);

			wheel_template =  GASS::SceneObjectTemplatePtr(new GASS::SceneObjectTemplate);
			wheel_template->SetName("FrontRightWheel");
			wheel_template->SetID("FR_WHEEL");
			wheel_template->SetInheritance("WheelObject");
			location_comp  = wheel_template->AddComponent("LocationComponent");
			DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(1,offset,-1));
			vehicle_template->AddChild(wheel_template);

			
			wheel_template =  GASS::SceneObjectTemplatePtr(new GASS::SceneObjectTemplate);
			wheel_template->SetName("RearLeftWheel");
			wheel_template->SetID("RL_WHEEL");
			wheel_template->SetInheritance("WheelObject");
			location_comp  = wheel_template->AddComponent("LocationComponent");
			DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(-1,offset,1));
			vehicle_template->AddChild(wheel_template);

			wheel_template =  GASS::SceneObjectTemplatePtr(new GASS::SceneObjectTemplate);
			wheel_template->SetName("RearRightWheel");
			wheel_template->SetID("RR_WHEEL");
			wheel_template->SetInheritance("WheelObject");
			location_comp  = wheel_template->AddComponent("LocationComponent");
			DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(1,offset,1));
			vehicle_template->AddChild(wheel_template);
		}

		{
			GASS::SceneObjectTemplatePtr wheel_template (new GASS::SceneObjectTemplate);
			wheel_template->SetName("WheelObject");
			wheel_template->SetID("WHEEL");
			wheel_template->AddComponent("LocationComponent");
			GASS::BaseComponentPtr sphere_comp  = DYNAMIC_PTR_CAST<GASS::BaseComponent>(wheel_template->AddComponent("SphereGeometryComponent"));
			sphere_comp->SetPropertyByType("Radius",0.3);
			wheel_template->AddComponent("PhysicsSphereGeometryComponent");
			wheel_template->AddComponent("PhysicsBodyComponent");
			wheel_template->AddComponent("ManualMeshComponent");
			wheel_template->AddComponent("PhysicsHingeComponent");
			GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(wheel_template);
		}
	}

	{
		GASS::SceneObjectTemplatePtr box_template (new GASS::SceneObjectTemplate);
		box_template->SetName("BoxObject");
		box_template->AddComponent("LocationComponent");
		box_template->AddComponent("ManualMeshComponent");
		box_template->AddComponent("PhysicsBoxGeometryComponent");
		box_template->AddComponent("PhysicsBodyComponent");
		GASS::BaseComponentPtr box_comp  = DYNAMIC_PTR_CAST<GASS::BaseComponent>(box_template->AddComponent("BoxGeometryComponent"));
		box_comp->SetPropertyByType("Size",GASS::Vec3(1,1,1));
		box_comp->SetPropertyByType("Lines",false);
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(box_template);
	}

	{
		//Create bridge tempaltes
		GASS::SceneObjectTemplatePtr bridge_seg_template (new GASS::SceneObjectTemplate);
		bridge_seg_template->SetName("BridgeSegment");
		bridge_seg_template->AddComponent("LocationComponent");
		bridge_seg_template->AddComponent("ManualMeshComponent");
		bridge_seg_template->AddComponent("PhysicsBoxGeometryComponent");
		bridge_seg_template->AddComponent("PhysicsBodyComponent");
		GASS::BaseComponentPtr box_comp  = DYNAMIC_PTR_CAST<GASS::BaseComponent>(bridge_seg_template->AddComponent("BoxGeometryComponent"));
		box_comp->SetPropertyByType("Size",GASS::Vec3(2,0.3,0.6));
		box_comp->SetPropertyByType("Lines",false);
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(bridge_seg_template);

		GASS::SceneObjectTemplatePtr bridge_hinge_template (new GASS::SceneObjectTemplate);
		bridge_hinge_template->SetName("BridgeHinge");
		bridge_hinge_template->AddComponent("PhysicsHingeComponent");
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(bridge_hinge_template);
	}

	{
		GASS::SceneObjectTemplatePtr mesh_template (new GASS::SceneObjectTemplate);
		mesh_template->SetName("MeshObject");
		mesh_template->AddComponent("LocationComponent");
		mesh_template->AddComponent("MeshComponent");
		mesh_template->AddComponent("PhysicsBoxGeometryComponent");
		mesh_template->AddComponent("PhysicsBodyComponent");
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(mesh_template);
	}

	GASS::SceneObjectPtr terrain_obj = scene->LoadObjectFromTemplate("PlaneObject",scene->GetRootSceneObject());
	terrain_obj->SendImmediate(GASS::MessagePtr(new GASS::PositionMessage(GASS::Vec3(0,0,0))));
	//terrain_obj->SendImmediate(GASS::MessagePtr(new GASS::MeshFileMessage("terrain.3DS")));

	GASS::SceneObjectPtr light_obj = scene->LoadObjectFromTemplate("LightObject",scene->GetRootSceneObject());
	light_obj->SendImmediate(GASS::MessagePtr(new GASS::RotationMessage(GASS::Vec3(40,32,0))));
	
	GASS::SceneObjectPtr bdrige_seg_obj2 = scene->LoadObjectFromTemplate("BridgeSegment",scene->GetRootSceneObject());
	bdrige_seg_obj2->SendImmediate(GASS::MessagePtr(new GASS::PositionMessage(GASS::Vec3(10,2,0))));
	GASS::BaseComponentPtr body_comp = DYNAMIC_PTR_CAST<GASS::BaseComponent>(bdrige_seg_obj2->GetComponent("PhysicsBodyComponent"));
	body_comp->SetPropertyByType("Kinematic",true);
	
	for(int i= 1 ; i < 11 ; i++)
	{
		GASS::SceneObjectPtr bdrige_seg_obj1 = bdrige_seg_obj2; 
		bdrige_seg_obj2 = scene->LoadObjectFromTemplate("BridgeSegment",scene->GetRootSceneObject());
		bdrige_seg_obj2->SendImmediate(GASS::MessagePtr(new GASS::PositionMessage(GASS::Vec3(10,2,i))));

		GASS::SceneObjectPtr bdrige_hinge_obj = scene->LoadObjectFromTemplate("BridgeHinge",scene->GetRootSceneObject());
		GASS::BaseComponentPtr hinge_comp = DYNAMIC_PTR_CAST<GASS::BaseComponent>(bdrige_hinge_obj->GetComponent("PhysicsHingeComponent"));
		if(i == 1)
		{
			hinge_comp->SetPropertyByType("Body1",GASS::SceneObjectRef(bdrige_seg_obj2->GetGUID()));
			hinge_comp->SetPropertyByType("Body2",GASS::SceneObjectRef(bdrige_seg_obj1->GetGUID()));
		}
		else
		{
			hinge_comp->SetPropertyByType("Body1",GASS::SceneObjectRef(bdrige_seg_obj1->GetGUID()));
			hinge_comp->SetPropertyByType("Body2",GASS::SceneObjectRef(bdrige_seg_obj2->GetGUID()));
		}
	}
	body_comp = DYNAMIC_PTR_CAST<GASS::BaseComponent>(bdrige_seg_obj2->GetComponent("PhysicsBodyComponent"));
	body_comp->SetPropertyByType("Kinematic",true);

	/*GASS::SceneObjectPtr mesh_obj = scene->LoadObjectFromTemplate("MeshObject",scene->GetRootSceneObject());
	mesh_obj->SendImmediate(GASS::MessagePtr(new GASS::MeshFileMessage("car.3ds")));
	mesh_obj->SendImmediate(GASS::MessagePtr(new GASS::PositionMessage(GASS::Vec3(0,5,1))));
	*/
	//GASS::ScenePtr scene = GASS::ScenePtr(m_Scene);
	//scene->Load(m_SceneName);


	//create bridge

	GASS::SceneObjectPtr box_obj = scene->LoadObjectFromTemplate("BoxObject",scene->GetRootSceneObject());
	box_obj->SendImmediate(GASS::MessagePtr(new GASS::PositionMessage(GASS::Vec3(10,0.6,2))));

	GASS::SceneObjectPtr box_obj2 = scene->LoadObjectFromTemplate("BoxObject",scene->GetRootSceneObject());
	box_obj2->SendImmediate(GASS::MessagePtr(new GASS::PositionMessage(GASS::Vec3(10,0.6,20))));

		
	GASS::SceneObjectPtr vehicle_obj = scene->LoadObjectFromTemplate("VehicleObject",scene->GetRootSceneObject());
	vehicle_obj->SendImmediate(GASS::MessagePtr(new GASS::PositionMessage(GASS::Vec3(0,2,2))));
	

	//create free camera and set start pos
	GASS::SceneObjectPtr free_obj = scene->LoadObjectFromTemplate("FreeCameraObject",scene->GetRootSceneObject());
	GASS::MessagePtr pos_msg(new GASS::PositionMessage(GASS::Vec3(0,2,0)));
	if(free_obj)
	{
		free_obj->SendImmediate(pos_msg);
		GASS::SystemMessagePtr camera_msg(new GASS::ChangeCameraRequest(free_obj->GetFirstComponentByClass<GASS::ICameraComponent>()));
		m_Engine->GetSimSystemManager()->PostMessage(camera_msg);
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
				GASS::Vec3 vel = rot_mat.GetViewDirVector()*-2500;
				GASS::Vec3 torq(0,0,2000);
				torq = rot_mat * torq;
				GASS::SceneObjectPtr box_obj = scene->LoadObjectFromTemplate("BoxObject",scene->GetRootSceneObject());
				box_obj->SendImmediate(GASS::MessagePtr(new GASS::PositionMessage(pos)));
				box_obj->SendImmediate(GASS::MessagePtr(new GASS::RotationMessage(rot)));
				box_obj->SendImmediate(GASS::MessagePtr(new GASS::PhysicsForceRequest(vel)));
				box_obj->SendImmediate(GASS::MessagePtr(new GASS::PhysicsTorqueRequest(torq)));
			}
		}
		else if(GetAsyncKeyState(VK_DELETE))
		{
			if(box_obj)
			{
				scene->GetRootSceneObject()->RemoveChild(box_obj);
				box_obj= GASS::SceneObjectPtr();
			}
		}
		else if(GetAsyncKeyState(VK_F1))
		{
			if(!key_down)
			{
				key_down = true;
//				mesh_obj->SendImmediate(GASS::MessagePtr(new GASS::MeshFileMessage("wheel.3ds")));
			}
		}
		else if(GetAsyncKeyState(VK_F2))
		{
			if(!key_down)
			{
				key_down = true;
	//			mesh_obj->SendImmediate(GASS::MessagePtr(new GASS::MeshFileMessage("car.3ds")));
			}
		}

		else if(GetAsyncKeyState(VK_F3))
		{
			if(!key_down)
			{
				key_down = true;
				GASS::SceneObjectPtr rr_wheel = vehicle_obj->GetChildByID("RR_WHEEL");
				rr_wheel->PostMessage(GASS::MessagePtr(new GASS::PhysicsJointMessage(GASS::PhysicsJointMessage::AXIS2_VELOCITY,10)));
			
				GASS::SceneObjectPtr rl_wheel = vehicle_obj->GetChildByID("RL_WHEEL");
				rl_wheel->PostMessage(GASS::MessagePtr(new GASS::PhysicsJointMessage(GASS::PhysicsJointMessage::AXIS2_VELOCITY,10)));
			
			}
		}
		else
			key_down = false;
	}
	return 0;
}


