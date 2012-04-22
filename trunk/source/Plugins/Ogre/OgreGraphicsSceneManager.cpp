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
#include "Sim/Scenario/Scene/GASSSceneManagerFactory.h"
#include "Sim/Scenario/GASSScenario.h"
#include "Sim/Scenario/Scene/GASSSceneObjectManager.h"
#include "Sim/Scenario/Scene/GASSSceneObject.h"
#include "Sim/Scenario/Scene/GASSSceneObjectTemplate.h"
#include "Sim/Scheduling/GASSIRuntimeController.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/OgrePostProcess.h"
#include "Plugins/Ogre/OgreGraphicsSystem.h"
#include "Plugins/Ogre/Components/OgreCameraComponent.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include "Plugins/Ogre/Helpers/DebugDrawer.h"


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
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreShadowCameraSetupLiSPSM.h>
#include <OgreShadowCameraSetupPlaneOptimal.h>

using namespace Ogre;

namespace GASS
{
	OgreGraphicsSceneManager::OgreGraphicsSceneManager(void) :	m_FogStart(200),
		m_FogEnd(400),
		m_UseFog(1),
		m_FogMode("Linear"),
		m_FogDensity(0.01),
		m_FogColor(1,1,1),
		m_AmbientColor(1,1,1),
		//Shadows
		m_ShadowType ("AdditiveIntegratedTextureShadows"),
		m_ShadowCasterMaterial("DepthShadowmap_Caster_Float"),
		m_ShadowProjType ("LiSPSM"),
		m_TextureShadowSize (1024),
		m_NumShadowTextures (1),
		m_SelfShadowing (false),
		m_UseAggressiveFocusRegion(true),
		m_OptimalAdjustFactor (1),
		m_FarShadowDistance (100),
		m_ShadowDirectionalLightExtrusionDistance(1000),
		m_SkyboxMaterial(""),
		m_SceneManagerType("TerrainSceneManager"),
		m_SceneMgr (NULL)
	{
	}

	OgreGraphicsSceneManager::~OgreGraphicsSceneManager(void)
	{

	}

