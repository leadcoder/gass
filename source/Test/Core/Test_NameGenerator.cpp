#include "Core/Utils/GASSNameGenerator.h"
#include "catch.hpp"


TEST_CASE("Test NameGenerator")
{
	SECTION("Test NameGenerator")
	{
		GASS::NameGenerator ng;
		{
			auto car1 = ng.CreateUniqueName("Car");
			REQUIRE(car1 == "Car");
			auto car2 = ng.CreateUniqueName("Car");
			REQUIRE(car2 == "Car (2)");
			auto car3 = ng.CreateUniqueName("Car");
			REQUIRE(car3 == "Car (3)");

			auto human1 = ng.CreateUniqueName("Human");
			REQUIRE(human1 == "Human");
			auto human2 = ng.CreateUniqueName("Human");
			REQUIRE(human2 == "Human (2)");
		}
		
		ng = GASS::NameGenerator(true);
		{
			auto human1 = ng.CreateUniqueName("Human");
			REQUIRE(human1 == "Human (1)");
			auto human2 = ng.CreateUniqueName("Human");
			REQUIRE(human2 == "Human (2)");
		}

		ng = GASS::NameGenerator(true);
		{
			auto human1 = ng.CreateUniqueName("Human");
			REQUIRE(human1 == "Human (1)");
			auto human2 = ng.CreateUniqueName("Human");
			REQUIRE(human2 == "Human (2)");
		}
	}
}