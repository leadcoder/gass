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

#include <cmath>
#include <limits>
#include "Core/Math/GASSPath.h"
#include "Core/Math/GASSLineSegment.h"
#include "Core/Math/GASSPlane.h"

#undef min
#undef max

namespace GASS
{
	bool Path::GetClosestPointOnPath(const Vec3& source_pos , const std::vector<Vec3> &wps, int &segment_index, Vec3& point )
	{
		double shortest_dist = std::numeric_limits<double>::max();
		if(wps.size() > 1)
		{
			for(size_t i = 0; i < wps.size()-1; i++)
			{
				const Vec3 wp1 = wps[i];
				const Vec3 wp2 = wps[i+1];
				const Vec3 closest_point_on_line = LineSegment(wp1,wp2).ClosestPointOnLine(source_pos);
				const double dist = (source_pos  - closest_point_on_line).Length();
				if(dist < shortest_dist)
				{
					point = closest_point_on_line;
					shortest_dist = dist;
					segment_index =  static_cast<int>(i);
				}
			}
		}
		else
			return false;
		return true;
	}


	Vec3 Path::GetPointOnPath(Float pathDistance, const std::vector<Vec3> &wps, bool cyclic, int &index)
	{
		Float  totalPathLength  = 0;
		std::vector<Float> lengths;
		for (unsigned int i = 1; i < wps.size(); i++)
		{
			const Float segmentLength = (wps[i-1] - wps[i]).Length();
			lengths.push_back(segmentLength);
			totalPathLength += segmentLength;
		}

		// clip or wrap given path distance according to cyclic flag
		Float remaining = pathDistance;
		if (cyclic)
		{
			remaining = fmod(pathDistance, totalPathLength);
		}
		else
		{
			if (pathDistance < 0)
			{
				index = 0;
				return wps[0];
			}
			if (pathDistance >= totalPathLength)
			{
				if(wps.size() > 1)
				{
					index = static_cast<int>(wps.size() - 1);
					return wps[wps.size() - 1];
				}
			}
		}

		// step through segments, subtracting off segment lengths until
		// locating the segment that contains the original pathDistance.
		// Interpolate along that segment to find 3d point value to return.
		Vec3 result(0,0,0);
		for (unsigned int i = 1; i < wps.size(); i++)
		{
			const Float segmentLength = lengths[i-1];
			if (segmentLength < remaining)
			{
				remaining -= segmentLength;
			}
			else
			{
				const Float ratio = remaining / segmentLength;
				result = wps[i-1] + ((wps[i] - wps[i-1])*ratio);
				index = i-1;
				break;
			}
		}
		return result;
	}


	Float Path::GetPathDistance(const Vec3& point, const std::vector<Vec3> &wps, int &index, Float &distance_to_path)
	{
		Float shortest_dist = std::numeric_limits<Float>::max();
		Float segmentLengthTotal = 0;
		Float pathDistance = 0;

		for (unsigned int i = 1; i < wps.size(); i++)
		{
			const Float segmentLength = (wps[i] - wps[i-1]).Length();
			const Vec3 closest_point_on_line = LineSegment(wps[i-1],wps[i]).ClosestPointOnLine(point);
			const double dist = (point  - closest_point_on_line).Length();
			if(dist < shortest_dist)
			{
				shortest_dist = dist;
				distance_to_path = dist;
				pathDistance = segmentLengthTotal + (wps[i-1] - closest_point_on_line).Length();
				index = i-1;
			}
			segmentLengthTotal += segmentLength;
		}
		// return distance along path of onPath point
		return pathDistance;
	}

	std::vector<Vec3> Path::ClipPath(Float start_distance, Float end_distance, const std::vector<Vec3> &wps)
	{
		Float  totalPathLength  = 0;
		std::vector<Float> lengths;
		for (unsigned int i = 1; i < wps.size(); i++)
		{
			const Float segmentLength = (wps[i-1] - wps[i]).Length();
			lengths.push_back(segmentLength);
			totalPathLength += segmentLength;
		}

		Float start_remaining = start_distance;
		Float end_remaining = end_distance;
		std::vector<Vec3> path;
		int index = 0;
		for (unsigned int i = 1; i < wps.size(); i++)
		{
			const Float segmentLength = lengths[i-1];
			if (segmentLength < start_remaining)
			{
				start_remaining -= segmentLength;
				end_remaining -= segmentLength;
			}
			else
			{
				const Float ratio = start_remaining / segmentLength;
				const Vec3 start_point = wps[i-1] + ((wps[i] - wps[i-1])*ratio);
				path.push_back(start_point);
				index = i;
				break;
			}
		}

		for (unsigned int i = index; i < wps.size(); i++)
		{
			const Float segmentLength = lengths[i-1];

			if (segmentLength < end_remaining)
			{
				end_remaining -= segmentLength;
				//path.push_back(wps[i-1]);
				//if(index == wps.size()-1) // if last
				path.push_back(wps[i]);
			}
			else
			{
				const Float ratio = end_remaining / segmentLength;
				const Vec3 end_wp = wps[i-1] + ((wps[i] - wps[i-1])*ratio);
				path.push_back(end_wp);
				break;
			}
		}
		return path;
	}

