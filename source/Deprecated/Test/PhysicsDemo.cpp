// CoreTest.cpp : Defines the entry point for the console application.
//
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
#include "Sim/Systems/SimSystemManager.h"

#include "Core/PluginSystem/PluginManager.h"

#include "Sim/SimEngine.h"
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/SceneObject.h"

#include "Sim/Systems/Collision/ICollisionSystem.h"
#include "Sim/Components/Graphics/Geometry/ILineComponent.h"

#include "Sim/Systems/Input/IInputSystem.h"

#include <stdio.h>
#include <iostream>
#include <fstream>



GASS::MessageManager mm;



void CreateManualObject()
{
	GASS::SceneObject* container = static_cast<GASS::SceneObject*>( GASS::ComponentContainerFactory::Get().Create("SceneObject"));
	container->SetName("ContainerTemplate");
	GASS::BaseComponent* lc = static_cast<GASS::BaseComponent*>(GASS::ComponentFactory::Get().Create("LocationComponent"));
	lc->SetName("ContainerLocation");
	lc->SetPropertyByType("Position",GASS::Vec3(0,0,0));
	lc->SetPropertyByType("Rotation",GASS::Vec3(45,45,45));	
	container->AddComponent(lc);

	GASS::BaseComponent* mc = static_cast<GASS::BaseComponent*>(GASS::ComponentFactory::Get().Create("MeshComponent"));
	mc->SetName("ContainerMesh");
	mc->SetPropertyByType("Filename",std::string("container_01.mesh"));
	mc->SetPropertyByType("CastShadow",true);	
	container->AddComponent(mc);

	GASS::BaseComponent* geom_comp = static_cast<GASS::BaseComponent*>(GASS::ComponentFactory::Get().Create("ODEGeometry"));
	geom_comp->SetName("ContainerPhysicsGeom");
	geom_comp->SetPropertyByType("GeometryType",std::string("box"));	
	container->AddComponent(geom_comp);

	GASS::BaseComponent* body_comp = static_cast<GASS::BaseComponent*>(GASS::ComponentFactory::Get().Create("ODEBody"));
	body_comp->SetName("ContainerPhysicsBody");
	body_comp->SetPropertyByType("Mass",2.0f);
	//body_comp->SetPropertyByValue("GeometryType","box");	
	container->AddComponent(body_comp);
	GASS::SimEngine::Get().GetSimObjectManager()->AddTemplate(container);
}

GASS::SceneObject* vodnik_wheel_LB;
GASS::SceneObject* vodnik_wheel_RB;
GASS::SceneObject* vodnik_wheel_LF;
GASS::SceneObject* vodnik_wheel_RF;

