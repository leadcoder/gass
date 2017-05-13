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

#include <math.h>
#include <limits>
#include "Core/Math/GASSMath.h"

#undef min
#undef max

namespace GASS
{
	bool Math::GetLineIntersection(const Vec2 &p1, const Vec2 &p2, const Vec2 &p3, const Vec2 &p4, Vec2 &isect)
	{
		// Store the values for fast access and easy
		// equations-to-code conversion

		const Float x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
		const Float y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;

		const Float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		// If d is zero, there is no intersection
		if (d == 0)
			return false;

		// Get the x and y
		const Float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
		const Float x = (pre * (x3 - x4) - (x1 - x2) * post) / d;
		const Float y = (pre * (y3 - y4) - (y1 - y2) * post) / d;

		// Check if the x and y coordinates are within both lines
		if (x < std::min(x1, x2) || x > std::max(x1, x2) ||
			x < std::min(x3, x4) || x > std::max(x3, x4)) return false;
		if (y < std::min(y1, y2) || y > std::max(y1, y2) ||
			y < std::min(y3, y4) || y > std::max(y3, y4)) return false;

		// Return the point of intersection
		isect.x = x;
		isect.y = y;
		return true;
	}

	/*bool Math::TriangleIsectTriangle(const Triangle &t1,const Triangle &t2,Vec3 &isect_point)
	{
		//check first against second
		if(LineIsectTriangle(LineSegment(t1.P1, t1.P2), t2, isect_point)) return true;
		if(LineIsectTriangle(LineSegment(t1.P1, t1.P3), t2, isect_point)) return true;
		if(LineIsectTriangle(LineSegment(t1.P2, t1.P3), t2, isect_point)) return true;

		//check second against first
		if(LineIsectTriangle(LineSegment(t2.P1, t2.P2),t1,isect_point)) return true;
		if(LineIsectTriangle(LineSegment(t2.P1, t2.P3),t1,isect_point)) return true;
		if(LineIsectTriangle(LineSegment(t2.P2, t2.P3),t1,isect_point)) return true;

		return false;
	}*/

	/*bool Math::LineIsectTriangle(const LineSegment &line_segment,const Triangle &tri, Vec3 &isect_point)
	{

		Vec3 normal;
		const Vec3 edge1 = tri.P2 - tri.P1;
		const Vec3 edge2 = tri.P3 - tri.P1;

		normal = Vec3::Cross(edge1,edge2);
		normal.Normalize();

		const int side1 = _ClassifyPoint(line_segment.m_Start,  tri.P1,normal);
		const int side2 = _ClassifyPoint(line_segment.m_End,  tri.P1,normal);

		if((side1 == GASS_PLANE_BACK && side2 == GASS_PLANE_BACK) || (side1 == GASS_PLANE_FRONT && side2 == GASS_PLANE_FRONT)) return false;

		Vec3 ray_dir = line_segment.m_End - line_segment.m_Start;

		ray_dir.Normalize();

		const Float ray_scale = Plane::RayIsect(Ray(line_segment.m_Start, ray_dir),  Plane(tri.P1 ,normal));

		if(ray_scale == -1) return false;

		isect_point = line_segment.m_Start + ray_dir * ray_scale;

		if(_CheckPointInTriangle2(isect_point, tri)) return true;

		return false;
	}*/

