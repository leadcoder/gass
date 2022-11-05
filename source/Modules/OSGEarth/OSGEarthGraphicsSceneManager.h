#pragma once
#include "Sim/GASS.h"
#include "OSGEarthCommonIncludes.h"
#include "Modules/Graphics/OSGGraphicsSceneManager.h"
#include "Modules/Graphics/OSGCommon.h"
#include "Modules/Graphics/OSGDebugDraw.h"
#include "Sim/Interface/GASSIEarthSceneManager.h"
#include "Sim/Interface/GASSITerrainSceneManager.h"
#include "Sim/Interface/GASSIWGS84SceneManager.h"

namespace osgShadow
{
	class ShadowedScene;
}

namespace GASS
{
	class OETerrainCallbackProxy;

	class OSGEarthGraphicsSceneManager : public Reflection<OSGEarthGraphicsSceneManager, OSGGraphicsSceneManager>, public IWGS84SceneManager, public ITerrainSceneManager, public IEarthSceneManager
	{
	public:
		OSGEarthGraphicsSceneManager(SceneWeakPtr scene);
		~OSGEarthGraphicsSceneManager() override;
		static void RegisterReflection();
		void OnPostConstruction() override;
		void OnSceneCreated() override;
		void OnSceneShutdown() override;
		void OnUpdate(double delta) override;
		bool GetSerialize() const override { return true; }
		void PostProcess(osg::Node* node) override;
		void DrawLine(const Vec3& start_point, const Vec3& end_point, const ColorRGBA& start_color, const ColorRGBA& end_color) override;
		osg::ref_ptr<osg::Group> GetOSGRootNode() override
		{ 
			return m_RootNode;
		}

		osg::Group* GetObjectGroup() override 
		{
			return m_ObjectRoot;
		}

		osgEarth::MapNode* GetMap() const { return m_MapNode; }

		osg::ref_ptr<osgEarth::Util::EarthManipulator> GetManipulator() const { return m_EarthManipulator; }

		void OnTileAdded(
			const osgEarth::TileKey& key,
			osg::Node* graph,
			osgEarth::TerrainCallbackContext& context);
		void OnMapModelChanged(const osgEarth::MapModelChange& change);
		

		//ITerrainSceneManager
		bool GetTerrainHeight(const Vec3& location, double& height, GeometryFlags flags) const override;
		bool GetHeightAboveTerrain(const Vec3& location, double& height, GeometryFlags flags) const override;
		bool GetUpVector(const Vec3& location, Vec3& up_vec) const override;
		bool GetOrientation(const Vec3& location, Quaternion& rot) const override;

		//IWGS84SceneManager
		bool WGS84ToScene(const GeoLocation& geo_location, Vec3& scene_location) const override;
		bool SceneToWGS84(const Vec3& scene_location, GeoLocation& geo_location) const override;

		//helpers
		bool GetHeightAboveTerrain(const GeoLocation& location, double& height, GeometryFlags flags) const;
		bool GetTerrainHeight(const GeoLocation& location, double& height, GeometryFlags flags) const;

		//IEarthSceneManager
		std::vector<std::string> GetViewpointNames() const override;
		void SetViewpointByName(const std::string& viewpoint_name) override;
		const MapLayers& GetMapLayers() const override;
		double GetTimeOfDay() const override { return m_Hour; }
		void SetTimeOfDay(double time) override;
		float GetMinimumAmbient() const override;
		void SetMinimumAmbient(float value) override;
		void SetSkyLighting(bool value) override;
		bool GetSkyLighting() const override;
		void SetShadowEnabled(bool value) override;
		bool GetShadowEnabled() const override;

		void SetEarthFile(const ResourceHandle& earth_file) override;
		ResourceHandle GetEarthFile() const override { return m_EarthFile; }

	private:
		bool GetSceneHeight(const GeoLocation& location, double& height, GeometryFlags flags) const;

		void SetSkyHazeCutoff(float value);
		float GetSkyHazeCutoff() const { return m_SkyHazeCutoff; }
		void SetSkyHazeStrength(float value);
		float GetSkyHazeStrength() const { return m_SkyHazeStrength; }
		float GetSkyExposure() const;
		void SetSkyExposure(float value);
		float GetSkyContrast() const;
		void SetSkyContrast(float value);
		float GetSkyAmbientBoost() const;
		void SetSkyAmbientBoost(float value);
		void SetShadowBlur(float value);
		float GetShadowBlur() const { return m_ShadowBlur; }
		void SetShadowColor(float value);
		float GetShadowColor() const { return m_ShadowColor; }
		void SetShadowRanges(std::vector<float> value);
		std::vector<float> GetShadowRanges() const { return m_ShadowRanges; }
		std::vector<std::string> GetMapLayerNames() const;
		void Shutdown();
		void UpdateMapLayers();
		void SetupNodeMasks();
		void OnElevationChanged();
		std::vector<std::string> GetVisibleMapLayers() const;
		void SetVisibleMapLayers(const std::vector<std::string>& layers);
		void LoadXML(tinyxml2::XMLElement* elem) override;
	private:
		osg::ref_ptr<osg::Group> m_RootNode;
		osg::ref_ptr <osg::Group> m_ObjectRoot;
		bool m_MapIsRoot = false;
		ResourceHandle m_EarthFile;
		bool m_Initlized{ false };
		osg::ref_ptr<osgEarth::MapNode> m_MapNode;
		osg::ref_ptr<osgEarth::PhongLightingEffect> m_Lighting;
#ifdef HAS_FOG
		osg::ref_ptr<osgEarth::Util::FogEffect> m_FogEffect;
#endif
		osg::ref_ptr<osgEarth::Util::AutoClipPlaneCullCallback> m_AutoClipCB;
		osg::ref_ptr<osg::Node> m_TopNode;
		std::vector<osgEarth::Viewpoint> m_Viewpoints;
		osgEarth::Util::SkyNode* m_SkyNode{ nullptr };
		bool m_UseAutoClipPlane{ true };
		MapLayers m_MapLayers;
		osg::ref_ptr<OETerrainCallbackProxy> m_TerrainCallbackProxy;
		bool m_TerrainChangedLastFrame{ false };
		bool m_AddSky = false;
		double m_Hour{ 10 };
		float m_SkyExposure = 10.0f;
		float m_SkyContrast = 2.0f;
		float m_SkyAmbientBoost = 5;
		float m_SkyHazeStrength = 50;
		float m_SkyHazeCutoff = 0.5;
		bool m_ShadowEnabled = false;
		std::vector<float> m_ShadowRanges;
		float m_ShadowBlur = 0.001f;
		float m_ShadowColor = 0;
		osgEarth::ShadowCaster* m_ShadowCaster = nullptr;

		osgEarth::ElevationPool::WorkingSet* m_WorkingSet = nullptr;
		osgEarth::Util::Controls::Container* m_GUI;
		osgEarth::SpatialReference* m_WGS84;
		osgEarth::Viewpoint m_OldVP;
		ICollisionSceneManager* m_CollisionSceneManager;
		osg::ref_ptr<osgEarth::Util::EarthManipulator> m_EarthManipulator;
	};
	using OSGEarthGraphicsSceneManagerPtr = std::shared_ptr<OSGEarthGraphicsSceneManager>;
	using OSGEarthGraphicsSceneManagerWeakPtr = std::weak_ptr<OSGEarthGraphicsSceneManager>;
}