#include "Core/Math/GASSLineSegment2D.h"
#include "Core/Math/GASSLineSegment.h"
#include "catch.hpp"


TEST_CASE("Test LineSegment2D")
{

	SECTION("Test Constructor")
	{
		const GASS::Vec2f p1(0, 0);
		const GASS::Vec2f p2(0, 4);
		const GASS::LineSegment2Df line(p1, p2);
		REQUIRE(line.Start.Equal(p1) == true);
		REQUIRE(line.End.Equal(p2) == true);
	}

	SECTION("Test GetPoint")
	{
		const GASS::Vec2f p1(0, 0);
		const GASS::Vec2f p2(0, 4);
		const GASS::LineSegment2Df line(p1, p2);
		{
			//test that start point is closest 
			const GASS::Vec2f point = line.GetPoint(0.5f);
			REQUIRE(point.Equal(GASS::Vec2f(0,2)) == true);
		}
	}

	SECTION("Test GetLength")
	{
		const GASS::Vec2f p1(0, 0);
		const GASS::Vec2f p2(0, 4);
		const GASS::LineSegment2Df line(p1, p2);
		{
			//test that start point is closest 
			const float l = line.GetLength();
			REQUIRE(GASS::Math::Equal(l, 4.0f) == true);
		}
	}

	SECTION("Test ClosestPointOnLine")
	{
		const GASS::Vec2f p1(0, 0);
		const GASS::Vec2f p2(0, 4);
		const GASS::LineSegment2Df line(p1, p2);

		{
			//test that start point is closest 
			const GASS::Vec2f point = line.ClosestPointOnLine(GASS::Vec2f(0, -4));
			REQUIRE(point.Equal(p1) == true);
		}

		{
			//test that end point is closest
			const GASS::Vec2f point = line.ClosestPointOnLine(GASS::Vec2f(0, 6));
			REQUIRE(point.Equal(p2) == true);
		}

		{
			//test point on line
			const GASS::Vec2f point_on_line(0, 2);
			const GASS::Vec2f point = line.ClosestPointOnLine(point_on_line);
			REQUIRE(point.Equal(point_on_line) == true);
		}

		{
			//test point off line
			const GASS::Vec2f point = line.ClosestPointOnLine(GASS::Vec2f(11, 11));
			REQUIRE(point.Equal(p2) == true);
		}
	}

	SECTION("Test GetIntersection")
	{
		const GASS::LineSegment2Df line1(GASS::Vec2f(-4, 0), GASS::Vec2f(4, 0));
		const GASS::LineSegment2Df line2(GASS::Vec2f(1, -4), GASS::Vec2f(1, 4));
		const GASS::LineSegment2Df line3(GASS::Vec2f(5, -4), GASS::Vec2f(5, 4));
		{
			//test that we get intersection
			GASS::Vec2f isect;
			bool got_isect = GASS::LineSegment2Df::GetIntersection(line1, line2, isect);
			REQUIRE(got_isect);
			REQUIRE(isect.Equal(GASS::Vec2f(1, 0)) == true);
		}

		{
			//test that we don't get intersection
			GASS::Vec2f isect;
			bool got_isect = GASS::LineSegment2Df::GetIntersection(line1, line3, isect);
			REQUIRE(got_isect == false);
		}
	}
}

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

	SECTION("Test GetPoint")
	{
		const GASS::Vec3f p1(0, 0, 2);
		const GASS::Vec3f p2(0, 4, 2);
		const GASS::LineSegmentf line(p1, p2);
		{
			//test that start point is closest 
			const GASS::Vec3f point = line.GetPoint(0.5f);
			REQUIRE(point.Equal(GASS::Vec3f(0, 2, 2)) == true);
		}
	}

	SECTION("Test GetLength")
	{
		const GASS::Vec3f p1(0, 0, 2);
		const GASS::Vec3f p2(0, 4, 2);
		const GASS::LineSegmentf line(p1, p2);
		{
			//test that start point is closest 
			const float l = line.GetLength();
			REQUIRE(GASS::Math::Equal(l, 4.0f) == true);
		}
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
			const GASS::Vec3 point = line.ClosestPointOnLine(GASS::Vec3(11, 11, 13));
			REQUIRE(point.Equal(p2) == true);
		}
	}
}