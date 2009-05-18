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

#include <boost/bind.hpp>

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
#include "Core/ComponentSystem/BaseObject.h"
#include "Core/ComponentSystem/BaseObjectTemplateManager.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"


#include "Core/PluginSystem/PluginManager.h"

#include "Sim/SimEngine.h"
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/SceneObject.h"

#include "Sim/Systems/Collision/ICollisionSystem.h"
#include "Sim/Systems/SimSystemManager.h"

#include "Sim/Components/Graphics/Geometry/ILineComponent.h"

#include <stdio.h>
#include <iostream>
#include <fstream>



GASS::MessageManager mm;



void CreateManualObject()
{
	GASS::SceneObjectPtr container = boost::shared_static_cast<GASS::SceneObject>( GASS::ComponentContainerFactory::Get().Create("SceneObject"));
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

void TestCollision(GASS::ScenarioScene* scene)
{
	GASS::CollisionSystemPtr col_sys = GASS::SimEngine::GetPtr()->GetSystemManager()->GetFirstSystem<GASS::ICollisionSystem>();
	
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

int main(int argc, char* argv[])
{
	GASS::SimEngine* engine = new GASS::SimEngine();
	engine->Init();
	GASS::Scenario* scenario = new GASS::Scenario();

	//CreateManualObject();

	scenario->Load("../data/scenarios/ogre_demo_scenario");
	//scenario->Load("../../../data/scenarios/camp_genesis");
	//scenario->Load("../../../data/advantage_scenario");
	
	/*for(int i = 0; i < 2; i++)
	{
		for(int j = 0; j < 2; j++)
		{
			//GASS::SceneObject* scene_object = scenario->GetScene(0)->GetObjectManager()->LoadFromTemplate("ContainerObject");
			GASS::SceneObjectPtr scene_object = scenario->GetScene(0)->GetObjectManager()->LoadFromTemplate("ContainerTemplate");
	
			int from_id = i*40 + j;
			GASS::MessagePtr pos_msg(new GASS::Message(GASS::ScenarioScene::OBJECT_MESSAGE_POSITION,from_id));
			GASS::Vec3 pos(1705.18 + i*10 ,100.3, 2808+10*j);
			pos_msg->SetData("Position",pos);
			scene_object->GetMessageManager()->SendImmediate(pos_msg);
		}
	}*/
	
	GASS::Timer timer;

	timer.Reset();

	double prev = 0;
	double temp_t = 0;
	bool check_reset = true;
	while(timer.GetTime() < 200)
	{
		double time = timer.GetTime();
		engine->Update(time - temp_t);
		scenario->OnUpdate(time - temp_t);
		temp_t = time;
		if(time - prev > 1.0)
		{
			std::cout << "Time is:" << time << std::endl;
			prev = time;
			//TestCollision(scenario->GetScene(0));
		}
		
		/*if(check_reset && time > 5.0)
		{
			check_reset = false;
			timer.Reset();
			time = timer.GetTime();
			std::cout << "Reset timer after 5 sec , current time is:" << time << std::endl;
			prev = time;
		}*/
	}

	/////////////////TEST plugin manager/////////////

	
	return 0;
}


