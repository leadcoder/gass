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
#include <vector>
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSSphere.h"
#include "Core/Math/GASSMatrix.h"
#include "Core/Math/GASSPolygon.h"
#include "Core/Math/GASSLineSegment.h"
#include "Core/Math/GASSRay.h"

#undef min
#undef max

namespace GASS
{

	/** \addtogroup GASSCore
	*  @{
	*/

	/** \addtogroup Math
	*  @{
	*/

	/**
		A Axis Aligned Box usually holding bounding information. 
		The box is represented by a max 3d-coordinate and min 3d-coordinate
	*/

	template<class TYPE>
	class TAABox
	{
	public:
		TAABox();
		TAABox(const TVec3<TYPE> &min_pos, const TVec3<TYPE> &max_pos);
		explicit TAABox(const TPolygon<TYPE> &poly);
		explicit TAABox(const std::vector< TPolygon<TYPE> > &polys);
		
		/**
			Merge this bounding box with other
		*/
		void Union(const TAABox &TAABox);
		
		/**
			Extend bounding box to include 3d point
		*/
		void Union(TYPE x, TYPE y, TYPE z);
		
		/**
			Extend bounding box to include 3d point
		*/
		void Union(const TVec3<TYPE> &point);

		/**
		Extend bounding box from polygons
		*/
		void Union(const std::vector< TPolygon<TYPE> > &poly_vec);

		/**
		Extend bounding box from polygon
		*/

		void Union(const TPolygon<TYPE> &poly);
		
		/**
			Check if polygon intersect bounds
		*/
		bool PolyIntersect(const TPolygon<TYPE> &poly) const;
		
		

		/**
		Check intersection between line and box
		@param segment Line segment to check with
		@param line_dist Potential normalized intersection distance along line segment
		@return true if intersection exist
		*/
		bool LineIntersect(const TLineSegment<TYPE> &segment, TYPE &line_dist) const;

		/**
		Check intersection between line and box
		@param segment Line segment to check with
		@return true if intersection exist
		@note Diffrent code path from above, cause no dist request. TODO: profile this!  
		*/
		bool LineIntersect(const TLineSegment<TYPE> &segment) const;
	
		/**
		Check intersection between ray and box. 
		Note that the implementation is branchless and for performance reasons there are some limitations:
		If ray is parallel to box plane this is not considered an hit, the same if ray origin
		is located on box-plane (and ray direction is facing away from box). In other words; 
		if the ray only touch the box shell this is not considered a hit.
	
		@param ray Ray to check against
		@param inv_dir Inverted Ray direction 
		@param t_near If ray instersection (return true) this is the distance where the ray enter the box.
		              If ray origin is inside box this value will be zero
		@param t_far If ray instersection (return true) this is the distance where the ray will exit the box
		@return true If intersection exist
		*/
		bool RayIntersect(const TRay<TYPE> &ray, const TVec3<TYPE> &inv_dir, TYPE &t_near, TYPE &t_far) const;

		/**
		Convenience function that call RayIntersect function above with the inverted ray direction.
		Note that if you want to call this function many times with the same ray you should probably precalcualte the inverted
		ray direction and call the function above instead.
		*/
		bool RayIntersect(const TRay<TYPE> &ray, TYPE &t_near, TYPE &t_far) const;
	
		/**
			Check intersection with other bounding box
		*/
		bool BoxIntersect(const TAABox &box) const;

		/**
			Check if point is inside bounds
		*/
		bool PointInside(const TVec3<TYPE> &point) const;
		
		/**
			Transform this bounding box by matrix
		*/
		void Transform(const TMat4<TYPE> &mat);

		/**
			Get bounding sphere of this box
		*/
		TSphere<TYPE> GetBoundingSphere() const;
		
		/**
			Get bounding box size
		*/
		TVec3<TYPE> GetSize() const;


		/**
			Check that Max is greater then Min
		*/
		bool IsValid() const;
		
		/**
			Get all corner points of this box
		*/
		std::vector<TVec3<TYPE> > GetCorners() const;

		bool operator== (const TAABox &box) const;

		bool Equal(const TAABox& box, TYPE tolerance = std::numeric_limits<TYPE>::epsilon()) const;


		/**
			Get intersection box between this box and provided box
		*/
		TAABox GetIntersection(const TAABox& box) const;
	
		//public for fast access
		TVec3<TYPE> Max;
		TVec3<TYPE> Min;

