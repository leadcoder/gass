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
#include "Sim/Interface/GASSIMapComponent.h"

namespace GASS
{
	class OSGEarthSceneManager;
	class OETerrainCallbackProxy;

	class OSGEarthMapComponent : public Reflection<OSGEarthMapComponent, BaseSceneComponent>, public IGeometryComponent, public IMapComponent
	{
	public:
		OSGEarthMapComponent();
		~OSGEarthMapComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		void SceneManagerTick(double delta_time) override;
		AABox GetBoundingBox() const override { return AABox(); }
		Sphere GetBoundingSphere() const override { return Sphere(); };
		GeometryFlags GetGeometryFlags() const override { return static_cast<GeometryFlags>(static_cast<int>(GEOMETRY_FLAG_GROUND) | static_cast<int>(GEOMETRY_FLAG_STATIC_OBJECT)); }
		void SetGeometryFlags(GeometryFlags /*flags*/) override { };
		bool GetCollision() const override { return true; }
		void SetCollision(bool /*value*/) override {  }
		osg::ref_ptr<osgEarth::MapNode> GetMap() { return m_MapNode; }
	
		//IMapComponent
		std::vector<std::string> GetViewpointNames() const;
		void SetViewpointByName(const std::string& viewpoint_name);

		std::vector<std::string> GetMapLayerNames() const;
		const MapLayers& GetMapLayers() const;

		double GetTimeOfDay() const { return m_Hour; }
		void SetTimeOfDay(double time);
		float GetMinimumAmbient() const;
		void SetMinimumAmbient(float value);
		void SetSkyLighting(bool value);
		bool GetSkyLighting() const;
		void SetEarthFile(const ResourceHandle &earth_file);
		ResourceHandle GetEarthFile() const { return m_EarthFile; }
		//IMapComponent end

		void onTileAdded(
			const osgEarth::TileKey&          key,
			osg::Node*              graph,
			osgEarth::TerrainCallbackContext& context);
		void onMapModelChanged(const osgEarth::MapModelChange& change);

	protected:
		void Shutdown();
		void _SetupNodeMasks();

		std::string GetViewpointName() const {return std::string(""); }
		void _UpdateMapLayers();

		std::vector<std::string> GetVisibleMapLayers() const;
		void SetVisibleMapLayers(const std::vector<std::string> &layers);

		bool m_Initlized;
		osg::ref_ptr<osgEarth::MapNode> m_MapNode;
		osg::ref_ptr<osgEarth::PhongLightingEffect> m_Lighting;
		osg::ref_ptr<osgEarth::Util::FogEffect> m_FogEffect;
		osg::ref_ptr<osgEarth::Util::AutoClipPlaneCullCallback> m_AutoClipCB;
		osg::ref_ptr<osg::Node> m_TopNode;
		ResourceHandle m_EarthFile;
		std::vector<osgEarth::Viewpoint> m_Viewpoints;
		osgEarth::Util::SkyNode* m_SkyNode;
		double m_Hour;
		bool m_UseAutoClipPlane;

		MapLayers m_MapLayers;
		OSGEarthSceneManager* m_OESceneManager;
		osg::ref_ptr<OETerrainCallbackProxy> m_TerrainCallbackProxy;
		bool m_TerrainChangedLastFrame;
	};
	typedef GASS_SHARED_PTR<OSGEarthMapComponent> OSGEarthMapComponentPtr;
}

