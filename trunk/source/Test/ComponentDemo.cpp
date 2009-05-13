// CoreTest.cpp : Defines the entry point for the console application.
//
#include <boost/bind.hpp>

#include "Core/ComponentSystem/BaseObjectTemplateManager.h"
#include "Core/ComponentSystem/BaseObject.h"
#include "Core/ComponentSystem/BaseComponent.h"

#include "Core/ComponentSystem/IComponent.h"
#include "Core/ComponentSystem/IComponentContainer.h"
#include "Core/Math/Vector.h"
#include "Core/Utils/Factory.h"
#include "Core/Utils/Log.h"
#include "Core/Serialize/Serialize.h"
#include "Core/Utils/Timer.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/ComponentContainerFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"

#include "Core/PluginSystem/PluginManager.h"

#include <stdio.h>
#include <iostream>
#include <fstream>

#include "Core/Reflection/Reflection.h"


GASS::BaseObjectTemplateManager template_manager;

class GameComponent : public GASS::Reflection<GameComponent, GASS::BaseComponent>
{
public:
	static void RegisterReflection()
	{
		RegisterProperty<std::string>("Description", &GameComponent::GetDescription, &GameComponent::SetDescription);
	}
	std::string GetDescription()const {return m_Description;}
	void SetDescription(const std::string &des){m_Description = des;}
	virtual void Init() {};
	virtual void Update(double delta_time) {};
private:
	std::string m_Description;
};

class GameObject : public GASS::Reflection<GameObject,GASS::BaseObject> 
{
public:
	static void RegisterReflection()
	{
		GASS::ComponentContainerFactory::GetPtr()->Register("GameObject",new GASS::Creator<GameObject, IComponentContainer>);
		RegisterProperty<std::string>("Description", &GameObject::GetDescription, &GameObject::SetDescription);
	}
	std::string GetDescription()const {return m_Description;}
	void SetDescription(const std::string &des){m_Description = des;}
	void Init()
	{
		GASS::BaseObject::ComponentVector::iterator iter;
		for(iter = m_ComponentVector.begin(); iter != m_ComponentVector.end(); iter++)
		{
			GameComponent* comp = static_cast<GameComponent*>(*iter);
			comp->Init();
		}

		GASS::BaseObject::ComponentContainerVector::iterator comp_iter;
		for(comp_iter = m_ComponentContainerVector.begin(); comp_iter != m_ComponentContainerVector.end(); comp_iter++)
		{
			GameObject* child = static_cast<GameObject*>(*comp_iter);
			child->Init();
		}
	}

	void Update(double delta_time)
	{
		BaseObject::ComponentVector::iterator iter;
		for(iter = m_ComponentVector.begin(); iter != m_ComponentVector.end(); iter++)
		{
			GameComponent* comp = static_cast<GameComponent*>(*iter);
			comp->Update(delta_time);
		}

		GASS::BaseObject::ComponentContainerVector::iterator comp_iter;
		for(comp_iter = m_ComponentContainerVector.begin(); comp_iter != m_ComponentContainerVector.end(); comp_iter++)
		{
			GameObject* child = static_cast<GameObject*>(*comp_iter);
			child->Update(delta_time);
		}
	}
private:
	std::string m_Description;
};




class PlayerComponent : public GASS::Reflection<PlayerComponent, GameComponent>
{
public:
	PlayerComponent():
		m_MaxSpeed (1.f),
		m_MaxTurnSpeed (0.1f),
		m_Health(99)
	{

	}
	~PlayerComponent()
	{
	
	}
	static void RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("PlayerComponent",new GASS::Creator<PlayerComponent, IComponent>);
		RegisterProperty<int>("Health", &PlayerComponent::GetHealth, &PlayerComponent::SetHealth);
		RegisterProperty<float>("MaxSpeed", &PlayerComponent::GetMaxSpeed, &PlayerComponent::SetMaxSpeed);
		RegisterProperty<float>("MaxTurnSpeed", &PlayerComponent::GetMaxTurnSpeed, &PlayerComponent::SetMaxTurnSpeed);
	}
	float GetMaxSpeed()const {return m_MaxSpeed;}
	void SetMaxSpeed(float speed){m_MaxSpeed =speed;}
	float GetMaxTurnSpeed()const {return m_MaxTurnSpeed;}
	void SetMaxTurnSpeed(float speed){m_MaxTurnSpeed =speed;}
	int GetHealth()const {return m_Health;}
	void SetHealth(int health){m_Health = health;}
	
	void Init()
	{

	}
	void Update(double delta_time)
	{

	}
private:
	float m_MaxSpeed;
	float m_MaxTurnSpeed;
	int m_Health;
};


class WeaponComponent : public GASS::Reflection<WeaponComponent, GameComponent>
{
public:
	WeaponComponent() :
	  	m_MaxSpeed (200.f),
		m_Rounds (200)
	{

	}
	~WeaponComponent()
	{
	
	}
	static void RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("PlayerComponent",new GASS::Creator<PlayerComponent, IComponent>);
		//RegisterProperty<float>("Size", &TestComponent::GetSize, &TestComponent::SetSize);
	}
	float GetRound()const {return m_Rounds;}
	void SetRounds(float rounds){m_Rounds =rounds;}
	float GetMaxSpeed()const {return m_MaxSpeed;}
	void SetMaxSpeed(float speed){m_MaxSpeed =speed;}
	void Init()
	{
	}
	void Update(double delta_time)
	{
	}
