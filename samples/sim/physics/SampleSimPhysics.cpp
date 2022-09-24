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
#include "Sim/GASSSceneObjectTemplateManager.h"
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

std::string CreateVehicleTemplate(GASS::SimEngine* engine, bool use_ode)
{
	GASS::SceneObjectTemplatePtr vehicle_template(new GASS::SceneObjectTemplate);
	{
		vehicle_template->SetName("VehicleObject");
		vehicle_template->SetID("VEHICLE");
		vehicle_template->AddComponent("LocationComponent");
		GASS::ComponentPtr mmc = vehicle_template->AddComponent("ManualMeshComponent");
		mmc->SetPropertyValue("CastShadow", true);

		GASS::ComponentPtr body_comp = vehicle_template->AddComponent("PhysicsBodyComponent");
		body_comp->SetPropertyValue("Mass", 1.0f);

		GASS::ComponentPtr geom_comp = vehicle_template->AddComponent("PhysicsBoxGeometryComponent");
		geom_comp->SetPropertyValue<GASS::Vec3>("Offset", GASS::Vec3(0, -0.9, 0));
		geom_comp->SetPropertyValue<GASS::Vec3>("Size", GASS::Vec3(2, 1, 4));
		geom_comp->SetPropertyValue<bool>("SizeFromMesh", false);

		GASS::ComponentPtr box_comp = vehicle_template->AddComponent("BoxGeometryComponent");
		box_comp->SetPropertyValue("Size", GASS::Vec3(2, 1, 4));
		box_comp->SetPropertyValue("Lines", false);

		engine->GetSceneObjectTemplateManager()->AddTemplate(vehicle_template);
	}
	{
		GASS::SceneObjectTemplatePtr wheel_template(new GASS::SceneObjectTemplate);
		wheel_template->SetName("WheelObject");
		wheel_template->SetID("WHEEL");
		wheel_template->AddComponent("LocationComponent");
		GASS::ComponentPtr sphere_comp = wheel_template->AddComponent("SphereGeometryComponent");
		sphere_comp->SetPropertyValue<GASS::Float>("Radius", 0.3);
		GASS::ComponentPtr psphere_comp = wheel_template->AddComponent("PhysicsSphereGeometryComponent");
		psphere_comp->SetPropertyValue<bool>("SizeFromMesh", false);
		psphere_comp->SetPropertyValue<GASS::Float>("Radius", 0.3);
		GASS::ComponentPtr body_comp = wheel_template->AddComponent("PhysicsBodyComponent");
		body_comp->SetPropertyValue<float>("Mass", 0.1f);
		wheel_template->AddComponent("ManualMeshComponent");
		//wheel_template->AddComponent("PhysicsHingeComponent");
		GASS::ComponentPtr susp_comp = wheel_template->AddComponent("PhysicsSuspensionComponent");
		susp_comp->SetPropertyValue<float>("Damping", 8.0f);
		susp_comp->SetPropertyValue<float>("Strength", 50.0f);

		if (use_ode)
		{
			susp_comp->SetPropertyValue<float>("HighStop", 1.0f);
			susp_comp->SetPropertyValue<float>("LowStop", -1.0f);
		}
		else
			susp_comp->SetPropertyValue<float>("SteerLimit", 1.0f);

		GASS::SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(wheel_template);
	}
	{
		//add wheel refs
		GASS::SceneObjectTemplatePtr wheel_template;
		GASS::Float offset = -1.4;

		wheel_template = GASS::SceneObjectTemplatePtr(new GASS::SceneObjectTemplate);
		wheel_template->SetName("FrontLeftWheel");
		wheel_template->SetID("FL_WHEEL");
		wheel_template->SetInheritance("WheelObject");
		GASS::ComponentPtr location_comp = wheel_template->AddComponent("LocationComponent");
		GASS_DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(-1, offset, -1));
		vehicle_template->AddChild(wheel_template);

		wheel_template = GASS::SceneObjectTemplatePtr(new GASS::SceneObjectTemplate);
		wheel_template->SetName("FrontRightWheel");
		wheel_template->SetID("FR_WHEEL");
		wheel_template->SetInheritance("WheelObject");
		location_comp = wheel_template->AddComponent("LocationComponent");
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
		location_comp = wheel_template->AddComponent("LocationComponent");
		GASS_DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(1, offset, 1));
		vehicle_template->AddChild(wheel_template);
	}
	return vehicle_template->GetName();
}

