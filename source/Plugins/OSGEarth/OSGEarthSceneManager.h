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

namespace GASS
{
	class OSGEarthSceneManager  : public Reflection<OSGEarthSceneManager, BaseSceneManager> 
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
		void FromLatLongToMap(double latitude, double longitude, Vec3 &pos, Quaternion &rot);
	protected:
		void OnLoadSceneObject(PreSceneObjectInitializedEventPtr message);
		void SetEarthFile(const std::string &earth_file);
		std::string GetEarthFile() const {return m_EarthFile;}
		void Load(const std::string earth_file);
		
		ADD_PROPERTY(bool,UseSky)
		ADD_PROPERTY(bool,ShowSkyControl)
		ADD_PROPERTY(bool,UseOcean)
		ADD_PROPERTY(bool,ShowOceanControl)
		ADD_PROPERTY(bool,DisableGLSL)
		

		osg::ref_ptr<osgEarth::MapNode> m_MapNode;
		std::string m_EarthFile;
		bool m_Initlized;
		osg::ref_ptr<osgEarth::Util::EarthManipulator> m_EarthManipulator;
		bool m_AutoAdd;
		double m_OffsetEast;
		double m_OffsetNorth;
		
		
	};
	typedef GASS_SHARED_PTR<OSGEarthSceneManager> OSGEarthSceneManagerPtr;
	typedef GASS_WEAK_PTR<OSGEarthSceneManager> OSGEarthSceneManagerWeakPtr;
	
	
}

#endif

