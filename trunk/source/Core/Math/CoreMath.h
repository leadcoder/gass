/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
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

#ifndef COREMATH_HH
#define COREMATH_HH

#include "Core/Common.h"

#define PLANE_FRONT 0
#define PLANE_BACK 1
#define ON_PLANE 2

#include "Core/Math/Vector.h"
//#include "Core/Math/Polygon.h"

namespace GASS
{
	class Polygon;


	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Math
	*  @{
	*/
	
	/**\class Math
	* \brief Class with a couple of math functions.
	*/
	class GASSCoreExport Math
	{
	public:
		/**\fn static float Rad2Deg(float deg)
		* \brief Convert radians to degrees.
		* \param rad Number of radians, as a float.
		* \return Number of degrees.
		*/
		static Float Rad2Deg(Float rad);

		/**\fn static float Deg2Rad(float deg)
		* \brief Convert degrees to radians.
		* \param deg Number of degrees, as a float.
		* \return Number of radians.
		*/
		static Float Deg2Rad(Float deg);


		static Vec3 Deg2Rad(const Vec3 &vec);
		static Vec3 Rad2Deg(const Vec3 &vec);

		/**\fn static bool LineIsectPolygon(const Vec3 &start,const Vec3 &end,const Ploygon &Ploygon)
		* \brief Check if a line intersect a Polygon.
		* \param start Start position of line, as a Vec3.
		* \param end End position of line, as a Vec3.
		* \param Ploygon Polygon to check intersection with, as a Ploygon.
		* \return True if intersection, otherwise false.
		*/
		static bool LineIsectPolygon( const Vec3 &start,  const Vec3 &end, const Polygon &poly);

		/**\fn static bool Dot(const Vec3 &v1,const Vec3 &v2);
		* \brief Calculate the dot product of two vectors.
		* \param v1 First vector, as a Vec3.
		* \param v2 Second vector, as a Vec3.
		* \return The dot product.
		*/
		static Float Dot(const Vec3 &v1,const Vec3 &v2);
		static Vec3 Cross(const Vec3 &v1,const Vec3 &v2);
	//	static Vec3 GetTangent(const Vec3 &pos1,const  Vec3 &pos2,const  Vec3 &pos3, const Vec3 &texc1, const Vec3 &texc2, const Vec3 &texc3);
	//	static void GetTangentAndBinormal(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2, const Vec3 &t0, const Vec3 &t1, const Vec3 &t2,const Vec3 &tangent,const Vec3 &binormal);
		static Float IsectRayPlane(const Vec3 &start, const Vec3 &dir, const Vec3 &origin,const Vec3 &normal);
		static bool RayIsectTriangle(const Vec3 &start,const Vec3 &end,const Vec3 &p1,const Vec3 &p2,const Vec3 &p3,Vec3 &isect_point);
		static Float Min(const Float &v1,const Float &v2,const Float &v3);
		static Float Max(const Float &v1,const Float &v2,const Float &v3);
		static bool ClosestPointOnTriangle(const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec3 &p, Vec3 &closest, Float radius);
		static Vec3 ClosestPointOnTriangleEdge(Vec3 a, Vec3 b, Vec3 c, Vec3 p);
		static Vec3 ClosestPointOnLine(const Vec3 &a, const Vec3 &b, const Vec3 &p);
		static bool	CheckPointInTriangle1(const Vec3 &point, const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec3 &normal);
		static int CheckPointInTriangle3(const Vec3& point, const Vec3& pa,const Vec3& pb, const Vec3& pc);
		static bool GetLowestRoot(Float a, Float b, Float c, Float maxR,Float* root);
		static Vec3 GetNormal(const Vec3 &p1,const Vec3 &p2,const Vec3 &p3);
		static bool TriangleIsectTriangle(const Vec3 &t1_v1,const Vec3 &t1_v2,const Vec3 &t1_v3,const Vec3 &t2_v1,const Vec3 &t2_v2,const Vec3 &t2_v3,Vec3 &isect_point);
		static Float Min(const Float &v1,const Float &v2);
		static Float Max(const Float &v1,const Float &v2);

		static inline float InvSqrt(float x)
		{
			float xhalf = 0.5f*x;
			int i = *(int*)&x;
			i = 0x5f3759df - (i >> 1); // This line hides a LOT of math!
			x = *(float*)&i;
			x = x*(1.5f - xhalf*x*x); // repeat this statement for a better approximation
			return x;
		}
		static Float GetHorizontalAngleBetweenVectors(const Vec3 &v1, const Vec3 &v2);
		static Vec3 ProjectVectorOnPlane(const Vec3 plane_normal,const Vec3 &v);		
#define MY_PI 3.14159265358f
	private:
		static int	ClassifyPoint(const Vec3 &point, const Polygon &poly);
		static int	ClassifyPoint(const Vec3 &point, const Vec3 &origin, const Vec3 &normal);
		static char	GetMaxCoord(const Vec3 &coord);
		static bool	CheckPointInTriangle2(const Vec3 &point, const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec3 &normal);
		static Float Angle2D(Float x1, Float y1, Float x2, Float y2);

	};
}

#endif // #ifndef COREMATH_HH