GASS::SceneObjectPtr CreateEnvironment()
{
	auto ground_plane = std::make_shared<GASS::SceneObject>();
	ground_plane->SetName("PlaneObject");
	ground_plane->AddComponent(GASS::ComponentFactory::Get().Create("LocationComponent"));
	auto mmc = GASS::ComponentFactory::Get().Create("ManualMeshComponent");
	ground_plane->AddComponent(mmc);
	mmc->SetPropertyValue("CastShadow", true);
	mmc->SetPropertyValue("ReceiveShadow", true);


	auto plane_geom_comp = GASS::ComponentFactory::Get().Create("PlaneGeometryComponent");
	plane_geom_comp->SetPropertyValue<GASS::Vec2>("Size", GASS::Vec2(100, 100));
	ground_plane->AddComponent(GASS::ComponentFactory::Get().Create("PhysicsPlaneGeometryComponent"));
	ground_plane->AddComponent(plane_geom_comp);

	auto light = std::make_shared<GASS::SceneObject>();
	light->SetName("LightObject");
	auto location_comp = GASS::ComponentFactory::Get().Create("LocationComponent");
	location_comp->SetPropertyValue("Rotation", GASS::EulerRotation(42, 32, 0));
	light->AddComponent(location_comp);

	auto light_comp = GASS::ComponentFactory::Get().Create("LightComponent");
	light_comp->SetPropertyValue("DiffuseColor", GASS::ColorRGB(0.5, 0.5, 0.5));
	light_comp->SetPropertyValue("AmbientColor", GASS::ColorRGB(0.2, 0.2, 0.2));
	light_comp->SetPropertyValue("CastShadow", true);
	light->AddComponent(light_comp);
	auto env = std::make_shared<GASS::SceneObject>();
	env->AddChild(ground_plane);
	env->AddChild(light);
	return env;
}

std::string CreateBoxTemplate(GASS::SimEngine* engine)
{
	GASS::SceneObjectTemplatePtr box_template(new GASS::SceneObjectTemplate);
	box_template->SetName("BoxObject");
	box_template->AddComponent("LocationComponent");
	GASS::ComponentPtr mmc = box_template->AddComponent("ManualMeshComponent");
	mmc->SetPropertyValue("CastShadow", true);
	mmc->SetPropertyValue("ReceiveShadow", true);
	
	box_template->AddComponent("PhysicsBoxGeometryComponent");
	box_template->AddComponent("PhysicsBodyComponent");
	GASS::ComponentPtr box_comp = box_template->AddComponent("BoxGeometryComponent");
	box_comp->SetPropertyValue("Size", GASS::Vec3(1, 1, 1));
	box_comp->SetPropertyValue("Lines", false);
	engine->GetSceneObjectTemplateManager()->AddTemplate(box_template);
	return box_template->GetName();
}

