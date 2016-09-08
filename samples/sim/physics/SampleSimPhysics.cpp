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
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Core/PluginSystem/GASSPluginManager.h"

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
	//Load plugins

	std::cout << "Select render system, press [1] for Ogre , [2] for OSG";
	char key = _getch();
	std::string gfx_plugin = "GASSPluginOgre";
	std::string gfx_system_name = "OgreGraphicsSystem";

	if(key == '1')
	{
		gfx_system_name = "OgreGraphicsSystem";
		gfx_plugin = "GASSPluginOgre";
	}
	else if(key == '2')
	{
		gfx_system_name = "OSGGraphicsSystem";
		gfx_plugin = "GASSPluginOSG";
	}

	std::vector<std::string> plugins;
	plugins.push_back(gfx_plugin);
	plugins.push_back("GASSPluginOIS");
	plugins.push_back("GASSPluginODE");
	plugins.push_back("GASSPluginOpenAL");
	plugins.push_back("GASSPluginInput");
	plugins.push_back("GASSPluginBase");
	plugins.push_back("GASSPluginPhysX3");

	std::vector<std::string> systems;
	systems.push_back(gfx_system_name);
	systems.push_back("MaterialSystem");
	systems.push_back("OISInputSystem");
	systems.push_back("ControlSettingsSystem");
	systems.push_back("CoreSystem");
	systems.push_back("SimulationSystem");
	systems.push_back("PhysXPhysicsSystem");
	systems.push_back("ODECollisionSystem");
	
	GASS::SimEngine* m_Engine = new GASS::SimEngine();

	GASS::ResourceGroupPtr gfx_group(new GASS::ResourceGroup("GASS_GFX"));
	gfx_group->AddResourceLocation(GASS::FilePath("../../data/gfx"),GASS::RLT_FILESYSTEM,true);
	m_Engine->GetResourceManager()->AddResourceGroup(gfx_group);

	GASS::ResourceGroupPtr physics_group(new GASS::ResourceGroup("GASS_PHYSICS"));
	physics_group->AddResourceLocation(GASS::FilePath("../../data/physics"),GASS::RLT_FILESYSTEM,true);
	m_Engine->GetResourceManager()->AddResourceGroup(physics_group);

	GASS::ResourceGroupPtr input_group(new GASS::ResourceGroup("GASS_INPUT"));
	input_group->AddResourceLocation(GASS::FilePath("../../data/input"),GASS::RLT_FILESYSTEM,true);
	m_Engine->GetResourceManager()->AddResourceGroup(input_group);

	GASS::ResourceGroupPtr font_group(new GASS::ResourceGroup("GASS_FONTS"));
	font_group->AddResourceLocation(GASS::FilePath("../../data/gfx/Ogre/Fonts"), GASS::RLT_FILESYSTEM, true);
	m_Engine->GetResourceManager()->AddResourceGroup(font_group);


	GASS::ResourceGroupPtr temp_group(new GASS::ResourceGroup("GASS_TEMPLATES"));
	temp_group->AddResourceLocation(GASS::FilePath("../../data/templates/camera"),GASS::RLT_FILESYSTEM,true);
	m_Engine->GetResourceManager()->AddResourceGroup(temp_group);
	
	m_Engine->Init(GASS::FilePath("SampleSimPhysics.xml"));

	//Create systems
	for(size_t i = 0 ; i < plugins.size();i++)
	{
		m_Engine->GetPluginManager()->LoadPlugin(plugins[i]);
	}

	for(size_t i = 0 ; i < systems.size();i++)
	{
		GASS::SimSystemPtr system = GASS::SystemFactory::Get().Create(systems[i]);
		system->OnCreate(m_Engine->GetSimSystemManager());
		system->Init();
		system->RegisterForUpdate();
		m_Engine->GetSimSystemManager()->AddSystem(system);
	}
	
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
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(light_template);
	}
	{
		{
			GASS::SceneObjectTemplatePtr vehicle_template (new GASS::SceneObjectTemplate);
			vehicle_template->SetName("VehicleObject");
			vehicle_template->SetID("VEHICLE");
			vehicle_template->AddBaseSceneComponent("LocationComponent");
			GASS::BaseSceneComponentPtr mmc  = vehicle_template->AddBaseSceneComponent("ManualMeshComponent");
			mmc->SetPropertyByType("CastShadow",true);

			GASS::ComponentPtr body_comp = vehicle_template->AddBaseSceneComponent("PhysicsBodyComponent");
			body_comp->SetPropertyByType("Mass",1.0f);
			GASS::ComponentPtr geom_comp = vehicle_template->AddBaseSceneComponent("PhysicsBoxGeometryComponent");
			geom_comp->SetPropertyByType("Offset",GASS::Vec3(0,-0.9,0));
			GASS::BaseSceneComponentPtr box_comp  = vehicle_template->AddBaseSceneComponent("BoxGeometryComponent");
			box_comp->SetPropertyByType("Size",GASS::Vec3(2,1,4));
			box_comp->SetPropertyByType("Lines",false);
	
			GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(vehicle_template);

			//add wheels
			GASS::SceneObjectTemplatePtr wheel_template;
			GASS::Float offset = -1.4;

			wheel_template =  GASS::SceneObjectTemplatePtr(new GASS::SceneObjectTemplate);
			wheel_template->SetName("FrontLeftWheel");
			wheel_template->SetID("FL_WHEEL");
			wheel_template->SetInheritance("WheelObject");
			GASS::ComponentPtr location_comp  = wheel_template->AddBaseSceneComponent("LocationComponent");
			GASS_DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(-1,offset,-1));
			vehicle_template->AddChild(wheel_template);

			wheel_template =  GASS::SceneObjectTemplatePtr(new GASS::SceneObjectTemplate);
			wheel_template->SetName("FrontRightWheel");
			wheel_template->SetID("FR_WHEEL");
			wheel_template->SetInheritance("WheelObject");
			location_comp  = wheel_template->AddBaseSceneComponent("LocationComponent");
			GASS_DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(1,offset,-1));
			vehicle_template->AddChild(wheel_template);

			
			wheel_template =  GASS::SceneObjectTemplatePtr(new GASS::SceneObjectTemplate);
			wheel_template->SetName("RearLeftWheel");
			wheel_template->SetID("RL_WHEEL");
			wheel_template->SetInheritance("WheelObject");
			location_comp  = wheel_template->AddComponent("LocationComponent");
			GASS_DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(-1,offset,1));
			vehicle_template->AddChild(wheel_template);

			wheel_template =  GASS::SceneObjectTemplatePtr(new GASS::SceneObjectTemplate);
			wheel_template->SetName("RearRightWheel");
			wheel_template->SetID("RR_WHEEL");
			wheel_template->SetInheritance("WheelObject");
			location_comp  = wheel_template->AddBaseSceneComponent("LocationComponent");
			GASS_DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(1,offset,1));
			vehicle_template->AddChild(wheel_template);
		}

		{
			GASS::SceneObjectTemplatePtr wheel_template (new GASS::SceneObjectTemplate);
			wheel_template->SetName("WheelObject");
			wheel_template->SetID("WHEEL");
			wheel_template->AddBaseSceneComponent("LocationComponent");
			GASS::BaseSceneComponentPtr sphere_comp  = wheel_template->AddBaseSceneComponent("SphereGeometryComponent");
			sphere_comp->SetPropertyByType("Radius",0.3);
			wheel_template->AddBaseSceneComponent("PhysicsSphereGeometryComponent");
			GASS::BaseSceneComponentPtr body_comp = wheel_template->AddBaseSceneComponent("PhysicsBodyComponent");
			body_comp->SetPropertyByType("Mass",0.1f);
			wheel_template->AddBaseSceneComponent("ManualMeshComponent");
			//wheel_template->AddComponent("PhysicsHingeComponent");
			GASS::BaseSceneComponentPtr susp_comp = wheel_template->AddBaseSceneComponent("PhysicsSuspensionComponent");
			susp_comp->SetPropertyByType("Damping",300.0f);
			susp_comp->SetPropertyByType("Strength",3000.0f);
			GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(wheel_template);
		}
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
		//Create bridge templates
		GASS::SceneObjectTemplatePtr bridge_seg_template (new GASS::SceneObjectTemplate);
		bridge_seg_template->SetName("BridgeSegment");
		bridge_seg_template->AddBaseSceneComponent("LocationComponent");
		GASS::BaseSceneComponentPtr mmc  = bridge_seg_template->AddBaseSceneComponent("ManualMeshComponent");
		mmc->SetPropertyByType("CastShadow",true);

		bridge_seg_template->AddBaseSceneComponent("PhysicsBoxGeometryComponent");
		bridge_seg_template->AddBaseSceneComponent("PhysicsBodyComponent");
		GASS::BaseSceneComponentPtr box_comp  = bridge_seg_template->AddBaseSceneComponent("BoxGeometryComponent");
		box_comp->SetPropertyByType("Size",GASS::Vec3(2,0.3,0.6));
		box_comp->SetPropertyByType("Lines",false);

		//test
		//GASS::BaseSceneComponentPtr script_comp  = bridge_seg_template->AddBaseSceneComponent("ASScriptComponent");
		//std::string script ="c:/temp/test.lua";
		//script_comp->SetPropertyByType("Script",script);
	
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(bridge_seg_template);

		GASS::SceneObjectTemplatePtr bridge_hinge_template (new GASS::SceneObjectTemplate);
		bridge_hinge_template->SetName("BridgeHinge");
		bridge_hinge_template->AddBaseSceneComponent("PhysicsHingeComponent");
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(bridge_hinge_template);
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
	terrain_obj->SendImmediateRequest(GASS::PositionRequestPtr(new GASS::PositionRequest(GASS::Vec3(0,0,0))));
	//terrain_obj->SendImmediate(GASS::MessagePtr(new GASS::MeshFileRequest("terrain.3DS")));

	GASS::SceneObjectPtr light_obj = scene->LoadObjectFromTemplate("LightObject",scene->GetRootSceneObject());
	light_obj->SendImmediateRequest(GASS::RotationRequestPtr(new GASS::RotationRequest(GASS::Vec3(40,32,0))));
	
	GASS::SceneObjectPtr bdrige_seg_obj2 = scene->LoadObjectFromTemplate("BridgeSegment",scene->GetRootSceneObject());
	bdrige_seg_obj2->SendImmediateRequest(GASS::PositionRequestPtr(new GASS::PositionRequest(GASS::Vec3(10,2,0))));
	
	GASS::BaseSceneComponentPtr body_comp = bdrige_seg_obj2->GetBaseSceneComponent("PhysicsBodyComponent");
	try
	{
	body_comp->SetPropertyByType("Kinematic",true);
	}
	catch(...){}
	for(int i= 1 ; i < 11 ; i++)
	{
		GASS::SceneObjectPtr bdrige_seg_obj1 = bdrige_seg_obj2; 
		bdrige_seg_obj2 = scene->LoadObjectFromTemplate("BridgeSegment",scene->GetRootSceneObject());
		bdrige_seg_obj2->SendImmediateRequest(GASS::PositionRequestPtr(new GASS::PositionRequest(GASS::Vec3(10,2,i))));
		
		GASS::SceneObjectPtr bdrige_hinge_obj = scene->LoadObjectFromTemplate("BridgeHinge",scene->GetRootSceneObject());
		GASS::BaseSceneComponentPtr hinge_comp = bdrige_hinge_obj->GetBaseSceneComponent("PhysicsHingeComponent");
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
	body_comp = bdrige_seg_obj2->GetBaseSceneComponent("PhysicsBodyComponent");
	
	
	try
	{
	body_comp->SetPropertyByType("Kinematic",true);
	}
	catch(...){}
	
	/*GASS::SceneObjectPtr intersection1;
	GASS::SceneObjectPtr intersection2  = scene->LoadObjectFromTemplate("TestInter",scene->GetRootSceneObject());
	intersection2->SendImmediate(GASS::MessagePtr(new GASS::PositionRequest(GASS::Vec3(0,2,0))));
	
	for(int i = 1 ; i < 11 ; i++)
	{
		intersection1 = intersection2;
		intersection2  = scene->LoadObjectFromTemplate("TestInter",scene->GetRootSceneObject());
		intersection2->SendImmediate(GASS::MessagePtr(new GASS::PositionRequest(GASS::Vec3(0,2,i*50))));
		
		GASS::SceneObjectPtr road_seg = scene->LoadObjectFromTemplate("TestRoad",scene->GetRootSceneObject());
		GASS::BaseComponentPtr rs_comp = GASS_DYNAMIC_PTR_CAST<GASS::BaseComponent>(road_seg->GetFirstComponentByClass("RoadSegmentComponent"));
		rs_comp->SetPropertyByType("StartNode",GASS::SceneObjectRef(intersection1->GetGUID()));
		rs_comp->SetPropertyByType("EndNode",GASS::SceneObjectRef(intersection2->GetGUID()));
	}*/


	//GASS::ScenePtr scene = GASS::ScenePtr(m_Scene);
	//scene->Load(m_SceneName);


	//create bridge

	GASS::SceneObjectPtr box_obj = scene->LoadObjectFromTemplate("BoxObject",scene->GetRootSceneObject());
	box_obj->SendImmediateRequest(GASS::PositionRequestPtr(new GASS::PositionRequest(GASS::Vec3(10,0.6,2))));

	GASS::SceneObjectPtr box_obj2 = scene->LoadObjectFromTemplate("BoxObject",scene->GetRootSceneObject());
	box_obj2->SendImmediateRequest(GASS::PositionRequestPtr(new GASS::PositionRequest(GASS::Vec3(10,0.6,20))));

	
	GASS::SceneObjectPtr vehicle_obj = scene->LoadObjectFromTemplate("VehicleObject",scene->GetRootSceneObject());
	vehicle_obj->SendImmediateRequest(GASS::PositionRequestPtr(new GASS::PositionRequest(GASS::Vec3(0,3,5))));

	//GASS::SceneObjectPtr car_obj = scene->LoadObjectFromTemplate("PxCar",scene->GetRootSceneObject());
	//car_obj->SendImmediate(GASS::MessagePtr(new GASS::PositionRequest(GASS::Vec3(0,3,0))));

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

	GASS::BaseSceneManagerPtr ogre_sm = GASS_DYNAMIC_PTR_CAST<GASS::BaseSceneManager>(scene->GetSceneManagerByName("OgreGraphicsSceneManager"));
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
				GASS::Vec3 vel = rot.GetZAxis()*-2500;
/*
				GASS::Mat4 rot_mat;
				rot_mat.Identity();
				rot.ToRotationMatrix(rot_mat);
				GASS::Vec3 vel = rot_mat.GetZAxis()*-2500;
*/
				GASS::Vec3 torq(0,0,2000);
				torq = rot * torq;
				box_obj = scene->LoadObjectFromTemplate("BoxObject",scene->GetRootSceneObject());
				box_obj->SendImmediateRequest(GASS::PositionRequestPtr(new GASS::PositionRequest(pos)));
				box_obj->SendImmediateRequest(GASS::RotationRequestPtr(new GASS::RotationRequest(rot)));
				box_obj->SendImmediateRequest(GASS::PhysicsBodyAddForceRequestPtr(new GASS::PhysicsBodyAddForceRequest(vel)));
				box_obj->SendImmediateRequest(GASS::PhysicsBodyAddTorqueRequestPtr(new GASS::PhysicsBodyAddTorqueRequest(torq)));
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
//				mesh_obj->SendImmediate(GASS::MessagePtr(new GASS::MeshFileRequest("wheel.3ds")));
			}
		}
		else if(GetAsyncKeyState(VK_F2))
		{
			if(!key_down)
			{
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
				bdrige_seg_obj2->PostRequest(GASS::ResetMaterialRequestPtr(new GASS::ResetMaterialRequest()));
			}
		}

		/*else if(GetAsyncKeyState(VK_F5))
		{
			if(!key_down)
			{
				key_down = true;
				GASS::CameraComponentPtr camera = free_obj->GetFirstComponentByClass<GASS::ICameraComponent>();
				GASS::BaseSceneComponentPtr cam = GASS_DYNAMIC_PTR_CAST<GASS::BaseSceneComponent>(camera);
				cam->SetPropertyByString("MaterialScheme","FFP_STD");

				ogre_sm->SetPropertyByString("ShadowMode","SHADOWS_DISABLED");
				ogre_sm->SetPropertyByString("ShadowMode","TEXTURE_SHADOWS_MODULATIVE");

				//ogre_sm->SetPropertyByString("ShadowMode","TEXTURE_SHADOWS_ADDITIVE");
				ogre_sm->SetPropertyByString("TextureShadowProjection","UNIFORM_FOCUSED");
				ogre_sm->SetPropertyByType("SelfShadowing",false);

			}
		}*/

		
		

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

		wheel_vel *= 0.9;

		if(wheel_vel > 200)
			wheel_vel = 200;
		GASS::SceneObjectPtr rr_wheel = vehicle_obj->GetChildByID("RR_WHEEL");
		rr_wheel->PostRequest(GASS::PhysicsSuspensionJointDriveVelocityRequestPtr(new GASS::PhysicsSuspensionJointDriveVelocityRequest(wheel_vel)));
			
				
		GASS::SceneObjectPtr rl_wheel = vehicle_obj->GetChildByID("RL_WHEEL");
		rl_wheel->PostRequest(GASS::PhysicsSuspensionJointDriveVelocityRequestPtr(new GASS::PhysicsSuspensionJointDriveVelocityRequest(wheel_vel)));

		GASS::SceneObjectPtr fr_wheel = vehicle_obj->GetChildByID("FR_WHEEL");
		fr_wheel->PostRequest(GASS::PhysicsSuspensionJointSteerVelocityRequestPtr(new GASS::PhysicsSuspensionJointSteerVelocityRequest(steer_vel)));
			
		GASS::SceneObjectPtr fl_wheel = vehicle_obj->GetChildByID("FL_WHEEL");
		fl_wheel->PostRequest(GASS::PhysicsSuspensionJointSteerVelocityRequestPtr(new GASS::PhysicsSuspensionJointSteerVelocityRequest(steer_vel)));
		
	}
	return 0;
}
