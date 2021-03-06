#include "Core/Reflection/GASSProperty.h"
#include "catch.hpp"

class ReflectionTest
{
public:
	std::string GetName() const { return m_Name; }
	void SetName(const std::string& name) { m_Name = name; }
	std::string m_Name;
private:
	
};

TEST_CASE("Test Property")
{
	auto member_prop = GASS::MakeMemberProperty("Name", &ReflectionTest::m_Name, GASS::PF_RESET, "Name property", GASS::PropertyMetaDataPtr());
	auto getset_property = GASS::MakeGetSetProperty("Name", &ReflectionTest::GetName, &ReflectionTest::SetName, GASS::PF_RESET, "Name property", GASS::PropertyMetaDataPtr());

	ReflectionTest obj;
	getset_property->Set(&obj, "Hello");
	std::string value =  getset_property->Get(&obj);
	member_prop->Set(&obj, "World");
	value = getset_property->Get(&obj);
}