	private:
		bool _LineSlabIntersect(TYPE slabmin, TYPE slabmax, TYPE line_start, TYPE line_end, TYPE& tbenter, TYPE& tbexit) const;
	};

	typedef TAABox<Float> AABox;
	typedef TAABox<double> AABoxd;
	typedef TAABox<float> AABoxf;
	typedef TAABox<int> AABoxi;

	template<class TYPE>
	TAABox<TYPE>::TAABox()
	{
		Max.x = Max.y = Max.z = -std::numeric_limits<TYPE>::max();
		Min.x = Min.y = Min.z = std::numeric_limits<TYPE>::max();
	}

	template<class TYPE>
	TAABox<TYPE>::TAABox(const TVec3<TYPE> &min_pos, const TVec3<TYPE> &max_pos) : Max(max_pos), Min(min_pos)
	{
	}

	template<class TYPE>
	TAABox<TYPE>::TAABox(const TPolygon<TYPE> &poly)
	{
		Max.x = Max.y = Max.z = -std::numeric_limits<TYPE>::max();
		Min.x = Min.y = Min.z = std::numeric_limits<TYPE>::max();
		Union(poly);
	}

	template<class TYPE>
	TAABox<TYPE>::TAABox(const std::vector<TPolygon<TYPE> > &polys)
	{
		Max.x = Max.y = Max.z = -std::numeric_limits<TYPE>::max();
		Min.x = Min.y = Min.z = std::numeric_limits<TYPE>::max();
		Union(polys);
	}

	template<class TYPE>
	bool TAABox<TYPE>::IsValid() const
	{
		return (Max.x >  Min.x &&
			Max.y >  Min.y &&
			Max.z >  Min.z);
	}

	template<class TYPE>
	void TAABox<TYPE>::Union(TYPE x, TYPE y, TYPE z)
	{
		const TVec3<TYPE> point(x,y,z);
		Union(point);
	}

	template<class TYPE>
	void TAABox<TYPE>::Union(const TVec3<TYPE> &point)
	{
		if (point.x > Max.x) Max.x = point.x;
		if (point.x < Min.x) Min.x = point.x;

		if (point.y > Max.y) Max.y = point.y;
		if (point.y < Min.y) Min.y = point.y;

		if (point.z > Max.z) Max.z = point.z;
		if (point.z < Min.z) Min.z = point.z;
	}

	template<class TYPE>
	void TAABox<TYPE>::Union(const TAABox<TYPE> &TAABox)
	{
		if (TAABox.Max.x > Max.x) Max.x = TAABox.Max.x;
		if (TAABox.Min.x < Min.x) Min.x = TAABox.Min.x;

		if (TAABox.Max.y > Max.y) Max.y = TAABox.Max.y;
		if (TAABox.Min.y < Min.y) Min.y = TAABox.Min.y;

		if (TAABox.Max.z > Max.z) Max.z = TAABox.Max.z;
		if (TAABox.Min.z < Min.z) Min.z = TAABox.Min.z;
	}

	template<class TYPE>
	void TAABox<TYPE>::Union(const std::vector<TPolygon<TYPE> > &poly_vec)
	{
		for (size_t i = 0; i < poly_vec.size(); i++)
		{
			Union(poly_vec[i]);
		}
	}

	template<class TYPE>
	void TAABox<TYPE>::Union(const TPolygon<TYPE> &poly)
	{
		for (size_t i = 0; i < poly.m_VertexVector.size(); i++)
		{
			Union(poly.m_VertexVector[i]);
		}
	}

	template<class TYPE>
	void TAABox<TYPE>::Transform(const TMat4<TYPE> &mat)
	{
		TVec3<TYPE> p1, p2, p3, p4, p5, p6, p7, p8;

		p1.Set(Min.x, Max.y, Max.z);
		p2 = Max;
		p3.Set(Min.x, Min.y, Max.z);
		p4.Set(Max.x, Min.y, Max.z);
		p5.Set(Min.x, Max.y, Min.z);
		p6.Set(Max.x, Max.y, Min.z);
		p7.Set(Max.x, Min.y, Min.z);
		p8 = Min;

		p1 = mat * p1;
		p2 = mat * p2;
		p3 = mat * p3;
		p4 = mat * p4;
		p5 = mat * p5;
		p6 = mat * p6;
		p7 = mat * p7;
		p8 = mat * p8;

		Max.x = Max.y = Max.z = -std::numeric_limits<TYPE>::max();
		Min.x = Min.y = Min.z = std::numeric_limits<TYPE>::max();
		Union(p1);
		Union(p2);
		Union(p3);
		Union(p4);
		Union(p5);
		Union(p6);
		Union(p7);
		Union(p8);
	}

