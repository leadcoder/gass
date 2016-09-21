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
#include "Plugins/OSG/IOSGCameraManipulator.h"
#include <osgEarthUtil/EarthManipulator>

namespace GASS
{
	class OSGEarthMapComponent : public Reflection<OSGEarthMapComponent,BaseSceneComponent>, public IGeometryComponent
	{
	public:
		OSGEarthMapComponent();
		virtual ~OSGEarthMapComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void SceneManagerTick(double delta_time);

		virtual AABox GetBoundingBox() const { return AABox(); }
		virtual Sphere GetBoundingSphere() const { return Sphere(); };
		virtual GeometryFlags GetGeometryFlags() const { return static_cast<GeometryFlags>(static_cast<int>(GEOMETRY_FLAG_GROUND) | static_cast<int>(GEOMETRY_FLAG_STATIC_OBJECT)); }
		virtual void SetGeometryFlags(GeometryFlags flags) { };
		virtual bool GetCollision() const { return true; }
		virtual void SetCollision(bool value) {  }
	protected:
		void SetEarthFile(const std::string &earth_file);
		std::string GetEarthFile() const { return m_EarthFile; }
		bool m_Initlized;
		osg::ref_ptr<osgEarth::MapNode> m_MapNode;
		std::string m_EarthFile;
	};
}

