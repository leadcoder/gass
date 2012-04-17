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

#include "Core/PluginSystem/PluginManager.h"
#include "tinyxml.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "Core/Reflection/Reflection.h"

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
using namespace boost::unit_test;



class TestReflectionObject :  public GASS::Reflection<TestReflectionObject, GASS::BaseReflectionObject> 
{
public:
	static void	RegisterReflection()
	{
		RegisterProperty<std::string>("String", &TestReflectionObject::GetString, &TestReflectionObject::SetString);
		RegisterProperty<GASS::Vec2>("Vec2", &TestReflectionObject::GetVec2, &TestReflectionObject::SetVec2);
		RegisterProperty<GASS::Vec3>("Vec3", &TestReflectionObject::GetVec3, &TestReflectionObject::SetVec3);
		RegisterProperty<GASS::Vec4>("Vec4", &TestReflectionObject::GetVec4, &TestReflectionObject::SetVec4);
		RegisterProperty<GASS::Float>("Float", &TestReflectionObject::GetFloat, &TestReflectionObject::SetFloat);
		RegisterVectorProperty<int>("IntVec", &TestReflectionObject::GetIntVec, &TestReflectionObject::SetIntVec);
		RegisterVectorProperty<std::string>("StrVec", &TestReflectionObject::GetStrVec, &TestReflectionObject::SetStrVec);
		RegisterVectorProperty<GASS::Vec2>("Vec2Vec", &TestReflectionObject::GetVec2Vec, &TestReflectionObject::SetVec2Vec);
	}
	std::string GetString()const {return m_String;}
	void SetString(const std::string &value){m_String = value;}
	GASS::Float GetFloat()const {return m_Float;}
	void SetFloat(const GASS::Float &value){m_Float = value;}
	
	GASS::Vec2 GetVec2()const {return m_Vec2;}
	void SetVec2(const GASS::Vec2 &value){m_Vec2 = value;}
	GASS::Vec3 GetVec3()const {return m_Vec3;}
	void SetVec3(const GASS::Vec3 &value){m_Vec3 = value;}
	GASS::Vec4 GetVec4()const {return m_Vec4;}
	void SetVec4(const GASS::Vec4 &value){m_Vec4 = value;}
	
	std::vector<int> GetIntVec()const {return m_IntVec;}
	void SetIntVec(const std::vector<int> &vec){m_IntVec= vec;}
	std::vector<std::string> GetStrVec()const {return m_StrVec;}
	void SetStrVec(const std::vector<std::string> &vec){m_StrVec= vec;}
	std::vector<GASS::Vec2> GetVec2Vec()const {return m_Vec2Vec;}
	void SetVec2Vec(const std::vector<GASS::Vec2> &value){m_Vec2Vec = value;}
	

private:
	std::string m_String;
	GASS::Float m_Float;
	GASS::Vec2 m_Vec2;
	GASS::Vec3 m_Vec3;
	GASS::Vec4 m_Vec4;
	std::vector<int> m_IntVec;
	std::vector<std::string> m_StrVec;
	std::vector<GASS::Vec2> m_Vec2Vec;
};


BOOST_AUTO_TEST_CASE( TestGetSetByStringReflection )
{
    //BOOST_CHECK( false );
    // unit test framework can catch operating system signals
    //BOOST_TEST_CHECKPOINT("About to test reflection system!");

	std::string value;
	TestReflectionObject obj;

	value = "";
	obj.SetPropertyByString("Float","99");
	obj.GetPropertyByString("Float",value);
	BOOST_CHECK( value == "99");

	value = "";
	obj.SetPropertyByString("Vec2","45 54");
	obj.GetPropertyByString("Vec2",value);
	BOOST_CHECK( value == "45 54");

	value = "";
	obj.SetPropertyByString("Vec3","1 2 77");
	obj.GetPropertyByString("Vec3",value);
	BOOST_CHECK( value == "1 2 77");

	value = "";
	obj.SetPropertyByString("Vec4","121 22 177 42");
	obj.GetPropertyByString("Vec4",value);
	BOOST_CHECK( value == "121 22 177 42");

	value = "";
	obj.SetPropertyByString("IntVec","221 22 177");
	obj.GetPropertyByString("IntVec",value);
	BOOST_CHECK( value == "221 22 177");

	value = "";
	obj.SetPropertyByString("StrVec","string vector test");
	obj.GetPropertyByString("StrVec",value);
	BOOST_CHECK( value == "string vector test");

	value = "";
	obj.SetPropertyByString("Vec2Vec","33 34 23 24");
	obj.GetPropertyByString("Vec2Vec",value);
	BOOST_CHECK( value == "33 34 23 24");
}