private:
	float m_Rounds;
	float m_MaxSpeed;
};



class LocationComponent : public GASS::Reflection<LocationComponent, GameComponent>
{
public:
	LocationComponent() :
	  m_Pos(0,0,0),
	  m_Rot(0,0,0)
	{
	}
	~LocationComponent()
	{
	
	}

	static void RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("LocationComponent",new GASS::Creator<LocationComponent, IComponent>);
		RegisterProperty<GASS::Vec3>("Position", &LocationComponent::GetPos, &LocationComponent::SetPos);
		RegisterProperty<GASS::Vec3>("Rotation", &LocationComponent::GetRot, &LocationComponent::SetRot);
	}
	GASS::Vec3 GetPos()const {return m_Pos;}
	void SetPos(const GASS::Vec3 &pos){m_Pos = pos;}
	GASS::Vec3 GetRot()const {return m_Rot;}
	void SetRot(const GASS::Vec3 &rot){m_Rot = rot;}	

	void Init()
	{
		m_Owner->GetComponent("");
		std::cout << "init:" << m_Name << std::endl;
	}

	void Update(double delta_time)
	{
				
		//if(((int) update_msg->m_Tick) % 10 == 0)
		//std::cout << "update:" << m_Name << " tick:" << update_msg->m_Tick <<std::endl;
		
	}

private:
	GASS::Vec3 m_Pos;
	GASS::Vec3 m_Rot;
};


void CreateTemplates()
{
	GameObject* player = new GameObject();
	player->SetName("Soldier");
	player->SetDescription("Standard soldier");

	PlayerComponent* p_comp = new PlayerComponent();
	p_comp->SetName("player_component");
	p_comp->SetHealth(99);
	player->AddComponent(p_comp);

	LocationComponent* location_comp = new LocationComponent();
	location_comp->SetName("LocationComponent");
	player->AddComponent(location_comp);
	
	GameObject* back_pack = new GameObject();
	back_pack->SetName("player_back_pack");
	WeaponComponent* wc = new WeaponComponent();
	wc->SetName("machine_gun_component");
	wc->SetRounds(100);
	wc->SetMaxSpeed(200);
	back_pack->AddComponent(wc);
	//LocationComponent* clc = new LocationComponent();

	player->AddChild(back_pack);
	//go1->OnCreate();
	//boost::shared_ptr<GASS::Message> init_msg(new GASS::Message(MESSAGE_INIT,100));
	//mm.SendImmediate(init_msg);
	template_manager.AddTemplate(player);

	player = static_cast<GameObject*>( template_manager.CreateFromTemplate("player_game_object"));
	player->DebugPrint();
	//delete go1;
}

int main(int argc, char* argv[])
{
	CreateTemplates();
	

	//Test serializing
	/*GASS::SerialSizer ss;
	go1->Serialize(&ss);
	unsigned long size=ss.getLength();

	unsigned char *buffer=new unsigned char[size];
	GASS::SerialSaver sv(buffer,size);
	go1->Serialize(&sv);

	std::ofstream outfile;
	outfile.open("test.bin", std::ios::out | std::ios::trunc | std::ios::binary);
	outfile.write( ((char *)buffer), size );
	outfile.close();

	std::ifstream infile ("test.bin", std::ios::in|std::ios::binary|std::ios::ate);

	GameObject bin_go;
	if (infile.is_open())
	{
		size = infile.tellg();
		char *memblock = new char [size];
		infile.seekg (0, std::ios::beg);
		infile.read (memblock, size);
		infile.close();
		buffer = (unsigned char*) memblock;
		GASS::SerialLoader sl(buffer, size);
		bin_go.Serialize(&sl);
		delete[] memblock;
	}

	std::cout << " " << std::endl;
	std::cout << "Loaded from binary file: " << std::endl;
	test_comp1 = (TestComponent*) bin_go.GetComponent("TestComponent1");
	std::cout << "size " << test_comp1->GetSize() << std::endl;

	test_comp2 = (TestComponent*) bin_go.GetComponent("TestComponent2");
	std::cout << "size " << test_comp2->GetSize() << std::endl;
	test_comp3 = (LocationComponent*) bin_go.GetComponent("TestComponent3");
	
	std::cout << "pos " << test_comp3->GetPos() << std::endl;

	bin_go.DebugPrint();*/

	///////////test timer

	GASS::Timer timer;

	timer.Reset();

	double prev = 0;
	bool check_reset = true;
	while(timer.GetTime() < 20)
	{
		double time = timer.GetTime();
		if(time - prev > 1.0)
		{
			std::cout << "Time is:" << time << std::endl;
			prev = time;
		}
		if(check_reset && time > 5.0)
		{
			check_reset = false;
			timer.Reset();
			time = timer.GetTime();
			std::cout << "Reset timer after 5 sec , current time is:" << time << std::endl;
			prev = time;
		}
	}

	
	
	return 0;
}