	void OgreGraphicsSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register("OgreGraphicsSceneManager",new GASS::Creator<OgreGraphicsSceneManager, ISceneManager>);
		RegisterProperty<std::string>( "FogMode", &GASS::OgreGraphicsSceneManager::GetFogMode, &GASS::OgreGraphicsSceneManager::SetFogMode);
		RegisterProperty<float>( "FogStart", &GASS::OgreGraphicsSceneManager::GetFogStart, &GASS::OgreGraphicsSceneManager::SetFogStart);
		RegisterProperty<float>( "FogEnd", &GASS::OgreGraphicsSceneManager::GetFogEnd, &GASS::OgreGraphicsSceneManager::SetFogEnd);
		RegisterProperty<float>( "FogDensity", &GASS::OgreGraphicsSceneManager::GetFogDensity, &GASS::OgreGraphicsSceneManager::SetFogDensity);
		RegisterProperty<Vec3>( "FogColor", &GASS::OgreGraphicsSceneManager::GetFogColor, &GASS::OgreGraphicsSceneManager::SetFogColor);
		RegisterProperty<Vec3>( "AmbientColor", &GASS::OgreGraphicsSceneManager::GetAmbientColor, &GASS::OgreGraphicsSceneManager::SetAmbientColor);
		RegisterProperty<std::string>("SceneManagerType", &GASS::OgreGraphicsSceneManager::GetSceneManagerType, &GASS::OgreGraphicsSceneManager::SetSceneManagerType);
		RegisterProperty<std::string>("SkyboxMaterial", &GASS::OgreGraphicsSceneManager::GetSkyboxMaterial, &GASS::OgreGraphicsSceneManager::SetSkyboxMaterial);
		RegisterProperty<bool> ("SelfShadowing", &GASS::OgreGraphicsSceneManager::GetSelfShadowing ,&GASS::OgreGraphicsSceneManager::SetSelfShadowing );
		RegisterProperty<bool> ("UseAggressiveFocusRegion", &GASS::OgreGraphicsSceneManager::GetUseAggressiveFocusRegion,&GASS::OgreGraphicsSceneManager::SetUseAggressiveFocusRegion);
		RegisterProperty<float> ("FarShadowDistance", &GASS::OgreGraphicsSceneManager::GetFarShadowDistance,&GASS::OgreGraphicsSceneManager::SetFarShadowDistance);
		RegisterProperty<float> ("ShadowDirectionalLightExtrusionDistance", &GASS::OgreGraphicsSceneManager::GetShadowDirectionalLightExtrusionDistance,&GASS::OgreGraphicsSceneManager::SetShadowDirectionalLightExtrusionDistance);
		RegisterProperty<float> ("OptimalAdjustFactor", &GASS::OgreGraphicsSceneManager::GetOptimalAdjustFactor,&GASS::OgreGraphicsSceneManager::SetOptimalAdjustFactor);
		RegisterProperty<int>("NumShadowTextures",&GASS::OgreGraphicsSceneManager::GetNumShadowTextures,&GASS::OgreGraphicsSceneManager::SetNumShadowTextures);
		RegisterProperty<int>("TextureShadowSize",&GASS::OgreGraphicsSceneManager::GetTextureShadowSize,&GASS::OgreGraphicsSceneManager::SetTextureShadowSize);
		RegisterProperty<std::string>("ShadowProjType",&GASS::OgreGraphicsSceneManager::GetShadowProjType,&GASS::OgreGraphicsSceneManager::SetShadowProjType);
		RegisterProperty<std::string>("ShadowType",&GASS::OgreGraphicsSceneManager::GetShadowType,&GASS::OgreGraphicsSceneManager::SetShadowType);
		RegisterProperty<std::string>("ShadowCasterMaterial",&GASS::OgreGraphicsSceneManager::GetShadowCasterMaterial,&GASS::OgreGraphicsSceneManager::SetShadowCasterMaterial);

	}

	void OgreGraphicsSceneManager::OnCreate()
	{
		int address = (int) this;
		m_GFXSystem = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<OgreGraphicsSystem>();
		ScenarioPtr scenario = GetScenario();
		assert(scenario);

		scenario->RegisterForMessage(REG_TMESS(OgreGraphicsSceneManager::OnLoad ,LoadSceneManagersMessage,Scenario::GFX_SYSTEM_LOAD_PRIORITY));
		scenario->RegisterForMessage(REG_TMESS(OgreGraphicsSceneManager::OnUnload, UnloadSceneManagersMessage,0));
		scenario->RegisterForMessage(REG_TMESS(OgreGraphicsSceneManager::OnLoadSceneObject,SceneObjectCreatedNotifyMessage ,Scenario::GFX_COMPONENT_LOAD_PRIORITY));
		scenario->RegisterForMessage(REG_TMESS(OgreGraphicsSceneManager::OnChangeCamera,ChangeCameraMessage,0));
		scenario->RegisterForMessage(REG_TMESS(OgreGraphicsSceneManager::OnWeatherMessage,WeatherMessage,0));

	}

	void OgreGraphicsSceneManager::OnUnload(UnloadSceneManagersMessagePtr message)
	{
		if(m_SceneMgr)
		{
			delete DebugDrawer::getSingletonPtr();
			m_SceneMgr->clearScene();
			Root::getSingleton().destroySceneManager(m_SceneMgr);
			m_SceneMgr = NULL;
			
		}
	}

	void OgreGraphicsSceneManager::OnLoad(MessagePtr message)
	{
		m_SceneMgr = Root::getSingleton().createSceneManager(m_SceneManagerType, m_Name);
		if(m_SceneMgr == NULL)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"SceneManager " + m_SceneManagerType +" not found","OgreGraphicsSceneManager::OnLoad");
		UpdateShadowSettings();
		UpdateSkySettings();
		UpdateLightSettings();
		UpdateFogSettings();
		OgreGraphicsSystemPtr(m_GFXSystem)->SetActiveSceneManger(m_SceneMgr);

		OgreGraphicsSystemPtr(m_GFXSystem)->Register(shared_from_this());

		//Give hook to 3dparty plugins to attach, maybee send other info
		void* root = static_cast<void*>(m_SceneMgr->getRootSceneNode());
		MessagePtr loaded_msg(new GFXSceneManagerLoadedNotifyMessage(std::string("Ogre3D"),root,root));
		SimSystemManagerPtr sim_sm = boost::shared_dynamic_cast<SimSystemManager>(OgreGraphicsSystemPtr(m_GFXSystem)->GetOwner());
		sim_sm->SendImmediate(loaded_msg);

		//Create debug render system
		new DebugDrawer(m_SceneMgr, 0.5f);
	}

	void OgreGraphicsSceneManager::OnWeatherMessage(WeatherMessagePtr message)
	{
		//float fog_end = 100 + (1.0-(message->GetFog()))*2000;
		SetFogEnd(message->GetFogDistance());
		SetFogDensity(message->GetFogDensity());
	}

	void OgreGraphicsSceneManager::OnChangeCamera(ChangeCameraMessagePtr message)
	{
		SceneObjectPtr cam_obj = message->GetCamera();
		const std::string vp_name = message->GetViewport();

		if(cam_obj)
		{
			OgreCameraComponentPtr cam_comp = cam_obj->GetFirstComponentByClass<OgreCameraComponent>();
			OgreGraphicsSystemPtr(m_GFXSystem)->ChangeCamera(vp_name, cam_comp);
			OgreGraphicsSystemPtr(m_GFXSystem)->GetPostProcess()->Update(cam_comp);

			MessagePtr cam_message(new CameraChangedNotifyMessage(cam_obj,cam_comp->GetOgreCamera()));
			GetScenario()->PostMessage(cam_message);
		}
		else
		{
			OgreGraphicsSystemPtr(m_GFXSystem)->RemoveViewport(vp_name,"MainWindow");
		}
	}

	void OgreGraphicsSceneManager::OnLoadSceneObject(SceneObjectCreatedNotifyMessagePtr message)
	{
		//Initlize all gfx components and send scene mananger as argument
		SceneObjectPtr obj = message->GetSceneObject();
		assert(obj);
		MessagePtr gfx_msg(new LoadGFXComponentsMessage(shared_from_this(),m_SceneMgr));
		obj->SendImmediate(gfx_msg);
	}

	void OgreGraphicsSceneManager::UpdateFogSettings()
	{
		if(m_SceneMgr == NULL) return;
		ColourValue fogColour(m_FogColor.x, m_FogColor.y, m_FogColor.z);
		if(m_FogMode == "Linear")
			m_SceneMgr->setFog(Ogre::FOG_LINEAR, fogColour, m_FogDensity, m_FogStart, m_FogEnd);
		else if(m_FogMode == "Exp")
			m_SceneMgr->setFog(Ogre::FOG_EXP, fogColour, m_FogDensity, m_FogStart, m_FogEnd);
		else if(m_FogMode == "Exp2")
			m_SceneMgr->setFog(Ogre::FOG_EXP2, fogColour, m_FogDensity, m_FogStart, m_FogEnd);
		else if(m_FogMode == "None")
			m_SceneMgr->setFog(Ogre::FOG_NONE, fogColour, m_FogDensity, m_FogStart, m_FogEnd);
		
	}

	void OgreGraphicsSceneManager::UpdateSkySettings()
	{
		if(m_SceneMgr == NULL) return;
		if(m_SkyboxMaterial != "")
			m_SceneMgr->setSkyBox(true, m_SkyboxMaterial, 50);
	}

	void OgreGraphicsSceneManager::UpdateLightSettings()
	{
		if(m_SceneMgr == NULL) return;
		m_SceneMgr->setAmbientLight(ColourValue(m_AmbientColor.x, m_AmbientColor.y, m_AmbientColor.z));
	}

	void OgreGraphicsSceneManager::Update(double delta_time)
	{

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
		if(m_ShadowType == "AdditiveIntegratedTextureShadows")
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

		if(m_ShadowType == "TextureShadowsAdditive")
		{
			tex_shadow = true;
			m_SceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);
			m_SceneMgr->setShadowTextureCasterMaterial(Ogre::StringUtil::BLANK);
			m_SceneMgr->setShadowTextureReceiverMaterial(Ogre::StringUtil::BLANK);

		}
		if(m_ShadowType == "TextureShadowsModulative")
		{
			tex_shadow = true;
			m_SceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
			m_SceneMgr->setShadowTextureCasterMaterial(Ogre::StringUtil::BLANK);
			m_SceneMgr->setShadowTextureReceiverMaterial(Ogre::StringUtil::BLANK);
		}

		if(tex_shadow)
		{

			m_SceneMgr->setShadowTextureSize(m_TextureShadowSize);
			m_SceneMgr->setShadowTextureCount(m_NumShadowTextures);
			m_SceneMgr->setShadowTextureSelfShadow(m_SelfShadowing);
			//sm->setShadowTextureCasterMaterial("DepthShadowmap_Caster_Float");
			if(m_ShadowCasterMaterial != "")
			{

				m_SceneMgr->setShadowTextureCasterMaterial(m_ShadowCasterMaterial);
			}

			ShadowCameraSetupPtr currentShadowCameraSetup;
			if(m_ShadowProjType == "LiSPSM")
			{
				m_LiSPSMSetup = new LiSPSMShadowCameraSetup();
				currentShadowCameraSetup = ShadowCameraSetupPtr(m_LiSPSMSetup);
				m_LiSPSMSetup->setOptimalAdjustFactor(m_OptimalAdjustFactor);
				m_LiSPSMSetup->setUseAggressiveFocusRegion(m_UseAggressiveFocusRegion);
				m_LiSPSMSetup->setCameraLightDirectionThreshold(Ogre::Degree( 10));
			}
			else if (m_ShadowProjType == "Uniform")
			{
				currentShadowCameraSetup = ShadowCameraSetupPtr(new DefaultShadowCameraSetup());
				m_SceneMgr->setShadowCasterRenderBackFaces(false);

			}
			else if (m_ShadowProjType == "UniformFocused")
			{
				Ogre::FocusedShadowCameraSetup* fscs = new Ogre::FocusedShadowCameraSetup();
				fscs->setUseAggressiveFocusRegion(m_UseAggressiveFocusRegion);
				currentShadowCameraSetup = ShadowCameraSetupPtr(fscs);

			}
			else
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Undefined projection " + m_ShadowProjType, "OgreGraphicsSceneManager::UpdateShadowSettings");

				
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




