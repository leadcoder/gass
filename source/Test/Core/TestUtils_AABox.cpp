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

	SECTION("Test LineIntersect")
	{
		const GASS::Vec3 min(0, 0, 0);
		const GASS::Vec3 max(2, 2, 2);
		const GASS::AABox box(min, max);
		GASS::Float line_dist = 0;
		GASS::LineSegment line(GASS::Vec3(1, 1, 4), GASS::Vec3(1, 1, 0));
		bool hit = box.LineIntersect(line, line_dist);
		REQUIRE(hit == true);
		REQUIRE(0.5 == line_dist);
		GASS::Vec3 isect_point = line.GetPoint(line_dist);
		REQUIRE(isect_point == GASS::Vec3(1, 1, 2));
	}

	SECTION("Test LineIntersect edge")
	{
		const GASS::Vec3 min(0, 0, 0);
		const GASS::Vec3 max(2, 2, 2);
		const GASS::AABox box(min, max);
		GASS::Float line_dist = 0;
		GASS::LineSegment line(GASS::Vec3(0, 0, 4), GASS::Vec3(0, 0, 0));
		bool hit = box.LineIntersect(line, line_dist);
		REQUIRE(hit == true);
		REQUIRE(0.5 == line_dist);
		GASS::Vec3 isect_point = line.GetPoint(line_dist);
		REQUIRE(isect_point == GASS::Vec3(0, 0, 2));
	}

	SECTION("Test LineIntersect inside box")
	{
		const GASS::Vec3 min(0, 0, 0);
		const GASS::Vec3 max(2, 2, 2);
		const GASS::AABox box(min, max);
		GASS::Float line_dist = 0;
		GASS::LineSegment line(GASS::Vec3(1, 1, 1), GASS::Vec3(0, 3, 0));
		bool hit = box.LineIntersect(line, line_dist);
		REQUIRE(hit == true);
		REQUIRE(0 == line_dist);
		GASS::Vec3 isect_point = line.GetPoint(line_dist);
		REQUIRE(isect_point == GASS::Vec3(1, 1, 1));
	}

	SECTION("Test LineIntersect on box side")
	{
		const GASS::Vec3 min(0, 0, 0);
		const GASS::Vec3 max(2, 2, 2);
		const GASS::AABox box(min, max);
		GASS::Float line_dist = 0;
		GASS::LineSegment line(GASS::Vec3(1, 1, 2), GASS::Vec3(1, 1, 3));
		bool hit = box.LineIntersect(line, line_dist);
		REQUIRE(hit == true);
		REQUIRE(0 == line_dist);
		GASS::Vec3 isect_point = line.GetPoint(line_dist);
		REQUIRE(isect_point == GASS::Vec3(1, 1, 2));
	}

}