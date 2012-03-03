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

#ifndef AABOX_HH
#define AABOX_HH

#include "Core/Common.h"
#include <vector>
#include "Core/Math/Vector.h"
#include "Core/Math/Sphere.h"

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
		void Union(const AABox &aabox);
		void Union(Float x,Float y,Float z);
		void Union(const Vec3 &point);
		bool PolyInside(const Polygon &poly) const;
		bool LineInside(const Vec3 &p1,const Vec3 &p2) const;
		bool PointInside(const Vec3 &point) const;
		void Transform(const Mat4 &mat);
		Sphere GetBoundingSphere() const;
		Vec3 GetSize() const;
		static AABox GetAABox(std::vector<Polygon> &poly_vec);
		static AABox GetAABox(const Polygon &poly);
		Vec3 m_Max;
		Vec3 m_Min;
	};
}
#endif // #ifndef AABOX_HH
