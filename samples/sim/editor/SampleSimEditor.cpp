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
#include "Sim/GASSSceneObjectTemplateManager.h"
#include "Sim/Interface/GASSIBillboardComponent.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/ToolSystem/CreateTool.h"
#include "Modules/Editor/ToolSystem/GraphTool.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"

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
int _getch() {
	struct termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}
#endif


void CreateTemps()
{

	{
		GASS::SceneObjectTemplatePtr mesh_template(new GASS::SceneObjectTemplate);
		mesh_template->SetName("StaticMesh");
		mesh_template->SetInstantiable(true);
		mesh_template->AddComponent("LocationComponent");
		GASS::ComponentPtr mesh = mesh_template->AddComponent("MeshComponent");
		mesh->SetPropertyValue("CastShadow", true);
		mesh->SetPropertyByString("GeometryFlags", std::string("GEOMETRY_FLAG_GROUND"));

		GASS::ComponentPtr px_geom_comp = mesh_template->AddComponent("PhysicsMeshGeometryComponent");
		//px_geom_comp->SetPropertyValue<bool>("SizeFromMesh", true);
		GASS::SimEngine::GetPtr()->GetSceneObjectTemplateManager()->AddTemplate(mesh_template);
	}

	{
		GASS::SceneObjectTemplatePtr plane_template(new GASS::SceneObjectTemplate);
		plane_template->SetName("Plane");
		plane_template->SetInstantiable(true);
		plane_template->AddComponent("LocationComponent");
		GASS::ComponentPtr mmc = plane_template->AddComponent("ManualMeshComponent");
		mmc->SetPropertyValue("CastShadow", true);

		//GASS::ComponentPtr geom_comp = plane_template->AddComponent("PhysicsBoxGeometryComponent");
		//geom_comp->SetPropertyValue<GASS::Vec3>("Offset", GASS::Vec3(0, 0, 0));
		//geom_comp->SetPropertyValue<bool>("SizeFromMesh", true);

		GASS::ComponentPtr plane_comp = plane_template->AddComponent("PlaneGeometryComponent");
		plane_comp->SetPropertyValue("Size", GASS::Vec2(10, 10));
		GASS::SimEngine::GetPtr()->GetSceneObjectTemplateManager()->AddTemplate(plane_template);
	}

	{
		GASS::SceneObjectTemplatePtr box_template(new GASS::SceneObjectTemplate);
		box_template->SetName("StaticBox");
		box_template->SetInstantiable(true);
		box_template->AddComponent("LocationComponent");
		GASS::ComponentPtr mesh = box_template->AddComponent("ManualMeshComponent");
		mesh->SetPropertyValue("CastShadow", true);
		mesh->SetPropertyValue("ReceiveShadow", true);
		mesh->SetPropertyByString("GeometryFlags", std::string("GEOMETRY_FLAG_GROUND"));

		GASS::ComponentPtr px_geom_comp = box_template->AddComponent("PhysicsBoxGeometryComponent");
		//px_geom_comp->SetPropertyValue<GASS::Vec3>("Offset", GASS::Vec3(0, 0, 0));
		px_geom_comp->SetPropertyValue<bool>("SizeFromMesh", true);
		GASS::ComponentPtr box_comp = box_template->AddComponent("BoxGeometryComponent");
		box_comp->SetPropertyValue("Size", GASS::Vec3(2, 1, 4));
		box_comp->SetPropertyValue("Lines", false);
		GASS::SimEngine::GetPtr()->GetSceneObjectTemplateManager()->AddTemplate(box_template);
	}

	{
		GASS::SceneObjectTemplatePtr sphere_template(new GASS::SceneObjectTemplate);
		sphere_template->SetName("StaticSphere");
		sphere_template->SetInstantiable(true);
		sphere_template->AddComponent("LocationComponent");
		GASS::ComponentPtr mesh = sphere_template->AddComponent("ManualMeshComponent");
		mesh->SetPropertyValue("CastShadow", true);
		mesh->SetPropertyValue("ReceiveShadow", true);
		mesh->SetPropertyByString("GeometryFlags", std::string("GEOMETRY_FLAG_GROUND"));

		GASS::ComponentPtr px_geom_comp = sphere_template->AddComponent("PhysicsSphereGeometryComponent");
		//px_geom_comp->SetPropertyValue<GASS::Vec3>("Offset", GASS::Vec3(0, 0, 0));
		px_geom_comp->SetPropertyValue<bool>("SizeFromMesh", true);
		GASS::ComponentPtr sphere_comp = sphere_template->AddComponent("SphereGeometryComponent");
		sphere_comp->SetPropertyValue("Radius", double(2));
		sphere_comp->SetPropertyValue("Wireframe", false);
		//box_comp->SetPropertyValue("Lines", false);
		GASS::SimEngine::GetPtr()->GetSceneObjectTemplateManager()->AddTemplate(sphere_template);
	}

	{
		GASS::SceneObjectTemplatePtr box_template(new GASS::SceneObjectTemplate);
		box_template->SetName("DynamicBox");
		box_template->SetInstantiable(true);
		box_template->AddComponent("LocationComponent");
		auto mesh = box_template->AddComponent("ManualMeshComponent");
		mesh->SetPropertyValue("CastShadow", true);

		auto px_geom = box_template->AddComponent("PhysicsBoxGeometryComponent");
		px_geom->SetPropertyValue<GASS::Vec3>("Offset", GASS::Vec3(0, 0, 0));
		px_geom->SetPropertyValue<bool>("SizeFromMesh", true);

		auto px_body = box_template->AddComponent("PhysicsBodyComponent");
		px_body->SetPropertyValue("Mass", 1.0f);

		auto box = box_template->AddComponent("BoxGeometryComponent");
		box->SetPropertyValue("Size", GASS::Vec3(2, 1, 4));
		box->SetPropertyValue("Lines", false);

		GASS::SimEngine::GetPtr()->GetSceneObjectTemplateManager()->AddTemplate(box_template);
	}
	{
		GASS::SceneObjectTemplatePtr sphere_template(new GASS::SceneObjectTemplate);
		sphere_template->SetName("DynamicSphere");
		sphere_template->SetInstantiable(true);
		sphere_template->AddComponent("LocationComponent");
		auto mesh = sphere_template->AddComponent("ManualMeshComponent");
		mesh->SetPropertyValue("CastShadow", true);
		mesh->SetPropertyValue("ReceiveShadow", true);
		//mesh->SetPropertyByString("GeometryFlags", std::string("GEOMETRY_FLAG_GROUND"));

		auto px_geom = sphere_template->AddComponent("PhysicsSphereGeometryComponent");
		px_geom->SetPropertyValue<bool>("SizeFromMesh", true);

		auto px_body = sphere_template->AddComponent("PhysicsBodyComponent");
		px_body->SetPropertyValue("Mass", 1.0f);
		
		auto sphere = sphere_template->AddComponent("SphereGeometryComponent");
		sphere->SetPropertyValue("Radius", double(2));
		sphere->SetPropertyValue("Wireframe", false);
		//box_comp->SetPropertyValue("Lines", false);
		GASS::SimEngine::GetPtr()->GetSceneObjectTemplateManager()->AddTemplate(sphere_template);
	}

	{
		GASS::SceneObjectTemplatePtr sky_template(new GASS::SceneObjectTemplate);
		sky_template->SetName("Sky");
		sky_template->SetInstantiable(true);
		sky_template->AddComponent("LocationComponent");
		GASS::ComponentPtr light = sky_template->AddComponent("LightComponent");
		light->SetPropertyByString("LightType", std::string("LT_DIRECTIONAL"));
		light->SetPropertyValue("CastShadow", true);
		GASS::ComponentPtr box = sky_template->AddComponent("SkyboxComponent");
		box->SetPropertyValue("Material", std::string("blue_sb_.jpg"));
		box->SetPropertyValue("Size", 200.0);

		
		GASS::SimEngine::GetPtr()->GetSceneObjectTemplateManager()->AddTemplate(sky_template);
	}

	{
		GASS::SceneObjectTemplatePtr bb_template(new GASS::SceneObjectTemplate);
		bb_template->SetName("Billboard");
		bb_template->SetInstantiable(true);
		bb_template->AddComponent("LocationComponent");
	
		auto bb = bb_template->AddComponent("BillboardComponent");
		bb->SetPropertyValue("Material", std::string("checker.png"));
		
		bb_template->AddComponent("DistanceScaleComponent");

		GASS::SimEngine::GetPtr()->GetSceneObjectTemplateManager()->AddTemplate(bb_template);
	}
}