	/*bool Math::LineIsectPolygon(const LineSegment &line_segment,const Polygon &poly)
	{
		const int side1 = _ClassifyPoint(line_segment.m_Start, poly);
		const int side2 = _ClassifyPoint(line_segment.m_End, poly);

		if((side1 == GASS_PLANE_BACK && side2 == GASS_PLANE_BACK) || (side1 == GASS_PLANE_FRONT && side2 == GASS_PLANE_FRONT)) return false;

		Vec3 ray_dir = line_segment.m_End - line_segment.m_Start;

		ray_dir.Normalize();

		const Float ray_scale = Plane::RayIsect(Ray(line_segment.m_Start, ray_dir), Plane(poly.m_VertexVector[0], poly.m_Normal));

		if(ray_scale == -1) return false;

		const Vec3 isect_point = line_segment.m_Start + ray_dir * ray_scale;

		size_t size = poly.m_VertexVector.size();
		size %= 3;
		size_t index = 1;
		for(size_t i = 0; i < size+1;i++)
		{

			if(_CheckPointInTriangle1(isect_point, Triangle(
				poly.m_VertexVector[0],
				poly.m_VertexVector[index],
				poly.m_VertexVector[index+1]),
				poly.m_Normal))
				return true;
			++index;
		}
		return false;
	}

	int Math::_ClassifyPoint(const Vec3 &point, const Polygon &poly)
	{
		const Vec3 dir = poly.m_VertexVector[0] - point;
		const double d = Vec3::Dot(dir, poly.m_Normal);

		if (d < -0.001f)
			return GASS_PLANE_FRONT;
		else if (d > 0.001f)
			return GASS_PLANE_BACK;

		return GASS_ON_PLANE;
	}

	int Math::_ClassifyPoint(const Vec3 &point, const Vec3 &origin, const Vec3 &normal)
	{
		const Vec3 dir = origin - point;
		const double d = Vec3::Dot(dir, normal);

		if (d<-0.001f)
			return GASS_PLANE_FRONT;
		else if (d>0.001f)
			return GASS_PLANE_BACK;

		return GASS_ON_PLANE;
	}*/

	/*char Math::_GetMaxCoord(const Vec3 &coord)
	{
		const Float x = fabs(coord.x);
		const Float y = fabs(coord.y);
		const Float z = fabs(coord.z);
		if(x > y)
		{
			if(x > z) return 0;
			else return 2;
		}
		else if(y > z) return 1;
		else return 2;
	}*/

	/*Float Math::_Angle2D(Float x1, Float y1, Float x2, Float y2)
	{
		const Float theta1 = atan2(y1,x1);
		const Float theta2 = atan2(y2,x2);
		Float dtheta = theta2 - theta1;
		while (dtheta > GASS_PI)
			dtheta -= 2.0f*GASS_PI;
		while (dtheta < -GASS_PI)
			dtheta += 2.0f*GASS_PI;

		return dtheta;
	}

	bool Math::_CheckPointInTriangle1(const Vec3 &point, const Triangle &tri, const Vec3 &normal)
	{
		double total_angles = 0.0f;

		Float a1,a2,b1,b2,c1,c2,p1,p2;
		Float va1,va2,vb1,vb2,vc1,vc2;
		const char max = _GetMaxCoord(normal);
		if(max == 0) //x
		{
			a1 = tri.P1.y;
			a2 = tri.P1.z;
			b1 = tri.P2.y;
			b2 = tri.P2.z;
			c1 = tri.P3.y;
			c2 = tri.P3.z;
			p1 = point.y;
			p2 = point.z;
		}
		else if(max == 1)//y
		{
			a1 = tri.P1.x;
			a2 = tri.P1.z;
			b1 = tri.P2.x;
			b2 = tri.P2.z;
			c1 = tri.P3.x;
			c2 = tri.P3.z;
			p1 = point.x;
			p2 = point.z;
		}
		else //if(max == 2) //z
		{
			a1 = tri.P1.x;
			a2 = tri.P1.y;
			b1 = tri.P2.x;
			b2 = tri.P2.y;
			c1 = tri.P3.x;
			c2 = tri.P3.y;
			p1 = point.x;
			p2 = point.y;
		}

		va1 = a1-p1;
		va2 = a2-p2;

		vb1 = b1-p1;
		vb2 = b2-p2;

		vc1 = c1-p1;
		vc2 = c2-p2;

		total_angles  = _Angle2D(va1,va2,vb1,vb2);
		total_angles  += _Angle2D(vb1,vb2,vc1,vc2);
		total_angles  += _Angle2D(vc1,vc2,va1,va2);
		if (fabs(total_angles) < GASS_PI)
			return false;

		return true;
	}*/

/*	int Math::_CheckPointInTriangle2(const Vec3& point, const Triangle& tri)
	{
		const Vec3 e10= tri.P2 - tri.P1;
		const Vec3 e20= tri.P3 - tri.P1;
		const Float a = Vec3::Dot(e10,e10);
		const Float b = Vec3::Dot(e10,e20);
		const Float c = Vec3::Dot(e20,e20);
		const Float ac_bb=(a*c)-(b*b);

		const Vec3 vp(point.x - tri.P1.x, point.y - tri.P1.y, point.z - tri.P1.z);
		const Float d = Vec3::Dot(vp,e10);
		const Float e = Vec3::Dot(vp,e20);
		const Float x = (d*c)-(e*b);
		const Float y = (e*a)-(d*b);
		const Float z = x+y-ac_bb;
		//return (( in(z)& ~(in(x)|in(y)) ) & 0x80000000);
		return (( int(z) & ~(int(x)|int(y)) ) & 0x80000000);
	}*/