	template<class TYPE>
	std::vector<TVec3<TYPE> > TAABox<TYPE>::GetCorners() const
	{
		std::vector< TVec3<TYPE> > ret;
		ret.push_back(Min);
		ret.push_back(TVec3<TYPE>(Max.x, Min.y, Min.z));
		ret.push_back(TVec3<TYPE>(Max.x, Min.y, Max.z));
		ret.push_back(TVec3<TYPE>(Min.x, Min.y, Max.z));

		ret.push_back(TVec3<TYPE>(Min.x, Max.y, Min.z));
		ret.push_back(TVec3<TYPE>(Max.x, Max.y, Min.z));
		ret.push_back(TVec3<TYPE>(Max.x, Max.y, Max.z));
		ret.push_back(TVec3<TYPE>(Min.x, Max.y, Max.z));

		return ret;
	}

	template<class TYPE>
	TSphere<TYPE> TAABox<TYPE>::GetBoundingSphere() const
	{
		TSphere<TYPE> sphere;
		sphere.m_Pos = (Max + Min) * static_cast<TYPE>(0.5);
		sphere.m_Radius = (sphere.m_Pos - Max).Length();
		return sphere;
	}

	template<class TYPE>
	bool TAABox<TYPE>::PolyIntersect(const TPolygon<TYPE> &poly) const
	{
		size_t i = 0;
		for (; i < poly.m_VertexVector.size(); i++)
		{
			const TVec3<TYPE>* pos = &poly.m_VertexVector[i];
			if (PointInside(*pos)) return true;
		}

		// Get polygon center
		const TVec3<TYPE> center = poly.GetCenter();

		if (PointInside(center)) return true;

		const TVec3<TYPE>* p1, *p2;
		for (i = 0; i < poly.m_VertexVector.size(); i++)
		{
			size_t i2;
			if (i == poly.m_VertexVector.size() - 1) i2 = 0; else i2 = i + 1;
			p1 = &poly.m_VertexVector[i];
			p2 = &poly.m_VertexVector[i2];
			if (LineIntersect(TLineSegment<TYPE>(*p1, *p2))) 
				return true;
		}
		return false;
	}

	template<class TYPE>
	bool TAABox<TYPE>::PointInside(const TVec3<TYPE> &point) const
	{
		if (point.x < Max.x && point.x > Min.x &&
			point.y < Max.y && point.y > Min.y &&
			point.z < Max.z && point.z > Min.z)return true;
		else return false;
	}

	template<class TYPE>
	bool TAABox<TYPE>::BoxIntersect(const TAABox<TYPE> &box) const
	{
		return (Min.x <= box.Max.x &&
			Max.x >= box.Min.x &&
			Min.y <= box.Max.y &&
			Max.y >= box.Min.y &&
			Min.z <= box.Max.z &&
			Max.z >= box.Min.z);
	}
	