	std::vector<Vec3> Path::GenerateOffset(const std::vector<Vec3> &wps, Float start_offset, Float end_offset)
	{
		Float totalPathLength = 0;
		for (unsigned int i = 1; i < wps.size(); i++)
		{
			const Float segmentLength = (wps[i-1] - wps[i]).Length();
			totalPathLength += segmentLength;
		}
		Float dist = 0;
		std::vector<Vec3> offset_path;
		for(size_t i = 0; i < wps.size(); i++)
		{
			Vec3 side;
			const Float inter = dist/totalPathLength;
			if(i < wps.size()-2)
				dist += (wps[i-1] - wps[i]).Length();
			const Float offset = start_offset + inter*(end_offset - start_offset);
			Float width_mult = 1.0;

			if( i== 0)
			{
				side = (wps[1] - wps[0]);
				side.y = 0;
			}
			else if(i < wps.size() - 1)
			{
				Vec3 d1 = (wps[i]-wps[i-1]);
				Vec3 d2 = (wps[i+1]-wps[i]);
				d1.Normalize();
				d2.Normalize();
				side = d1 + d2;
				side.Normalize();
				width_mult = Vec3::Dot(d1,side);
				if(width_mult > 0)
					width_mult = 1.0/width_mult;

			}
			else
			{
				side = wps[i]-wps[i-1];
			}
			side.Normalize();
			side = Vec3::Cross(side,Vec3(0,1,0));
			side.Normalize();
			offset_path.push_back(wps[i] + side*offset*width_mult);
		}
		return offset_path;
	}

	std::vector<Vec3> Path::GenerateOffset(const std::vector<Vec3> &wps, Float offset)
	{
		std::vector<Vec3> offset_path;
		for(size_t i = 0; i < wps.size(); i++)
		{
			Vec3 side; //= wps[i+1] - wps[i];

			Float width_mult = 1.0;

			if( i== 0)
			{
				side = (wps[1] - wps[0]);
				side.y = 0;
			}
			else if(i < wps.size() - 1)
			{
				Vec3 d1 = (wps[i]-wps[i-1]);
				Vec3 d2 = (wps[i+1]-wps[i]);
				d1.Normalize();
				d2.Normalize();
				side = d1 + d2;
				side.Normalize();
				width_mult = Vec3::Dot(d1,side);
				if(width_mult > 0)
					width_mult = 1.0/width_mult;

			}
			else
			{
				side = wps[i]-wps[i-1];
			}
			side.Normalize();
			side = Vec3::Cross(side,Vec3(0,1,0));
			side.Normalize();
			offset_path.push_back(wps[i] + side*offset*width_mult);
		}
		return offset_path;
	}

	std::vector<Vec3> Path::GenerateNormals(const std::vector<Vec3> &wps)
	{
		std::vector<Vec3> normals;
		for(size_t i = 0; i < wps.size(); i++)
		{
			Vec3 side; //= wps[i+1] - wps[i];

			Float width_mult = 1.0;

			if( i== 0)
			{
				side = (wps[1] - wps[0]);
				side.y = 0;
			}
			else if(i < wps.size() - 1)
			{
				Vec3 d1 = (wps[i]-wps[i-1]);
				Vec3 d2 = (wps[i+1]-wps[i]);
				d1.Normalize();
				d2.Normalize();
				side = d1 + d2;
				side.Normalize();
				width_mult = Vec3::Dot(d1,side);
				if(width_mult > 0)
					width_mult = 1.0/width_mult;

			}
			else
			{
				side = wps[i]-wps[i-1];
			}
			side.Normalize();
			side = Vec3::Cross(side,Vec3(0,1,0));
			side.Normalize();
			normals.push_back(side*width_mult);
		}
		return normals;
	}


	Float Path::GetPathLength(const std::vector<Vec3> &wps)
	{
		Float  total_path_length  = 0;
		for (unsigned int i = 1; i < wps.size(); i++)
		{
			total_path_length += (wps[i-1] - wps[i]).Length();
		}
		return total_path_length;
	}
}
