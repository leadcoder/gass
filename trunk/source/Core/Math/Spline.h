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

#ifndef SPLINE_HH
#define SPLINE_HH

#include "Core/Math/Vector.h"
#include "Core/Math/Matrix.h"
#include <vector>

namespace GASS
{

class GASSCoreExport Spline
{
public:
	Spline(void);
	~Spline(void);
	Vec3 Interpolate(unsigned int fromIndex, float t) const;
	void Clear();
	void AddPoint(Vec3 &p);
	void RecalcTangents(void);
private:
	std::vector<Vec3> m_Points;
	std::vector<Vec3> m_Tangents;
	Mat4 m_Coeffs;
	bool m_AutoCalc;
	float m_TanStrength;
};
}
#endif // #ifndef SPLINE_HH
