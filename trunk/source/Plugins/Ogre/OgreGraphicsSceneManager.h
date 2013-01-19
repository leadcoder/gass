/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
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

#include "Sim/Interface/GASSIGraphicsSystem.h"
#include "Sim/Interface/GASSIGraphicsSceneManager.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Core/MessageSystem/GASSMessageType.h"
#include "IOgreSceneManagerProxy.h"
#include <string>
#include <OgreSceneManager.h>

namespace Ogre
{
	
	class LiSPSMShadowCameraSetup;
	
}
class DebugDrawer;

namespace GASS
{
	class IComponent;
	class OgreGraphicsSystem;
	class GASSPluginExport OgreGraphicsSceneManager : public Reflection<OgreGraphicsSceneManager, BaseSceneManager>, public IOgreSceneManagerProxy, public IGraphicsSceneManager, Ogre::FrameListener
	{
	private:
		void UpdateFogSettings();
	public:
		OgreGraphicsSceneManager();
		virtual ~OgreGraphicsSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void OnInit();
		virtual void OnShutdown();
		virtual bool GetSerialize() const {return true;}
		//Fog
		std::string GetFogMode() const {return m_FogMode;}
		float GetFogStart() const {return m_FogStart;}
		float GetFogEnd() const {return m_FogEnd;}
		float GetFogDensity() const {return m_FogDensity;}
		Vec3 GetFogColor() const {return m_FogColor;}

		void SetFogMode(const std::string &mode) {m_FogMode=mode; UpdateFogSettings();}
		void SetFogStart(float value) {m_FogStart = value; UpdateFogSettings();}
		void SetFogEnd(float value) {m_FogEnd = value; UpdateFogSettings();}
		void SetFogColor(const Vec3 value) {m_FogColor = value; UpdateFogSettings();}
		void SetFogDensity(float value) {m_FogDensity = value; UpdateFogSettings();}

		//IGraphicsSceneManager
		void DrawLine(const Vec3 &start, const Vec3 &end, const Vec4 &color);
		
		//IOgreSceneManagerProxy
		Ogre::SceneManager* GetOgreSceneManager() const {return m_SceneMgr;}
	protected:
		//Ogre::SceneManager::Listener
		virtual bool frameStarted (const Ogre::FrameEvent &evt);
		virtual bool frameEnded (const Ogre::FrameEvent &evt);
		void OnWeatherRequest(WeatherRequestPtr message);
	private:
		void UpdateLightSettings();
		void UpdateSkySettings();
		void UpdateShadowSettings();

		
		

		//Keep private for now, 
		void SetAmbientColor(const Vec3 value) {m_AmbientColor = value; UpdateLightSettings();}
		Vec3 GetAmbientColor() const {return m_AmbientColor;}

		void SetSceneManagerType(const std::string &name) {m_SceneManagerType = name;}
		std::string GetSceneManagerType() const {return m_SceneManagerType;}

		void SetSkyboxMaterial(const std::string &name) {m_SkyboxMaterial = name; UpdateSkySettings();}
		std::string GetSkyboxMaterial() const {return m_SkyboxMaterial;}

		//shadows
		std::string GetShadowType() const {return m_ShadowType;}
		std::string GetShadowCasterMaterial() const {return m_ShadowCasterMaterial;}
		std::string GetShadowProjType() const {return m_ShadowProjType;}
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
		void SetShadowProjType(const std::string &name) {m_ShadowProjType = name;UpdateShadowSettings();}
		void SetShadowType(const std::string &name) {m_ShadowType = name;UpdateShadowSettings();}
		void SetShadowCasterMaterial(const std::string &name) {m_ShadowCasterMaterial = name;UpdateShadowSettings();}
		void SetUseAggressiveFocusRegion(bool value) {m_UseAggressiveFocusRegion = value ;UpdateShadowSettings();}
		void SetShadowDirectionalLightExtrusionDistance(float value) {m_ShadowDirectionalLightExtrusionDistance = value;UpdateShadowSettings();}
	private:	
		//fog
		float m_FogDensity;
		int m_UseFog;
		Vec3 m_FogColor;
		float m_FogEnd;
		float m_FogStart;
		std::string m_FogMode;

		//light attributes
		Vec3 m_AmbientColor;
		
		//Type of scene mangaer
		std::string m_SceneManagerType;

		//Sky
		std::string m_SkyboxMaterial;

		//Shadows
		std::string m_ShadowType;
		std::string m_ShadowCasterMaterial;
		std::string m_ShadowProjType;
		int m_TextureShadowSize;
		int m_NumShadowTextures;
		bool m_SelfShadowing;
		bool m_UseAggressiveFocusRegion;
		float m_OptimalAdjustFactor;
		float m_FarShadowDistance;
		float m_ShadowDirectionalLightExtrusionDistance;
		Ogre::LiSPSMShadowCameraSetup* m_LiSPSMSetup;
		Ogre::SceneManager* m_SceneMgr;
		WPTR<OgreGraphicsSystem> m_GFXSystem;
		std::vector<IComponent*> m_GFXComponents;
		DebugDrawer* m_DebugDrawer;
	};
	typedef SPTR<OgreGraphicsSceneManager> OgreGraphicsSceneManagerPtr;
	typedef WPTR<OgreGraphicsSceneManager> OgreGraphicsSceneManagerWeakPtr;
}
