/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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

#ifndef GASS_ROTATIONSPLINE_H
#define GASS_ROTATIONSPLINE_H

#include "Core/Math/GASSQuaternion.h"
#include <vector>


namespace GASS
{
	class GASSCoreExport RotationSpline
	{
	public:
		RotationSpline(void);
		~RotationSpline(void);
		Quaternion Interpolate(unsigned int fromIndex, float t) const;
		void Clear();
		void AddPoint(const Quaternion &p);
		void RecalcTangents(void);
	private:
		std::vector<Quaternion> m_Points;
		std::vector<Quaternion> m_Tangents;
		Mat4 m_Coeffs;
		bool m_AutoCalc;
	};
}
#endif // #ifndef ROTATIONSPLINE_HH
