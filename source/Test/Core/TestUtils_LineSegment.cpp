#include "Core/Math/GASSLineSegment.h"
#include "catch.hpp"

TEST_CASE("Test LineSegment")
{

	SECTION("Test Constructor")
	{
		const GASS::Vec3 p1(0, 0, 0);
		const GASS::Vec3 p2(0, 0, 4);
		const GASS::LineSegment line(p1, p2);
		REQUIRE(line.m_Start.Equal(p1) == true);
		REQUIRE(line.m_End.Equal(p2) == true);
	}

	SECTION("Test ClosestPointOnLine")
	{
		const GASS::Vec3 p1(0, 0, 0);
		const GASS::Vec3 p2(0, 0, 4);
		const GASS::LineSegment line(p1, p2);
		
		{
			//test that start point is closest 
			const GASS::Vec3 point = line.ClosestPointOnLine(GASS::Vec3(0, 0, -4));
			REQUIRE(point.Equal(p1) == true);
		}

		{
			//test that end point is closest
			const GASS::Vec3 point = line.ClosestPointOnLine(GASS::Vec3(0, 0, 6));
			REQUIRE(point.Equal(p2) == true);
		}

		{
			//test point on line
			const GASS::Vec3 point_on_line(0, 0, 2);
			const GASS::Vec3 point = line.ClosestPointOnLine(point_on_line);
			REQUIRE(point.Equal(point_on_line) == true);
		}

		{ 
			//test point off line
			const GASS::Vec3 point_off_line(0, 10, 2);
			const GASS::Vec3 point = line.ClosestPointOnLine(GASS::Vec3(11, 11, 13));
			REQUIRE(point.Equal(p2) == true);
		}
	}
}