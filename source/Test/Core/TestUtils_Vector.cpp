#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSMath.h"

#include "catch.hpp"

TEST_CASE("Test Vec3")
{
	SECTION("Test constructor")
	{
		GASS::Vec3 vec(1,1,1);
		REQUIRE(vec.x == 1);
		REQUIRE(vec.y == 1);
		REQUIRE(vec.z == 1);
	}

	SECTION("Test set")
	{
		GASS::Vec3 vec;
		vec.Set(1, 1, 1);
		REQUIRE(vec.x == 1);
		REQUIRE(vec.y == 1);
		REQUIRE(vec.z == 1);
	}

	SECTION("Test operator=")
	{
		GASS::Vec3 vec(1, 1, 1);
		REQUIRE(vec == GASS::Vec3(1, 1, 1));
	}

	SECTION("Test operator+")
	{
		GASS::Vec3 vec1(1, 1, 1);
		GASS::Vec3 vec2(2, 2, 2);
		GASS::Vec3 res = vec1 + vec2;
		REQUIRE(res == GASS::Vec3(3, 3, 3));
	}

	SECTION("Test operator +=")
	{
		GASS::Vec3 vec1(1, 1, 1);
		GASS::Vec3 vec2(2, 2, 2);
		vec1 += vec2;
		REQUIRE(vec1 == GASS::Vec3(3, 3, 3));
	}

	SECTION("Test operator- 1")
	{
		GASS::Vec3 vec1(2, 2, 2);
		GASS::Vec3 vec2(1, 1, 1);
		GASS::Vec3 res = vec1 - vec2;
	
		REQUIRE(res == GASS::Vec3(1, 1, 1));
	}

	SECTION("Test operator- 2")
	{
		GASS::Vec3 vec(1, 1, 1);
		GASS::Vec3 res = -vec;
		REQUIRE(res == GASS::Vec3(-1, -1, -1));
	}

	SECTION("Test operator -=")
	{
		GASS::Vec3 res(2, 2, 2);
		res -= GASS::Vec3(1,1,1);
		REQUIRE(res == GASS::Vec3(1, 1, 1));
	}

	SECTION("Test operator* (scalar)")
	{
		GASS::Vec3 res = GASS::Vec3(2, 2, 2)*2;
		REQUIRE(res == GASS::Vec3(4, 4, 4));
	}

	SECTION("Test operator=* (scalar)")
	{
		GASS::Vec3 res(2, 2, 2);
		res *= 2;
		REQUIRE(res == GASS::Vec3(4, 4, 4));
	}

	SECTION("Test operator* (Vec3)")
	{
		GASS::Vec3 res = GASS::Vec3(2, 2, 2) * GASS::Vec3(2, 1, 3);
		REQUIRE(res == GASS::Vec3(4, 2, 6));
	}

	SECTION("Test operator=* (Vec3)")
	{
		GASS::Vec3 res(2, 2, 2);
		res *= GASS::Vec3(2, 1, 3);
		REQUIRE(res == GASS::Vec3(4, 2, 6));
	}

	SECTION("Test operator/ (Vec3)")
	{
		GASS::Vec3 res = GASS::Vec3(2, 2, 6) / GASS::Vec3(2, 1, 2);
		REQUIRE(res == GASS::Vec3(1, 2, 3));
	}

	SECTION("Test operator/ (Scalar)")
	{
		GASS::Vec3 res = GASS::Vec3(2, 2, 6) / 2;
		REQUIRE(res == GASS::Vec3(1, 1, 3));
	}
	
	SECTION("Test operator[] ")
	{
		GASS::Vec3 vec(2, 3, 4);
		REQUIRE(vec[0] == 2);
		REQUIRE(vec[1] == 3);
		REQUIRE(vec[2] == 4);
	}

	SECTION("Test operator * (Type scalar, const TVec3& v) ")
	{
		GASS::Vec3 res = 2 * GASS::Vec3(2, 2, 2);
		REQUIRE(res == GASS::Vec3(4, 4, 4));
	}

	SECTION("Test operator != ")
	{
		REQUIRE(GASS::Vec3(4, 2, 4) != GASS::Vec3(4, 4, 4));
	}
	
	SECTION("Test SquaredLength")
	{
		REQUIRE(GASS::Vec3(1, 2, 2).SquaredLength() == 9);
	}

	SECTION("Test Length")
	{
		REQUIRE(GASS::Vec3(1, 2, 2).Length() == 3);
	}

	SECTION("Test Normalize")
	{
		GASS::Vec3 vec(2, 0, 0);
		vec.Normalize();
		REQUIRE(vec == GASS::Vec3(1,0,0));
	}

	SECTION("Test FastNormalize")
	{
		GASS::Vec3 vec(2, 0, 0);
		vec.FastNormalize();
		REQUIRE(vec == GASS::Vec3(1, 0, 0));
	}

	SECTION("Test Fast Length")
	{
		REQUIRE(GASS::Vec3(1, 2, 2).FastLength() == 3);
	}

	SECTION("Test Fast Inv Length")
	{
		REQUIRE(GASS::Vec3(1, 0, 0).FastInvLength() == 1);
	}
}