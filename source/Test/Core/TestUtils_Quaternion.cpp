#include "Core/Math/GASSQuaternion.h"
#include "catch.hpp"


TEST_CASE("Test Quaternion")
{
	SECTION("Test Quaternion rotations")
	{
		GASS::Quaternion q1 = GASS::Quaternion::CreateFromEulerYXZ(GASS::Vec3(0, 0,0));
		GASS::Quaternion q2 = GASS::Quaternion::CreateFromEulerYXZ(GASS::Vec3(GASS_HALF_PI, GASS_HALF_PI, 0));
		GASS::Quaternion q3 = GASS::Quaternion::CreateFromEulerYXZ(GASS::Vec3(0, GASS_PI, 0));
		GASS::Quaternion q4 = q1 * q2 * q3;
		auto xa = q4.GetXAxis();
		auto ya = q4.GetYAxis();
		auto za = q4.GetZAxis();
		REQUIRE(xa.Equal(GASS::Vec3(0, 0, 1), 1.0e-10));
		REQUIRE(ya.Equal(GASS::Vec3(1, 0, 0), 1.0e-10));
		REQUIRE(za.Equal(GASS::Vec3(0, 1, 0), 1.0e-10));
	}
}