void CreateJeep()
{
	GASS::Vec3 pos(1725.18,50, 2858);
	GASS::SceneObject* vodnik = static_cast<GASS::SceneObject*>( GASS::ComponentContainerFactory::Get().Create("SceneObject"));
	vodnik->SetName("VodnikTemplate");
	GASS::BaseComponent* lc = static_cast<GASS::BaseComponent*>(GASS::ComponentFactory::Get().Create("LocationComponent"));
	lc->SetName("VodnikLocation");
	lc->SetPropertyByType("Position",pos);
	lc->SetPropertyByType("Rotation",GASS::Vec3(0,0,0));	
	vodnik->AddComponent(lc);

	GASS::BaseComponent* mc = static_cast<GASS::BaseComponent*>(GASS::ComponentFactory::Get().Create("MeshComponent"));
	mc->SetName("VodnikMesh");
	mc->SetPropertyByType("Filename",std::string("VodnikChasis.mesh"));
	mc->SetPropertyByType("CastShadow",true);	
	vodnik->AddComponent(mc);

	GASS::BaseComponent* geom_comp = static_cast<GASS::BaseComponent*>(GASS::ComponentFactory::Get().Create("ODEGeometry"));
	geom_comp->SetName("VodnikPhysicsGeom");
	geom_comp->SetPropertyByType("GeometryType",std::string("box"));	
	vodnik->AddComponent(geom_comp);

	GASS::BaseComponent* body_comp = static_cast<GASS::BaseComponent*>(GASS::ComponentFactory::Get().Create("ODEBody"));
	body_comp->SetName("VodnikPhysicsBody");
	body_comp->SetPropertyByType("Mass",0.5f);
	//body_comp->SetPropertyByValue("GeometryType","box");	
	vodnik->AddComponent(body_comp);
	
	GASS::Vec3 wheel_pos = pos;
	wheel_pos.x += 1.2;
	wheel_pos.y -= 0.6;
	wheel_pos.z += 1.6;
	GASS::SceneObject* vodnik_wheel = static_cast<GASS::SceneObject*>( GASS::ComponentContainerFactory::Get().Create("SceneObject"));
	vodnik_wheel->SetName("VodnikWheelTemplate");
	lc = static_cast<GASS::BaseComponent*>(GASS::ComponentFactory::Get().Create("LocationComponent"));
	lc->SetName("VodnikWheelLocation");
	lc->SetPropertyByType("Position",wheel_pos);
	lc->SetPropertyByType("Rotation",GASS::Vec3(0,0,0));	
	vodnik_wheel->AddComponent(lc);
	

	mc = static_cast<GASS::BaseComponent*>(GASS::ComponentFactory::Get().Create("MeshComponent"));
	mc->SetName("VodnikWheelMesh");
	mc->SetPropertyByType("Filename",std::string("VodnikWheel.mesh"));
	mc->SetPropertyByType("CastShadow",true);	
	vodnik_wheel->AddComponent(mc);

	geom_comp = static_cast<GASS::BaseComponent*>(GASS::ComponentFactory::Get().Create("ODEGeometry"));
	geom_comp->SetName("VodnikWheelPhysicsGeom");
	geom_comp->SetPropertyByType("GeometryType",std::string("sphere"));	
	geom_comp->SetPropertyByType("Friction",2.0f);
	vodnik_wheel->AddComponent(geom_comp);

	body_comp = static_cast<GASS::BaseComponent*>(GASS::ComponentFactory::Get().Create("ODEBody"));
	body_comp->SetName("VodnikWheelPhysicsBody");
	body_comp->SetPropertyByType("Mass",1.0f);
	
	vodnik_wheel->AddComponent(body_comp);
	
	GASS::BaseComponent* joint_comp = static_cast<GASS::BaseComponent*>(GASS::ComponentFactory::Get().Create("ODEJoint"));
	joint_comp->SetName("VodnikWheelPhysicsJoint");
	joint_comp->SetPropertyByType("Type",std::string("suspension"));
	joint_comp->SetPropertyByType("Strength",2.0f);
	joint_comp->SetPropertyByType("Damping",40.0f);
	vodnik_wheel->AddComponent(joint_comp);
	
	vodnik->AddChild(vodnik_wheel);
	
	vodnik_wheel = static_cast<GASS::SceneObject*>(vodnik_wheel->CreateCopy());
	lc = static_cast<GASS::BaseComponent*>(vodnik_wheel->GetComponent("VodnikWheelLocation"));
	wheel_pos = pos;
	wheel_pos.x -= 1.2;
	wheel_pos.y -= 0.6;
	wheel_pos.z += 1.6;
	lc->SetPropertyByType("Position",wheel_pos);
	vodnik->AddChild(vodnik_wheel);
	
	vodnik_wheel = static_cast<GASS::SceneObject*>(vodnik_wheel->CreateCopy());
	lc = static_cast<GASS::BaseComponent*>(vodnik_wheel->GetComponent("VodnikWheelLocation"));
	wheel_pos = pos;
	wheel_pos.x -= 1.3;
	wheel_pos.y -= 0.6;
	wheel_pos.z -= 1.4;
	lc->SetPropertyByType("Position",wheel_pos);
	joint_comp = static_cast<GASS::BaseComponent*>(vodnik_wheel->GetComponent("VodnikWheelPhysicsJoint"));
	joint_comp->SetPropertyByType("LowStop",GASS::Math::Deg2Rad(-30.0f));
	joint_comp->SetPropertyByType("HighStop",GASS::Math::Deg2Rad(30.0f));
	geom_comp = static_cast<GASS::BaseComponent*>(vodnik_wheel->GetComponent("VodnikWheelPhysicsGeom"));
	geom_comp->SetPropertyByType("Friction",2.0f);
	body_comp = static_cast<GASS::BaseComponent*>(vodnik_wheel->GetComponent("VodnikWheelPhysicsBody"));
	body_comp->SetPropertyByType("Mass",0.5f);

	vodnik->AddChild(vodnik_wheel);

	vodnik_wheel = static_cast<GASS::SceneObject*>(vodnik_wheel->CreateCopy());
	lc = static_cast<GASS::BaseComponent*>(vodnik_wheel->GetComponent("VodnikWheelLocation"));
	wheel_pos = pos;
	wheel_pos.x += 1.2;
	wheel_pos.y -= 0.6;
	wheel_pos.z -= 1.4;
	lc->SetPropertyByType("Position",wheel_pos);
	vodnik->AddChild(vodnik_wheel);

	GASS::SimEngine::Get().GetSimObjectManager()->AddTemplate(vodnik);
}


