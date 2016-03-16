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

#ifndef GASS_ELLIPSOID_H
#define GASS_ELLIPSOID_H

#include "Core/Math/GASSVector.h"
namespace GASS
{
	/**
		Class holding ellipsod data. 
		TODO: extened this class with more functions, pointinside, merge etc
	*/
	class GASSCoreExport Ellipsoid
	{
	public:
		Ellipsoid();
		~Ellipsoid();
		//public for fast access
		Vec3 m_Radius;
		Vec3 m_Pos;
	};
}
#endif // #ifndef ELLIPSOID_HH