BOOST_AUTO_TEST_CASE( TestGetSetByTypeReflection )
{
    //BOOST_CHECK( false );
    // unit test framework can catch operating system signals
   // BOOST_TEST_CHECKPOINT("About to test reflection system!");

	TestReflectionObject obj;

	boost::any value;
	GASS::Float input1(99);
	obj.SetPropertyByType("Float",input1);
	obj.GetPropertyByType("Float",value);
	BOOST_CHECK( boost::any_cast<GASS::Float>(value) == input1);

	GASS::Vec2 input2(45,54);
	obj.SetPropertyByType("Vec2",input2);
	obj.GetPropertyByType("Vec2",value);
	BOOST_CHECK( boost::any_cast<GASS::Vec2>(value) == input2);

	GASS::Vec3 input3(1,2,77);
	obj.SetPropertyByType("Vec3",input3);
	obj.GetPropertyByType("Vec3",value);
	BOOST_CHECK( boost::any_cast<GASS::Vec3>(value) == input3);

	GASS::Vec4 input4(121,22,177,42);
	obj.SetPropertyByType("Vec4",input4);
	obj.GetPropertyByType("Vec4",value);
	BOOST_CHECK( boost::any_cast<GASS::Vec4>(value) == input4);
	
}

BOOST_AUTO_TEST_CASE( TestReflectionSerialization )
{
    //BOOST_CHECK( false );
    // unit test framework can catch operating system signals
    //BOOST_TEST_CHECKPOINT("About to test reflection system!");

	TestReflectionObject obj;

	obj.SetString("My test object");
	GASS::Float float_val(99);
	obj.SetFloat(float_val);
	
	GASS::Vec2 vec2_val(45,54);
	obj.SetVec2(vec2_val);
	
	GASS::Vec3 vec3_val(1,2,77);
	obj.SetVec3(vec3_val);

	GASS::Vec4 vec4_val(121,22,177,42);
	obj.SetVec4(vec4_val);

	std::vector<std::string> str_vec;
	str_vec.push_back("Hello");
	str_vec.push_back("World");
	obj.SetStrVec(str_vec);


	TiXmlDocument* doc = new TiXmlDocument();  
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
	doc->LinkEndChild( decl );  

	TiXmlElement * object_elem = new TiXmlElement( "Object" );  
	doc->LinkEndChild(object_elem);  

	obj.SaveProperties(object_elem);

	TestReflectionObject obj2;
	obj2.LoadProperties(object_elem);
	delete doc;

	BOOST_CHECK( obj.GetFloat() == obj2.GetFloat());
	BOOST_CHECK( obj.GetString() == obj2.GetString());
	BOOST_CHECK( obj.GetVec2() == obj2.GetVec2());
	BOOST_CHECK( obj.GetVec3() == obj2.GetVec3());
	BOOST_CHECK( obj.GetVec4() == obj2.GetVec4());
	BOOST_CHECK( obj.GetStrVec() == obj2.GetStrVec());


	//Test serializing
	GASS::SerialSaver ss(NULL,0);
	obj.SerializeProperties(&ss);
	unsigned long size=ss.getLength();

	unsigned char *buffer=new unsigned char[size];
	GASS::SerialSaver sv(buffer,size);
	obj.SerializeProperties(&sv);

	TestReflectionObject bin_obj;
	GASS::SerialLoader sl(buffer, size);
	bin_obj.SerializeProperties(&sl);

	BOOST_CHECK( obj.GetFloat() == bin_obj.GetFloat());
	BOOST_CHECK( obj.GetString() == bin_obj.GetString());
	BOOST_CHECK( obj.GetVec2() == bin_obj.GetVec2());
	BOOST_CHECK( obj.GetVec3() == bin_obj.GetVec3());
	BOOST_CHECK( obj.GetVec4() == bin_obj.GetVec4());
	BOOST_CHECK( obj.GetStrVec() == bin_obj.GetStrVec());

		

	/*std::ofstream outfile;
	outfile.open("test.bin", std::ios::out | std::ios::trunc | std::ios::binary);
	outfile.write( ((char *)buffer), size );
	outfile.close();

	std::ifstream infile ("test.bin", std::ios::in|std::ios::binary|std::ios::ate);

	
	
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
	}*/


	//Test object copy

	boost::shared_ptr<TestReflectionObject> obj3 (new TestReflectionObject) ;
	obj.SetProperties(obj3);

	BOOST_CHECK( obj.GetFloat() == obj3->GetFloat());
	BOOST_CHECK( obj.GetString() == obj3->GetString());
	BOOST_CHECK( obj.GetVec2() == obj3->GetVec2());
	BOOST_CHECK( obj.GetVec3() == obj3->GetVec3());
	BOOST_CHECK( obj.GetVec4() == obj3->GetVec4());
	BOOST_CHECK( obj.GetStrVec() == obj3->GetStrVec());
	

}


