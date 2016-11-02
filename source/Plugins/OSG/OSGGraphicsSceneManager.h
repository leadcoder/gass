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
#include "Sim/GASS.h"
#include "Sim/Interface/GASSIGraphicsSceneManager.h"
#include "Plugins/OSG/OSGCommon.h"
#include "IOSGGraphicsSceneManager.h"
#include "OSGDebugDraw.h"

namespace osgShadow
{
	class ShadowedScene;
}

namespace GASS
{
	class OSGGraphicsSystem;
	typedef GASS_WEAK_PTR<OSGGraphicsSystem>  OSGGraphicsSystemWeakPtr;

	class OSGGraphicsSceneManager : public Reflection<OSGGraphicsSceneManager, BaseSceneManager> , public IGraphicsSceneManager , public IOSGGraphicsSceneManager
	{
	public:
		OSGGraphicsSceneManager();
		virtual ~OSGGraphicsSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void OnInit();		
		virtual void OnShutdown();
		virtual bool GetSerialize() const {return true;}
		virtual void DrawLine(const Vec3 &start_point, const Vec3 &end_point, const ColorRGBA &start_color , const ColorRGBA &end_color);
		osg::ref_ptr<osg::Group> GetOSGRootNode() {return m_RootNode;}
		osg::ref_ptr<osg::Group> GetOSGShadowRootNode()
		{
				if (m_ShadowedScene.valid())
					return m_ShadowedScene;
				return m_RootNode;
		}
	private:
		//Fog
		FogModeBinder GetFogMode() const {return m_FogMode;}
		float GetFogStart() const {return m_FogStart;}
		float GetFogEnd() const {return m_FogEnd;}
		float GetFogDensity() const {return m_FogDensity;}
		Vec3 GetFogColor() const {return m_FogColor;}
		void SetFogMode(FogModeBinder mode) {m_FogMode = mode; UpdateFogSettings();}
		void SetFogStart(float value) {m_FogStart = value; UpdateFogSettings();}
		void SetFogEnd(float value) {m_FogEnd = value; UpdateFogSettings();}
		void SetFogColor(const Vec3 &value) {m_FogColor = value; UpdateFogSettings();}
		void SetFogDensity(float value) {m_FogDensity = value; UpdateFogSettings();}
		void UpdateFogSettings();
		void SetAmbientColor(const Vec3 &value) {m_AmbientColor = value;}
		Vec3 GetAmbientColor() const {return m_AmbientColor;}
		void SystemTick(double delta_time);

		void OnDrawLine(DrawLineRequestPtr message);
	private:	
		//fog
		float m_FogDensity;
		int m_UseFog;
		Vec3 m_FogColor;
		float m_FogEnd;
		float m_FogStart;
		FogModeBinder m_FogMode;
		//light attributes
		Vec3 m_AmbientColor;
		//Shadows
		std::string m_ShadowType;
		std::string m_ShadowCasterMaterial;
		std::string m_ShadowProjType;
		OSGGraphicsSystemWeakPtr m_GFXSystem;
		osg::ref_ptr<osg::Group> m_RootNode;
		osg::ref_ptr<OSGDebugDraw> m_DebugDraw;
		osg::ref_ptr<osg::Fog> m_Fog;
		osg::ref_ptr<osgShadow::ShadowedScene> m_ShadowedScene;
	};
	typedef GASS_SHARED_PTR<OSGGraphicsSceneManager> OSGGraphicsSceneManagerPtr;
	typedef GASS_WEAK_PTR<OSGGraphicsSceneManager> OSGGraphicsSceneManagerWeakPtr;
}