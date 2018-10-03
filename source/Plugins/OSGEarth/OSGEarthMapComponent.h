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
#include <osgEarth/PhongLightingEffect>
#include <osgEarthUtil/Fog>
#include <osgEarthUtil/Sky>

namespace GASS
{
	class OSGEarthMapComponent : public Reflection<OSGEarthMapComponent, BaseSceneComponent>, public IGeometryComponent
	{
	public:
		OSGEarthMapComponent();
		virtual ~OSGEarthMapComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		virtual void SceneManagerTick(double delta_time);
		virtual AABox GetBoundingBox() const { return AABox(); }
		virtual Sphere GetBoundingSphere() const { return Sphere(); };
		virtual GeometryFlags GetGeometryFlags() const { return static_cast<GeometryFlags>(static_cast<int>(GEOMETRY_FLAG_GROUND) | static_cast<int>(GEOMETRY_FLAG_STATIC_OBJECT)); }
		virtual void SetGeometryFlags(GeometryFlags /*flags*/) { };
		virtual bool GetCollision() const { return true; }
		virtual void SetCollision(bool /*value*/) {  }
		osg::ref_ptr<osgEarth::MapNode> GetMap() {return m_MapNode;}
		std::vector<std::string> GetViewpointNames() const;
		std::vector<std::string> GetLayerNames() const;
	protected:
		bool _GetLLHAtLocation(const GASS::Vec3 &location, double lat, double lon, double &height) const;
		void Shutdown();
		void SetEarthFile(const ResourceHandle &earth_file);
		ResourceHandle GetEarthFile() const { return m_EarthFile; }
		std::string GetViewpoint() const;
		void SetViewpoint(const std::string &name);

		double GetTime() const { return m_Time; }
		void SetTime(double time);

		
		std::vector<std::string> GetMapLayers() const;
		void SetMapLayers(const std::vector<std::string> &layers);

		bool m_Initlized;
		osg::ref_ptr<osgEarth::MapNode> m_MapNode;
		osg::ref_ptr<osgEarth::PhongLightingEffect> m_Lighting;
		osg::ref_ptr<osgEarth::Util::FogEffect> m_FogEffect;
		osg::ref_ptr<osg::Node> m_TopNode;
		ResourceHandle m_EarthFile;
		std::vector<osgEarth::Viewpoint> m_Viewpoints;
		osgEarth::Util::SkyNode* m_SkyNode;
		double m_Time;
	};
	typedef GASS_SHARED_PTR<OSGEarthMapComponent> OSGEarthMapComponentPtr;
}

