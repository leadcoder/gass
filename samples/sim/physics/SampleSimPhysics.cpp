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
#include "Sim/Interface/GASSIPhysicsBodyComponent.h"
#include "Sim/Interface/GASSIPhysicsSuspensionComponent.h"
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

int _getch() {
	struct termios oldt,
		newt;
	int            ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;

}
#endif

std::vector<std::string> GetCommonPlugins()
{
	std::vector<std::string> plugins;
	plugins.push_back("GASSPluginOSG");
	plugins.push_back("GASSPluginOIS");
	plugins.push_back("GASSPluginOpenAL");
	plugins.push_back("GASSPluginInput");
	plugins.push_back("GASSPluginBase");
	plugins.push_back("GASSPluginODE");
	return plugins;
}

std::vector<std::string> GetCommonSystems()
{
	std::vector<std::string> systems;
	systems.push_back("OSGGraphicsSystem");
	systems.push_back("MaterialSystem");
	systems.push_back("OISInputSystem");
	systems.push_back("ControlSettingsSystem");
	systems.push_back("CoreSystem");
	systems.push_back("SimulationSystem");
	//systems.push_back("ODECollisionSystem");
	return systems;
}

int main(int/*argc*/, char* /*argv[]*/)
{
	//Load plugins
	std::cout << "Select physics system, press [1] for ODE , [2] for PhysX";
	char gfx_key = static_cast<char>(_getch());

	GASS::SimEngineConfig config;
	config.Plugins = GetCommonPlugins();
	std::vector<std::string> systems = GetCommonSystems();

	bool use_ode = gfx_key == 1;
	if (use_ode)
	{
		systems.push_back("ODEPhysicsSystem");
	}
	else
	{
		config.Plugins.push_back("GASSPluginPhysX");
		systems.push_back("PhysXPhysicsSystem");
	}

	for (auto system_name : systems)
	{
		GASS::SimSystemConfig sysc;
		sysc.Name = system_name;
		config.SimSystemManager.Systems.push_back(sysc);
	}

	config.DataPath = "../../data/";
	config.ResourceConfig.ResourceLocations.push_back(GASS::ResourceLocationConfig("GASS_GFX", "%GASS_DATA_HOME%/gfx",true));
	config.ResourceConfig.ResourceLocations.push_back(GASS::ResourceLocationConfig("GASS_PHYSICS", "%GASS_DATA_HOME%/physics", true));
	config.ResourceConfig.ResourceLocations.push_back(GASS::ResourceLocationConfig("GASS_INPUT", "%GASS_DATA_HOME%/input", true));
	config.ResourceConfig.ResourceLocations.push_back(GASS::ResourceLocationConfig("GASS_FONTS", "%GASS_DATA_HOME%/gfx/Ogre/Fonts", true));
	config.ResourceConfig.ResourceLocations.push_back(GASS::ResourceLocationConfig("GASS_TEMPLATES", "%GASS_DATA_HOME%/templates/camera", true));
	
	GASS::SimEngine* m_Engine = new GASS::SimEngine();

	m_Engine->Init(config);
	
	GASS::GraphicsSystemPtr gfx_sys = m_Engine->GetSimSystemManager()->GetFirstSystemByClass<GASS::IGraphicsSystem>();

	GASS::RenderWindowPtr win = gfx_sys->CreateRenderWindow("MainWindow", 800, 600);
	win->CreateViewport("MainViewport", 0, 0, 1, 1);

	GASS::InputSystemPtr input_system = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::IInputSystem>();
	input_system->SetMainWindowHandle(win->GetHWND());

	GASS::ScenePtr scene(m_Engine->CreateScene("NewScene"));

	{
		GASS::SceneObjectTemplatePtr plane_template(new GASS::SceneObjectTemplate);
		plane_template->SetName("PlaneObject");
		plane_template->AddBaseSceneComponent("LocationComponent");
		plane_template->AddBaseSceneComponent("ManualMeshComponent");
		plane_template->AddBaseSceneComponent("PhysicsPlaneGeometryComponent");
		GASS::ComponentPtr plane_comp = plane_template->AddBaseSceneComponent("PlaneGeometryComponent");
		plane_comp->SetPropertyValue<GASS::Vec2>("Size", GASS::Vec2(100, 100));
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(plane_template);
	}
	{
		GASS::SceneObjectTemplatePtr light_template(new GASS::SceneObjectTemplate);
		light_template->SetName("LightObject");
		light_template->AddBaseSceneComponent("LocationComponent");
		GASS::BaseSceneComponentPtr light_comp = light_template->AddBaseSceneComponent("LightComponent");

		light_comp->SetPropertyValue("DiffuseColor", GASS::ColorRGB(0.5, 0.5, 0.5));
		light_comp->SetPropertyValue("AmbientColor", GASS::ColorRGB(0.2, 0.2, 0.2));

		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(light_template);
	}
	{
		{
			GASS::SceneObjectTemplatePtr vehicle_template(new GASS::SceneObjectTemplate);
			vehicle_template->SetName("VehicleObject");
			vehicle_template->SetID("VEHICLE");
			vehicle_template->AddBaseSceneComponent("LocationComponent");
			GASS::BaseSceneComponentPtr mmc = vehicle_template->AddBaseSceneComponent("ManualMeshComponent");
			mmc->SetPropertyValue("CastShadow", true);

			GASS::ComponentPtr body_comp = vehicle_template->AddBaseSceneComponent("PhysicsBodyComponent");
			body_comp->SetPropertyValue("Mass", 1.0f);
			
			GASS::ComponentPtr geom_comp = vehicle_template->AddBaseSceneComponent("PhysicsBoxGeometryComponent");
			geom_comp->SetPropertyValue<GASS::Vec3>("Offset", GASS::Vec3(0, -0.9, 0));
			geom_comp->SetPropertyValue<GASS::Vec3>("Size", GASS::Vec3(2, 1, 4));
			geom_comp->SetPropertyValue<bool>("SizeFromMesh", false);
			
			
			GASS::BaseSceneComponentPtr box_comp = vehicle_template->AddBaseSceneComponent("BoxGeometryComponent");
			box_comp->SetPropertyValue("Size", GASS::Vec3(2, 1, 4));
			box_comp->SetPropertyValue("Lines", false);

			GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(vehicle_template);

			//add wheels
			GASS::SceneObjectTemplatePtr wheel_template;
			GASS::Float offset = -1.4;

			wheel_template = GASS::SceneObjectTemplatePtr(new GASS::SceneObjectTemplate);
			wheel_template->SetName("FrontLeftWheel");
			wheel_template->SetID("FL_WHEEL");
			wheel_template->SetInheritance("WheelObject");
			GASS::ComponentPtr location_comp = wheel_template->AddBaseSceneComponent("LocationComponent");
			GASS_DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(-1, offset, -1));
			vehicle_template->AddChild(wheel_template);

			wheel_template = GASS::SceneObjectTemplatePtr(new GASS::SceneObjectTemplate);
			wheel_template->SetName("FrontRightWheel");
			wheel_template->SetID("FR_WHEEL");
			wheel_template->SetInheritance("WheelObject");
			location_comp = wheel_template->AddBaseSceneComponent("LocationComponent");
			GASS_DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(1, offset, -1));
			vehicle_template->AddChild(wheel_template);


			wheel_template = GASS::SceneObjectTemplatePtr(new GASS::SceneObjectTemplate);
			wheel_template->SetName("RearLeftWheel");
			wheel_template->SetID("RL_WHEEL");
			wheel_template->SetInheritance("WheelObject");
			location_comp = wheel_template->AddComponent("LocationComponent");
			GASS_DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(-1, offset, 1));
			vehicle_template->AddChild(wheel_template);

			wheel_template = GASS::SceneObjectTemplatePtr(new GASS::SceneObjectTemplate);
			wheel_template->SetName("RearRightWheel");
			wheel_template->SetID("RR_WHEEL");
			wheel_template->SetInheritance("WheelObject");
			location_comp = wheel_template->AddBaseSceneComponent("LocationComponent");
			GASS_DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(1, offset, 1));
			vehicle_template->AddChild(wheel_template);
		}

		{
			GASS::SceneObjectTemplatePtr wheel_template(new GASS::SceneObjectTemplate);
			wheel_template->SetName("WheelObject");
			wheel_template->SetID("WHEEL");
			wheel_template->AddBaseSceneComponent("LocationComponent");
			GASS::BaseSceneComponentPtr sphere_comp = wheel_template->AddBaseSceneComponent("SphereGeometryComponent");
			sphere_comp->SetPropertyValue<GASS::Float>("Radius", 0.3);
			GASS::BaseSceneComponentPtr psphere_comp = wheel_template->AddBaseSceneComponent("PhysicsSphereGeometryComponent");
			psphere_comp->SetPropertyValue<bool>("SizeFromMesh", false);
			psphere_comp->SetPropertyValue<GASS::Float>("Radius", 0.3);
			GASS::BaseSceneComponentPtr body_comp = wheel_template->AddBaseSceneComponent("PhysicsBodyComponent");
			body_comp->SetPropertyValue<float>("Mass", 0.1f);
			wheel_template->AddBaseSceneComponent("ManualMeshComponent");
			//wheel_template->AddComponent("PhysicsHingeComponent");
			GASS::BaseSceneComponentPtr susp_comp = wheel_template->AddBaseSceneComponent("PhysicsSuspensionComponent");
			susp_comp->SetPropertyValue<float>("Damping", 8.0f);
			susp_comp->SetPropertyValue<float>("Strength", 50.0f);
			
			if(use_ode)
			{
				susp_comp->SetPropertyValue<float>("HighStop", 1.0f);
				susp_comp->SetPropertyValue<float>("LowStop", -1.0f);
			}
			else
				susp_comp->SetPropertyValue<float>("SteerLimit", 1.0f);

			GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(wheel_template);
		}
	}

	{
		GASS::SceneObjectTemplatePtr box_template(new GASS::SceneObjectTemplate);
		box_template->SetName("BoxObject");
		box_template->AddBaseSceneComponent("LocationComponent");
		GASS::BaseSceneComponentPtr mmc = box_template->AddBaseSceneComponent("ManualMeshComponent");
		mmc->SetPropertyValue("CastShadow", true);
		box_template->AddBaseSceneComponent("PhysicsBoxGeometryComponent");
		box_template->AddBaseSceneComponent("PhysicsBodyComponent");
		GASS::BaseSceneComponentPtr box_comp = box_template->AddBaseSceneComponent("BoxGeometryComponent");
		box_comp->SetPropertyValue("Size", GASS::Vec3(1, 1, 1));
		box_comp->SetPropertyValue("Lines", false);

		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(box_template);
	}

	{
		//Create bridge templates
		GASS::SceneObjectTemplatePtr bridge_seg_template(new GASS::SceneObjectTemplate);
		bridge_seg_template->SetName("BridgeSegment");
		bridge_seg_template->AddBaseSceneComponent("LocationComponent");
		GASS::BaseSceneComponentPtr mmc = bridge_seg_template->AddBaseSceneComponent("ManualMeshComponent");
		mmc->SetPropertyValue("CastShadow", true);

		bridge_seg_template->AddBaseSceneComponent("PhysicsBoxGeometryComponent");
		bridge_seg_template->AddBaseSceneComponent("PhysicsBodyComponent");
		GASS::BaseSceneComponentPtr box_comp = bridge_seg_template->AddBaseSceneComponent("BoxGeometryComponent");
		box_comp->SetPropertyValue("Size", GASS::Vec3(2, 0.3, 0.6));
		box_comp->SetPropertyValue("Lines", false);

		//test
		//GASS::BaseSceneComponentPtr script_comp  = bridge_seg_template->AddBaseSceneComponent("ASScriptComponent");
		//std::string script ="c:/temp/test.lua";
		//script_comp->SetPropertyValue("Script",script);

		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(bridge_seg_template);

		GASS::SceneObjectTemplatePtr bridge_hinge_template(new GASS::SceneObjectTemplate);
		bridge_hinge_template->SetName("BridgeHinge");
		bridge_hinge_template->AddBaseSceneComponent("PhysicsHingeComponent");
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(bridge_hinge_template);
	}

	{
		GASS::SceneObjectTemplatePtr mesh_template(new GASS::SceneObjectTemplate);
		mesh_template->SetName("MeshObject");
		mesh_template->AddBaseSceneComponent("LocationComponent");
		mesh_template->AddBaseSceneComponent("MeshComponent");
		mesh_template->AddBaseSceneComponent("PhysicsBoxGeometryComponent");
		mesh_template->AddBaseSceneComponent("PhysicsBodyComponent");
		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(mesh_template);
	}

	GASS::SceneObjectPtr terrain_obj = scene->LoadObjectFromTemplate("PlaneObject", scene->GetRootSceneObject());
	//terrain_obj->SendImmediateRequest(GASS::PositionRequestPtr(new GASS::PositionRequest(GASS::Vec3(0, 0, 0))));
	//terrain_obj->SendImmediate(GASS::MessagePtr(new GASS::MeshFileRequest("terrain.3DS")));

	GASS::SceneObjectPtr light_obj = scene->LoadObjectFromTemplate("LightObject", scene->GetRootSceneObject());
	GASS::Quaternion l_rot = GASS::EulerRotation(42, 32, 0).GetQuaternion();
	light_obj->SendImmediateRequest(GASS::RotationRequestPtr(new GASS::RotationRequest(l_rot)));

	//if (false) //create bridge
	{
		GASS::SceneObjectPtr bdrige_seg_obj2 = scene->LoadObjectFromTemplate("BridgeSegment", scene->GetRootSceneObject());
		bdrige_seg_obj2->GetFirstComponentByClass<GASS::ILocationComponent>()->SetPosition(GASS::Vec3(10, 2, 0));

		GASS::BaseSceneComponentPtr body_comp = bdrige_seg_obj2->GetBaseSceneComponent("PhysicsBodyComponent");
		try
		{
			body_comp->SetPropertyValue("Kinematic", true);
		}
		catch (...) {}
		for (int i = 1; i < 11; i++)
		{
			GASS::SceneObjectPtr bdrige_seg_obj1 = bdrige_seg_obj2;
			bdrige_seg_obj2 = scene->LoadObjectFromTemplate("BridgeSegment", scene->GetRootSceneObject());
			bdrige_seg_obj2->GetFirstComponentByClass<GASS::ILocationComponent>()->SetPosition(GASS::Vec3(10, 2, i));

			GASS::SceneObjectPtr bdrige_hinge_obj = scene->LoadObjectFromTemplate("BridgeHinge", scene->GetRootSceneObject());
			GASS::BaseSceneComponentPtr hinge_comp = bdrige_hinge_obj->GetBaseSceneComponent("PhysicsHingeComponent");
			if (i == 1)
			{
				hinge_comp->SetPropertyValue("Body1", GASS::SceneObjectRef(bdrige_seg_obj2->GetGUID()));
				hinge_comp->SetPropertyValue("Body2", GASS::SceneObjectRef(bdrige_seg_obj1->GetGUID()));
			}
			else
			{
				hinge_comp->SetPropertyValue("Body1", GASS::SceneObjectRef(bdrige_seg_obj1->GetGUID()));
				hinge_comp->SetPropertyValue("Body2", GASS::SceneObjectRef(bdrige_seg_obj2->GetGUID()));
			}
		}
		body_comp = bdrige_seg_obj2->GetBaseSceneComponent("PhysicsBodyComponent");


		try
		{
			body_comp->SetPropertyValue("Kinematic", true);
		}
		catch (...) {}
		GASS::SceneObjectPtr box_obj = scene->LoadObjectFromTemplate("BoxObject", scene->GetRootSceneObject());
		box_obj->GetFirstComponentByClass<GASS::ILocationComponent>()->SetPosition(GASS::Vec3(10, 0.6, 2));

		GASS::SceneObjectPtr box_obj2 = scene->LoadObjectFromTemplate("BoxObject", scene->GetRootSceneObject());
		box_obj2->GetFirstComponentByClass<GASS::ILocationComponent>()->SetPosition(GASS::Vec3(10, 0.6, 20));
	}


	GASS::SceneObjectPtr vehicle_obj = scene->LoadObjectFromTemplate("VehicleObject", scene->GetRootSceneObject());
	vehicle_obj->GetFirstComponentByClass<GASS::ILocationComponent>()->SetPosition(GASS::Vec3(0, 3, 5));

	//GASS::SceneObjectPtr car_obj = scene->LoadObjectFromTemplate("PxCar",scene->GetRootSceneObject());
	//car_obj->SendImmediate(GASS::MessagePtr(new GASS::PositionRequest(GASS::Vec3(0,3,0))));

	//create free camera and set start pos
	GASS::SceneObjectPtr free_obj = scene->LoadObjectFromTemplate("FreeCameraObject", scene->GetRootSceneObject());
	if (free_obj)
	{
		free_obj->GetFirstComponentByClass<GASS::ILocationComponent>()->SetPosition(GASS::Vec3(0, 2, 0));
		free_obj->GetFirstComponentByClass<GASS::ICameraComponent>()->ShowInViewport();
	}

	static float wheel_vel = 0;
	static float steer_vel = 0;

	GASS::BaseSceneManagerPtr ogre_sm = GASS_DYNAMIC_PTR_CAST<GASS::BaseSceneManager>(scene->GetSceneManagerByName("OgreGraphicsSceneManager"));
	if (ogre_sm)
	{
		/*ogre_sm->SetPropertyByString("ShadowMode","SHADOWS_DISABLED");
		ogre_sm->SetPropertyByString("ShadowMode","TEXTURE_SHADOWS_MODULATIVE");

		//ogre_sm->SetPropertyByString("ShadowMode","TEXTURE_SHADOWS_ADDITIVE");
		ogre_sm->SetPropertyByString("TextureShadowProjection","UNIFORM_FOCUSED");
		ogre_sm->SetPropertyValue("SelfShadowing",false);*/
	}

	GASS::SceneObjectPtr box_obj;
	while (true)
	{
		
		m_Engine->Update();
		static bool key_down = false;
		if (GetAsyncKeyState(VK_SPACE))
		{
			if (!key_down)
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
				GASS::Vec3 torq(0, 0, 2000);
				torq = rot * torq;
				box_obj = scene->LoadObjectFromTemplate("BoxObject", scene->GetRootSceneObject());
				box_obj->GetFirstComponentByClass<GASS::ILocationComponent>()->SetPosition(pos);
				box_obj->GetFirstComponentByClass<GASS::ILocationComponent>()->SetRotation(rot);
				box_obj->GetFirstComponentByClass<GASS::IPhysicsBodyComponent>()->AddForce(vel);
				box_obj->GetFirstComponentByClass<GASS::IPhysicsBodyComponent>()->AddTorque(torq);

			}
		}
		else if (GetAsyncKeyState(VK_DELETE))
		{
			if (box_obj)
			{
				scene->GetRootSceneObject()->RemoveChild(box_obj);
				box_obj = GASS::SceneObjectPtr();
			}
		}
		else if (GetAsyncKeyState(VK_F1))
		{
			if (!key_down)
			{
				key_down = true;
				//				mesh_obj->SendImmediate(GASS::MessagePtr(new GASS::MeshFileRequest("wheel.3ds")));
			}
		}
		else if (GetAsyncKeyState(VK_F2))
		{
			if (!key_down)
			{
				key_down = true;
				//			mesh_obj->SendImmediate(GASS::MessagePtr(new GASS::MeshFileRequest("car.3ds")));
			}
		}
		else if (GetAsyncKeyState(VK_F3))
		{
			if (!key_down)
			{
				key_down = true;
				//bdrige_seg_obj2->PostMessage(GASS::MessagePtr(new GASS::MaterialMessage(GASS::Vec4(1,1,1,1),GASS::Vec3(1,1,1))));
			}
		}
		else if (GetAsyncKeyState(VK_F4))
		{
			if (!key_down)
			{
				key_down = true;
				//bdrige_seg_obj2->PostRequest(GASS::ResetMaterialRequestPtr(new GASS::ResetMaterialRequest()));
			}
		}
		else
		{
			key_down = false;
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
				ogre_sm->SetPropertyValue("SelfShadowing",false);

			}
		}*/




		if (GetAsyncKeyState(VK_DOWN))
		{
			wheel_vel -= 2;
		}
		if (GetAsyncKeyState(VK_UP))
		{
			wheel_vel += 2;
		}
		if (GetAsyncKeyState(VK_LEFT))
		{
			steer_vel = 2;
		}
		else if (GetAsyncKeyState(VK_RIGHT))
		{
			steer_vel = -2;
		}
		else
		{
			steer_vel = 0;
		}

		wheel_vel *= 0.9f;

		if (wheel_vel > 200)
			wheel_vel = 200;

		if (fabs(wheel_vel) < 0.0001)
			wheel_vel = 0;

		if (wheel_vel < -200)
			wheel_vel = -200;

		const float max_torq = 1;
		GASS::SceneObjectPtr rr_wheel = vehicle_obj->GetChildByID("RR_WHEEL");
		rr_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetDriveVelocity(wheel_vel);
		rr_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetMaxDriveTorque(max_torq);
		rr_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetAngularSteerVelocity(0);
		rr_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetMaxSteerTorque(100);


		GASS::SceneObjectPtr rl_wheel = vehicle_obj->GetChildByID("RL_WHEEL");
		rl_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetDriveVelocity(wheel_vel);
		rl_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetMaxDriveTorque(max_torq);
		rl_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetAngularSteerVelocity(0);
		rl_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetMaxSteerTorque(100);

		GASS::SceneObjectPtr fr_wheel = vehicle_obj->GetChildByID("FR_WHEEL");
		fr_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetDriveVelocity(wheel_vel);
		fr_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetMaxDriveTorque(0);
		fr_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetAngularSteerVelocity(steer_vel);
		fr_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetMaxSteerTorque(100);

		GASS::SceneObjectPtr fl_wheel = vehicle_obj->GetChildByID("FL_WHEEL");
		fl_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetDriveVelocity(wheel_vel);
		fl_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetMaxDriveTorque(0);
		fl_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetAngularSteerVelocity(steer_vel);
		fl_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetMaxSteerTorque(100);

	}
	return 0;
}
