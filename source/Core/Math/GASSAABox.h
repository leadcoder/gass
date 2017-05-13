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
		TAABox(const TPolygon<TYPE> &poly);
		TAABox(const std::vector< TPolygon<TYPE> > &polys);
		virtual ~TAABox();
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
		Check if line intersect box. 
		TODO: investigate difference from method above and select one implementation
		*/
		bool LineIntersect2(const TLineSegment<TYPE> &segment) const;

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
			Check that m_Max is greater then m_Min
		*/
		bool IsValid() const;
		
		/**
			Get all corner points of this box
		*/
		std::vector<TVec3<TYPE> > GetCorners() const;

		//public for fast access
		TVec3<TYPE> m_Max;
		TVec3<TYPE> m_Min;
	private:
		bool _LineSlabIntersect(TYPE slabmin, TYPE slabmax, TYPE line_start, TYPE line_end, TYPE& tbenter, TYPE& tbexit) const;
	};
	typedef TAABox<double> AABoxd;
	typedef TAABox<float> AABoxf;
	typedef TAABox<Float> AABox;

	template<class TYPE>
	TAABox<TYPE>::TAABox()
	{
		m_Max.x = m_Max.y = m_Max.z = -std::numeric_limits<TYPE>::max();
		m_Min.x = m_Min.y = m_Min.z = std::numeric_limits<TYPE>::max();
	}

	template<class TYPE>
	TAABox<TYPE>::TAABox(const TVec3<TYPE> &min_pos, const TVec3<TYPE> &max_pos)
	{
		m_Max = max_pos;
		m_Min = min_pos;
	}

	template<class TYPE>
	TAABox<TYPE>::TAABox(const TPolygon<TYPE> &poly)
	{
		m_Max.x = m_Max.y = m_Max.z = -std::numeric_limits<TYPE>::max();
		m_Min.x = m_Min.y = m_Min.z = std::numeric_limits<TYPE>::max();
		Union(poly);
	}

	template<class TYPE>
	TAABox<TYPE>::TAABox(const std::vector<TPolygon<TYPE> > &polys)
	{
		m_Max.x = m_Max.y = m_Max.z = -std::numeric_limits<TYPE>::max();
		m_Min.x = m_Min.y = m_Min.z = std::numeric_limits<TYPE>::max();
		Union(polys);
	}

	template<class TYPE>
	TAABox<TYPE>::~TAABox()
	{

	}

	template<class TYPE>
	bool TAABox<TYPE>::IsValid() const
	{
		return (m_Max.x >  m_Min.x &&
			m_Max.y >  m_Min.y &&
			m_Max.z >  m_Min.z);
	}

	template<class TYPE>
	void TAABox<TYPE>::Union(TYPE x, TYPE y, TYPE z)
	{
		TVec3<TYPE> point(x,y,z);
		Union(point);
	}

	template<class TYPE>
	void TAABox<TYPE>::Union(const TVec3<TYPE> &point)
	{
		if (point.x > m_Max.x) m_Max.x = point.x;
		if (point.x < m_Min.x) m_Min.x = point.x;

		if (point.y > m_Max.y) m_Max.y = point.y;
		if (point.y < m_Min.y) m_Min.y = point.y;

		if (point.z > m_Max.z) m_Max.z = point.z;
		if (point.z < m_Min.z) m_Min.z = point.z;
	}

	template<class TYPE>
	void TAABox<TYPE>::Union(const TAABox<TYPE> &TAABox)
	{
		if (TAABox.m_Max.x > m_Max.x) m_Max.x = TAABox.m_Max.x;
		if (TAABox.m_Min.x < m_Min.x) m_Min.x = TAABox.m_Min.x;

		if (TAABox.m_Max.y > m_Max.y) m_Max.y = TAABox.m_Max.y;
		if (TAABox.m_Min.y < m_Min.y) m_Min.y = TAABox.m_Min.y;

		if (TAABox.m_Max.z > m_Max.z) m_Max.z = TAABox.m_Max.z;
		if (TAABox.m_Min.z < m_Min.z) m_Min.z = TAABox.m_Min.z;
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
		TAABox<TYPE> ret;
		for (size_t i = 0; i < poly.m_VertexVector.size(); i++)
		{
			Union(poly.m_VertexVector[i]);
		}
	}

	template<class TYPE>
	void TAABox<TYPE>::Transform(const TMat4<TYPE> &mat)
	{
		TVec3<TYPE> p1, p2, p3, p4, p5, p6, p7, p8;

		p1.Set(m_Min.x, m_Max.y, m_Max.z);
		p2 = m_Max;
		p3.Set(m_Min.x, m_Min.y, m_Max.z);
		p4.Set(m_Max.x, m_Min.y, m_Max.z);
		p5.Set(m_Min.x, m_Max.y, m_Min.z);
		p6.Set(m_Max.x, m_Max.y, m_Min.z);
		p7.Set(m_Max.x, m_Min.y, m_Min.z);
		p8 = m_Min;

		p1 = mat * p1;
		p2 = mat * p2;
		p3 = mat * p3;
		p4 = mat * p4;
		p5 = mat * p5;
		p6 = mat * p6;
		p7 = mat * p7;
		p8 = mat * p8;

		m_Max.x = m_Max.y = m_Max.z = -std::numeric_limits<TYPE>::max();
		m_Min.x = m_Min.y = m_Min.z = std::numeric_limits<TYPE>::max();
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
		ret.push_back(m_Min);
		ret.push_back(TVec3<TYPE>(m_Max.x, m_Min.y, m_Min.z));
		ret.push_back(TVec3<TYPE>(m_Max.x, m_Min.y, m_Max.z));
		ret.push_back(TVec3<TYPE>(m_Min.x, m_Min.y, m_Max.z));

		ret.push_back(TVec3<TYPE>(m_Min.x, m_Max.y, m_Min.z));
		ret.push_back(TVec3<TYPE>(m_Max.x, m_Max.y, m_Min.z));
		ret.push_back(TVec3<TYPE>(m_Max.x, m_Max.y, m_Max.z));
		ret.push_back(TVec3<TYPE>(m_Min.x, m_Max.y, m_Max.z));

		return ret;
	}

	template<class TYPE>
	TSphere<TYPE> TAABox<TYPE>::GetBoundingSphere() const
	{
		TSphere<TYPE> sphere;
		sphere.m_Pos = (m_Max + m_Min)*0.5;
		sphere.m_Radius = (sphere.m_Pos - m_Max).Length();
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
			if (LineIntersect2(TLineSegment<TYPE>(*p1, *p2))) 
				return true;
		}
		return false;
	}

	template<class TYPE>
	bool TAABox<TYPE>::PointInside(const TVec3<TYPE> &point) const
	{
		if (point.x < m_Max.x && point.x > m_Min.x &&
			point.y < m_Max.y && point.y > m_Min.y &&
			point.z < m_Max.z && point.z > m_Min.z)return true;
		else return false;
	}

	template<class TYPE>
	bool TAABox<TYPE>::BoxIntersect(const TAABox<TYPE> &box) const
	{
		return (m_Min.x <= box.m_Max.x &&
			m_Max.x >= box.m_Min.x &&
			m_Min.y <= box.m_Max.y &&
			m_Max.y >= box.m_Min.y &&
			m_Min.z <= box.m_Max.z &&
			m_Max.z >= box.m_Min.z);
	}

	template<class TYPE>
	bool TAABox<TYPE>::LineIntersect2(const TLineSegment<TYPE> &seg) const
	{
		TYPE x, y, z, scale;
		TVec3<TYPE> dir, pos, isect;
		pos = seg.m_Start;
		dir = seg.m_End - seg.m_Start;

		if (m_Max.x < seg.m_Start.x && m_Max.x < seg.m_End.x) return false;
		if (m_Min.x > seg.m_Start.x && m_Min.x > seg.m_End.x) return false;

		if (m_Max.y < seg.m_Start.y && m_Max.y < seg.m_End.y) return false;
		if (m_Min.y > seg.m_Start.y && m_Min.y > seg.m_End.y) return false;

		if (m_Max.z < seg.m_Start.z && m_Max.z < seg.m_End.z) return false;
		if (m_Min.z > seg.m_Start.z && m_Min.z > seg.m_End.z) return false;

		//Check max  X plane
		if ((m_Max.x < seg.m_Start.x && m_Max.x > seg.m_End.x) || (m_Max.x > seg.m_Start.x && m_Max.x <= seg.m_End.x))
		{
			x = m_Max.x;
			scale = (x - pos.x) / dir.x;
			isect = pos + dir * scale;
			if (isect.y > m_Min.y && isect.y < m_Max.y && isect.z > m_Min.z && isect.z < m_Max.z) return true;
		}
		//Check min  X plane
		if ((m_Min.x < seg.m_Start.x && m_Min.x > seg.m_End.x) || (m_Min.x > seg.m_Start.x && m_Min.x < seg.m_End.x))
		{
			x = m_Min.x;
			scale = (x - pos.x) / dir.x;
			isect = pos + dir * scale;
			if (isect.y > m_Min.y && isect.y < m_Max.y &&
				isect.z > m_Min.z && isect.z < m_Max.z) return true;
		}

		//Check max  Y plane
		if ((m_Max.y < seg.m_Start.y && m_Max.y > seg.m_End.y) || (m_Max.y > seg.m_Start.y && m_Max.y < seg.m_End.y))
		{
			y = m_Max.y;
			scale = (y - pos.y) / dir.y;
			isect = pos + dir * scale;
			if (isect.x > m_Min.x && isect.x < m_Max.x &&
				isect.z > m_Min.z && isect.z < m_Max.z) return true;
		}

		//Check min  Y plane
		if ((m_Min.y < seg.m_Start.y && m_Min.y > seg.m_End.y) || (m_Min.y > seg.m_Start.y && m_Min.y < seg.m_End.y))
		{
			y = m_Min.y;
			scale = (y - pos.y) / dir.y;
			isect = pos + dir * scale;
			if (isect.x > m_Min.x && isect.x < m_Max.x &&
				isect.z > m_Min.z && isect.z < m_Max.z) return true;
		}
		//Check max  Z plane
		if ((m_Max.z < seg.m_Start.z && m_Max.z > seg.m_End.z) || (m_Max.z > seg.m_Start.z && m_Max.z < seg.m_End.z))
		{
			z = m_Max.z;
			scale = (z - pos.z) / dir.z;
			isect = pos + dir * scale;
			if (isect.y > m_Min.y && isect.y < m_Max.y &&
				isect.x > m_Min.x && isect.x < m_Max.x) return true;
		}

		//Check min  Z plane
		if ((m_Min.z < seg.m_Start.z && m_Min.z > seg.m_End.z) || (m_Min.z > seg.m_Start.y && m_Min.z < seg.m_End.z))
		{
			z = m_Min.z;
			scale = (z - pos.z) / dir.z;
			isect = pos + dir * scale;
			if (isect.y > m_Min.y && isect.y < m_Max.y &&
				isect.x > m_Min.x && isect.x < m_Max.x) return true;
		}
		return false;
	}

	template<class TYPE>
	bool TAABox<TYPE>::LineIntersect(const TLineSegment<TYPE> &line_seg, TYPE &line_dist) const
	{
		// initialize to the segment's boundaries.
		TYPE tenter = 0.0f, texit = 1.0f;

		// test X slab
		if (!_LineSlabIntersect(m_Min.x, m_Max.x, line_seg.m_Start.x, line_seg.m_End.x, tenter, texit))
		{
			return false;
		}

		// test Y slab

		if (!_LineSlabIntersect(m_Min.y, m_Max.y, line_seg.m_Start.y, line_seg.m_End.y, tenter, texit))
		{
			return false;
		}

		// test Z slab
		if (!_LineSlabIntersect(m_Min.z, m_Max.z, line_seg.m_Start.z, line_seg.m_End.z, tenter, texit))
		{
			return false;
		}
		// all intersections in the green. Return the first time of intersection, tenter.
		line_dist = tenter;
		return  true;
	}


	template<class TYPE>
	TVec3<TYPE> TAABox<TYPE>::GetSize() const
	{
		return (m_Max - m_Min);
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
}

