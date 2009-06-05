// CoreTest.cpp : Defines the entry point for the console application.
//
#include <boost/bind.hpp>

#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplate.h"
#include "Core/ComponentSystem/BaseComponentContainer.h"
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
#include "Core/ComponentSystem/ComponentContainerTemplateFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"

#include "Core/PluginSystem/PluginManager.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "tinyxml.h"

#include "Core/Reflection/Reflection.h"


enum
{
	MESSAGE_INIT,
	MESSAGE_UPDATE,
};

GASS::MessageManager mm;
boost::shared_ptr<GASS::BaseComponentContainerTemplateManager> template_manager;

class MyGameObject : public GASS::Reflection<MyGameObject,GASS::BaseComponentContainer> 
{
public:
	static void RegisterReflection()
	{
		GASS::ComponentContainerFactory::GetPtr()->Register("MyGameObject",new GASS::Creator<MyGameObject, IComponentContainer>);
		RegisterProperty<std::string>("Description", &MyGameObject::GetDescription, &MyGameObject::SetDescription);
	}
	std::string GetDescription()const {return m_Des;}
	void SetDescription(const std::string &des){m_Des = des;}
private:
	std::string m_Des;
};


class MyGameObjectTemplate : public GASS::Reflection<MyGameObjectTemplate,GASS::BaseComponentContainerTemplate> 
{
public:
	static void RegisterReflection()
	{
		GASS::ComponentContainerTemplateFactory::GetPtr()->Register("MyGameObjectTemplate",new GASS::Creator<MyGameObjectTemplate, IComponentContainerTemplate>);
		RegisterProperty<std::string>("Description", &MyGameObjectTemplate::GetDescription, &MyGameObjectTemplate::SetDescription);
	}
	std::string GetDescription()const {return m_Des;}
	void SetDescription(const std::string &des){m_Des = des;}
private:
	std::string m_Des;
};



class TestComponent : public GASS::Reflection<TestComponent, GASS::BaseComponent>
{
public:
	static void RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("TestComponent",new GASS::Creator<TestComponent, IComponent>);
		RegisterProperty<float>("Size", &TestComponent::GetSize, &TestComponent::SetSize);
	}
	float GetSize()const {return m_Size;}
	void SetSize(const float &size){m_Size = size;}
	void OnCreate()
	{

	}
private:
	float m_Size;
};

class UpdateMessage : public GASS::Message
{
public:
	UpdateMessage(int type, int from): Message(type,from)
	{

	}
	virtual ~UpdateMessage(){}
	float m_Tick;
};

class LocationComponent : public GASS::Reflection<LocationComponent, GASS::BaseComponent>
{
public:
	LocationComponent()
	{

	}

	~LocationComponent()
	{
		
		mm.UnregisterForMessage(MESSAGE_INIT,MESSAGE_FUNC(LocationComponent::OnInit));
		mm.UnregisterForMessage(MESSAGE_UPDATE,MESSAGE_FUNC(LocationComponent::OnInit));
	}

	static void RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("LocationComponent",new GASS::Creator<LocationComponent, IComponent>);
		RegisterProperty<GASS::Vec3>("Pos", &LocationComponent::GetPos, &LocationComponent::SetPos);
		RegisterProperty<int>("InitPriority", &LocationComponent::GetInitPriority, &LocationComponent::SetInitPriority);
	}

	GASS::Vec3 GetPos()const {return m_Pos;}
	void SetPos(const GASS::Vec3 &pos){m_Pos = pos;}
	int GetInitPriority()const {return m_InitPriority;}
	void SetInitPriority(const int &prio){m_InitPriority = prio;}

	void OnCreate()
	{
		mm.RegisterForMessage(MESSAGE_INIT,  MESSAGE_FUNC(LocationComponent::OnInit),m_InitPriority);
		mm.RegisterForMessage(MESSAGE_UPDATE, MESSAGE_FUNC(LocationComponent::OnInit),m_InitPriority);
	}

	void OnInit(GASS::MessagePtr message)
	{
		std::cout << "init:" << m_Name << std::endl;
	}

	void OnUpdate(GASS::MessagePtr message)
	{

		//if(((int) update_msg->m_Tick) % 10 == 0)
		//std::cout << "update:" << m_Name << " tick:" << update_msg->m_Tick <<std::endl;

	}

