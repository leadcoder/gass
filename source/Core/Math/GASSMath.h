/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
*                                                                           *
* GASS is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* GASS is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with GASS. If not, see <http://www.gnu.org/licenses/>.              *
*****************************************************************************/

#pragma once

#include "Core/Common.h"

#include <math.h>

#define GASS_PLANE_FRONT 0
#define GASS_PLANE_BACK 1
#define GASS_ON_PLANE 2
#define GASS_PI 3.14159265358

namespace GASS
{
	class Polygon;
	class Vec3;
	class Vec2;
	class LineSegment;
	class Ray;
	class Triangle;
	class Plane;
	class AABox;

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Math
	*  @{
	*/

	/**\class Math
	@brief Class with a couple of math functions.
	*/
	class GASSCoreExport Math
	{
	public:
		/**
		@brief Convert radians to degrees.
		@param rad Number of radians, as a float.
		@return Number of degrees.
		*/
		static Float Rad2Deg(Float rad);

		/**
		@brief Convert degrees to radians.
		@param deg Number of degrees, as a float.
		@return Number of radians.
		*/

		static Float Deg2Rad(Float deg);
		static Vec3 Deg2Rad(const Vec3 &vec);
		static Vec3 Rad2Deg(const Vec3 &vec);

		/**
		@brief Check if a line intersect a Polygon.
		@param line_segment Line segment to check.
		@param poly Polygon to check intersection with, as a Polygon.
		@return True if intersection, otherwise false.
		*/
		static bool LineIsectPolygon( const LineSegment &line_segment, const Polygon &poly);

		/**
		@brief Calculate the dot product of two vectors.
		@param v1 First vector, as a Vec3.
		@param v2 Second vector, as a Vec3.
		@return The dot product.
		*/
		static Float Dot(const Vec3 &v1,const Vec3 &v2);

		/**
		@brief Calculate the cross product of two vectors.
		@param v1 First vector, as a Vec3.
		@param v2 Second vector, as a Vec3.
		@return The cross product.
		*/
		static Vec3 Cross(const Vec3 &v1,const Vec3 &v2);

		/**
		@brief Calculate the distance (along the ray) where a infinite
			ray intersect a infinite plane.
		@param ray Ray to check.
		@param plane Plane to check against.
		@return Intersection distance.
		*/
		static Float IsectRayPlane(const Ray& ray, const Plane &plane);

		/**
		@brief Calculate if a line intersect a triangle and if so provide intersection point.
		@param line_segment Line segment to check.
		@param tri Triangle to check.
		@param isect_point Possible intersection point, as a Vec3.
		@return True if line intersect triangle.
		*/
		static bool LineIsectTriangle(const LineSegment &line_segment,
			const Triangle &tri,
			Vec3 &isect_point);

		/**
		Get Min value of v1, v2,v3
		*/
		static Float Min(const Float &v1,const Float &v2,const Float &v3);

		/**
		Get Max value of v1, v2,v3
		*/
		static Float Max(const Float &v1,const Float &v2,const Float &v3);


		/**
		Get closest point on triangle
		@param tri Triangle to check against
		@param p Input point
		@param radius ?
		@return Closest point on triangle to input point
		*/
		static bool ClosestPointOnTriangle(const Triangle &tri, const Vec3 &p, Vec3 &closest, Float radius);

		/**
		Get closest point on line segment
		@param line Line to check against
		@param point Input point
		@return Closest point on line to input point
		*/
		static Vec3 ClosestPointOnLine(const LineSegment& line, const Vec3 &point);

		/**
		Get triangle normal
		@param tri input triangle
		@return triangle normal
		*/
		static Vec3 GetNormal(const Triangle &tri);

		/**
		Check if two triangles intersect
		@param t1 First triangle
		@param t2 Second triangle
		@param iscent_point potential intersection point
		@return true if intersection found
		*/
		static bool TriangleIsectTriangle(const Triangle &t1,const Triangle &t2,Vec3 &isect_point);
		static Float Min(const Float &v1,const Float &v2);
		static Float Max(const Float &v1,const Float &v2);

		/**
		Fast invert root square function, (maybe not so fast anymore)
		*/
		static inline float InvSqrt(float x)
		{
#ifdef GASS_USE_FAST_INV
			float xhalf = 0.5f*x;
			int i = *(int*)&x;
			i = 0x5f3759df - (i >> 1); // This line hides a LOT of math!
			x = *(float*)&i;
			x = x*(1.5f - xhalf*x*x); // repeat this statement for a better approximation
			return x;
#else
            return 1.0f/sqrt(x);
#endif
		}

		/**
		Project vector on plane
		@param plane_normal Normal of the projection plane
		@param v Vector to project
		@return Projected vector
		*/
		//TODO: change name or input args,
		static Vec3 ProjectVectorOnPlane(const Vec3 &plane_normal,const Vec3 &v);

		/**
		Get float random value
		@param start Start of random span
		@param end End of random span
		@return random value between start and end
		*/
		static Float RandomValue(Float start, Float end);

		/**
		Get 2D-line intersection
		@param  p1 first point in first line
		@param  p2 second point in first line
		@param  p3 first point in second  line
		@param  p4 second point in second  line
		@param  isect Potential intersection point
		@return true if intersection exist
		*/
		static bool GetLineIntersection(const Vec2 &p1, const Vec2 &p2, const Vec2 &p3, const Vec2 &p4, Vec2 &isect);


		// path utilities, to be moved to path class?
		/**

		*/
		static bool GetClosestPointOnPath(const Vec3& source_pos , const std::vector<Vec3> &wps, int &segment_index, Vec3& point);
		static Vec3 GetPointOnPath(Float pathDistance, const std::vector<Vec3> &wps, bool cyclic,int &index);
		static Float GetPathDistance(const Vec3& point, const std::vector<Vec3> &wps,int &index,Float &distance_to_path);
		static Float GetPathLength(const std::vector<Vec3> &wps);
		static std::vector<Vec3> ClipPath(Float start_distance, Float end_distance, const std::vector<Vec3> &wps);
		static std::vector<Vec3> GenerateOffset(const std::vector<Vec3> &wps, Float offset);
		static std::vector<Vec3> GenerateOffset(const std::vector<Vec3> &wps, Float start_offset,Float end_offset);
		static std::vector<Vec3> GenerateNormals(const std::vector<Vec3> &wps);


		/**
			Get intersection between line and axis aligned box
			@param line_seg Line segment to check with
			@param box Box to check with
			@param tinter Potential intersection distance along line segment
			@return true if intersection exist
		*/
		static bool LineSegmentAABBoxIntersect(const LineSegment& line_seg, const AABox& box, Float& tinter);
	private:
		//helpers
		static bool	_CheckPointInTriangle1(const Vec3 &point, const Triangle& tri, const Vec3 &normal);
		static int _CheckPointInTriangle2(const Vec3& point, const Triangle& tri);
		//static Vec3 _ClosestPointOnTriangleEdge(Vec3 a, Vec3 b, Vec3 c, Vec3 p);
		static int	_ClassifyPoint(const Vec3 &point, const Polygon &poly);
		static int	_ClassifyPoint(const Vec3 &point, const Vec3 &origin, const Vec3 &normal);
		static char	_GetMaxCoord(const Vec3 &coord);
		static Float _Angle2D(Float x1, Float y1, Float x2, Float y2);
		static bool _LineSlabIntersect(Float slabmin, Float slabmax, Float raystart, Float rayend, Float& tbenter, Float& tbexit);
	};
}
