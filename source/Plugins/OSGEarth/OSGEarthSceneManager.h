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

#ifndef OSG_EARTH_SCENE_MANAGER
#define OSG_EARTH_SCENE_MANAGER

#include "Sim/GASS.h"
#include "Sim/Interface/GASSITerrainSceneManager.h"
#include "Sim/Interface/GASSIWGS84SceneManager.h"

namespace GASS
{
	class OSGEarthSceneManager : public Reflection<OSGEarthSceneManager, BaseSceneManager>, public IWGS84SceneManager, public ITerrainSceneManager
	{
	public:
		OSGEarthSceneManager();
		~OSGEarthSceneManager() override;
		static void RegisterReflection();
		void OnCreate() override;
		void OnInit() override;
		void OnShutdown() override;
		bool GetSerialize() const override {return true;}
		osg::ref_ptr<osgEarth::Util::EarthManipulator> GetManipulator() const{return m_EarthManipulator;}
		osgEarth::Util::Controls::Container* GetGUI() const { return m_GUI; }
		void FromLatLongToMap(double latitude, double longitude, Vec3 &pos, Quaternion &rot) const;
		void SetMapNode(osgEarth::MapNode* map_node);
		osgEarth::MapNode* GetMapNode() const { return m_MapNode;}

		//ITerrainSceneManager
		bool GetTerrainHeight(const Vec3 &location, double &height) const override;
		bool GetHeightAboveTerrain(const Vec3 &location, double &height) const override;
		bool GetUpVector(const Vec3 &location, Vec3 &up_vec) const override;
		bool GetOrientation(const Vec3 &location, Quaternion &rot) const override;

		//IWGS84SceneManager
		bool WGS84ToScene(const GeoLocation &geo_location, Vec3 &scene_location) const override;
		bool SceneToWGS84(const Vec3 &scene_location, GeoLocation &geo_location) const override;

		//helpers
		bool GetHeightAboveTerrain(const GeoLocation &location, double &height) const;
		bool GetTerrainHeight(const GeoLocation &location, double &height) const;

	protected:
		void OnLoadSceneObject(PreSceneObjectInitializedEventPtr message);
		ADD_PROPERTY(bool,DisableGLSL)

		osgEarth::MapNode* m_MapNode;
		bool m_Initlized;
		osg::ref_ptr<osgEarth::Util::EarthManipulator> m_EarthManipulator;
		bool m_AutoAdd;
		osgEarth::Util::Controls::Container* m_GUI;
		std::string m_DummyProjection;
	};
	typedef GASS_SHARED_PTR<OSGEarthSceneManager> OSGEarthSceneManagerPtr;
	typedef GASS_WEAK_PTR<OSGEarthSceneManager> OSGEarthSceneManagerWeakPtr;
	
	
}

#endif