	template<class TYPE>
	TAABox<TYPE> TAABox<TYPE>::GetIntersection(const TAABox<TYPE>& rect) const
	{
		TAABox<TYPE> ret;
		ret.Min = TVec3<TYPE>::Max(rect.Min, Min);
		ret.Max = TVec3<TYPE>::Min(rect.Min, Min);
		return ret;
	}
	


#ifdef USE_EXPERIMENTAL_RAY_TEST
	template<class TYPE>
	bool TAABox<TYPE>::RayIntersection(const TRay<TYPE> &ray, const TVec3<TYPE>& dir_inv, TYPE &tmin, TYPE &tmax) const
	{
		const TYPE t1 = (Min.x - ray.m_Origin.x)*dir_inv.x;
		const TYPE t2 = (Max.x - ray.m_Origin.x)*dir_inv.x;
		const TYPE t3 = (Min.y - ray.m_Origin.y) * dir_inv.y;
		const TYPE t4 = (Max.y - ray.m_Origin.y) * dir_inv.y;
		const TYPE t5 = (Min.z - ray.m_Origin.z) * dir_inv.z;
		const TYPE t6 = (Max.z - ray.m_Origin.z) * dir_inv.z;

		tmin = std::min(t1, t2);
		tmax = std::max(t1, t2);
		tmin = std::max(tmin, std::min(std::min(t3, t4), tmax));
		tmax = std::min(tmax, std::max(std::max(t3, t4), tmin));
		tmin = std::max(tmin, std::min(std::min(t5, t6), tmax));
		tmax = std::min(tmax, std::max(std::max(t5, t6), tmin));

		return tmax > std::max(tmin, static_cast<TYPE>(0.0));
	}
#else
	template<class TYPE>
	bool TAABox<TYPE>::RayIntersect(const TRay<TYPE> &ray, const TVec3<TYPE> &inv_dir, TYPE &t_near, TYPE &t_far) const
	{
		//From: "FAST, BRANCHLESS RAY/BOUNDING BOX INTERSECTIONS, PART 2: NANS"
		//https://tavianator.com/fast-branchless-raybounding-box-intersections-part-2-nans
		TYPE t1 = (Min.x - ray.m_Origin.x)*inv_dir.x;
		TYPE t2 = (Max.x - ray.m_Origin.x)*inv_dir.x;

		t_near = std::min(t1, t2);
		t_far = std::max(t1, t2);

		for (int i = 1; i < 3; ++i) {
			t1 = (Min[i] - ray.m_Origin[i])*inv_dir[i];
			t2 = (Max[i] - ray.m_Origin[i])*inv_dir[i];

			//t_near = std::max(t_near, std::min(t1, t2));
			//t_far =  std::min(t_far,  std::max(t1, t2));

			//Handle NaN propagated from above
			t_near = std::max(t_near, std::min(std::min(t1, t2), t_far));
			t_far =  std::min(t_far,  std::max(std::max(t1, t2), t_near));
		}
		return t_far > std::max(t_near, static_cast<TYPE>(0.0));
	}
#endif
	
	template<class TYPE>
	bool TAABox<TYPE>::RayIntersect(const TRay<TYPE> &ray, TYPE &t_near, TYPE &t_far) const
	{
		const TVec3<TYPE> inv_dir(static_cast<TYPE>(1.0) / ray.m_Dir.x, 
			static_cast<TYPE>(1.0) / ray.m_Dir.y, 
			static_cast<TYPE>(1.0) / ray.m_Dir.z);
		return RayIntersect(ray, inv_dir, t_near, t_far);
	}

