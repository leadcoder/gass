#include "Core/Math/GASSAABox.h"
#include "Core/Math/GASSMath.h"
#include "catch.hpp"

TEST_CASE("Test AABox")
{
	SECTION("Test constructor")
	{
		const GASS::Vec3 min(1, 2, 3);
		const GASS::Vec3 max(2, 3, 4);
		const GASS::AABox box(min,max);
		REQUIRE(box.Min == min);
		REQUIRE(box.Max == max);
	}
}