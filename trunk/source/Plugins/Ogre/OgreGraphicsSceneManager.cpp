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
#include "Core/Common.h"
#include "Plugins/Ogre/Helpers/OgreText.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSScene.h"

#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneObjectTemplate.h"

#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/OgrePostProcess.h"
#include "Plugins/Ogre/OgreGraphicsSystem.h"
#include "Plugins/Ogre/Components/OgreCameraComponent.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include "Plugins/Ogre/Helpers/DebugDrawer.h"
#include "Plugins/Ogre/OgreConvert.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"
#include "Core/System/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include <boost/bind.hpp>
#include <OgreRoot.h>
#include <OgreRenderSystem.h>
#include <OgreRenderWindow.h>

/*#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>*/
#include <OgreShadowCameraSetupLiSPSM.h>
#include <OgreShadowCameraSetupPlaneOptimal.h>

using namespace Ogre;

namespace GASS
{
	OgreGraphicsSceneManager::OgreGraphicsSceneManager(void) :	m_FogStart(200),
		m_FogEnd(40000),
		m_UseFog(1),
		m_FogMode(Ogre::FOG_LINEAR),
		m_FogDensity(0.01),
		m_FogColor(1,1,1),
		m_AmbientColor(1,1,1),
		//Shadows
		m_ShadowMode(TEXTURE_SHADOWS_ADDITIVE_INTEGRATED),
		m_ShadowCasterMaterial("DepthShadowmap_Caster_Float"),
		m_TextureShadowProjection(LISPSM),
		m_TextureShadowSize(1024),
		m_NumShadowTextures(1),
		m_SelfShadowing (false),
		m_UseAggressiveFocusRegion(true),
		m_OptimalAdjustFactor(1),
		m_FarShadowDistance(100),
		m_ShadowDirectionalLightExtrusionDistance(1000),
		m_SkyboxMaterial(""),
		m_SceneManagerType("OctreeSceneManager"),
		m_SceneMgr (NULL)
	{

	}

	OgreGraphicsSceneManager::~OgreGraphicsSceneManager(void)
	{

	}

