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
#include "Plugins/Ogre/GASSOgreGraphicsSceneManager.h"
#include "Core/Common.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSException.h"

#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneObjectTemplate.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSystemFactory.h"

#include "Plugins/Ogre/Helpers/OgreText.h"
#include "Plugins/Ogre/Helpers/GASSOgreMeshExporter.h"
#include "Plugins/Ogre/GASSOgrePostProcess.h"
#include "Plugins/Ogre/GASSOgreGraphicsSystem.h"
#include "Plugins/Ogre/Components/GASSOgreLocationComponent.h"
#include "Plugins/Ogre/Helpers/DebugDrawer.h"
#include "Plugins/Ogre/GASSOgreConvert.h"

//warning C4512

#include <OgreRoot.h>
#include <OgreRenderSystem.h>
#include <Overlay/OgreOverlaySystem.h>
#include <OgreShadowCameraSetupLiSPSM.h>
#include <OgreShadowCameraSetupPlaneOptimal.h>

using namespace Ogre;

namespace GASS
{
	void OgreGraphicsSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register<OgreGraphicsSceneManager>("OgreGraphicsSceneManager");

		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("Handle ogre scene nodes and global graphics properties related to the scene", OF_VISIBLE)));

		RegisterGetSet("FogMode", &GASS::OgreGraphicsSceneManager::GetFogMode, &GASS::OgreGraphicsSceneManager::SetFogMode, PF_VISIBLE | PF_EDITABLE, "Fog type");
		RegisterGetSet("FogStart", &GASS::OgreGraphicsSceneManager::GetFogStart, &GASS::OgreGraphicsSceneManager::SetFogStart, PF_VISIBLE | PF_EDITABLE,"Fog start distance");
		RegisterGetSet("FogEnd", &GASS::OgreGraphicsSceneManager::GetFogEnd, &GASS::OgreGraphicsSceneManager::SetFogEnd, PF_VISIBLE | PF_EDITABLE,"Fog end distance");
		RegisterGetSet("FogDensity", &GASS::OgreGraphicsSceneManager::GetFogDensity, &GASS::OgreGraphicsSceneManager::SetFogDensity, PF_VISIBLE | PF_EDITABLE,"Fog density");
		RegisterGetSet("FogColor", &GASS::OgreGraphicsSceneManager::GetFogColor, &GASS::OgreGraphicsSceneManager::SetFogColor, PF_VISIBLE | PF_EDITABLE,"Fog Color");
		RegisterGetSet("AmbientColor", &GASS::OgreGraphicsSceneManager::GetAmbientColor, &GASS::OgreGraphicsSceneManager::SetAmbientColor, PF_VISIBLE | PF_EDITABLE,"Scene ambient color");
		RegisterGetSet("SceneManagerType", &GASS::OgreGraphicsSceneManager::GetSceneManagerType, &GASS::OgreGraphicsSceneManager::SetSceneManagerType, PF_VISIBLE,"Scene manager type");
		RegisterGetSet("UseSkybox", &GASS::OgreGraphicsSceneManager::GetUseSkybox, &GASS::OgreGraphicsSceneManager::SetUseSkybox, PF_VISIBLE | PF_EDITABLE,"Enable/Disable skybox, change sky box material in with SkyboxMaterial property");
		auto prop_skybox = RegisterGetSet("SkyboxMaterial", &GASS::OgreGraphicsSceneManager::GetSkyboxMaterial, &GASS::OgreGraphicsSceneManager::SetSkyboxMaterial, PF_VISIBLE, "Skybox Material selection");
		prop_skybox->SetOptionsCallback(std::make_shared<OgreMaterialOptions>("GASS_SKYBOX_MATERIALS"));
		RegisterGetSet("SelfShadowing", &GASS::OgreGraphicsSceneManager::GetSelfShadowing, &GASS::OgreGraphicsSceneManager::SetSelfShadowing, PF_VISIBLE | PF_EDITABLE,"SelfShadowing");
		RegisterGetSet("UseAggressiveFocusRegion", &GASS::OgreGraphicsSceneManager::GetUseAggressiveFocusRegion, &GASS::OgreGraphicsSceneManager::SetUseAggressiveFocusRegion, PF_VISIBLE | PF_EDITABLE,"UseAggressiveFocusRegion");
		RegisterGetSet("FarShadowDistance", &GASS::OgreGraphicsSceneManager::GetFarShadowDistance, &GASS::OgreGraphicsSceneManager::SetFarShadowDistance, PF_VISIBLE | PF_EDITABLE,"FarShadowDistance");
		RegisterGetSet("ShadowDirectionalLightExtrusionDistance", &GASS::OgreGraphicsSceneManager::GetShadowDirectionalLightExtrusionDistance, &GASS::OgreGraphicsSceneManager::SetShadowDirectionalLightExtrusionDistance, PF_VISIBLE | PF_EDITABLE,"ShadowDirectionalLightExtrusionDistance");
		RegisterGetSet("OptimalAdjustFactor", &GASS::OgreGraphicsSceneManager::GetOptimalAdjustFactor, &GASS::OgreGraphicsSceneManager::SetOptimalAdjustFactor, PF_VISIBLE | PF_EDITABLE,"OptimalAdjustFactor for LIPSM");
		RegisterGetSet("NumShadowTextures", &GASS::OgreGraphicsSceneManager::GetNumShadowTextures, &GASS::OgreGraphicsSceneManager::SetNumShadowTextures, PF_VISIBLE | PF_EDITABLE,"NumShadowTextures");
		RegisterGetSet("TextureShadowSize", &GASS::OgreGraphicsSceneManager::GetTextureShadowSize, &GASS::OgreGraphicsSceneManager::SetTextureShadowSize, PF_VISIBLE | PF_EDITABLE,"TextureShadowSize");
		RegisterGetSet("TextureShadowProjection", &GASS::OgreGraphicsSceneManager::GetTextureShadowProjection, &GASS::OgreGraphicsSceneManager::SetTextureShadowProjection, PF_VISIBLE | PF_EDITABLE, "Texture Shadow Projection Type");
		RegisterGetSet("ShadowMode", &GASS::OgreGraphicsSceneManager::GetShadowMode, &GASS::OgreGraphicsSceneManager::SetShadowMode, PF_VISIBLE | PF_EDITABLE, "Shadow Mode");
		auto prop_shadow = RegisterGetSet("ShadowCasterMaterial", &OgreGraphicsSceneManager::GetShadowCasterMaterial, &GASS::OgreGraphicsSceneManager::SetShadowCasterMaterial, PF_VISIBLE, "Shadow Caster Material");
		prop_shadow->SetOptionsCallback(std::make_shared<OgreMaterialOptions>());
	}

	OgreGraphicsSceneManager::OgreGraphicsSceneManager(SceneWeakPtr scene) : Reflection(scene),
		m_FogStart(200),
		m_FogEnd(40000),
		m_UseFog(1),
		m_FogMode(FM_LINEAR),
		m_FogDensity(0.01f),
		m_FogColor(1,1,1),
		m_AmbientColor(0.5,0.5,0.5),
		//Shadows
		m_ShadowMode(TEXTURE_SHADOWS_ADDITIVE_INTEGRATED),
		m_ShadowCasterMaterial("DepthShadowmap_Caster_Float"),
		m_TextureShadowProjection(LISPSM),
		m_TextureShadowSize(1024),
		m_NumShadowTextures(4),
		m_SelfShadowing (false),
		m_UseAggressiveFocusRegion(true),
		m_OptimalAdjustFactor(1),
		m_FarShadowDistance(100),
		m_ShadowDirectionalLightExtrusionDistance(1000),
		m_SceneManagerType("OctreeSceneManager"),
		m_SceneMgr (NULL),
		m_UseSkybox(false),
		m_DebugDrawer(NULL)
	{

	}

	void OgreGraphicsSceneManager::OnPostConstruction()
	{
		//register on system to get updates
		RegisterForPostUpdate<OgreGraphicsSystem>();

		OgreGraphicsSystemPtr system = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<OgreGraphicsSystem>();
		m_GFXSystem = system;

		ScenePtr scene = GetScene();
		scene->RegisterForMessage(REG_TMESS(OgreGraphicsSceneManager::OnWeatherRequest, WeatherRequest, 0));
		scene->RegisterForMessage(REG_TMESS(OgreGraphicsSceneManager::OnDrawCircle, DrawCircleRequest, 0));
		scene->RegisterForMessage(REG_TMESS(OgreGraphicsSceneManager::OnExportMesh, ExportMeshRequest, 0));

		//create unique name
		static unsigned int scene_man_id = 0;
		std::stringstream ss;
		std::string name;
		ss << GetName() << scene_man_id;
		ss >> name;
		scene_man_id++;
		m_SceneMgr = Root::getSingleton().createSceneManager(m_SceneManagerType, name);
		if (m_SceneMgr == NULL)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "SceneManager " + m_SceneManagerType + " not found", "OgreGraphicsSceneManager::OnLoad");
		UpdateShadowSettings();
		UpdateSkySettings();
		UpdateLightSettings();
		UpdateFogSettings();
		//Create debug render system
		m_DebugDrawer = new DebugDrawer(m_SceneMgr, 0.5f);

		Ogre::Root::getSingletonPtr()->addFrameListener(this);

		m_SceneMgr->addRenderQueueListener(system->GetOverlaySystem());

	}

	void OgreGraphicsSceneManager::OnSceneShutdown()
	{
		delete m_DebugDrawer;
		m_SceneMgr->clearScene();
		Ogre::Root::getSingletonPtr()->removeFrameListener(this);
		Root::getSingleton().destroySceneManager(m_SceneMgr);
		m_SceneMgr = NULL;
		//OgreGraphicsSystemPtr(m_GFXSystem)->_UpdateListeners(0); //why?
	}

	OgreGraphicsSceneManager::~OgreGraphicsSceneManager(void)
	{

	}

	void OgreGraphicsSceneManager::OnExportMesh(ExportMeshRequestPtr message)
	{
		OgreMeshExporter exporter;
		exporter.Export(message->m_Filename, message->m_RootObject);
	}

	void OgreGraphicsSceneManager::OnDrawCircle(DrawCircleRequestPtr message)
	{
		const ColorRGBA color = message->GetColor();
		const Ogre::ColourValue ogre_color = OgreConvert::ToOgre(color);
		if(m_DebugDrawer)
		{
			m_DebugDrawer->drawCircle(OgreConvert::ToOgre(message->GetCenter()), static_cast<float>(message->GetRadius()),message->GetSegments(),ogre_color,message->GetFilled());
		}
	}

	void OgreGraphicsSceneManager::DrawLine(const Vec3 &start_point, const Vec3 &end_point, const ColorRGBA &start_color , const ColorRGBA &/*end_color*/)
	{
		
		const Ogre::ColourValue ogre_color = OgreConvert::ToOgre(start_color);
		if(m_DebugDrawer)
			m_DebugDrawer->drawLine(OgreConvert::ToOgre(start_point),OgreConvert::ToOgre(end_point),ogre_color);
	}

	void OgreGraphicsSceneManager::OnSceneCreated()
	{
		//Give hook to 3dparty plugins to attach, maybe send more info?
		void* root = static_cast<void*>(m_SceneMgr->getRootSceneNode());
		SystemMessagePtr loaded_msg(new GraphicsSceneManagerLoadedEvent(std::string("Ogre3D"),root,root));
		SimSystemManagerPtr sim_sm = OgreGraphicsSystemPtr(m_GFXSystem)->GetSimSystemManager();
		sim_sm->SendImmediate(loaded_msg);
	}

	bool  OgreGraphicsSceneManager::frameStarted (const Ogre::FrameEvent &/*evt*/)
	{
		m_DebugDrawer->build();
		return true;
	}

	bool OgreGraphicsSceneManager::frameEnded (const Ogre::FrameEvent &/*evt*/)
	{
		m_DebugDrawer->clear();
		return true;
	}

	void OgreGraphicsSceneManager::OnWeatherRequest(WeatherRequestPtr message)
	{
		SetFogEnd(message->GetFogDistance());
		SetFogDensity(message->GetFogDensity());
	}

	void OgreGraphicsSceneManager::UpdateFogSettings()
	{
		if(m_SceneMgr == NULL) return;

		const ColourValue fogColour = OgreConvert::ToOgre(m_FogColor);

		Ogre::FogMode ogre_fm =Ogre::FOG_NONE;
		switch(m_FogMode.GetValue())
		{
		case FM_LINEAR:
			ogre_fm = Ogre::FOG_LINEAR;
			break;
		case FM_EXP:
			ogre_fm = Ogre::FOG_EXP;
			break;
		case FM_EXP2:
			ogre_fm = Ogre::FOG_EXP2;
			break;
		case FM_NONE:
			ogre_fm = Ogre::FOG_NONE;
			break;
		}
		m_SceneMgr->setFog(ogre_fm, fogColour, m_FogDensity, m_FogStart, m_FogEnd);
	}

	void OgreGraphicsSceneManager::UpdateSkySettings()
	{
		if(m_SceneMgr == NULL) return;
		if(m_SkyboxMaterial.GetName() != "")
			m_SceneMgr->setSkyBox(m_UseSkybox, m_SkyboxMaterial.GetName(), 50);
		m_SceneMgr->setSkyBoxEnabled(m_UseSkybox);
	}

	void OgreGraphicsSceneManager::UpdateLightSettings()
	{
		if(m_SceneMgr == NULL) return;
		m_SceneMgr->setAmbientLight(OgreConvert::ToOgre(m_AmbientColor));
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
		case SHADOWS_DISABLED:
			m_SceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
			break;
		case STENCIL_SHADOWS_ADDITIVE:

			break;
		case STENCIL_SHADOWS_MODULATIVE:
			m_SceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
			break;
		case TEXTURE_SHADOWS_ADDITIVE_INTEGRATED:
			{
				tex_shadow = true;
				if(isOpenGL)
					m_SceneMgr->setShadowTexturePixelFormat(Ogre::PF_FLOAT16_RGBA);
				else m_SceneMgr->setShadowTexturePixelFormat(Ogre::PF_FLOAT32_RGBA);
				m_SceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
				m_SceneMgr->setShadowTextureCasterMaterial(m_ShadowCasterMaterial.GetName());
				m_SceneMgr->setShadowTextureSelfShadow(m_SelfShadowing);
				//m_SceneMgr->setShadowCasterRenderBackFaces(true);
				//m_SceneMgr->setShadowTextureReceiverMaterial("Ogre/DepthShadowmap/Receiver/Float");
			}
			break;
		case TEXTURE_SHADOWS_ADDITIVE:
			{
				tex_shadow = true;
				m_SceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);
				m_SceneMgr->setShadowTextureCasterMaterial(Ogre::StringUtil::BLANK);
				m_SceneMgr->setShadowTextureReceiverMaterial(Ogre::StringUtil::BLANK);
				m_SceneMgr->setShadowTexturePixelFormat(PF_X8R8G8B8);
				m_SceneMgr->setShadowTextureSelfShadow(m_SelfShadowing);
			}
			break;
		case TEXTURE_SHADOWS_MODULATIVE:
			{
				tex_shadow = true;
				m_SceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
				m_SceneMgr->setShadowTextureCasterMaterial(Ogre::StringUtil::BLANK);
				m_SceneMgr->setShadowTextureReceiverMaterial(Ogre::StringUtil::BLANK);
				m_SceneMgr->setShadowTexturePixelFormat(PF_X8R8G8B8);
				m_SceneMgr->setShadowTextureSelfShadow(m_SelfShadowing);
			}
			break;
		}

		if(tex_shadow)
		{
			m_SceneMgr->setShadowTextureSize(static_cast<unsigned short>(m_TextureShadowSize));
			m_SceneMgr->setShadowTextureCount(m_NumShadowTextures);

			//shared pointer!
			ShadowCameraSetupPtr new_shadow_camera_setup;

			switch(m_TextureShadowProjection.GetValue())
			{
			case LISPSM:
				{
					Ogre::LiSPSMShadowCameraSetup* LiSPSMSetup = new LiSPSMShadowCameraSetup();
					new_shadow_camera_setup = ShadowCameraSetupPtr(LiSPSMSetup);
					LiSPSMSetup->setOptimalAdjustFactor(m_OptimalAdjustFactor);
					LiSPSMSetup->setUseAggressiveFocusRegion(m_UseAggressiveFocusRegion);
					LiSPSMSetup->setCameraLightDirectionThreshold(Ogre::Degree( 10));
				}
				break;
			case UNIFORM:
				{
					new_shadow_camera_setup = ShadowCameraSetupPtr(new DefaultShadowCameraSetup());
					//m_SceneMgr->setShadowCasterRenderBackFaces(false);
				}
				break;
			case UNIFORM_FOCUSED:
				{
					Ogre::FocusedShadowCameraSetup* fscs = new Ogre::FocusedShadowCameraSetup();
					fscs->setUseAggressiveFocusRegion(m_UseAggressiveFocusRegion);
					new_shadow_camera_setup = ShadowCameraSetupPtr(fscs);
				}
				break;
			}
			m_SceneMgr->setShadowCameraSetup(new_shadow_camera_setup);
		}
		m_SceneMgr->setShadowFarDistance(m_FarShadowDistance);
		m_SceneMgr->setShadowDirectionalLightExtrusionDistance(m_ShadowDirectionalLightExtrusionDistance);

#ifdef GASS_DEBUG_OGRE_SHADOWS
		m_SceneMgr->setShowDebugShadows(true);
		if(OverlayManager::getSingleton().hasOverlayElement("Ogre/DebugShadowPanel0"))
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
		}
#endif
		/*debugMat = MaterialManager::getSingleton().getByName("Ogre/DebugShadowMap1");
		shadowTex = mSceneMgr->getShadowTexture(1);
		debugMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(shadowTex->getName());*/
	}

}




