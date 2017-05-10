#include "Core/Math/GASSPlane.h"
#include "catch.hpp"

TEST_CASE("Test Plane")
{
	SECTION("Test constructor")
	{
		GASS::Vec3 origin(1, 2, 3);
		GASS::Vec3 normal(0, 1, 0);
		GASS::Plane plane(origin, normal);
		REQUIRE(plane.m_Origin == origin);
		REQUIRE(plane.m_Normal == normal);
	}

	SECTION("Test IsFrontFacingTo")
	{
		GASS::Vec3 origin(1, 2, 3);
		GASS::Vec3 normal(0, 1, 0);
		GASS::Plane plane(origin, normal);
		GASS::Vec3 dir(0,0,1);
		REQUIRE(plane.IsFrontFacingTo(dir));
	}

	SECTION("Test SignedDistanceTo")
	{
		GASS::Vec3 origin(1, 2, 3);
		GASS::Vec3 normal(0, 1, 0);
		GASS::Plane plane(origin, normal);
		GASS::Vec3 pos(10, 20, 0);
		GASS::Float dist = plane.SignedDistanceTo(pos);
		REQUIRE(dist == 18);
	}

	SECTION("Test ClassifyPoint")
	{
		GASS::Vec3 origin(1, 2, 3);
		GASS::Vec3 normal(0, 1, 0);
		GASS::Plane plane(origin, normal);
		GASS::Vec3 point(10, 20, 0);
		GASS::PlaneSide side = plane.ClassifyPoint(point);
		REQUIRE(side == GASS::PS_FRONT);
	}

	SECTION("Test RayIsect")
	{
		GASS::Vec3 origin(1, 2, 3);
		GASS::Vec3 normal(0, 1, 0);
		GASS::Plane plane(origin, normal);
		GASS::Ray ray(GASS::Vec3(10, 20, 0), -normal);
		GASS::Float isect = plane.RayIsect(ray);
		REQUIRE(isect == 18);
		GASS::Vec3 isect_p = ray.m_Origin + ray.m_Dir*isect;
		REQUIRE(isect_p == GASS::Vec3(10, 2, 0));
	}

	SECTION("Test GetProjectedVector")
	{
		GASS::Vec3 origin(1, 2, 3);
		GASS::Vec3 normal(0, 1, 0);
		GASS::Plane plane(origin, normal);
		GASS::Vec3 vec(10, 20, 10);
		GASS::Vec3  proj_vec = plane.GetProjectedVector(vec);
		REQUIRE(proj_vec == GASS::Vec3(10, 0, 10));
	}

	
}