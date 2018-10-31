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
#include "Core/Math/GASSVector.h"
#include <cmath>

namespace GASS
{
	//temp class for waypoint path... 
	class GASSCoreExport Path
	{
	public:
		// path utilities
		static bool GetClosestPointOnPath(const Vec3& source_pos , const std::vector<Vec3> &wps, int &segment_index, Vec3& point);
		static Vec3 GetPointOnPath(Float pathDistance, const std::vector<Vec3> &wps, bool cyclic,int &index);
		static Float GetPathDistance(const Vec3& point, const std::vector<Vec3> &wps,int &index,Float &distance_to_path);
		static Float GetPathLength(const std::vector<Vec3> &wps);
		static std::vector<Vec3> ClipPath(Float start_distance, Float end_distance, const std::vector<Vec3> &wps);
		static std::vector<Vec3> GenerateOffset(const std::vector<Vec3> &wps, Float offset);
		static std::vector<Vec3> GenerateOffset(const std::vector<Vec3> &wps, Float start_offset,Float end_offset);
		static std::vector<Vec3> GenerateNormals(const std::vector<Vec3> &wps);
	private:
	};
}