///////////////////////////TEst component system


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

typedef boost::shared_ptr<MyGameObject> MyGameObjectPtr;
typedef boost::shared_ptr<MyGameObjectTemplate> MyGameObjectTemplatePtr;
typedef boost::shared_ptr<TestComponent> TestComponentPtr;
typedef boost::weak_ptr<TestComponent> TestComponentWeakPtr;




BOOST_AUTO_TEST_CASE( TestComponentSystem)
{
	boost::shared_ptr<GASS::BaseComponentContainerTemplateManager> template_manager( new GASS::BaseComponentContainerTemplateManager());
    //BOOST_CHECK( false );
    // unit test framework can catch operating system signals
    //BOOST_TEST_CHECKPOINT("About to test reflection system!");

	MyGameObjectTemplatePtr go1(new MyGameObjectTemplate());
	go1->SetName("MyManualGameObject");
	MyGameObjectTemplatePtr child_go (new MyGameObjectTemplate());
	child_go->SetName("ChildGameObject");
	go1->AddChild(child_go);
	TestComponentPtr tc (new TestComponent());
	tc->SetName("TestComp");
	tc->SetSize(1000);
	tc->SetName("MyTestComponent");
	go1->AddComponent(tc);
	//LocationComponent* clc = new LocationComponent();
	child_go->AddComponent(tc);

	//go1->OnCreate();
	//boost::shared_ptr<GASS::Message> init_msg(new GASS::Message(MESSAGE_INIT,100));
	//mm.SendImmediate(init_msg);
	TiXmlDocument* doc = new TiXmlDocument();  
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
	doc->LinkEndChild( decl );  

	TiXmlElement * template_elem = new TiXmlElement( "Templates" );  
	doc->LinkEndChild(template_elem);  
	go1->SaveXML(template_elem);

	doc->SaveFile("debug.xml" );
	MyGameObjectTemplatePtr go2(new MyGameObjectTemplate());
	go2->LoadXML(template_elem->FirstChildElement());

	

	delete doc;

	BOOST_CHECK( go1->GetName() == go2->GetName());

	template_manager->AddTemplate(go1);

	boost::shared_ptr<GASS::BaseComponentContainer> bo = boost::shared_static_cast<GASS::BaseComponentContainer>( template_manager->CreateFromTemplate("MyManualGameObject"));


	doc = new TiXmlDocument();  
	decl = new TiXmlDeclaration( "1.0", "", "" );  
	doc->LinkEndChild( decl );  

	template_elem = new TiXmlElement( "Objects" );  
	doc->LinkEndChild(template_elem);  
	bo->SaveXML(template_elem);
	doc->SaveFile("debug.xml" );

	/*BOOST_CHECK( obj.GetFloat() == obj2.GetFloat());
	BOOST_CHECK( obj.GetString() == obj2.GetString());
	BOOST_CHECK( obj.GetVec2() == obj2.GetVec2());
	BOOST_CHECK( obj.GetVec3() == obj2.GetVec3());
	BOOST_CHECK( obj.GetVec4() == obj2.GetVec4());
	BOOST_CHECK( obj.GetStrVec() == obj2.GetStrVec());*/


	
		


}



/*
int main(int argc, char* argv[])
{
	TestReflection();
	return 0;
}*/