	/*	Vec3 Math::GetTangent(Vec3 &pos1, Vec3 &pos2, Vec3 &pos3, Vec3 &texc1, Vec3 &texc2, Vec3 &texc3)
	{
	Vec3 tangent;
	Vec3 edge1 = pos2 - pos1;
	Vec3 edge2 = pos3 - pos1;
	Vec3 dsdt1 = texc2  - texc1;
	Vec3 dsdt2 = texc3  - texc1;


	tangent = (edge2 * (dsdt1.y/(dsdt2.x*dsdt1.y-dsdt1.x*dsdt2.y)))-
	(edge1 * (dsdt2.y/(dsdt2.x*dsdt1.y-dsdt1.x*dsdt2.y)));


	tangent.Normalize();
	return tangent;
	}


	void Math::GetTangentAndBinormal(Vec3 &v0, Vec3 &v1, Vec3 &v2, Vec3 &t0, Vec3 &t1, Vec3 &t2,Vec3 &tangent,Vec3 &binormal)
	{
	Vec3 cp;
	Vec3 e0;
	Vec3 e1;
	e0.Set(v1.x-v0.x, t1.x-t0.x, t1.y-t0.y);
	e1.Set(v2.x-v0.x, t2.x-t0.x, t2.y-t0.y);

	cp.x=e0.y*e1.z - e0.z*e1.y;
	cp.y=e0.z*e1.x - e0.x*e1.z;
	cp.z=e0.x*e1.y - e0.y*e1.x;

	if(fabs(cp.x)>0.00001f)
	{
	tangent.x=-cp.y/cp.x;
	binormal.x=-cp.z/cp.x;
	}

	e0.x=v1.y-v0.y;
	e1.x=v2.y-v0.y;

	cp.x=e0.y*e1.z-e0.z*e1.y;
	cp.y=e0.z*e1.x-e0.x*e1.z;
	cp.z=e0.x*e1.y-e0.y*e1.x;

	if(fabs(cp.x)>0.00001f)
	{
	tangent.y=-cp.y/cp.x;
	binormal.y=-cp.z/cp.x;
	}

	e0.x=v1.z - v0.z;
	e1.x=v2.z - v0.z;

	cp.x=e0.y*e1.z - e0.z*e1.y;
	cp.y=e0.z*e1.x - e0.x*e1.z;
	cp.z=e0.x*e1.y - e0.y*e1.x;

	if(fabs(cp.x) > 0.00001f)
	{
	tangent.z=-cp.y/cp.x;
	binormal.z=-cp.z/cp.x;
	}
	}*/

	/*bool Math::ClosestPointOnTriangle(const Triangle &tri,
		const Vec3 &p, Vec3 &closest, Float radius)
	{

		// Make the plane containing this triangle.
		const Vec3 pOrigin = tri.P1;
		const Vec3 v1 = tri.P2 - tri.P1;
		const Vec3 v2 = tri.P3 - tri.P1;

		
		//All triangles are valid - MOB

		// You might not need this if you KNOW all your triangles are valid
		//if ((v1.length() == 0) || (v2.length() == 0)) return false;
		

		// determine normal to plane containing polygon
		Vec3 pNormal = Vec3::Cross(v1, v2);
		pNormal.Normalize();

		// find how far away the plane is from point p along the planes normal

		const Float distToPlaneIntersection = Plane::RayIsect(Ray(p, -pNormal), Plane(pOrigin, pNormal));
		if ((distToPlaneIntersection == -1) || (distToPlaneIntersection > radius)) return false;

		// find the nearest point on the plane to p
		closest = p - (pNormal * distToPlaneIntersection);

		// determine if that point is in the triangle
		return _CheckPointInTriangle1(closest, tri,pNormal);
	}*/