int main(int argc, char* argv[])
{
	GASS::SimEngine* engine = new GASS::SimEngine();
	engine->Init();
	GASS::Scenario* scenario = new GASS::Scenario();
	
	CreateManualObject();
	CreateJeep();

	scenario->Load("../../../data/demo_scenario");

	GASS::SceneObject* scene_object = scenario->GetScene(0)->GetObjectManager()->LoadFromTemplate("VodnikTemplate");
	
	
	int from_id = 0;
	//boost::shared_ptr<GASS::Message> pos_msg(new GASS::Message(GASS::ScenarioScene::OBJECT_MESSAGE_POSITION,from_id));
	//GASS::Vec3 pos(1725.18,50, 2808);
	//pos_msg->SetData("Position",pos);
	//scene_object->GetMessageManager()->SendImmediate(pos_msg);

	/*for(int i = 0; i < 15; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			//GASS::SceneObject* scene_object = scenario->GetScene(0)->GetObjectManager()->LoadFromTemplate("ContainerObject");
			GASS::SceneObject* scene_object = scenario->GetScene(0)->GetObjectManager()->LoadFromTemplate("ContainerTemplate");
	
			int from_id = i*40 + j;
			boost::shared_ptr<GASS::Message> pos_msg(new GASS::Message(GASS::ScenarioScene::OBJECT_MESSAGE_POSITION,from_id));
			GASS::Vec3 pos(1705.18 + i*10 ,100.3, 2808+10*j);
			pos_msg->SetData("Position",pos);
			scene_object->GetMessageManager()->SendImmediate(pos_msg);
		}
	}*/
	

	GASS::IInputSystem* input_system = GASS::SimEngine::GetPtr()->GetSystemManager()->GetFirstSystem<GASS::IInputSystem>();

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

		float force = 10;
		float vel = 0;

		float steer_force = 10;
		float steer_vel = 0;


		if(input_system->KeyStillDown(GASS::KEY_I) > 0)
		{
			vel = 10;
		}
		else if(input_system->KeyStillDown(GASS::KEY_K) > 0)
		{
			vel = -10;
		}

		if(input_system->KeyStillDown(GASS::KEY_J) > 0)
		{
			steer_vel = -10;
		}
		else if(input_system->KeyStillDown(GASS::KEY_L) > 0)
		{
			steer_vel = 10;
		}


			boost::shared_ptr<GASS::Message> force_msg(new GASS::Message(GASS::ScenarioScene::OBJECT_MESSAGE_PHYSICS_JOINT_PARAMETER,from_id));
			
			force_msg->SetData("Parameter",GASS::ScenarioScene::AXIS2_FORCE);
			force_msg->SetData("Value",force);

			boost::shared_ptr<GASS::Message> vel_msg(new GASS::Message(GASS::ScenarioScene::OBJECT_MESSAGE_PHYSICS_JOINT_PARAMETER,from_id));
			
			vel_msg->SetData("Parameter",GASS::ScenarioScene::AXIS2_VELOCITY);
			vel_msg->SetData("Value",vel);


			boost::shared_ptr<GASS::Message> steer_force_msg(new GASS::Message(GASS::ScenarioScene::OBJECT_MESSAGE_PHYSICS_JOINT_PARAMETER,from_id));
			
			steer_force_msg->SetData("Parameter",GASS::ScenarioScene::AXIS1_FORCE);
			steer_force_msg->SetData("Value",steer_force);

			boost::shared_ptr<GASS::Message> steer_vel_msg(new GASS::Message(GASS::ScenarioScene::OBJECT_MESSAGE_PHYSICS_JOINT_PARAMETER,from_id));
			
			steer_vel_msg->SetData("Parameter",GASS::ScenarioScene::AXIS1_VELOCITY);
			steer_vel_msg->SetData("Value",steer_vel);


			GASS::IComponentContainer::ComponentContainerVector children = scene_object->GetChildren();
			GASS::IComponentContainer::ComponentContainerVector::iterator iter;
			int wi =0;
			for(iter  = children.begin(); iter != children.end(); iter++, wi++)
			{
				
				GASS::SceneObject* child = static_cast<GASS::SceneObject*>(*iter);
				if(wi < 2)
				{
					child->GetMessageManager()->SendGlobalMessage(force_msg);
					child->GetMessageManager()->SendGlobalMessage(vel_msg);
				}
				else
				{
					child->GetMessageManager()->SendGlobalMessage(steer_force_msg);
					child->GetMessageManager()->SendGlobalMessage(steer_vel_msg);
				}

				
			}

		
	//	vodnik_wheel_RB->GetMessageManager()->SendGlobalMessage(force_msg);
	//	vodnik_wheel_RB->GetMessageManager()->SendGlobalMessage(vel_msg);
		
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




