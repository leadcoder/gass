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

#ifndef GASS_SPLINE_H
#define GASS_SPLINE_H

#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSMatrix.h"
#include <vector>

namespace GASS
{
	class GASSCoreExport Spline
	{
	public:
		Spline(void);
		~Spline(void);
		Vec3 Interpolate(unsigned int fromIndex, Float t) const;
		void Clear();
		void AddPoint(Vec3 &p);
		void RecalcTangents(void);
		void SetAutoCalc(bool value){m_AutoCalc = value;}
		std::vector<Vec3>& GetPoints() {return m_Points;}
		std::vector<Vec3>& GetTangents() {return m_Tangents;}
	private:
		std::vector<Vec3> m_Points;
		std::vector<Vec3> m_Tangents;
		Mat4 m_Coeffs;
		bool m_AutoCalc;
		Float m_TanStrength;
	};
}
#endif // #ifndef SPLINE_HH