	/*Vec3 Math::_ClosestPointOnTriangleEdge(Vec3 a, Vec3 b, Vec3 c, Vec3 p)
	{
		Vec3 Rab = ClosestPointOnLine(a, b, p);
		Vec3 Rbc = ClosestPointOnLine(b, c, p);
		Vec3 Rca = ClosestPointOnLine(c, a, p);

		double dAB = (p-Rab).Length();
		double dBC = (p-Rbc).Length();
		double dCA = (p-Rca).Length();

		double min = dAB;
		Vec3 result = Rab;

		if (dBC < min)
		{
			min = dBC;
			result = Rbc;
		}

		if (dCA < min)
		{
			result = Rca;
		}

		return (result);
	}*/

	/*bool Math::GetClosestPointOnPath(const Vec3& source_pos , const std::vector<Vec3> &wps, int &segment_index, Vec3& point )
	{
		double shortest_dist = std::numeric_limits<double>::max();
		if(wps.size() > 1)
		{
			for(size_t i = 0; i < wps.size()-1; i++)
			{
				const Vec3 wp1 = wps[i];
				const Vec3 wp2 = wps[i+1];
				const Vec3 closest_point_on_line = LineSegment(wp1,wp2).ClosestPointOnLine(source_pos);
				double dist = (source_pos  - closest_point_on_line).Length();
				if(dist < shortest_dist)
				{
					point = closest_point_on_line;
					shortest_dist = dist;
					segment_index =  static_cast<int>(i);
				}
			}
		}
		else
			return false;
		return true;
	}


	Vec3 Math::GetPointOnPath(Float pathDistance, const std::vector<Vec3> &wps, bool cyclic, int &index)
	{
		Float  totalPathLength  = 0;
		std::vector<Float> lengths;
		for (unsigned int i = 1; i < wps.size(); i++)
		{
			const Float segmentLength = (wps[i-1] - wps[i]).Length();
			lengths.push_back(segmentLength);
			totalPathLength += segmentLength;
		}

		// clip or wrap given path distance according to cyclic flag
		Float remaining = pathDistance;
		if (cyclic)
		{
			remaining = fmod(pathDistance, totalPathLength);
		}
		else
		{
			if (pathDistance < 0)
			{
				index = 0;
				return wps[0];
			}
			if (pathDistance >= totalPathLength)
			{
				if(wps.size() > 1)
				{
					index = static_cast<int>(wps.size() - 1);
					return wps[wps.size() - 1];
				}
			}
		}

		// step through segments, subtracting off segment lengths until
		// locating the segment that contains the original pathDistance.
		// Interpolate along that segment to find 3d point value to return.
		Vec3 result;
		for (unsigned int i = 1; i < wps.size(); i++)
		{
			const Float segmentLength = lengths[i-1];
			if (segmentLength < remaining)
			{
				remaining -= segmentLength;
			}
			else
			{
				const Float ratio = remaining / segmentLength;
				result = wps[i-1] + ((wps[i] - wps[i-1])*ratio);
				index = i-1;
				break;
			}
		}
		return result;
	}


	Float Math::GetPathDistance(const Vec3& point, const std::vector<Vec3> &wps,int &index, Float &distance_to_path)
	{
		Float shortest_dist = std::numeric_limits<Float>::max();
		Float segmentLengthTotal = 0;
		Float pathDistance = 0;

		for (unsigned int i = 1; i < wps.size(); i++)
		{
			const Float segmentLength = (wps[i] - wps[i-1]).Length();
			const Vec3 closest_point_on_line = LineSegment(wps[i-1],wps[i]).ClosestPointOnLine(point);
			const double dist = (point  - closest_point_on_line).Length();
			if(dist < shortest_dist)
			{
				shortest_dist = dist;
				distance_to_path = dist;
				pathDistance = segmentLengthTotal + (wps[i-1] - closest_point_on_line).Length();
				index = i-1;
			}
			segmentLengthTotal += segmentLength;
		}
		// return distance along path of onPath point
		return pathDistance;
	}

	std::vector<Vec3> Math::ClipPath(Float start_distance, Float end_distance, const std::vector<Vec3> &wps)
	{
		Float  totalPathLength  = 0;
		std::vector<Float> lengths;
		for (unsigned int i = 1; i < wps.size(); i++)
		{
			const Float segmentLength = (wps[i-1] - wps[i]).Length();
			lengths.push_back(segmentLength);
			totalPathLength += segmentLength;
		}

		Float start_remaining = start_distance;
		Float end_remaining = end_distance;
		std::vector<Vec3> path;
		int index = 0;
		for (unsigned int i = 1; i < wps.size(); i++)
		{
			const Float segmentLength = lengths[i-1];
			if (segmentLength < start_remaining)
			{
				start_remaining -= segmentLength;
				end_remaining -= segmentLength;
			}
			else
			{
				const Float ratio = start_remaining / segmentLength;
				const Vec3 start_point = wps[i-1] + ((wps[i] - wps[i-1])*ratio);
				path.push_back(start_point);
				index = i;
				break;
			}
		}

		for (unsigned int i = index; i < wps.size(); i++)
		{
			const Float segmentLength = lengths[i-1];

			if (segmentLength < end_remaining)
			{
				end_remaining -= segmentLength;
				//path.push_back(wps[i-1]);
				//if(index == wps.size()-1) // if last
				path.push_back(wps[i]);
			}
			else
			{
				const Float ratio = end_remaining / segmentLength;
				const Vec3 end_wp = wps[i-1] + ((wps[i] - wps[i-1])*ratio);
				path.push_back(end_wp);
				break;
			}
		}
		return path;
	}

	std::vector<Vec3> Math::GenerateOffset(const std::vector<Vec3> &wps, Float start_offset,Float end_offset)
	{
		Float totalPathLength = 0;
		for (unsigned int i = 1; i < wps.size(); i++)
		{
			const Float segmentLength = (wps[i-1] - wps[i]).Length();
			totalPathLength += segmentLength;
		}
		Float dist = 0;
		std::vector<Vec3> offset_path;
		for(size_t i = 0; i < wps.size(); i++)
		{
			Vec3 side;
			const Float inter = dist/totalPathLength;
			if(i < wps.size()-2)
				dist += (wps[i-1] - wps[i]).Length();
			const Float offset = start_offset + inter*(end_offset - start_offset);
			Float width_mult = 1.0;

			if( i== 0)
			{
				side = (wps[1] - wps[0]);
				side.y = 0;
			}
			else if(i < wps.size() - 1)
			{
				Vec3 d1 = (wps[i]-wps[i-1]);
				Vec3 d2 = (wps[i+1]-wps[i]);
				d1.Normalize();
				d2.Normalize();
				side = d1 + d2;
				side.Normalize();
				width_mult = Vec3::Dot(d1,side);
				if(width_mult > 0)
					width_mult = 1.0/width_mult;

			}
			else
			{
				side = wps[i]-wps[i-1];
			}
			side.Normalize();
			side = Vec3::Cross(side,Vec3(0,1,0));
			side.Normalize();
			offset_path.push_back(wps[i] + side*offset*width_mult);
		}
		return offset_path;
	}

	std::vector<Vec3> Math::GenerateOffset(const std::vector<Vec3> &wps, Float offset)
	{
		std::vector<Vec3> offset_path;
		for(size_t i = 0; i < wps.size(); i++)
		{
			Vec3 side; //= wps[i+1] - wps[i];

			Float width_mult = 1.0;

			if( i== 0)
			{
				side = (wps[1] - wps[0]);
				side.y = 0;
			}
			else if(i < wps.size() - 1)
			{
				Vec3 d1 = (wps[i]-wps[i-1]);
				Vec3 d2 = (wps[i+1]-wps[i]);
				d1.Normalize();
				d2.Normalize();
				side = d1 + d2;
				side.Normalize();
				width_mult = Vec3::Dot(d1,side);
				if(width_mult > 0)
					width_mult = 1.0/width_mult;

			}
			else
			{
				side = wps[i]-wps[i-1];
			}
			side.Normalize();
			side = Vec3::Cross(side,Vec3(0,1,0));
			side.Normalize();
			offset_path.push_back(wps[i] + side*offset*width_mult);
		}
		return offset_path;
	}

	std::vector<Vec3> Math::GenerateNormals(const std::vector<Vec3> &wps)
	{
		std::vector<Vec3> normals;
		for(size_t i = 0; i < wps.size(); i++)
		{
			Vec3 side; //= wps[i+1] - wps[i];

			Float width_mult = 1.0;

			if( i== 0)
			{
				side = (wps[1] - wps[0]);
				side.y = 0;
			}
			else if(i < wps.size() - 1)
			{
				Vec3 d1 = (wps[i]-wps[i-1]);
				Vec3 d2 = (wps[i+1]-wps[i]);
				d1.Normalize();
				d2.Normalize();
				side = d1 + d2;
				side.Normalize();
				width_mult = Vec3::Dot(d1,side);
				if(width_mult > 0)
					width_mult = 1.0/width_mult;

			}
			else
			{
				side = wps[i]-wps[i-1];
			}
			side.Normalize();
			side = Vec3::Cross(side,Vec3(0,1,0));
			side.Normalize();
			normals.push_back(side*width_mult);
		}
		return normals;
	}


	Float Math::GetPathLength(const std::vector<Vec3> &wps)
	{
		Float  total_path_length  = 0;
		for (unsigned int i = 1; i < wps.size(); i++)
		{
			total_path_length += (wps[i-1] - wps[i]).Length();
		}
		return total_path_length;
	}*/


