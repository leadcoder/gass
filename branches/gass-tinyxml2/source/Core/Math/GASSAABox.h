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

#ifndef GASS_AABOX_H
#define GASS_AABOX_H

#include "Core/Common.h"
#include <vector>
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSSphere.h"

namespace GASS
{
	class Polygon;

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

	class GASSCoreExport AABox
	{
	public:
		AABox();
		AABox(const Vec3 &min_pos, const Vec3 &max_pos);
		virtual ~AABox();
		/**
			Merge this bounding box with other
		*/
		void Union(const AABox &aabox);
		
		/**
			Extened bounding box to include 3d point
		*/
		void Union(Float x,Float y,Float z);
		
		/**
			Extened bounding box to include 3d point
		*/
		void Union(const Vec3 &point);
		
		/**
			Check if polygon is inside bounds
		*/
		bool PolyInside(const Polygon &poly) const;
		
		/**
			Check if line is inside bounds
		*/
		bool LineInside(const Vec3 &p1,const Vec3 &p2) const;

		/**
			Check if point is inside bounds
		*/
		bool PointInside(const Vec3 &point) const;
		
		/**
			Transform this bounding box by matrix
		*/
		void Transform(const Mat4 &mat);

		/**
			Get bounding sphere of this box
		*/
		Sphere GetBoundingSphere() const;
		
		/**
			Get bounding box size
		*/
		Vec3 GetSize() const;

		/**
			Get bounding box from polygons
		*/
		static AABox GetAABox(std::vector<Polygon> &poly_vec);
		
		/**
			Get bounding box from polygon
		*/
		static AABox GetAABox(const Polygon &poly);

		/**
			Get all corner points of thsi box
		*/
		std::vector<Vec3> GetCorners() const;

		//public for fast access
		Vec3 m_Max;
		Vec3 m_Min;
	};
}
#endif // #ifndef AABOX_HH