private:
	GASS::Vec3 m_Pos;
	int m_InitPriority;
};

typedef boost::shared_ptr<MyGameObject> MyGameObjectPtr;
typedef boost::shared_ptr<MyGameObjectTemplate> MyGameObjectTemplatePtr;
typedef boost::shared_ptr<TestComponent> TestComponentPtr;
typedef boost::weak_ptr<TestComponent> TestComponentWeakPtr;

void TestComponentSystem()
{
	MyGameObjectTemplatePtr go1(new MyGameObjectTemplate());
	go1->SetName("MyManualGameObject");
	MyGameObjectTemplatePtr child_go (new MyGameObjectTemplate());
	child_go->SetName("child_game_object");
	go1->AddChild(child_go);
	TestComponentPtr tc (new TestComponent());
	tc->SetName("TestComp");
	float new_size = 1000;
	tc->SetPropertyByType("Size",new_size);
	tc->SetPropertyByType("Name",std::string("MyTestComponent"));
	go1->AddComponent(tc);
	//LocationComponent* clc = new LocationComponent();
	child_go->AddComponent(tc);

	//go1->OnCreate();
	//boost::shared_ptr<GASS::Message> init_msg(new GASS::Message(MESSAGE_INIT,100));
	//mm.SendImmediate(init_msg);
	template_manager->AddTemplate(go1);

	boost::shared_ptr<GASS::BaseComponentContainer> bo = boost::shared_static_cast<GASS::BaseComponentContainer>( template_manager->CreateFromTemplate("MyManualGameObject"));
	bo->DebugPrint();
	//delete go1;
}
//reflection test-------------------------------------------------------------------------------

class TestReflectionObject :  public GASS::Reflection<TestReflectionObject, GASS::BaseReflectionObject> 
{
public:
	static void	RegisterReflection()
	{
		RegisterProperty<GASS::Vec3>("Pos", &TestReflectionObject::GetPos, &TestReflectionObject::SetPos);
		RegisterProperty<std::vector<int>>("IntVec", &TestReflectionObject::GetVec, &TestReflectionObject::SetVec);
		RegisterProperty<std::vector<std::string>>("StrVec", &TestReflectionObject::GetVecStr, &TestReflectionObject::SetVecStr);
	}

	GASS::Vec3 GetPos()const {return m_Pos;}
	void SetPos(const GASS::Vec3 &pos){m_Pos = pos;}
	std::vector<int> GetVec()const {return m_Vec;}
	void SetVec(const std::vector<int> &vec){m_Vec= vec;}
	std::vector<std::string> GetVecStr()const {return m_StrVec;}
	void SetVecStr(const std::vector<std::string> &vec){m_StrVec= vec;}
private:
	GASS::Vec3 m_Pos;
	std::vector<int> m_Vec;
	std::vector<std::string> m_StrVec;

};

void TestReflection()
{
	GASS::Vec3 pos;
	TestReflectionObject obj;
	obj.SetPropertyByString("Pos","1 2 77");
	boost::any data;
	obj.GetPropertyByType("Pos",data);
	pos = boost::any_cast<GASS::Vec3>(data);
	std::cout << pos.ToString();

	std::string value;
	obj.SetPropertyByString("IntVec","221 22 177");
	obj.GetPropertyByType("IntVec",data);
	std::vector<int> vec = boost::any_cast<std::vector<int>>(data);
	obj.GetPropertyByString("IntVec",value);

	obj.SetPropertyByString("StrVec","string vector test");
	obj.GetPropertyByType("StrVec",data);
	std::vector<std::string> vecstr = boost::any_cast<std::vector<std::string>>(data);
	obj.GetPropertyByString("StrVec",value);
	//std::cout << vec;
}

