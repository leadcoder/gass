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
#include "Plugins/Ogre/GASSOgreCommon.h"
#include "Sim/Interface/GASSIGraphicsSceneManager.h"
#include "GASSIOgreSceneManagerProxy.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Core/Utils/GASSColorRGB.h"
#include "Core/Utils/GASSColorRGBA.h"
#include "Core/Utils/GASSEnumBinder.h"
#include "GASSOgreMaterial.h"


namespace Ogre
{
	class LiSPSMShadowCameraSetup;
}

class DebugDrawer;

namespace GASS
{
	enum ShadowMode
	{
		SHADOWS_DISABLED,
		TEXTURE_SHADOWS_ADDITIVE_INTEGRATED,
		TEXTURE_SHADOWS_ADDITIVE,
		TEXTURE_SHADOWS_MODULATIVE,
		STENCIL_SHADOWS_ADDITIVE,
		STENCIL_SHADOWS_MODULATIVE
	};

	enum TextureShadowProjection
	{
		LISPSM,
		UNIFORM,
		UNIFORM_FOCUSED
	};

	START_ENUM_BINDER(ShadowMode,ShadowModeBinder)
		BIND(SHADOWS_DISABLED)
		BIND(TEXTURE_SHADOWS_ADDITIVE_INTEGRATED)
		BIND(TEXTURE_SHADOWS_ADDITIVE)
		BIND(TEXTURE_SHADOWS_MODULATIVE)
		BIND(STENCIL_SHADOWS_ADDITIVE)
		BIND(STENCIL_SHADOWS_MODULATIVE)
	END_ENUM_BINDER(ShadowMode,ShadowModeBinder)

	START_ENUM_BINDER(TextureShadowProjection,TextureShadowProjectionBinder)
		BIND(LISPSM)
		BIND(UNIFORM)
		BIND(UNIFORM_FOCUSED)
	END_ENUM_BINDER(TextureShadowProjection,TextureShadowProjectionBinder)

	/*START_ENUM_BINDER(Ogre::FogMode,FogModeBinder)
		Bind("FOG_LINEAR",Ogre::FOG_LINEAR);
		Bind("FOG_EXP",Ogre::FOG_EXP);
		Bind("FOG_EXP2",Ogre::FOG_EXP2);
		Bind("FOG_NONE",Ogre::FOG_NONE);
	END_ENUM_BINDER(Ogre::FogMode,FogModeBinder)
	*/
	class Component;
	class OgreGraphicsSystem;
	class OgreGraphicsSceneManager : public Reflection<OgreGraphicsSceneManager, BaseSceneManager>, public IOgreSceneManagerProxy, public IGraphicsSceneManager, Ogre::FrameListener
	{
	private:
		void UpdateFogSettings();
	public:
		OgreGraphicsSceneManager(SceneWeakPtr scene);
		~OgreGraphicsSceneManager() override;
		static void RegisterReflection();
		void OnCreate() override;
		void OnInit() override;
		void OnShutdown() override;
		bool GetSerialize() const override {return true;}
		//Fog
		FogModeBinder GetFogMode() const {return m_FogMode;}
		float GetFogStart() const {return m_FogStart;}
		float GetFogEnd() const {return m_FogEnd;}
		float GetFogDensity() const {return m_FogDensity;}
		ColorRGB GetFogColor() const {return m_FogColor;}

		void SetFogMode(const FogModeBinder &mode) {m_FogMode=mode; UpdateFogSettings();}
		void SetFogStart(float value) {m_FogStart = value; UpdateFogSettings();}
		void SetFogEnd(float value) {m_FogEnd = value; UpdateFogSettings();}
		void SetFogColor(const ColorRGB value) {m_FogColor = value; UpdateFogSettings();}
		void SetFogDensity(float value) {m_FogDensity = value; UpdateFogSettings();}

		//IGraphicsSceneManager
		void DrawLine(const Vec3 &start_point, const Vec3 &end_point, const ColorRGBA &start_color , const ColorRGBA &end_color) override;

		//IOgreSceneManagerProxy
		Ogre::SceneManager* GetOgreSceneManager() const override {return m_SceneMgr;}
	protected:
		//Ogre::SceneManager::Listener
		bool frameStarted (const Ogre::FrameEvent &evt) override;
		bool frameEnded (const Ogre::FrameEvent &evt) override;
		void OnWeatherRequest(WeatherRequestPtr message);
	private:
		void UpdateLightSettings();
		void UpdateSkySettings();
		void UpdateShadowSettings();

