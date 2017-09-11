#include "Core/Reflection/GASSRTTI.h"
#include "Core/Reflection/GASSProperty.h"
#include "catch.hpp"

class Car : public GASS::IPropertyOwner
{
public:
	Car(const std::string &name, float fuel = 0.0f) : m_Name(name),
		m_Fuel(fuel){}
	~Car() {}
	void SetName(const std::string &name){m_Name = name;}
	std::string GetName() const{return m_Name;}

	void SetFuel(float value) { m_Fuel = value; }
	float GetFuel() const { return m_Fuel; }

	std::string m_Name;
	float m_Fuel;
	
	static void RegisterReflection()
	{
		GASS::IProperty* property = new GASS::Property<Car, std::string>("Name", &Car::GetName, &Car::SetName, GASS::PropertyMetaDataPtr());
		m_RTTI.GetProperties()->push_back(property);
		property = new GASS::Property<Car, float>("Fuel", &Car::GetFuel, &Car::SetFuel, GASS::PropertyMetaDataPtr());
		m_RTTI.GetProperties()->push_back(property);
	}
	static GASS::RTTI m_RTTI;
};
GASS::RTTI Car::m_RTTI = GASS::RTTI("SimpleRTTITest", NULL, Car::RegisterReflection);

TEST_CASE("Test RTTI")
{
	SECTION("Test property")
	{
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

		GASS::IProperty* property = new GASS::Property<SimplePropOwner, std::string>("Name", &SimplePropOwner::GetName, &SimplePropOwner::SetName, GASS::PropertyMetaDataPtr());
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
		GASS::IProperty* property = mycar.m_RTTI.GetPropertyByName("Name");
		std::string name = property->GetValueAsString(&mycar);
		REQUIRE(name == "NoName");
		property->SetValueByString(&mycar, "Volvo");
		name = property->GetValueAsString(&mycar);
		REQUIRE(name == "Volvo");

		GASS::TypedProperty<float>* fuel_property = dynamic_cast<GASS::TypedProperty<float>* >(mycar.m_RTTI.GetPropertyByName("Fuel"));
		
		float fule = fuel_property->GetValue(&mycar);
		REQUIRE(fule == 1.0f);
		fuel_property->SetValue(&mycar, 2.0f);
		fule = fuel_property->GetValue(&mycar);
		REQUIRE(fule == 2.0f);

	}
}