	void OgreGraphicsSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register("OgreGraphicsSceneManager",new GASS::Creator<OgreGraphicsSceneManager, ISceneManager>);
		RegisterEnumProperty<FogModeBinder>( "FogMode", &GASS::OgreGraphicsSceneManager::GetFogMode, &GASS::OgreGraphicsSceneManager::SetFogMode);
		RegisterProperty<float>( "FogStart", &GASS::OgreGraphicsSceneManager::GetFogStart, &GASS::OgreGraphicsSceneManager::SetFogStart);
		RegisterProperty<float>( "FogEnd", &GASS::OgreGraphicsSceneManager::GetFogEnd, &GASS::OgreGraphicsSceneManager::SetFogEnd);
		RegisterProperty<float>( "FogDensity", &GASS::OgreGraphicsSceneManager::GetFogDensity, &GASS::OgreGraphicsSceneManager::SetFogDensity);
		RegisterProperty<ColorRGB>( "FogColor", &GASS::OgreGraphicsSceneManager::GetFogColor, &GASS::OgreGraphicsSceneManager::SetFogColor);
		RegisterProperty<ColorRGB>( "AmbientColor", &GASS::OgreGraphicsSceneManager::GetAmbientColor, &GASS::OgreGraphicsSceneManager::SetAmbientColor);
		RegisterProperty<std::string>("SceneManagerType", &GASS::OgreGraphicsSceneManager::GetSceneManagerType, &GASS::OgreGraphicsSceneManager::SetSceneManagerType);
		RegisterEnumProperty<OgreMaterial>("SkyboxMaterial", &GASS::OgreGraphicsSceneManager::GetSkyboxMaterial, &GASS::OgreGraphicsSceneManager::SetSkyboxMaterial);
		RegisterProperty<bool> ("SelfShadowing", &GASS::OgreGraphicsSceneManager::GetSelfShadowing ,&GASS::OgreGraphicsSceneManager::SetSelfShadowing );
		RegisterProperty<bool> ("UseAggressiveFocusRegion", &GASS::OgreGraphicsSceneManager::GetUseAggressiveFocusRegion,&GASS::OgreGraphicsSceneManager::SetUseAggressiveFocusRegion);
		RegisterProperty<float> ("FarShadowDistance", &GASS::OgreGraphicsSceneManager::GetFarShadowDistance,&GASS::OgreGraphicsSceneManager::SetFarShadowDistance);
		RegisterProperty<float> ("ShadowDirectionalLightExtrusionDistance", &GASS::OgreGraphicsSceneManager::GetShadowDirectionalLightExtrusionDistance,&GASS::OgreGraphicsSceneManager::SetShadowDirectionalLightExtrusionDistance);
		RegisterProperty<float> ("OptimalAdjustFactor", &GASS::OgreGraphicsSceneManager::GetOptimalAdjustFactor,&GASS::OgreGraphicsSceneManager::SetOptimalAdjustFactor);
		RegisterProperty<int>("NumShadowTextures",&GASS::OgreGraphicsSceneManager::GetNumShadowTextures,&GASS::OgreGraphicsSceneManager::SetNumShadowTextures);
		RegisterProperty<int>("TextureShadowSize",&GASS::OgreGraphicsSceneManager::GetTextureShadowSize,&GASS::OgreGraphicsSceneManager::SetTextureShadowSize);
		RegisterEnumProperty<TextureShadowProjectionBinder>("TextureShadowProjection",&GASS::OgreGraphicsSceneManager::GetTextureShadowProjection,&GASS::OgreGraphicsSceneManager::SetTextureShadowProjection);
		RegisterEnumProperty<ShadowModeBinder>("ShadowMode",&GASS::OgreGraphicsSceneManager::GetShadowMode,&GASS::OgreGraphicsSceneManager::SetShadowMode);
		RegisterEnumProperty<OgreMaterial>("ShadowCasterMaterial",&GASS::OgreGraphicsSceneManager::GetShadowCasterMaterial,&GASS::OgreGraphicsSceneManager::SetShadowCasterMaterial);
	}

	void OgreGraphicsSceneManager::OnCreate()
	{
		int address = (int) this;
		OgreGraphicsSystemPtr system = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<OgreGraphicsSystem>();
		m_GFXSystem = system;
		//register on system to get updates
		system->Register(shared_from_this());
		ScenePtr scene = GetScene();
		scene->RegisterForMessage(REG_TMESS(OgreGraphicsSceneManager::OnWeatherRequest,WeatherRequest,0));

		//create unique name
		static unsigned int scene_man_id = 0;
		std::stringstream ss;
		std::string name;
		ss << GetName() << scene_man_id;
		ss >> name;
		scene_man_id++;
		//m_SceneMgr = Root::getSingleton().createSceneManager(m_GFXSystem->GetSceneManagerType(), name);
		m_SceneMgr = Root::getSingleton().createSceneManager(m_SceneManagerType, name);
		if(m_SceneMgr == NULL)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"SceneManager " + m_SceneManagerType +" not found","OgreGraphicsSceneManager::OnLoad");
		UpdateShadowSettings();
		UpdateSkySettings();
		UpdateLightSettings();
		UpdateFogSettings();
		//Create debug render system
		m_DebugDrawer = new DebugDrawer(m_SceneMgr, 0.5f);

		Ogre::Root::getSingletonPtr()->addFrameListener(this);
	}


	void OgreGraphicsSceneManager::DrawLine(const Vec3 &start, const Vec3 &end, const Vec4 &color)
	{
		Ogre::ColourValue ogre_color(color.x,color.y,color.z,color.w);
		if(m_DebugDrawer)
			m_DebugDrawer->drawLine(Convert::ToOgre(start),Convert::ToOgre(end),ogre_color);		
	}

	void OgreGraphicsSceneManager::OnInit()
	{
		//Give hook to 3dparty plugins to attach, maybee send other info
		void* root = static_cast<void*>(m_SceneMgr->getRootSceneNode());
		SystemMessagePtr loaded_msg(new GraphicsSceneManagerLoadedEvent(std::string("Ogre3D"),root,root));
		SimSystemManagerPtr sim_sm = OgreGraphicsSystemPtr(m_GFXSystem)->GetSimSystemManager();
		sim_sm->SendImmediate(loaded_msg);
	}

	void OgreGraphicsSceneManager::OnShutdown()
	{
		delete m_DebugDrawer;
		m_SceneMgr->clearScene();
		Ogre::Root::getSingletonPtr()->removeFrameListener(this);
		Root::getSingleton().destroySceneManager(m_SceneMgr);
		m_SceneMgr = NULL;
		OgreGraphicsSystemPtr(m_GFXSystem)->Update(0); //why?
	}


	
	bool  OgreGraphicsSceneManager::frameStarted (const Ogre::FrameEvent &evt)
	{
		m_DebugDrawer->build();
		return true;
	}

	bool OgreGraphicsSceneManager::frameEnded (const Ogre::FrameEvent &evt)
	{
		m_DebugDrawer->clear();
		return true;
	}

	void OgreGraphicsSceneManager::OnWeatherRequest(WeatherRequestPtr message)
	{
		//float fog_end = 100 + (1.0-(message->GetFog()))*2000;
		SetFogEnd(message->GetFogDistance());
		SetFogDensity(message->GetFogDensity());
	}

	void OgreGraphicsSceneManager::UpdateFogSettings()
	{
		if(m_SceneMgr == NULL) return;
		ColourValue fogColour(m_FogColor.r, m_FogColor.g, m_FogColor.b);
		m_SceneMgr->setFog(m_FogMode.GetValue(), fogColour, m_FogDensity, m_FogStart, m_FogEnd);
		/*else if(m_FogMode == "Exp")
			m_SceneMgr->setFog(Ogre::FOG_EXP, fogColour, m_FogDensity, m_FogStart, m_FogEnd);
		else if(m_FogMode == "Exp2")
			m_SceneMgr->setFog(Ogre::FOG_EXP2, fogColour, m_FogDensity, m_FogStart, m_FogEnd);
		else if(m_FogMode == "None")
			m_SceneMgr->setFog(Ogre::FOG_NONE, fogColour, m_FogDensity, m_FogStart, m_FogEnd);*/
	}

	void OgreGraphicsSceneManager::UpdateSkySettings()
	{
		if(m_SceneMgr == NULL) return;
		if(m_SkyboxMaterial.GetName() != "")
			m_SceneMgr->setSkyBox(true, m_SkyboxMaterial.GetName(), 50);
	}

	void OgreGraphicsSceneManager::UpdateLightSettings()
	{
		if(m_SceneMgr == NULL) return;
		m_SceneMgr->setAmbientLight(ColourValue(m_AmbientColor.r, m_AmbientColor.g, m_AmbientColor.b));
	}


	void OgreGraphicsSceneManager::UpdateShadowSettings()
	{
		if(m_SceneMgr == NULL) 
			return;
		bool isOpenGL = false;
		if(Root::getSingleton().getRenderSystem()->getName().find("GL") != Ogre::String::npos)
		{
			isOpenGL = true;
		}
		else
		{
			isOpenGL = false;
		}
		bool tex_shadow = false;
		switch(m_ShadowMode.GetValue())
		{
		case TEXTURE_SHADOWS_ADDITIVE_INTEGRATED:
			{
				tex_shadow = true;
				if(isOpenGL)
					m_SceneMgr->setShadowTexturePixelFormat(Ogre::PF_FLOAT16_RGBA);
				else m_SceneMgr->setShadowTexturePixelFormat(Ogre::PF_FLOAT32_RGBA);
				//sm->setShadowTexturePixelFormat(Ogre::PF_FLOAT32_RGBA);
				//sm->setShadowTexturePixelFormat(Ogre::PF_FLOAT16_RGBA);
				m_SceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
				//sm->setShadowCasterRenderBackFaces(true);

				//sm->setShadowCasterRenderBackFaces(false);
				//sm->setShadowTextureReceiverMaterial("Ogre/DepthShadowmap/Receiver/Float");
			}
			break;
		case TEXTURE_SHADOWS_ADDITIVE:
			{
				tex_shadow = true;
				m_SceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);
				m_SceneMgr->setShadowTextureCasterMaterial(Ogre::StringUtil::BLANK);
				m_SceneMgr->setShadowTextureReceiverMaterial(Ogre::StringUtil::BLANK);
			}
			break;
		case TEXTURE_SHADOWS_MODULATIVE: 
			{
				tex_shadow = true;
				m_SceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
				m_SceneMgr->setShadowTextureCasterMaterial(Ogre::StringUtil::BLANK);
				m_SceneMgr->setShadowTextureReceiverMaterial(Ogre::StringUtil::BLANK);
			}
			break;
		}

		if(tex_shadow)
		{
			m_SceneMgr->setShadowTextureSize(m_TextureShadowSize);
			m_SceneMgr->setShadowTextureCount(m_NumShadowTextures);
			m_SceneMgr->setShadowTextureSelfShadow(m_SelfShadowing);
			//sm->setShadowTextureCasterMaterial("DepthShadowmap_Caster_Float");
			//if(m_ShadowCasterMaterial.Valid())
			{
				m_SceneMgr->setShadowTextureCasterMaterial(m_ShadowCasterMaterial.GetName());
			}

			ShadowCameraSetupPtr currentShadowCameraSetup;
			switch(m_TextureShadowProjection.GetValue())
			{
			case LISPSM:
				{
					m_LiSPSMSetup = new LiSPSMShadowCameraSetup();
					currentShadowCameraSetup = ShadowCameraSetupPtr(m_LiSPSMSetup);
					m_LiSPSMSetup->setOptimalAdjustFactor(m_OptimalAdjustFactor);
					m_LiSPSMSetup->setUseAggressiveFocusRegion(m_UseAggressiveFocusRegion);
					m_LiSPSMSetup->setCameraLightDirectionThreshold(Ogre::Degree( 10));
				}
				break;
			case UNIFORM:
				{
					currentShadowCameraSetup = ShadowCameraSetupPtr(new DefaultShadowCameraSetup());
					m_SceneMgr->setShadowCasterRenderBackFaces(false);

				}
				break;
			case UNIFORM_FOCUSED:
				{
					Ogre::FocusedShadowCameraSetup* fscs = new Ogre::FocusedShadowCameraSetup();
					fscs->setUseAggressiveFocusRegion(m_UseAggressiveFocusRegion);
					currentShadowCameraSetup = ShadowCameraSetupPtr(fscs);

				}
				break;
			}
			//GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Undefined projection " + m_ShadowProjType, "OgreGraphicsSceneManager::UpdateShadowSettings");
			m_SceneMgr->setShadowCameraSetup(currentShadowCameraSetup);
		}
		m_SceneMgr->setShadowFarDistance(m_FarShadowDistance);
		m_SceneMgr->setShowDebugShadows(true);
		m_SceneMgr->setShadowDirectionalLightExtrusionDistance(m_ShadowDirectionalLightExtrusionDistance);

		
		/*if(OverlayManager::getSingleton().hasOverlayElement("Ogre/DebugShadowPanel0"))
		{

			MaterialPtr debugMat = 	MaterialManager::getSingleton().getByName("Ogre/DebugShadowMap0");
			TexturePtr shadowTex = m_SceneMgr->getShadowTexture(0);
			debugMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(shadowTex->getName());

		}
		else
		{

			MaterialPtr debugMat = MaterialManager::getSingleton().create("Ogre/DebugShadowMap0", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			debugMat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
			TexturePtr shadowTex = m_SceneMgr->getShadowTexture(0);
			TextureUnitState *t = debugMat->getTechnique(0)->getPass(0)->createTextureUnitState(shadowTex->getName());
			t->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);


			OverlayContainer* debugPanel =  NULL;
			debugPanel = (OverlayContainer*)	(OverlayManager::getSingleton().createOverlayElement("Panel", "Ogre/DebugShadowPanel0"));
			debugPanel->_setPosition(0, 0);
			debugPanel->_setDimensions(0.3, 0.3);
			debugPanel->setMaterialName("Ogre/DebugShadowMap0");

			Overlay* debugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");
			debugOverlay->add2D(debugPanel);
			debugOverlay->show();
		}*/


		//END OF SET DEBUG OVERLAY WITH SHADOW_MAP

		
		/*debugMat = MaterialManager::getSingleton().getByName("Ogre/DebugShadowMap1");
		shadowTex = mSceneMgr->getShadowTexture(1);
		debugMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(shadowTex->getName());*/

	}

}