int main(int argc, char* argv[])
{

	TestReflection();
	/////////////////TEST plugin manager/////////////
	template_manager.reset ( new GASS::BaseComponentContainerTemplateManager());


	TestComponentSystem();
	GASS::PluginManager pm;
	pm.LoadFromFile("core_test_plugins.xml");

	//////////////////test message system

	boost::shared_ptr<GASS::Message> init_msg(new GASS::Message(MESSAGE_INIT,100));
	mm.SendGlobalMessage(init_msg);

	/////////////////////////////////////////////////

	if (!template_manager->Load("test.xml"))
		GASS::Log::Warning("main(...) - Couldn't load file: test.xml");
	MyGameObjectPtr go1 = boost::shared_static_cast<MyGameObject>(template_manager->CreateFromTemplate("GO2"));
	assert(go1);
	go1->DebugPrint(0);
	//register for messages
	go1->OnCreate();
	for(int i = 0 ; i < 190; i++)
	{
		MyGameObjectPtr go  = boost::shared_static_cast<MyGameObject>(template_manager->CreateFromTemplate("GO2"));
		go->OnCreate();
	}
	mm.Update(0.1);

	for(int i = 0 ; i < 100; i++)
	{
		if(i % 10 == 0)
			std::cout << "update:" << i <<std::endl;
		boost::shared_ptr<UpdateMessage> update_msg (new UpdateMessage(MESSAGE_UPDATE,99));
		update_msg->m_TypeID = MESSAGE_UPDATE;
		update_msg->m_Tick = i;
		mm.SendGlobalMessage(update_msg);
		mm.Update(i);
	}

	std::cout << "Loaded from game object template xml-file: " << std::endl;
	GASS::ComponentPtr t_comp1 (go1->GetComponent("TestComponent1"));
	TestComponentPtr test_comp1 = boost::shared_static_cast<TestComponent>(t_comp1);
	assert(test_comp1);

	std::cout << "Description go1:" << go1->GetDescription() << std::endl;
	std::cout << "size " << test_comp1->GetSize() << std::endl;

	//GASS::ComponentPtr t_comp2 = (go1->GetComponent("TestComponent2"));
	TestComponentPtr test_comp2 = boost::shared_static_cast<TestComponent>(GASS::ComponentPtr(go1->GetComponent("TestComponent2")));
	assert(test_comp2);
	std::cout << "size " << test_comp2->GetSize() << std::endl;
	boost::shared_ptr<LocationComponent> test_comp3 = boost::shared_static_cast<LocationComponent>(GASS::ComponentPtr(go1->GetComponent("TestComponent3")));

	std::cout << "pos " << test_comp3->GetPos() << std::endl;

	//Test serializing
	GASS::SerialSizer ss;
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

	MyGameObjectPtr bin_go (new MyGameObject());
	if (infile.is_open())
	{
		size = infile.tellg();
		char *memblock = new char [size];
		infile.seekg (0, std::ios::beg);
		infile.read (memblock, size);
		infile.close();
		buffer = (unsigned char*) memblock;
		GASS::SerialLoader sl(buffer, size);
		bin_go->Serialize(&sl);
		delete[] memblock;
	}

	std::cout << " " << std::endl;
	std::cout << "Loaded from binary file: " << std::endl;
	GASS::ComponentPtr c1(bin_go->GetComponent("TestComponent1"));
	test_comp1 = boost::shared_static_cast<TestComponent>( c1);
	std::cout << "size " << test_comp1->GetSize() << std::endl;

	test_comp2 = boost::shared_static_cast<TestComponent>(GASS::ComponentPtr(bin_go->GetComponent("TestComponent2")));
	std::cout << "size " << test_comp2->GetSize() << std::endl;
	test_comp3 = boost::shared_static_cast<LocationComponent>( GASS::ComponentPtr(bin_go->GetComponent("TestComponent3")));

	std::cout << "pos " << test_comp3->GetPos() << std::endl;

	bin_go->DebugPrint();

	//go1->SaveXML(objelem);
	///////////test xml save


	TiXmlDocument doc;  

	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
	doc.LinkEndChild( decl );  

	TiXmlElement * objects = new TiXmlElement( "Objects" );  
	doc.LinkEndChild( objects );  

	go1->SaveXML(objects);
	doc.SaveFile("save.xml" ); 

	MyGameObjectPtr xml_go (new MyGameObject());


	TiXmlDocument *xmlDoc = new TiXmlDocument("save.xml");
	xmlDoc->LoadFile();
	objects = xmlDoc->FirstChildElement("Objects");
	TiXmlElement* obj = objects->FirstChildElement();
	xml_go->LoadXML(obj);
	// Loop through each template
	/*while(obj)
	{

	obj  = obj->NextSiblingElement();
	}*/
	xmlDoc->Clear();
	// Delete our allocated document and return success ;)
	delete xmlDoc;

	xml_go->DebugPrint();

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




