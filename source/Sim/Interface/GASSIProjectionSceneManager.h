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

#include "Sim/GASSCommon.h"
#include <string>

namespace GASS
{
	/**
		Interface projection scene managers should be derived from.
		A projection scene manager support transformations from local scene space to 
		different coordinate systems
	*/
	class IProjectionSceneManager
	{
	public:
		virtual ~IProjectionSceneManager(){}
		virtual void WGS84ToScene(double lat,double lon, double &x,double &y) = 0;
		virtual void SceneToWGS84(double x,double y, double &lat,double &lon) = 0;
		virtual std::string GetProjection() const = 0;
		virtual void SetProjection(const std::string &projection) = 0;
	};
	typedef GASS_SHARED_PTR<IProjectionSceneManager> ProjectionSceneManagerPtr;

	class GeoLocation
	{
	public:
		GeoLocation(double lon = 0, double lat = 0, double height = 0, bool hat = false) : Longitude(lon),
			Latitude(lat),
			Height(height),
			HeightAboveTerrain(hat)
		{}
		virtual ~GeoLocation() {};
		double Latitude;
		double Longitude;
		double Height;
		bool HeightAboveTerrain;
	};


	class ITerrain
	{
	public:
		virtual ~ITerrain() {}
		virtual bool GetTerrainHeight(const Vec3 &location, double &height) const = 0;
		virtual bool GetHeightAboveTerrain(const Vec3 &location, double &height) const = 0;
		virtual bool GetUpVector(const Vec3 &location, GASS::Vec3 &up_vec) const = 0;
	};
	typedef GASS_SHARED_PTR<ITerrain> TerrainPtr;


	class IWGS84Terrain : public ITerrain
	{
	public:
		virtual ~IWGS84Terrain() {}
		virtual bool WGS84ToScene(const GeoLocation &geo_location, Vec3 &scene_location) const = 0;
		virtual bool SceneToWGS84(const Vec3 &scene_location, GeoLocation &geo_location) const = 0;
		virtual bool GetTerrainHeight(const GeoLocation &location, double &height) const = 0;
		virtual bool GetHeightAboveTerrain(const GeoLocation &location, double &height) const = 0;
	};
	typedef GASS_SHARED_PTR<IWGS84Terrain> WGS84TerrainPtr;
}
