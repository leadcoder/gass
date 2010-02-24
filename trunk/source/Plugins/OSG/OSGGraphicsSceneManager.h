/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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
#   pragma warning (disable : 4541)
#include "Sim/Systems/Graphics/IGraphicsSystem.h"
#include "Sim/Scenario/Scene/BaseSceneManager.h"
#include "Core/MessageSystem/MessageType.h"
#include "Sim/Scenario/Scene/ScenarioSceneMessages.h"
#include <osg/Node>
#include <osg/PositionAttitudeTransform>
#include <string>


namespace GASS
{
	class OSGGraphicsSystem;
	typedef boost::weak_ptr<OSGGraphicsSystem>  OSGGraphicsSystemWeakPtr;

	class OSGGraphicsSceneManager : public Reflection<OSGGraphicsSceneManager, BaseSceneManager>
	{
	public:
		OSGGraphicsSceneManager();
		virtual ~OSGGraphicsSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		osg::ref_ptr<osg::PositionAttitudeTransform> GetOSGRootNode(){return m_RootNode;}
		osg::ref_ptr<osg::PositionAttitudeTransform> GetSceneTransformatation() {return m_SceneTransform;}
	protected:
		void OnLoad(MessagePtr message);		
		void Update(double delta_time);
		void OnUnload(MessagePtr message);
		void OnSceneObjectCreated(SceneObjectCreatedNotifyMessagePtr message);
		void OnChangeCamera(ChangeCameraMessagePtr message);

	private:
		void UpdateShadowSettings();
		void UpdateFogSettings();
		void UpdateLightSettings();
		void UpdateSkySettings();

	
		//Keep private for now, 

		//Fog
		float GetFogStart() const {return m_FogStart;}
		float GetFogEnd() const {return m_FogEnd;}
		float GetFogDensity() const {return m_FogDensity;}
		Vec3 GetFogColor() const {return m_FogColor;}

		void SetFogStart(float value) {m_FogStart = value; UpdateFogSettings();}
		void SetFogEnd(float value) {m_FogEnd = value; UpdateFogSettings();}
		void SetFogColor(const Vec3 value) {m_FogColor = value; UpdateFogSettings();}
		void SetFogDensity(float value) {m_FogDensity = value; UpdateFogSettings();}

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
		bool GetSelfShadowing() const {return m_SelfShadowing;}

		void SetSelfShadowing(bool value) {m_SelfShadowing = value;UpdateShadowSettings();}
		void SetFarShadowDistance(float value) {m_FarShadowDistance = value;UpdateShadowSettings();}
		void SetOptimalAdjustFactor(float value) {m_OptimalAdjustFactor = value;UpdateShadowSettings();}
		void SetNumShadowTextures(int size) {m_NumShadowTextures = size;UpdateShadowSettings();}
		void SetTextureShadowSize(int size) {m_TextureShadowSize = size;UpdateShadowSettings();}
		void SetShadowProjType(const std::string &name) {m_ShadowProjType = name;UpdateShadowSettings();}
		void SetShadowType(const std::string &name) {m_ShadowType = name;UpdateShadowSettings();}
		void SetShadowCasterMaterial(const std::string &name) {m_ShadowCasterMaterial = name;UpdateShadowSettings();}

	
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
		float m_OptimalAdjustFactor;
		float m_FarShadowDistance;
		OSGGraphicsSystemWeakPtr m_GFXSystem;
		//osg::ref_ptr<osg::Group> m_RootNode;
		osg::ref_ptr<osg::PositionAttitudeTransform> m_RootNode;
		osg::ref_ptr<osg::PositionAttitudeTransform> m_SceneTransform;
	};
	typedef boost::shared_ptr<OSGGraphicsSceneManager> OSGGraphicsSceneManagerPtr;
	typedef boost::weak_ptr<OSGGraphicsSceneManager> OSGGraphicsSceneManagerWeakPtr;
}
