#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSMath.h"
#include "catch.hpp"


TEST_CASE("Test Vec2")
{
	SECTION("Test constructor")
	{
		GASS::Vec2 vec(1, 1);
		REQUIRE(vec.x == 1);
		REQUIRE(vec.y == 1);
	}

	SECTION("Test convert , from Vec3d to Vec3f")
	{
		GASS::Vec2d vecd(1, 2);
		GASS::Vec2f vecf = GASS::Vec2f::Convert(vecd);
		REQUIRE(vecf.x == 1);
		REQUIRE(vecf.y == 2);
	}

	SECTION("Test convert , from Vec3f to Vec3d")
	{
		GASS::Vec2d vecf(1, 2);
		GASS::Vec2f vecd = GASS::Vec2d::Convert(vecf);
		REQUIRE(vecd.x == 1);
		REQUIRE(vecd.y == 2);

		GASS::Vec2i vec2(1, 2);
		GASS::Vec2i vec4(1, 2);
		GASS::Vec2i vec3 = vec4 + vec2;
		REQUIRE(vec3 == GASS::Vec2i(2, 4));
	}

	SECTION("Test set")
	{
		GASS::Vec2 vec;
		vec.Set(1, 1);
		REQUIRE(vec.x == 1);
		REQUIRE(vec.y == 1);
	}

	SECTION("Test operator=")
	{
		GASS::Vec2 vec(1, 1);
		REQUIRE(vec == GASS::Vec2(1, 1));
	}

	SECTION("Test Equal")
	{
		GASS::Vec2 vec1(1.1, 1.3);
		GASS::Vec2 vec2(1.0, 1.0);

		//test if in tolerance
		REQUIRE(vec1.Equal(vec2, 0.4) == true);

		//test if out of tolerance
		REQUIRE(vec1.Equal(vec2, 0.2) == false);
	}

	SECTION("Test operator+")
	{
		GASS::Vec2 vec1(1, 1);
		GASS::Vec2 vec2(2, 2);
		GASS::Vec2 res = vec1 + vec2;
		REQUIRE(res == GASS::Vec2(3, 3));
	}

	SECTION("Test operator +=")
	{
		GASS::Vec2 vec1(1, 1);
		GASS::Vec2 vec2(2, 2);
		vec1 += vec2;
		REQUIRE(vec1 == GASS::Vec2(3, 3));
	}

	SECTION("Test operator- 1")
	{
		GASS::Vec2 vec1(2, 2);
		GASS::Vec2 vec2(1, 1);
		GASS::Vec2 res = vec1 - vec2;

		REQUIRE(res == GASS::Vec2(1, 1));
	}

	SECTION("Test operator- 2")
	{
		GASS::Vec2 vec(1, 1);
		GASS::Vec2 res = -vec;
		REQUIRE(res == GASS::Vec2(-1, -1));
	}

	SECTION("Test operator -=")
	{
		GASS::Vec2 res(2, 2);
		res -= GASS::Vec2(1, 1);
		REQUIRE(res == GASS::Vec2(1, 1));
	}

	SECTION("Test operator* (scalar)")
	{
		GASS::Vec2 res = GASS::Vec2(2, 2) * 2;
		REQUIRE(res == GASS::Vec2(4, 4));
	}

	SECTION("Test operator=* (scalar)")
	{
		GASS::Vec2 res(2, 2);
		res *= 2;
		REQUIRE(res == GASS::Vec2(4, 4));
	}

	SECTION("Test operator* (Vec2)")
	{
		GASS::Vec2 res = GASS::Vec2(2, 2) * GASS::Vec2(2, 1);
		REQUIRE(res == GASS::Vec2(4, 2));
	}

	SECTION("Test operator=* (Vec2)")
	{
		GASS::Vec2 res(2, 2);
		res *= GASS::Vec2(2, 1);
		REQUIRE(res == GASS::Vec2(4, 2));
	}

	SECTION("Test operator/ (Vec2)")
	{
		GASS::Vec2 res = GASS::Vec2(2, 2) / GASS::Vec2(2, 1);
		REQUIRE(res == GASS::Vec2(1, 2));
	}

	SECTION("Test operator/ (Scalar)")
	{
		GASS::Vec2 res = GASS::Vec2(2, 2) / 2;
		REQUIRE(res == GASS::Vec2(1, 1));
	}

	SECTION("Test operator[] ")
	{
		GASS::Vec2 vec(2, 3);
		REQUIRE(vec[0] == 2);
		REQUIRE(vec[1] == 3);
	}

	SECTION("Test operator * (Type scalar, const TVec2& v) ")
	{
		GASS::Vec2 res = 2 * GASS::Vec2(2, 4);
		REQUIRE(res == GASS::Vec2(4, 8));
	}

	SECTION("Test operator != ")
	{
		REQUIRE(GASS::Vec2(4, 2) != GASS::Vec2(4, 4));
	}

	SECTION("Test SquaredLength")
	{
		REQUIRE(GASS::Vec2(1, 2).SquaredLength() == 5);
	}

	SECTION("Test Length")
	{
		REQUIRE(GASS::Vec2(9, 0).Length() == 9);

		REQUIRE(GASS::Vec2(0, 0).Length() == 0);
	}

	SECTION("Test Normalize")
	{
		GASS::Vec2 vec(2, 0);
		vec.Normalize();
		REQUIRE(vec == GASS::Vec2(1, 0));

		//Test zero vector
		vec.Set(0, 0);
		vec.Normalize();
		//for now 1,0,0 should be returned, consider 0,0,0 instead 
		REQUIRE(vec == GASS::Vec2(1, 0));
	}

	SECTION("Test streaming <<")
	{
		std::stringstream ss;
		GASS::Vec2 vec(2, 0);
		ss << vec;
		const std::string str_vec = ss.str();
		REQUIRE(str_vec == "2 0");
	}

	SECTION("Test streaming >>")
	{
		std::stringstream ss;
		ss << "2 0";
		GASS::Vec2 vec;
		ss >> vec;
		REQUIRE(vec == GASS::Vec2(2, 0));
	}
}

