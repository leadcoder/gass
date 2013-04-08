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

	GASS::SceneObjectTemplatePtr plane_template (new GASS::SceneObjectTemplate);
	plane_template->SetName("PlaneObject");
	GASS::ComponentPtr location_comp (GASS::ComponentFactory::Get().Create("LocationComponent"));
	GASS::ComponentPtr mesh_comp (GASS::ComponentFactory::Get().Create("ManualMeshComponent"));
	GASS::ComponentPtr plane_comp (GASS::ComponentFactory::Get().Create("PhysicsPlaneGeometryComponent"));
	GASS::BaseComponentPtr plane_comp2  = DYNAMIC_PTR_CAST<GASS::BaseComponent>(GASS::ComponentFactory::Get().Create("PlaneGeometryComponent"));
	plane_comp2 ->SetPropertyByType("Size",GASS::Vec2(100,100));
	
	
	plane_template->AddComponent(location_comp);
	plane_template->AddComponent(mesh_comp);
	plane_template->AddComponent(plane_comp);
	plane_template->AddComponent(plane_comp2);
	GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(plane_template );

	GASS::SceneObjectTemplatePtr light_template (new GASS::SceneObjectTemplate);
	light_template ->SetName("LightObject");
	location_comp  =  GASS::ComponentPtr(GASS::ComponentFactory::Get().Create("LocationComponent"));
	GASS::ComponentPtr light_comp (GASS::ComponentFactory::Get().Create("LightComponent"));
	light_template->AddComponent(location_comp);
	light_template->AddComponent(light_comp);
	GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(light_template );

	{
		GASS::SceneObjectTemplatePtr box_template (new GASS::SceneObjectTemplate);
		box_template->SetName("BoxObject");
		GASS::ComponentPtr location_comp (GASS::ComponentFactory::Get().Create("LocationComponent"));
		GASS::ComponentPtr mesh_comp (GASS::ComponentFactory::Get().Create("ManualMeshComponent"));
		GASS::ComponentPtr pbox_comp (GASS::ComponentFactory::Get().Create("PhysicsBoxGeometryComponent"));
		GASS::ComponentPtr body_comp (GASS::ComponentFactory::Get().Create("PhysicsBodyComponent"));
		GASS::BaseComponentPtr box_comp  = DYNAMIC_PTR_CAST<GASS::BaseComponent>(GASS::ComponentFactory::Get().Create("BoxGeometryComponent"));
		box_comp->SetPropertyByType("Size",GASS::Vec3(1,1,1));
		box_comp->SetPropertyByType("Lines",false);
		box_template->AddComponent(location_comp);
		box_template->AddComponent(mesh_comp);
		box_template->AddComponent(pbox_comp);
		box_template->AddComponent(body_comp);
		box_template->AddComponent(box_comp);
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(box_template);
	}

	{
		GASS::SceneObjectTemplatePtr mesh_template (new GASS::SceneObjectTemplate);
		mesh_template->SetName("MeshObject");
		GASS::ComponentPtr location_comp (GASS::ComponentFactory::Get().Create("LocationComponent"));
		GASS::ComponentPtr mesh_comp (GASS::ComponentFactory::Get().Create("MeshComponent"));
		GASS::ComponentPtr pbox_comp (GASS::ComponentFactory::Get().Create("PhysicsBoxGeometryComponent"));
		GASS::ComponentPtr body_comp (GASS::ComponentFactory::Get().Create("PhysicsBodyComponent"));
		mesh_template->AddComponent(location_comp);
		mesh_template->AddComponent(mesh_comp);
		mesh_template->AddComponent(pbox_comp);
		mesh_template->AddComponent(body_comp);
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(mesh_template);
	}

	GASS::SceneObjectPtr terrain_obj = scene->LoadObjectFromTemplate("PlaneObject",scene->GetRootSceneObject());
	terrain_obj->SendImmediate(GASS::MessagePtr(new GASS::PositionMessage(GASS::Vec3(0,0,0))));
	//terrain_obj->SendImmediate(GASS::MessagePtr(new GASS::MeshFileMessage("terrain.3DS")));


	GASS::SceneObjectPtr light_obj = scene->LoadObjectFromTemplate("LightObject",scene->GetRootSceneObject());
	light_obj->SendImmediate(GASS::MessagePtr(new GASS::RotationMessage(GASS::Vec3(40,32,0))));
	
	
	GASS::SceneObjectPtr box_obj = scene->LoadObjectFromTemplate("BoxObject",scene->GetRootSceneObject());
	box_obj->SendImmediate(GASS::MessagePtr(new GASS::PositionMessage(GASS::Vec3(0,2,0))));
	
	GASS::SceneObjectPtr mesh_obj = scene->LoadObjectFromTemplate("MeshObject",scene->GetRootSceneObject());
	mesh_obj->SendImmediate(GASS::MessagePtr(new GASS::MeshFileMessage("car.3ds")));
	mesh_obj->SendImmediate(GASS::MessagePtr(new GASS::PositionMessage(GASS::Vec3(0,5,1))));
	
	//GASS::ScenePtr scene = GASS::ScenePtr(m_Scene);
	//scene->Load(m_SceneName);
	
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
				rot.ToRotationMatrix(rot_mat);
				GASS::Vec3 vel = rot_mat.GetViewDirVector()*-25;
				GASS::SceneObjectPtr box_obj = scene->LoadObjectFromTemplate("BoxObject",scene->GetRootSceneObject());
				box_obj->SendImmediate(GASS::MessagePtr(new GASS::WorldPositionMessage(pos)));
				box_obj->SendImmediate(GASS::MessagePtr(new GASS::PhysicsVelocityRequest(vel)));
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
				mesh_obj->SendImmediate(GASS::MessagePtr(new GASS::MeshFileMessage("wheel.3ds")));
			}
		}
		else if(GetAsyncKeyState(VK_F2))
		{
			if(!key_down)
			{
				key_down = true;
				mesh_obj->SendImmediate(GASS::MessagePtr(new GASS::MeshFileMessage("car.3ds")));
			}
		}
		else
			key_down = false;
	}
	return 0;
}


