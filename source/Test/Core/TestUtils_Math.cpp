
#include "Core/Math/GASSMath.h"
#include "catch.hpp"

TEST_CASE("Test Math")
{
	SECTION("Test Deg2Rad")
	{
		float radf = GASS::Math::Deg2Rad(180.0f);
		REQUIRE(GASS::Math::Equal(radf, static_cast<float>(GASS_PI), 0.001f) == true);

		double radd = GASS::Math::Deg2Rad(180.0);
		REQUIRE(GASS::Math::Equal(radd, GASS_PI, 0.00001) == true);
	}

	SECTION("Test Rad2Deg")
	{
		float deg = GASS::Math::Rad2Deg(static_cast<float>(GASS_PI));
		REQUIRE(GASS::Math::Equal(deg, 180.0f, 0.001f) == true);
	}
}