GASS::SceneObjectPtr CreateBridge(GASS::SimEngine* /*engine*/)
{
#if 0
	//Create bridge templates
	GASS::SceneObjectTemplatePtr bridge_seg_template(new GASS::SceneObjectTemplate);
	bridge_seg_template->SetName("BridgeSegment");
	GASS::ComponentPtr mmc = bridge_seg_template->AddComponent("ManualMeshComponent");
	mmc->SetPropertyValue("CastShadow", true);
	bridge_seg_template->AddComponent("PhysicsBoxGeometryComponent");
	bridge_seg_template->AddComponent("PhysicsBodyComponent");
	GASS::ComponentPtr box_comp = bridge_seg_template->AddComponent("BoxGeometryComponent");
	box_comp->SetPropertyValue("Size", GASS::Vec3(2, 0.3, 0.6));
	box_comp->SetPropertyValue("Lines", false);
	engine->GetSceneObjectTemplateManager()->AddTemplate(bridge_seg_template);

	auto bridge = std::make_shared<GASS::SceneObjectTemplate>();
	bridge->SetName("Bridge");
	{
		auto bdrige_seg_obj2 = std::make_shared<GASS::SceneObjectTemplate>();
		bridge->AddChild(bdrige_seg_obj2);
		bdrige_seg_obj2->SetID("B_SEG_0");
		bdrige_seg_obj2->SetInheritance("BridgeSegment");
		auto location_comp = bdrige_seg_obj2->AddComponent("LocationComponent");
		GASS_DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(10, 2, 0));
		auto body_comp = bdrige_seg_obj2->AddComponent("PhysicsBodyComponent");
		try
		{
			body_comp->SetPropertyValue("Kinematic", true);
		}
		catch (...) {}
		for (int i = 1; i < 11; i++)
		{
			GASS::SceneObjectTemplatePtr bdrige_seg_obj1 = bdrige_seg_obj2;
			bdrige_seg_obj2 = std::make_shared<GASS::SceneObjectTemplate>();
			bridge->AddChild(bdrige_seg_obj2);
			auto id = "B_SEG_" + std::to_string(i);
			bdrige_seg_obj2->SetID(id);
			bdrige_seg_obj2->SetInheritance("BridgeSegment");
			body_comp = bdrige_seg_obj2->AddComponent("PhysicsBodyComponent");
			location_comp = bdrige_seg_obj2->AddComponent("LocationComponent");
			GASS_DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(10, 2, i));
			auto bdrige_hinge_obj = std::make_shared<GASS::SceneObjectTemplate>();
			bridge->AddChild(bdrige_hinge_obj);
			auto hinge_comp = bdrige_hinge_obj->AddComponent("PhysicsHingeComponent");
			if (i == 1)
			{
				hinge_comp->SetPropertyByString("Body1", bdrige_seg_obj2->GetID());
				hinge_comp->SetPropertyByString("Body2", bdrige_seg_obj1->GetID());
			}
			else
			{
				hinge_comp->SetPropertyByString("Body1", bdrige_seg_obj1->GetID());
				hinge_comp->SetPropertyByString("Body2", bdrige_seg_obj2->GetID());
			}
		}
		try
		{
			body_comp->SetPropertyValue("Kinematic", true);
		}
		catch (...) {}

		auto base = std::make_shared<GASS::SceneObjectTemplate>();
		base->SetName("Base");
		//base->AddComponent("LocationComponent");
		GASS::ComponentPtr mmc = base->AddComponent("ManualMeshComponent");
		mmc->SetPropertyValue("CastShadow", true);
		base->AddComponent("PhysicsBoxGeometryComponent");
		base->AddComponent("PhysicsBodyComponent");
		GASS::ComponentPtr box_comp = base->AddComponent("BoxGeometryComponent");
		box_comp->SetPropertyValue("Size", GASS::Vec3(1, 1, 1));
		box_comp->SetPropertyValue("Lines", false);
		engine->GetSceneObjectTemplateManager()->AddTemplate(base);

		auto base1 = std::make_shared<GASS::SceneObjectTemplate>();
		base1->SetInheritance("Base");
		location_comp = base1->AddComponent("LocationComponent");
		GASS_DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(10, 0.6, 2));
		bridge->AddChild(base1);

		auto base2 = std::make_shared<GASS::SceneObjectTemplate>();
		base2->SetInheritance("Base");
		location_comp = base2->AddComponent("LocationComponent");
		GASS_DYNAMIC_PTR_CAST<GASS::ILocationComponent>(location_comp)->SetPosition(GASS::Vec3(10, 0.6, 20));
		bridge->AddChild(base2);

		engine->GetSceneObjectTemplateManager()->AddTemplate(bridge);
		return engine->CreateObjectFromTemplate(bridge->GetName());
	}

