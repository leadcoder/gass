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

	class OSGGraphicsSceneManager : public Reflection<OSGGraphicsSceneManager, BaseSceneManager>, public IGraphicsSceneManager, public IOSGGraphicsSceneManager
	{
	public:
		OSGGraphicsSceneManager(SceneWeakPtr scene);
		~OSGGraphicsSceneManager() override;
		static void RegisterReflection();
		void OnPostConstruction() override;
		void OnSceneCreated() override;
		void OnSceneShutdown() override;
		void OnUpdate(double delta) override;

		bool GetSerialize() const override { return true; }
		void DrawLine(const Vec3& start_point, const Vec3& end_point, const ColorRGBA& start_color, const ColorRGBA& end_color) override;
		osg::ref_ptr<osg::Group> GetOSGRootNode() override { return m_RootNode; }
		osg::ref_ptr<osg::Group> GetOSGShadowRootNode() override
		{
			if (m_ShadowRootNode.valid())
				return m_ShadowRootNode;
			return m_RootNode;
		}
	private:
		//Fog
		FogModeBinder GetFogMode() const { return m_FogMode; }
		float GetFogStart() const { return m_FogStart; }
		float GetFogEnd() const { return m_FogEnd; }
		float GetFogDensity() const { return m_FogDensity; }
		ColorRGB GetFogColor() const { return m_FogColor; }
		void SetFogMode(FogModeBinder mode) { m_FogMode = mode; UpdateFogSettings(); }
		void SetFogStart(float value) { m_FogStart = value; UpdateFogSettings(); }
		void SetFogEnd(float value) { m_FogEnd = value; UpdateFogSettings(); }
		void SetFogColor(const ColorRGB& value) { m_FogColor = value; UpdateFogSettings(); }
		void SetFogDensity(float value) { m_FogDensity = value; UpdateFogSettings(); }
		void UpdateFogSettings();
		void SetAmbientColor(const ColorRGB& value);
		ColorRGB GetAmbientColor() const { return m_AmbientColor; }
		float GetShadowMaxFarDistance() const;
		void SetShadowMaxFarDistance(float value);
		float GetShadowFadeDistanceRatio() const;
		float GetShadowSoftness() const;
		void SetShadowSoftness(float value);
		void SetShadowFadeDistanceRatio(float value);
		float GetShadowMinimumNearFarRatio() const;
		void SetShadowMinimumNearFarRatio(float value);
		bool GetEnableShadows() const;
		void SetEnableShadows(bool value);
	private:
		osg::ref_ptr<osgShadow::ShadowedScene> _CreateShadowNode();

		//fog
		float m_FogDensity;
		int m_UseFog;
		ColorRGB m_FogColor;
		float m_FogEnd;
		float m_FogStart;
		FogModeBinder m_FogMode;
		//light attributes
		ColorRGB m_AmbientColor;

		//Shadows
		float m_ShadowMaxFarDistance;
		float m_ShadowFadeDistanceRatio = 0.2f;
		osg::Uniform* m_ShadowMaxDistanceUniform = nullptr;

		float m_ShadowMinimumNearFarRatio;
		short m_ShadowTextureSize;
		bool m_EnableShadows;
		float m_ShadowSoftness = 0;
		osg::Uniform* m_ShadowSoftnessUniform = nullptr;

		osg::ref_ptr<osgShadow::ShadowedScene> m_ShadowedScene;

		OSGGraphicsSystemWeakPtr m_GFXSystem;
		osg::ref_ptr<osg::Group> m_RootNode;
		osg::ref_ptr<osg::Group> m_ShadowRootNode;
		osg::ref_ptr<OSGDebugDraw> m_DebugDraw;
		osg::ref_ptr<osg::Fog> m_Fog;
		osg::ref_ptr<osg::LightModel> m_LightModel;
	};
	typedef GASS_SHARED_PTR<OSGGraphicsSceneManager> OSGGraphicsSceneManagerPtr;
	typedef GASS_WEAK_PTR<OSGGraphicsSceneManager> OSGGraphicsSceneManagerWeakPtr;
}