	template<class TYPE>
	bool TAABox<TYPE>::LineIntersect(const TLineSegment<TYPE> &seg) const
	{
		TYPE x, y, z, scale;
		TVec3<TYPE> dir, pos, isect;
		pos = seg.m_Start;
		dir = seg.m_End - seg.m_Start;
		
		if (PointInside(seg.m_Start))
			return true;

		if (PointInside(seg.m_End))
			return true;

		if (Max.x < seg.m_Start.x && Max.x < seg.m_End.x) return false;
		if (Min.x > seg.m_Start.x && Min.x > seg.m_End.x) return false;

		if (Max.y < seg.m_Start.y && Max.y < seg.m_End.y) return false;
		if (Min.y > seg.m_Start.y && Min.y > seg.m_End.y) return false;

		if (Max.z < seg.m_Start.z && Max.z < seg.m_End.z) return false;
		if (Min.z > seg.m_Start.z && Min.z > seg.m_End.z) return false;

		
		//Check max  X plane
		if ((seg.m_Start.x > Max.x && seg.m_End.x < Max.x) || (Max.x > seg.m_Start.x && Max.x <= seg.m_End.x))
		{
			x = Max.x;
			scale = (x - pos.x) / dir.x;
			isect = pos + dir * scale;
			if (isect.y > Min.y && isect.y < Max.y && 
				isect.z > Min.z && isect.z < Max.z) return true;
		}
		//Check min  X plane
		if ((Min.x < seg.m_Start.x && Min.x > seg.m_End.x) || (Min.x > seg.m_Start.x && Min.x < seg.m_End.x))
		{
			x = Min.x;
			scale = (x - pos.x) / dir.x;
			isect = pos + dir * scale;
			if (isect.y > Min.y && isect.y < Max.y &&
				isect.z > Min.z && isect.z < Max.z) return true;
		}

		//Check max  Y plane
		if ((Max.y < seg.m_Start.y && Max.y > seg.m_End.y) || (Max.y > seg.m_Start.y && Max.y < seg.m_End.y))
		{
			y = Max.y;
			scale = (y - pos.y) / dir.y;
			isect = pos + dir * scale;
			if (isect.x > Min.x && isect.x < Max.x &&
				isect.z > Min.z && isect.z < Max.z) return true;
		}

		//Check min  Y plane
		if ((Min.y < seg.m_Start.y && Min.y > seg.m_End.y) || (Min.y > seg.m_Start.y && Min.y < seg.m_End.y))
		{
			y = Min.y;
			scale = (y - pos.y) / dir.y;
			isect = pos + dir * scale;
			if (isect.x > Min.x && isect.x < Max.x &&
				isect.z > Min.z && isect.z < Max.z) return true;
		}
		//Check max  Z plane
		if ((Max.z < seg.m_Start.z && Max.z > seg.m_End.z) || (Max.z > seg.m_Start.z && Max.z < seg.m_End.z))
		{
			z = Max.z;
			scale = (z - pos.z) / dir.z;
			isect = pos + dir * scale;
			if (isect.y > Min.y && isect.y < Max.y &&
				isect.x > Min.x && isect.x < Max.x) return true;
		}

		//Check min  Z plane
		if ((Min.z < seg.m_Start.z && Min.z > seg.m_End.z) || (Min.z > seg.m_Start.y && Min.z < seg.m_End.z))
		{
			z = Min.z;
			scale = (z - pos.z) / dir.z;
			isect = pos + dir * scale;
			if (isect.y > Min.y && isect.y < Max.y &&
				isect.x > Min.x && isect.x < Max.x) return true;
		}
		return false;
	}


#ifdef USE_RAY_CAST_FOR_LINE_INTERSECTION_TEST
	template<class TYPE>
	bool TAABox<TYPE>::LineIntersect(const TLineSegment<TYPE> &seg, TYPE& dist) const
	{
		TYPE tmin, tmax;
		TVec3<TYPE> ray_dir = seg.m_End - seg.m_Start;
		if (RayIntersect(TRay<TYPE>(seg.m_Start, ray_dir), tmin, tmax))
		{
			dist = std::max(tmin, static_cast<TYPE>(0.0));
			if (tmin < 1.0)
				return true;
		}
		return false;
	}
#else
	template<class TYPE>
	bool TAABox<TYPE>::LineIntersect(const TLineSegment<TYPE> &line_seg, TYPE &line_dist) const
	{
		// initialize to the segment's boundaries.
		TYPE tenter = static_cast<TYPE>(0.0);
		TYPE texit = static_cast<TYPE>(1.0);

		// test X slab
		if (!_LineSlabIntersect(Min.x, Max.x, line_seg.m_Start.x, line_seg.m_End.x, tenter, texit))
		{
			return false;
		}

		// test Y slab

		if (!_LineSlabIntersect(Min.y, Max.y, line_seg.m_Start.y, line_seg.m_End.y, tenter, texit))
		{
			return false;
		}

		// test Z slab
		if (!_LineSlabIntersect(Min.z, Max.z, line_seg.m_Start.z, line_seg.m_End.z, tenter, texit))
		{
			return false;
		}
		// all intersections in the green. Return the first time of intersection, tenter.
		line_dist = tenter;
		return  true;
	}
#endif

	template<class TYPE>
	TVec3<TYPE> TAABox<TYPE>::GetSize() const
	{
		return (Max - Min);
	}

	template<class TYPE>
	bool TAABox<TYPE>::_LineSlabIntersect(TYPE slabmin, TYPE slabmax, TYPE line_start, TYPE line_end, TYPE& tbenter, TYPE& tbexit) const
	{
		TYPE raydir = line_end - line_start;

		// ray parallel to the slab
		if (fabs(raydir) < 1.0E-9f)
		{
			// ray parallel to the slab, but ray not inside the slab planes
			if (line_start < slabmin || line_start > slabmax)
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
		TYPE tsenter = (slabmin - line_start) / raydir;
		TYPE tsexit = (slabmax - line_start) / raydir;

		// order the enter / exit values.
		if (tsenter > tsexit)
		{
			TYPE tmp = tsenter;
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

	template<class TYPE>
	bool TAABox<TYPE>::operator== (const TAABox<TYPE> &box) const
	{
		return (box.Min == Min && box.Max == Max);
	}

	template<class TYPE>
	bool TAABox<TYPE>::Equal(const TAABox<TYPE>& box, TYPE tolerance) const
	{
		return Min.Equal(box.Min, tolerance) &&	Max.Equal(box.Max, tolerance);
	}
}