TEST_CASE("Test Vec3")
{
	SECTION("Test constructor")
	{
		GASS::Vec3 vec(1, 1, 1);
		REQUIRE(vec.x == 1);
		REQUIRE(vec.y == 1);
		REQUIRE(vec.z == 1);
	}

	SECTION("Test convert , from Vec3d to Vec3f")
	{
		GASS::Vec3d vecd(1, 2, 3);
		GASS::Vec3f vecf = GASS::Vec3f::Convert(vecd);
		REQUIRE(vecf.x == 1);
		REQUIRE(vecf.y == 2);
		REQUIRE(vecf.z == 3);
	}

	SECTION("Test convert , from Vec3f to Vec3d")
	{
		GASS::Vec3d vecf(1, 2, 3);
		GASS::Vec3f vecd = GASS::Vec3d::Convert(vecf);
		REQUIRE(vecd.x == 1);
		REQUIRE(vecd.y == 2);
		REQUIRE(vecd.z == 3);
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

	

	SECTION("Test Equal")
	{
		GASS::Vec3 vec1(1.1, 1.2, 1.3);
		GASS::Vec3 vec2(1.0, 1.0, 1.0);

		//test if in tolerance
		REQUIRE(vec1.Equal(vec2, 0.4) == true);
		
		//test if out of tolerance
		REQUIRE(vec1.Equal(vec2, 0.2) == false);
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

		REQUIRE(GASS::Vec3(0, 0, 0).Length() == 0);
	}

	SECTION("Test Normalize")
	{
		GASS::Vec3 vec(2, 0, 0);
		vec.Normalize();
		REQUIRE(vec == GASS::Vec3(1,0,0));

		//Test zero vector
		vec.Set(0, 0, 0);
		vec.Normalize();
		//for now 1,0,0 should be returned, consider 0,0,0 instead 
		REQUIRE(vec == GASS::Vec3(1, 0, 0));
	}

	SECTION("Test streaming <<")
	{
		std::stringstream ss;
		GASS::Vec3 vec(2, 0, 0);
		ss << vec;
		const std::string str_vec = ss.str();
		REQUIRE(str_vec == "2 0 0");
	}

	SECTION("Test streaming >>")
	{
		std::stringstream ss;
		ss << "2 0 0";
		GASS::Vec3 vec;
		ss >> vec;
		REQUIRE(vec == GASS::Vec3(2, 0, 0));
	}

	/*SECTION("Test FastNormalize")
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
	}*/
}