#else
	
	auto bridge_seg_template = std::make_shared<GASS::SceneObject>();
	bridge_seg_template->SetName("BridgeSegment");
	GASS::ComponentPtr mmc = bridge_seg_template->AddComponent("ManualMeshComponent");
	mmc->SetPropertyValue("CastShadow", true);
	bridge_seg_template->AddComponent("LocationComponent");
	bridge_seg_template->AddComponent("PhysicsBoxGeometryComponent");
	bridge_seg_template->AddComponent("PhysicsBodyComponent");
	GASS::ComponentPtr box_comp = bridge_seg_template->AddComponent("BoxGeometryComponent");
	box_comp->SetPropertyValue("Size", GASS::Vec3(2, 0.3, 0.6));
	box_comp->SetPropertyValue("Lines", false);

	auto bridge = std::make_shared<GASS::SceneObject>();
	{
		GASS::SceneObjectPtr bdrige_seg_obj2 = bridge_seg_template->CreateCopy();
		bdrige_seg_obj2->GetFirstComponentByClass<GASS::ILocationComponent>()->SetPosition(GASS::Vec3(10, 2, 0));
		bridge->AddChildSceneObject(bdrige_seg_obj2, false);
		GASS::ComponentPtr body_comp = bdrige_seg_obj2->GetComponent("PhysicsBodyComponent");
		try
		{
			body_comp->SetPropertyValue("Kinematic", true);
		}
		catch (...) {}
		for (int i = 1; i < 11; i++)
		{
			GASS::SceneObjectPtr bdrige_seg_obj1 = bdrige_seg_obj2;
			bdrige_seg_obj2 = bridge_seg_template->CreateCopy();
			bdrige_seg_obj2->GetFirstComponentByClass<GASS::ILocationComponent>()->SetPosition(GASS::Vec3(10, 2, i));
			bridge->AddChildSceneObject(bdrige_seg_obj2, false);
			auto bdrige_hinge_obj = std::make_shared<GASS::SceneObject>();
			auto hinge_comp = GASS::ComponentFactory::Get().Create("PhysicsHingeComponent");
			bdrige_hinge_obj->AddComponent(hinge_comp);

			if (i == 1)
			{
				hinge_comp->SetPropertyValue("Body1", GASS::SceneObjectRef(bdrige_seg_obj2));
				hinge_comp->SetPropertyValue("Body2", GASS::SceneObjectRef(bdrige_seg_obj1));
			}
			else
			{
				hinge_comp->SetPropertyValue("Body1", GASS::SceneObjectRef(bdrige_seg_obj1));
				hinge_comp->SetPropertyValue("Body2", GASS::SceneObjectRef(bdrige_seg_obj2));
			}
			bridge->AddChildSceneObject(bdrige_hinge_obj, false);
		}
		body_comp = bdrige_seg_obj2->GetComponent("PhysicsBodyComponent");


		try
		{
			body_comp->SetPropertyValue("Kinematic", true);
		}
		catch (...) {}
		/*GASS::SceneObjectPtr box_obj = engine->CreateObjectFromTemplate("BoxObject");
		box_obj->GetFirstComponentByClass<GASS::ILocationComponent>()->SetPosition(GASS::Vec3(10, 0.6, 2));
		bridge->AddChildSceneObject(box_obj, false);
		GASS::SceneObjectPtr box_obj2 = engine->CreateObjectFromTemplate("BoxObject");
		box_obj2->GetFirstComponentByClass<GASS::ILocationComponent>()->SetPosition(GASS::Vec3(10, 0.6, 20));
		bridge->AddChildSceneObject(box_obj2,false);*/
	}
	return bridge;
#endif
}

