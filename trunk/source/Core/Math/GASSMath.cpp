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

#include <math.h>
#include <limits>
#include "Core/Math/GASSMath.h"
#include "Core/Math/GASSPolygon.h"

#undef min
#undef max
namespace GASS
{
	Float Math::Rad2Deg(Float rad )
	{
		return Float(360*rad/(2*MY_PI));
	}

	Float Math::Deg2Rad(Float deg)
	{
		return Float(2*MY_PI * deg/360.0);
	}

	Vec3 Math::Deg2Rad(const Vec3 &vec)
	{
		Vec3 ret;
		ret.x = Math::Deg2Rad(vec.x);
		ret.y = Math::Deg2Rad(vec.y);
		ret.z = Math::Deg2Rad(vec.z);
		return ret;
	}

	Vec3 Math::Rad2Deg(const Vec3 &vec)
	{
		Vec3 ret;
		ret.x = Math::Rad2Deg(vec.x);
		ret.y = Math::Rad2Deg(vec.y);
		ret.z = Math::Rad2Deg(vec.z);
		return ret;
	}


	Vec3 Math::ProjectVectorOnPlane(const Vec3 plane_normal,const Vec3 &v)
	{
		return  v - Math::Dot(v, plane_normal) * plane_normal;
	}


	Vec3 Math::GetNormal(const Vec3 &p1,const Vec3 &p2,const Vec3 &p3)
	{
		Vec3 normal;
		Vec3 edge1 = p2 - p1;
		Vec3 edge2 = p3 - p1;

		normal = Cross(edge1,edge2);
		normal.Normalize();
		return normal;
	}


	bool Math::TriangleIsectTriangle(const Vec3 &t1_v1,const Vec3 &t1_v2,const Vec3 &t1_v3,const Vec3 &t2_v1,const Vec3 &t2_v2,const Vec3 &t2_v3,Vec3 &isect_point)
	{
		//check first against second
		if(RayIsectTriangle(t1_v1, t1_v2,t2_v1,t2_v1,t2_v3,isect_point)) return true;
		if(RayIsectTriangle(t1_v1, t1_v3,t2_v1,t2_v1,t2_v3,isect_point)) return true;
		if(RayIsectTriangle(t1_v2, t1_v3,t2_v1,t2_v1,t2_v3,isect_point)) return true;

		//check second against first
		if(RayIsectTriangle(t2_v1, t2_v2,t1_v1,t1_v1,t1_v3,isect_point)) return true;
		if(RayIsectTriangle(t2_v1, t2_v3,t1_v1,t1_v1,t1_v3,isect_point)) return true;
		if(RayIsectTriangle(t2_v2, t2_v3,t1_v1,t1_v1,t1_v3,isect_point)) return true;

		return false;
	}

	bool Math::RayIsectTriangle(const Vec3 &start,const Vec3 &end,const Vec3 &p1,const Vec3 &p2,const Vec3 &p3,Vec3 &isect_point)
	{

		Vec3 normal;
		const Vec3 edge1 = p2 - p1;
		const Vec3 edge2 = p3 - p1;

		normal = Cross(edge1,edge2);
		normal.Normalize();

		int side1 = ClassifyPoint(start, p1,normal);
		int side2 = ClassifyPoint(end, p1,normal);

		if((side1 == PLANE_BACK && side2 == PLANE_BACK) || (side1 == PLANE_FRONT && side2 == PLANE_FRONT)) return false;

		Vec3 ray_dir = end - start;

		ray_dir.Normalize();

		const Float ray_scale = IsectRayPlane(start, ray_dir, p1,normal);

		if(ray_scale == -1) return false;

		isect_point = start + ray_dir * ray_scale;

		if(CheckPointInTriangle3(isect_point, p1, p2, p3)) return true;

		return false;
	}

	bool Math::LineIsectPolygon(const Vec3 &start,const Vec3 &end,const Polygon &poly)
	{
		int side1 = ClassifyPoint(start, poly);
		int side2 = ClassifyPoint(end, poly);

		if((side1 == PLANE_BACK && side2 == PLANE_BACK) || (side1 == PLANE_FRONT && side2 == PLANE_FRONT)) return false;

		Vec3 ray_dir = end - start;

		ray_dir.Normalize();

		const Float ray_scale = IsectRayPlane(start, ray_dir, poly.m_VertexVector[0],poly.m_Normal);

		if(ray_scale == -1) return false;

		const Vec3 isect_point = start + ray_dir * ray_scale;

		size_t size = poly.m_VertexVector.size();
		size %= 3;
		size_t index = 1;
		for(size_t i = 0; i < size+1;i++)
		{

			if(CheckPointInTriangle1(isect_point, 
				poly.m_VertexVector[0],
				poly.m_VertexVector[index],
				poly.m_VertexVector[index+1],
				poly.m_Normal)) 
				return true;
			++index;
		}
		return false;
	}

