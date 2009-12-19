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
		RegisterProperty<std::vector<int>>("IntVec", &TestReflectionObject::GetIntVec, &TestReflectionObject::SetIntVec);
		RegisterProperty<std::vector<std::string>>("StrVec", &TestReflectionObject::GetStrVec, &TestReflectionObject::SetStrVec);
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

private:
	std::string m_String;
	GASS::Float m_Float;
	GASS::Vec2 m_Vec2;
	GASS::Vec3 m_Vec3;
	GASS::Vec4 m_Vec4;
	std::vector<int> m_IntVec;
	std::vector<std::string> m_StrVec;
};


BOOST_AUTO_TEST_CASE( TestGetSetByStringReflection )
{
    //BOOST_CHECK( false );
    // unit test framework can catch operating system signals
    BOOST_TEST_CHECKPOINT("About to test reflection system!");

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
	obj.SetPropertyByString("Vec2","33 34");
	obj.GetPropertyByString("Vec2",value);
	BOOST_CHECK( value == "33 34");
}


BOOST_AUTO_TEST_CASE( TestGetSetByTypeReflection )
{
    //BOOST_CHECK( false );
    // unit test framework can catch operating system signals
    BOOST_TEST_CHECKPOINT("About to test reflection system!");


	
	TestReflectionObject obj;

	boost::any value;
	obj.SetPropertyByType("Float",GASS::Float(99));
	obj.GetPropertyByType("Float",value);
	BOOST_CHECK( boost::any_cast<GASS::Float>(value) == GASS::Float(99));

	
	obj.SetPropertyByType("Vec2",GASS::Vec2(45,54));
	obj.GetPropertyByType("Vec2",value);
	BOOST_CHECK( boost::any_cast<GASS::Vec2>(value) == GASS::Vec2(45,54));

	obj.SetPropertyByType("Vec3",GASS::Vec3(1,2,77));
	obj.GetPropertyByType("Vec3",value);
	BOOST_CHECK( boost::any_cast<GASS::Vec3>(value) == GASS::Vec3(1,2,77));

	obj.SetPropertyByType("Vec4",GASS::Vec4(121,22,177,42));
	obj.GetPropertyByType("Vec4",value);
	BOOST_CHECK( boost::any_cast<GASS::Vec4>(value) == GASS::Vec4(121,22,177,42));

	/*value = "";
	obj.SetPropertyByString("IntVec","221 22 177");
	obj.GetPropertyByString("IntVec",value);
	BOOST_CHECK( value == "221 22 177");

	value = "";
	obj.SetPropertyByString("StrVec","string vector test");
	obj.GetPropertyByString("StrVec",value);
	BOOST_CHECK( value == "string vector test");
	*/
}


/*
int main(int argc, char* argv[])
{
	TestReflection();
	return 0;
}*/