	/*Vec3 Math::ClosestPointOnLine(const LineSegment& line, const Vec3 &point)
	{
		// Determine t (the length of the vector from a to p)
		Vec3 c = point - line.m_Start;
		Vec3 V = line.m_End-line.m_Start;

		double d = V.Length();

		V.Normalize();
		double t = Vec3::Dot(V,c);

		// Check to see if t is beyond the extents of the line segment
		if (t < 0.0f) return (line.m_Start);
		if (t > d) return (line.m_End);

		// Return the point between a and b
		//set length of V to t. V is normalized so this is easy
		V.x = V.x * t;
		V.y = V.y * t;
		V.z = V.z * t;

		return (line.m_Start + V);
	}*/

	/*bool Math::_LineSlabIntersect(Float slabmin, Float slabmax, Float line_start, Float line_end, Float& tbenter, Float& tbexit)
	{
		Float raydir = line_end - line_start;

		// ray parallel to the slab
		if (fabs(raydir) < 1.0E-9f)
		{
			// ray parallel to the slab, but ray not inside the slab planes
			if(line_start < slabmin || line_start > slabmax)
			{
				return false;
			}
			// ray parallel to the slab, but ray inside the slab planes
			else
			{
				return true;
			}
		}

		// slab's enter and exit parameters
		Float tsenter = (slabmin - line_start) / raydir;
		Float tsexit = (slabmax - line_start) / raydir;

		// order the enter / exit values.
		if(tsenter > tsexit)
		{
			Float tmp = tsenter;
			tsenter = tsexit;
			tsexit = tmp;
			//swapf(tsenter, tsexit);
		}

		// make sure the slab interval and the current box intersection interval overlap
		if (tbenter > tsexit || tsenter > tbexit)
		{
			// nope. Ray missed the box.
			return false;
		}
		// yep, the slab and current intersection interval overlap
		else
		{
			// update the intersection interval
			tbenter = std::max(tbenter, tsenter);
			tbexit = std::min(tbexit, tsexit);
			return true;
		}
	}

	bool Math::LineSegmentAABBoxIntersect(const LineSegment& line_seg, const AABox& box, Float &tinter)
	{
		// initialize to the segment's boundaries.
		Float tenter = 0.0f, texit = 1.0f;

		// test X slab
		if (!_LineSlabIntersect(box.m_Min.x, box.m_Max.x, line_seg.m_Start.x, line_seg.m_End.x, tenter, texit))
		{
			return false;
		}

		// test Y slab

		if (!_LineSlabIntersect(box.m_Min.y, box.m_Max.y, line_seg.m_Start.y, line_seg.m_End.y, tenter, texit))
		{
			return false;
		}

		// test Z slab
		if (!_LineSlabIntersect(box.m_Min.z, box.m_Max.z, line_seg.m_Start.z, line_seg.m_End.z, tenter, texit))
		{
			return false;
		}

		// all intersections in the green. Return the first time of intersection, tenter.
		tinter = tenter;
		return  true;
	}*/

	/*Float Math::IsectRayPlane(const Ray &ray, const Plane &plane)
	{
		const Float d = -(Vec3::Dot(plane.m_Normal, plane.m_Origin));

		const Float numer = Vec3::Dot(plane.m_Normal, ray.m_Origin) + d;
		const Float denom = Vec3::Dot(plane.m_Normal, ray.m_Dir);

		if (denom == 0)  // normal is orthogonal to vector, cant intersect
			return (-1.0f);

		return -(numer / denom);
	}*/
}