	Float Math::Dot(const Vec3 &v1,const Vec3 &v2)
	{
		return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
	}

	Vec3 Math::Cross(const Vec3 &v1,const Vec3 &v2)
	{
		Vec3 ret;
		ret.x = (v1.y * v2.z) - (v1.z * v2.y);
		ret.y = (v1.z * v2.x) - (v1.x * v2.z);
		ret.z = (v1.x * v2.y) - (v1.y * v2.x);

		return ret;
	}

	int Math::ClassifyPoint(const Vec3 &point, const Polygon &poly) 
	{
		const Vec3 dir = poly.m_VertexVector[0] - point;
		double d = Dot(dir, poly.m_Normal);

		if (d < -0.001f)
			return PLANE_FRONT;	
		else if (d > 0.001f)
			return PLANE_BACK;	

		return ON_PLANE;	
	}

	int Math::ClassifyPoint(const Vec3 &point, const Vec3 &origin, const Vec3 &normal) 
	{
		const Vec3 dir = origin - point;
		double d = Dot(dir, normal);

		if (d<-0.001f)
			return PLANE_FRONT;	
		else if (d>0.001f)
			return PLANE_BACK;	

		return ON_PLANE;	
	}



	char Math::GetMaxCoord(const Vec3 &coord)
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
	}




	bool Math::CheckPointInTriangle1(const Vec3 &point, const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec3 &normal) 
	{

		double total_angles = 0.0f;

		Float a1,a2,b1,b2,c1,c2,p1,p2;
		Float va1,va2,vb1,vb2,vc1,vc2;

		char max = GetMaxCoord(normal);
		switch(max)
		{
		case 0:
			a1 = a.y;
			a2 = a.z;
			b1 = b.y;
			b2 = b.z;
			c1 = c.y;
			c2 = c.z;
			p1 = point.y;
			p2 = point.z;
			break;
		case 1:
			a1 = a.x;
			a2 = a.z;
			b1 = b.x;
			b2 = b.z;
			c1 = c.x;
			c2 = c.z;
			p1 = point.x;
			p2 = point.z;
			break;
		case 2:
			a1 = a.x;
			a2 = a.y;
			b1 = b.x;
			b2 = b.y;
			c1 = c.x;
			c2 = c.y;
			p1 = point.x;
			p2 = point.y;
			break;
		}

		va1 = a1-p1;
		va2 = a2-p2;

		vb1 = b1-p1;
		vb2 = b2-p2;

		vc1 = c1-p1;
		vc2 = c2-p2;


		total_angles  = Angle2D(va1,va2,vb1,vb2);
		total_angles  += Angle2D(vb1,vb2,vc1,vc2);
		total_angles  += Angle2D(vc1,vc2,va1,va2);
		if (fabs(total_angles) < MY_PI)
			return false;

		return true;
	}


	Float Math::IsectRayPlane(const Vec3 &start, const Vec3 &dir, const Vec3 &origin,const Vec3 &normal) 
	{

		Float d = - (Dot(normal,origin));

		Float numer = Dot(normal,start) + d;
		Float denom = Dot(normal,dir);

		if (denom == 0)  // normal is orthogonal to vector, cant intersect
			return (-1.0f);

		return -(numer / denom);	
	}

	Float Math::Angle2D(Float x1, Float y1, Float x2, Float y2)
	{
		Float dtheta,theta1,theta2;

		theta1 = atan2(y1,x1);
		theta2 = atan2(y2,x2);
		dtheta = theta2 - theta1;
		while (dtheta > MY_PI)
			dtheta -= 2.0f*(Float)MY_PI;
		while (dtheta < -(Float)MY_PI)
			dtheta += 2.0f*(Float)MY_PI;

		return dtheta;
	}

	//Some bug in this one
	bool Math::CheckPointInTriangle2(const Vec3 &point, const Vec3 &a, const Vec3 &b, const Vec3 &c, const Vec3 &normal) 
	{

		double total_angles = 0.0f;

		// make the 3 vectors
		Vec3 v1 = a-point;
		Vec3 v2 = b-point;
		Vec3 v3 = c-point;

		v1.Normalize();
		v2.Normalize();
		v3.Normalize();

		total_angles = acos(Dot(v1,v2));   
		total_angles += acos(Dot(v2,v3));
		total_angles += acos(Dot(v3,v1)); 


		if (fabs(total_angles-2*MY_PI) <= 0.005)
			return (true);
		return(false);
	}

	int Math::CheckPointInTriangle3(const Vec3& point, const Vec3& pa,const Vec3& pb, const Vec3& pc)
	{
		Vec3 e10= pb-pa;
		Vec3 e20= pc-pa;
		Float a = Math::Dot(e10,e10);
		Float b = Math::Dot(e10,e20);
		Float c = Math::Dot(e20,e20);
		Float ac_bb=(a*c)-(b*b);

		Vec3 vp;
		vp.Set(point.x-pa.x, point.y-pa.y, point.z-pa.z);
		Float d = Math::Dot(vp,e10);
		Float e = Math::Dot(vp,e20);
		Float x = (d*c)-(e*b);
		Float y = (e*a)-(d*b);
		Float z = x+y-ac_bb;
		//return (( in(z)& ~(in(x)|in(y)) ) & 0x80000000);
		return (( int(z) & ~(int(x)|int(y)) ) & 0x80000000);
	}

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

	Float Math::Min(const Float &v1,const Float &v2)
	{
		if(v1 < v2)
		{
			return v1;

		}
		else return v2;
	}


	Float Math::Max(const Float &v1,const Float &v2)
	{
		if(v1 > v2)
		{
			return v1;

		}
		else return v2;
	}


	Float Math::Min(const Float &v1,const Float &v2,const Float &v3)
	{
		if(v1 < v2)
		{
			if(v1 < v3) return v1;
			else return v3;
		}
		else if(v2 < v3) return v2;
		else return v3;
	}

	Float Math::Max(const Float &v1,const Float &v2,const Float &v3)
	{
		if(v1 > v2)
		{
			if(v1 > v3) return v1;
			else return v3;
		}
		else if(v2 > v3) return v2;
		else return v3;
	}


	bool Math::ClosestPointOnTriangle(const Vec3 &a, const Vec3 &b, const Vec3 &c,
		const Vec3 &p, Vec3 &closest, Float radius)
	{
		Vec3 pNormal;
		Vec3 pOrigin;
		Vec3 v1, v2;

		// Make the plane containing this triangle.
		pOrigin = a;
		v1 = b-a;
		v2 = c-a;

		/*
		All triangles are valid - MOB

		// You might not need this if you KNOW all your triangles are valid
		if ((v1.length() == 0) || (v2.length() == 0)) return false;
		*/

		// determine normal to plane containing polygon
		pNormal = Cross(v1, v2);
		pNormal.Normalize();

		// find how far away the plane is from point p along the planes normal

		Float distToPlaneIntersection = IsectRayPlane(p, -pNormal, pOrigin, pNormal);
		if ((distToPlaneIntersection == -1) || (distToPlaneIntersection > radius)) return false;

		// find the nearest point on the plane to p
		closest = p - (pNormal * distToPlaneIntersection);

		// determine if that point is in the triangle
		return CheckPointInTriangle1(closest, a, b, c,pNormal);
	}


	Vec3 Math::ClosestPointOnTriangleEdge(Vec3 a, Vec3 b, Vec3 c, Vec3 p)
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
	}


	bool Math::GetClosestPointOnPath(const Vec3& source_pos , const std::vector<Vec3> &wps, int &segment_index, Vec3& point )
	{
		double shortest_dist = std::numeric_limits<double>::max();
		if(wps.size() > 1)
		{
			for(size_t i = 0; i < wps.size()-1; i++)
			{
				Vec3 wp1 = wps[i];
				Vec3 wp2 = wps[i+1];
				Vec3 closest_point_on_line = Math::ClosestPointOnLine(wp1,wp2, source_pos);
				double dist = (source_pos  - closest_point_on_line).FastLength();
				if(dist < shortest_dist)
				{
					point = closest_point_on_line;
					shortest_dist = dist;
					segment_index = (int)i;
				}
			}
		}
		else
			return false;
		return true;
	}

	// ----------------------------------------------------------------------------
	// given a distance along the path, convert it to a point on the path
	Vec3 Math::GetPointOnPath(Float pathDistance, const std::vector<Vec3> &wps, bool cyclic, int &index)
	{
		Float  totalPathLength  = 0;
		std::vector<Float> lengths;
		for (unsigned int i = 1; i < wps.size(); i++)
		{
			Float segmentLength = (wps[i-1] - wps[i]).FastLength();
			lengths.push_back(segmentLength);
			totalPathLength += segmentLength; 
		}

		// clip or wrap given path distance according to cyclic flag
		Float remaining = pathDistance;
		if (cyclic)
		{
			remaining = (Float) fmod(pathDistance, totalPathLength);
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
				index = (int) wps.size()-1;
				return wps[wps.size()-1];
			}
		}

		// step through segments, subtracting off segment lengths until
		// locating the segment that contains the original pathDistance.
		// Interpolate along that segment to find 3d point value to return.
		Vec3 result;
		for (unsigned int i = 1; i < wps.size(); i++)
		{
			Float segmentLength = lengths[i-1];
			if (segmentLength < remaining)
			{
				remaining -= segmentLength;
			}
			else
			{
				Float ratio = remaining / segmentLength;
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
			Float segmentLength = (wps[i] - wps[i-1]).FastLength();
			Vec3 closest_point_on_line = Math::ClosestPointOnLine(wps[i-1],wps[i], point);
			double dist = (point  - closest_point_on_line).FastLength();
			if(dist < shortest_dist)
			{
				shortest_dist = dist;
				distance_to_path = dist;
				pathDistance = segmentLengthTotal + (wps[i-1] - closest_point_on_line).FastLength();
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
			Float segmentLength = (wps[i-1] - wps[i]).FastLength();
			lengths.push_back(segmentLength);
			totalPathLength += segmentLength; 
		}

		Float start_remaining = start_distance;
		Float end_remaining = end_distance;
		std::vector<Vec3> path;
		int index = 0;
		for (unsigned int i = 1; i < wps.size(); i++)
		{
			Float segmentLength = lengths[i-1];
			if (segmentLength < start_remaining)
			{
				start_remaining -= segmentLength;
				end_remaining -= segmentLength;
			}
			else
			{
				Float ratio = start_remaining / segmentLength;
				Vec3 start_point = wps[i-1] + ((wps[i] - wps[i-1])*ratio);
				path.push_back(start_point);
				index = i;
				break;
			}
		}

		for (unsigned int i = index; i < wps.size(); i++)
		{
			Float segmentLength = lengths[i-1];

			if (segmentLength < end_remaining)
			{
				end_remaining -= segmentLength;
				//path.push_back(wps[i-1]);
				//if(index == wps.size()-1) // if last
				path.push_back(wps[i]);
			}
			else
			{
				Float ratio = end_remaining / segmentLength;
				Vec3 end_wp = wps[i-1] + ((wps[i] - wps[i-1])*ratio);
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
			Float segmentLength = (wps[i-1] - wps[i]).FastLength();
			totalPathLength += segmentLength; 
		}
		Float dist = 0;
		std::vector<Vec3> offset_path;
		for(size_t i = 0; i < wps.size(); i++)
		{
			Vec3 side;
			Float inter = dist/totalPathLength;
			if(i < wps.size()-2)
				dist += (wps[i-1] - wps[i]).FastLength();
			Float offset = start_offset + inter*(end_offset - start_offset);
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
				width_mult = Math::Dot(d1,side);
				if(width_mult > 0)
					width_mult = 1.0/width_mult;

			}
			else
			{
				side = wps[i]-wps[i-1];
			}
			side.Normalize();
			side = Math::Cross(side,Vec3(0,1,0)); 
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
				width_mult = Math::Dot(d1,side);
				if(width_mult > 0)
					width_mult = 1.0/width_mult;

			}
			else
			{
				side = wps[i]-wps[i-1];
			}
			side.Normalize();
			side = Math::Cross(side,Vec3(0,1,0)); 
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
				width_mult = Math::Dot(d1,side);
				if(width_mult > 0)
					width_mult = 1.0/width_mult;

			}
			else
			{
				side = wps[i]-wps[i-1];
			}
			side.Normalize();
			side = Math::Cross(side,Vec3(0,1,0)); 
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
	}


	Vec3 Math::ClosestPointOnLine(const Vec3 &a, const Vec3 &b, const Vec3 &p)
	{
		// Determine t (the length of the vector from a to p)
		Vec3 c = p-a;
		Vec3 V = b-a;

		double d = V.Length();

		V.Normalize();
		double t = Dot(V,c);

		// Check to see if t is beyond the extents of the line segment
		if (t < 0.0f) return (a);
		if (t > d) return (b);

		// Return the point between a and b
		//set length of V to t. V is normalized so this is easy
		V.x = V.x * t;
		V.y = V.y * t;
		V.z = V.z * t;

		return (a + V);
	}

	bool Math::GetLineIntersection(const Vec2 &p1, const Vec2 &p2, const Vec2 &p3, const Vec2 &p4, Vec2 &isect) 
	{
		// Store the values for fast access and easy
		// equations-to-code conversion
		
		Float x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
		Float y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;

		Float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		// If d is zero, there is no intersection
		if (d == 0) 
			return false;

		// Get the x and y
		Float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
		Float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
		Float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;

		// Check if the x and y coordinates are within both lines
		if ( x < std::min(x1, x2) || x > std::max(x1, x2) ||
			x < std::min(x3, x4) || x > std::max(x3, x4) ) return false;
		if ( y < std::min(y1, y2) || y > std::max(y1, y2) ||
			y < std::min(y3, y4) || y > std::max(y3, y4) ) return false;

		// Return the point of intersection
		isect.x = x;
		isect.y = y;
		return true;
	}

}



