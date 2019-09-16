#include "Core/Reflection/GASSRTTI.h"
#include "Core/Reflection/GASSProperty.h"
#include "catch.hpp"


class SimplePropOwner : public GASS::IPropertyOwner
{
public:
	SimplePropOwner() {}
	~SimplePropOwner() {}
	void SetName(const std::string &name)
	{
		m_Name = name;
	}

	std::string GetName() const
	{
		return m_Name;
	}
	std::string m_Name;
};

class Car : public GASS::IPropertyOwner
{
public:
	Car(const std::string &name, float fuel = 0.0f) : m_Name(name),
		m_Fuel(fuel),
		m_Gears(5){}
	
	void SetName(const std::string &name){m_Name = name;}
	std::string GetName() const{return m_Name;}

	void SetFuel(float value) { m_Fuel = value; }
	float GetFuel() const { return m_Fuel; }

	std::string m_Name;
	float m_Fuel;
	int m_Gears;
	
	static void RegisterReflection()
	{
		GASS::IProperty* property = GASS::MakeGetSetProperty("Name", &Car::GetName, &Car::SetName);
		m_RTTI.GetProperties()->push_back(property);
		property = GASS::MakeGetSetProperty("Fuel", &Car::GetFuel, &Car::SetFuel);
		m_RTTI.GetProperties()->push_back(property);

		property = GASS::MakeMemberProperty("Gears", &Car::m_Gears);
		m_RTTI.GetProperties()->push_back(property);
	}
	static GASS::RTTI m_RTTI;
};
GASS::RTTI Car::m_RTTI = GASS::RTTI("SimpleRTTITest", NULL, Car::RegisterReflection);

TEST_CASE("Test RTTI")
{
	SECTION("Test property")
	{
		GASS::IProperty* property = GASS::MakeGetSetProperty("Name", &SimplePropOwner::GetName, &SimplePropOwner::SetName);
		SimplePropOwner po;
		property->SetValueByString(&po,"Hello world");
		std::string name = property->GetValueAsString(&po);
		REQUIRE(name == "Hello world");
	}

	SECTION("Test RTTI Constructor")
	{
		GASS::RTTI rtti("RTTITest", NULL, NULL);
		REQUIRE(rtti.IsTypeOf("RTTITest") == true);
		REQUIRE(rtti.GetAncestorRTTI() == NULL);
		REQUIRE(rtti.HasMetaData() == false);
		REQUIRE(rtti.GetPropertyByName("dummy") == NULL);
	}

	SECTION("Test RTTI and properties with Car class")
	{
		Car mycar("NoName",1.0);
		GASS::IProperty* name_property = mycar.m_RTTI.GetPropertyByName("Name");
		std::string name = name_property->GetValueAsString(&mycar);
		REQUIRE(name == "NoName");
		name_property->SetValueByString(&mycar, "Volvo");
		name = name_property->GetValueAsString(&mycar);
		REQUIRE(name == "Volvo");

		GASS::TypedProperty<float>* fuel_property = dynamic_cast<GASS::TypedProperty<float>* >(mycar.m_RTTI.GetPropertyByName("Fuel"));
		
		float fuel = fuel_property->GetValue(&mycar);
		REQUIRE(fuel == 1.0f);
		fuel_property->SetValue(&mycar, 2.0f);
		fuel = fuel_property->GetValue(&mycar);
		REQUIRE(fuel == 2.0f);

		GASS::TypedProperty<int>* gears_property = dynamic_cast<GASS::TypedProperty<int>*>(mycar.m_RTTI.GetPropertyByName("Gears"));

		int gears = gears_property->GetValue(&mycar);
		REQUIRE(gears == 5);
		gears_property->SetValue(&mycar, 4);
		gears = gears_property->GetValue(&mycar);
		REQUIRE(gears == 4);
	}
}