		//Keep private for now,
		void SetAmbientColor(const ColorRGB value) {m_AmbientColor = value; UpdateLightSettings();}
		ColorRGB GetAmbientColor() const {return m_AmbientColor;}

		void SetSceneManagerType(const std::string &name) {m_SceneManagerType = name;}
		std::string GetSceneManagerType() const {return m_SceneManagerType;}

		void SetSkyboxMaterial(const OgreMaterial &name) {m_SkyboxMaterial = name; UpdateSkySettings();}
		OgreMaterial GetSkyboxMaterial() const {return m_SkyboxMaterial;}

		void SetUseSkybox(bool value) {m_UseSkybox = value; UpdateSkySettings();}
		bool GetUseSkybox() const {return m_UseSkybox;}

		//shadows
		ShadowModeBinder GetShadowMode() const {return m_ShadowMode;}
		OgreMaterial GetShadowCasterMaterial() const {return m_ShadowCasterMaterial;}
		TextureShadowProjectionBinder GetTextureShadowProjection() const {return m_TextureShadowProjection;}
		int GetTextureShadowSize() const {return m_TextureShadowSize;}
		int GetNumShadowTextures() const {return m_NumShadowTextures;}
		float GetOptimalAdjustFactor() const {return m_OptimalAdjustFactor;}
		float GetFarShadowDistance() const {return m_FarShadowDistance;}
		float GetShadowDirectionalLightExtrusionDistance() const {return m_ShadowDirectionalLightExtrusionDistance;}
		bool GetUseAggressiveFocusRegion() const {return m_UseAggressiveFocusRegion;}
		bool GetSelfShadowing() const {return m_SelfShadowing;}

		void SetSelfShadowing(bool value) {m_SelfShadowing = value;UpdateShadowSettings();}
		void SetFarShadowDistance(float value) {m_FarShadowDistance = value;UpdateShadowSettings();}
		void SetOptimalAdjustFactor(float value) {m_OptimalAdjustFactor = value;UpdateShadowSettings();}
		void SetNumShadowTextures(int size) {m_NumShadowTextures = size;UpdateShadowSettings();}
		void SetTextureShadowSize(int size) {m_TextureShadowSize = size;UpdateShadowSettings();}
		void SetTextureShadowProjection(const TextureShadowProjectionBinder &value) {m_TextureShadowProjection = value;UpdateShadowSettings();}
		void SetShadowMode(const ShadowModeBinder &value) {m_ShadowMode = value;UpdateShadowSettings();}
		void SetShadowCasterMaterial(const OgreMaterial &value) {m_ShadowCasterMaterial = value; UpdateShadowSettings();}
		void SetUseAggressiveFocusRegion(bool value) {m_UseAggressiveFocusRegion = value ;UpdateShadowSettings();}
		void SetShadowDirectionalLightExtrusionDistance(float value) {m_ShadowDirectionalLightExtrusionDistance = value;UpdateShadowSettings();}
		
		void OnDrawCircle(DrawCircleRequestPtr message);
		void OnExportMesh(ExportMeshRequestPtr message);
	private:
		//fog
		float m_FogDensity;
		int m_UseFog;
		ColorRGB m_FogColor;
		float m_FogEnd;
		float m_FogStart;
		FogModeBinder m_FogMode;

		//light attributes
		ColorRGB m_AmbientColor;

		//Type of scene manager
		std::string m_SceneManagerType;

		//Sky
		bool m_UseSkybox;
		OgreMaterial m_SkyboxMaterial;

		//Shadows
		ShadowModeBinder m_ShadowMode;
		TextureShadowProjectionBinder m_TextureShadowProjection;
		OgreMaterial m_ShadowCasterMaterial;
		int m_TextureShadowSize;
		int m_NumShadowTextures;
		bool m_SelfShadowing;
		bool m_UseAggressiveFocusRegion;
		float m_OptimalAdjustFactor;
		float m_FarShadowDistance;
		float m_ShadowDirectionalLightExtrusionDistance;
		Ogre::SceneManager* m_SceneMgr;
		GASS_WEAK_PTR<OgreGraphicsSystem> m_GFXSystem;
		DebugDrawer* m_DebugDrawer;
	};
	typedef GASS_SHARED_PTR<OgreGraphicsSceneManager> OgreGraphicsSceneManagerPtr;
	typedef GASS_WEAK_PTR<OgreGraphicsSceneManager> OgreGraphicsSceneManagerWeakPtr;
}