int main(int/*argc*/, char* /*argv[]*/)
{
	//Load plugins
	std::cout << "Select physics system, press [1] for ODE , [2] for PhysX";
	char key = static_cast<char>(_getch());
	
	bool use_ode = (key == '1');


	GASS::SimEngineConfig config = GASS::SimEngineConfig::Create(use_ode ? 
		GASS::PhysicsOptions::ODE : 
		GASS::PhysicsOptions::PHYSX);

	GASS::SimEngine* engine = new GASS::SimEngine();
	engine->Init(config);
	
	GASS::GraphicsSystemPtr gfx_sys = engine->GetSimSystemManager()->GetFirstSystemByClass<GASS::IGraphicsSystem>();

	GASS::RenderWindowPtr win = gfx_sys->CreateRenderWindow("MainWindow", 800, 600);
	win->CreateViewport("MainViewport", 0, 0, 1, 1);

	GASS::InputSystemPtr input_system = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::IInputSystem>();
	input_system->SetMainWindowHandle(win->GetHWND());

	GASS::ScenePtr scene(engine->CreateScene("PhysicsScene"));
	
	const auto vehicle_template = CreateVehicleTemplate(engine, use_ode);
	const auto box_template = CreateBoxTemplate(engine);

	scene->GetRootSceneObject()->AddChildSceneObject(CreateEnvironment(),true);
	GASS::SceneObjectPtr vehicle = scene->LoadObjectFromTemplate(vehicle_template, scene->GetRootSceneObject());
	vehicle->GetFirstComponentByClass<GASS::ILocationComponent>()->SetPosition(GASS::Vec3(0, 3, 5));

	auto bridge = CreateBridge(engine);
	scene->GetRootSceneObject()->AddChildSceneObject(bridge,true);
	
	auto camera = scene->GetOrCreateCamera().lock().get();
	camera->SetPosition(GASS::Vec3(0, 3, 0));

	static float wheel_vel = 0;
	static float steer_vel = 0;

	GASS::SceneObjectPtr box_obj;
	while (true)
	{
		engine->Update();
		static bool key_down = false;
		#ifdef WIN32
		if (GetAsyncKeyState(VK_SPACE))
		{
			if (!key_down)
			{
				key_down = true;
				GASS::Vec3 pos = camera->GetFirstComponentByClass<GASS::ILocationComponent>()->GetPosition();
				GASS::Quaternion rot = camera->GetFirstComponentByClass<GASS::ILocationComponent>()->GetRotation();
				GASS::Vec3 vel = rot.GetZAxis()*- (GetAsyncKeyState(VK_RIGHT) ? 2500 : 1000);
				/*
								GASS::Mat4 rot_mat;
								rot_mat.Identity();
								rot.ToRotationMatrix(rot_mat);
								GASS::Vec3 vel = rot_mat.GetZAxis()*-2500;
				*/
				GASS::Vec3 torq(0, 0, 0);
				torq = rot * torq;
				box_obj = scene->LoadObjectFromTemplate(box_template, scene->GetRootSceneObject());
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
			}
		}
		else if (GetAsyncKeyState(VK_F2))
		{
			if (!key_down)
			{
				key_down = true;
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
		GASS::SceneObjectPtr rr_wheel = vehicle->GetChildByID("RR_WHEEL");
		rr_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetDriveVelocity(wheel_vel);
		rr_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetMaxDriveTorque(max_torq);
		rr_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetAngularSteerVelocity(0);
		rr_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetMaxSteerTorque(100);


		GASS::SceneObjectPtr rl_wheel = vehicle->GetChildByID("RL_WHEEL");
		rl_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetDriveVelocity(wheel_vel);
		rl_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetMaxDriveTorque(max_torq);
		rl_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetAngularSteerVelocity(0);
		rl_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetMaxSteerTorque(100);

		GASS::SceneObjectPtr fr_wheel = vehicle->GetChildByID("FR_WHEEL");
		fr_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetDriveVelocity(wheel_vel);
		fr_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetMaxDriveTorque(0);
		fr_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetAngularSteerVelocity(steer_vel);
		fr_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetMaxSteerTorque(100);

		GASS::SceneObjectPtr fl_wheel = vehicle->GetChildByID("FL_WHEEL");
		fl_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetDriveVelocity(wheel_vel);
		fl_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetMaxDriveTorque(0);
		fl_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetAngularSteerVelocity(steer_vel);
		fl_wheel->GetFirstComponentByClass<GASS::IPhysicsSuspensionComponent>()->SetMaxSteerTorque(100);
		#endif

	}
	return 0;
}
