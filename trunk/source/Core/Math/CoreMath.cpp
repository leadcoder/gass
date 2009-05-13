/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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
#include "Core/Math/Polygon.h"
#include "Core/Math/CoreMath.h"


namespace GASS
{
	Float Math::Rad2Deg(Float rad )
	{
		return Float(360*rad/(2*MY_PI));
	}
	Float Math::Deg2Rad(Float deg)
	{
		return Float(2*MY_PI * deg/360.f);
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
		Vec3 edge1 = p2 - p1;
		Vec3 edge2 = p3 - p1;

		normal = Cross(edge1,edge2);
		normal.Normalize();

		int side1 = ClassifyPoint(start, p1,normal);
		int side2 = ClassifyPoint(end, p1,normal);

		if((side1 == PLANE_BACK && side2 == PLANE_BACK) || (side1 == PLANE_FRONT && side2 == PLANE_FRONT)) return false;

		Vec3 ray_dir = end - start;

		ray_dir.Normalize();

		Float ray_scale = IsectRayPlane(start, ray_dir, p1,normal);

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

		Float ray_scale = IsectRayPlane(start, ray_dir, poly.m_VertexVector[0],poly.m_Normal);

		if(ray_scale == -1) return false;

		Vec3 isect_point = start + ray_dir * ray_scale;

		int size = poly.m_VertexVector.size();
		size %= 3;
		int index = 1;
		for(int i = 0; i < size+1;i++)
		{

			if(CheckPointInTriangle1(isect_point, 
				poly.m_VertexVector[0],
				poly.m_VertexVector[index],
				poly.m_VertexVector[index++],
				poly.m_Normal)) 
				return true;
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
		Vec3 dir = poly.m_VertexVector[0] - point;
		double d = Dot(dir, poly.m_Normal);

		if (d < -0.001f)
			return PLANE_FRONT;	
		else if (d > 0.001f)
			return PLANE_BACK;	

		return ON_PLANE;	
	}

	int Math::ClassifyPoint(const Vec3 &point, const Vec3 &origin, const Vec3 &normal) 
	{
		Vec3 dir = origin - point;
		double d = Dot(dir, normal);

		if (d<-0.001f)
			return PLANE_FRONT;	
		else if (d>0.001f)
			return PLANE_BACK;	

		return ON_PLANE;	
	}



	char Math::GetMaxCoord(const Vec3 &coord)
	{
		Float x = fabs(coord.x);
		Float y = fabs(coord.y);
		Float z = fabs(coord.z);
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

		theta1 = (Float)atan2(y1,x1);
		theta2 = (Float)atan2(y2,x2);
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

	bool Math::GetLowestRoot(Float a, Float b, Float c, Float maxR,Float* root) 
	{
		// Check if a solution exists
		Float determinant = b*b - 4.0f*a*c;
		// If determinant is negative it means no solutions.
		if (determinant < 0.0f) return false;
		// calculate the two roots: (if determinant == 0 then
		// x1==x2 but let’s disregard that slight optimization)
		Float sqrtD = sqrt(determinant);
		Float r1 = (-b - sqrtD) / (2*a);
		Float r2 = (-b + sqrtD) / (2*a);
		// Sort so x1 <= x2
		if (r1 > r2) {
			Float temp = r2;
			r2 = r1;
			r1 = temp;
		}
		// Get lowest root:
		if (r1 > 0 && r1 < maxR) {
			*root = r1;
			return true;
		}
		// It is possible that we want x2 - this can happen
		// if x1 < 0
		if (r2 > 0 && r2 < maxR) {
			*root = r2;
			return true;
		}
		// No (valid) solutions
		return false;
	}


	



/*	Mat4 Math::BuildReflectionMatrix(const Vec3& origin,const Vec3& normal )
	{
		Float d = -Math::Dot(normal,origin);
		Mat4 ret;

		ret.m_Data[0][0] = -2 * normal.x * normal.x + 1;
		ret.m_Data[1][0] = -2 * normal.x * normal.y;
		ret.m_Data[2][0] =-2 * normal.x * normal.z;
		ret.m_Data[3][0] =-2 * normal.x * d;
		ret.m_Data[0][1] =-2 * normal.y * normal.x;
		ret.m_Data[1][1] =-2 * normal.y * normal.y + 1;
		ret.m_Data[2][1] =-2 * normal.y * normal.z;
		ret.m_Data[3][1] =-2 * normal.y * d;
		ret.m_Data[0][2] =-2 * normal.z * normal.x;
		ret.m_Data[1][2] =-2 * normal.z * normal.y;
		ret.m_Data[2][2] =-2 * normal.z * normal.z + 1;
		ret.m_Data[3][2] =-2 * normal.z * d;
		ret.m_Data[0][3] =0;
		ret.m_Data[1][3] =0;
		ret.m_Data[2][3] =0;
		ret.m_Data[3][3] =1;

		return ret;
	}*/


	Float Math::GetHorizontalAngleBetweenVectors(const Vec3 &v1, const Vec3 &v2)
	{
		Vec3 v1_xz = v1;
		Vec3 v2_xz = v2;
		v1_xz.y = 0;
		v1_xz.Normalize();
		v2_xz.y = 0;
		v2_xz.Normalize();
		Float cos_angle = Math::Dot(v1_xz,v2_xz); //north
		Vec3 vec_cross = Math::Cross(v1_xz,v2_xz);
		if(cos_angle > 1) cos_angle= 1;
		if(cos_angle < -1) cos_angle= -1;
		Float angle = acos(cos_angle);

		if(vec_cross.y < 0)
		{
			angle = -angle;
			//if(angle_to_dir < angle) angle_to_dir = 2*MY_PI+angle_to_dir;
		}
		return angle;
	}
}



