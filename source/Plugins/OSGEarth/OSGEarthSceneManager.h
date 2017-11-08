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
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/Controls>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarth/PhongLightingEffect>
#include <osgEarthUtil/Fog>

namespace GASS
{
	class OSGEarthSceneManager : public Reflection<OSGEarthSceneManager, BaseSceneManager>, public IProjectionSceneManager
	{
	public:
		OSGEarthSceneManager();
		virtual ~OSGEarthSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void OnInit();
		virtual void OnShutdown();
		virtual bool GetSerialize() const {return true;}
		osg::ref_ptr<osgEarth::Util::EarthManipulator> GetManipulator() const{return m_EarthManipulator;}
		osgEarth::Util::Controls::Container* GetGUI() const { return m_GUI; }
		void FromLatLongToMap(double latitude, double longitude, Vec3 &pos, Quaternion &rot);
		void FromLatLongToMap(double latitude, double longitude, double height, Vec3 &pos, bool relative_height);
		void FromMapToLatLong(const Vec3 &pos, double &latitude, double &longitude, double &height, double *altitude);
		void SetMapNode(osgEarth::MapNode* map_node) { m_MapNode = map_node; }
		osgEarth::MapNode* GetMapNode() const { return m_MapNode;}


		//IProjectionSceneManager interface
		virtual void WGS84ToScene(double lat, double lon, double &x, double &y);
		virtual void SceneToWGS84(double x, double y, double &lat, double &lon);
		virtual std::string GetProjection() const;
		virtual void SetProjection(const std::string &projection);
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