int start(int argc, char* argv[])
{
	(void)argc;
	(void)argv;
	//Create engine instance and initialize with config file
	GASS::SimEngine* engine = new GASS::SimEngine();
	{

		GASS::SimEngineConfig config = GASS::SimEngineConfig::Create(GASS::PhysicsOptions::PHYSX,
			GASS::SoundOptions::NONE,
			GASS::NetworkOptions::NONE);
		config.ResourceConfig.ResourceLocations.emplace_back("SAMPLE_DATA", "%GASS_DATA_HOME%/sample_data", true);
		config.SimSystemManager.MaxSimulationSteps = 1;
		engine->Init(config);
		CreateTemps();
		//Get graphic system and create one main rendering window
		GASS::GraphicsSystemPtr gfx_sys = engine->GetSimSystemManager()->GetFirstSystemByClass<GASS::IGraphicsSystem>();
		GASS::RenderWindowPtr win = gfx_sys->CreateRenderWindow("MainWindow", 800, 600);

		//Create viewport in main window
		GASS::ViewportPtr viewport = win->CreateViewport("MainViewport", 0, 0, 1, 1);

		//Give input system window handle
		GASS::InputSystemPtr input_system = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::IInputSystem>();
		input_system->SetMainWindowHandle(win->GetHWND());

		GASS::EditorSystemPtr es = GASS::SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<GASS::EditorSystem>();
		es->SetPropertyValue("LockTerrainObjects", false);
		es->SetShowGUI(true);

		//Create the scene
		GASS::Scene* scene = GASS::SimEngine::Get().CreateScene().lock().get();

		//Load pre-build scene from data folder
		scene->Load(GASS::FilePath("%GASS_DATA_HOME%/sample_data/sceneries/osg_demo.scene"));
		//create free camera and add it to the scene under the root node
		auto camera = scene->GetOrCreateCamera();
		if(false)
		{
			//Create vehicle and add it to the root node of the scene
			GASS::SceneObjectPtr vehicle_obj = engine->CreateObjectFromTemplate("PXTank");
			scene->GetRootSceneObject()->AddChildSceneObject(vehicle_obj, true);

			//Set start position
			vehicle_obj->GetFirstComponentByClass<GASS::ILocationComponent>()->SetWorldPosition(scene->GetStartPos());
		}
		{

			GASS::EditorSceneManager* esm = scene->GetFirstSceneManagerByClass<GASS::EditorSceneManager>().get();
			esm->GetMouseToolController()->SelectTool("MoveTool");
			esm->GetMouseToolController()->SetEnableGizmo(true);

			GASS::CreateTool* ct = (GASS::CreateTool*)esm->GetMouseToolController()->GetTool("CreateTool");
			ct->SetTemplateName("CubeMeshObject");
			ct->SetParentObject(scene->GetRootSceneObject());
		}

		//std::string name = es->GetName();
		//Update the engine forever
		bool done = false;
		while (!done)
		{
#if 0
			if (GetAsyncKeyState(VK_F1))
			{
				esm->GetMouseToolController()->SelectTool("SelectTool");
			}
			else if (GetAsyncKeyState(VK_F2))
			{
				esm->GetMouseToolController()->SelectTool("RotateTool");
			}
			else if (GetAsyncKeyState(VK_F3))
			{
				esm->GetMouseToolController()->SelectTool("MoveTool");
			}
			else if (GetAsyncKeyState(VK_F5))
			{
				esm->GetMouseToolController()->SelectTool("CreateTool");

			}
			else if (GetAsyncKeyState(VK_ESCAPE))
			{
				done = true;
			}
#endif
			engine->Update();
		}
	}
	delete engine;
	return 0;
}

int main(int argc, char* argv[])
{
	int ret = 0;
	try
	{
		ret = start(argc, argv);
	}
	catch (std::exception& e)
	{
		std::cout << "Exception:" << e.what() << std::endl;
		_getch();
	}
	return ret;
}


