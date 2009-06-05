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
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectTemplate.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/OgrePostProcess.h"
#include "Plugins/Ogre/OgreGraphicsSystem.h"
#include "Plugins/Ogre/Components/OgreCameraComponent.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"
#include "Core/System/SystemFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Core/Utils/Log.h"
#include <boost/bind.hpp>
#include <OgreRoot.h>
#include <OgreRenderSystem.h>
#include <OgreRenderWindow.h>
#include <OgreShadowCameraSetupLiSPSM.h>
#include <OgreShadowCameraSetupPlaneOptimal.h>

using namespace Ogre;

namespace GASS
{
	OgreGraphicsSceneManager::OgreGraphicsSceneManager(void)
	{
		m_FogStart = 200;
		m_FogEnd = 400;
		m_UseFog = 1;
		m_FogMode = "Linear";
		m_FogDensity = 0.01;
		m_FogColor.Set(1,1,1);
		m_AmbientColor.Set(1,1,1);
		
		//Shadows
		m_ShadowType = "";
		m_ShadowCasterMaterial = "";
		m_ShadowProjType = "LISPM";
		m_TextureShadowSize = 1024;
		m_NumShadowTextures = 1;
		m_SelfShadowing = false;
		m_OptimalAdjustFactor = 1;
		m_FarShadowDistance = 100;

		m_SkyboxMaterial = "";
		m_SceneManagerType = "TerrainSceneManager";
		m_SceneMgr = NULL;
	}

	OgreGraphicsSceneManager::~OgreGraphicsSceneManager(void)
	{
	
	}

	void OgreGraphicsSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register("OgreGraphicsSceneManager",new GASS::Creator<OgreGraphicsSceneManager, ISceneManager>);
		RegisterProperty<float>( "FogStart", &GetFogStart, &SetFogStart);
		RegisterProperty<float>( "FogEnd", &GetFogEnd, &SetFogEnd);
		RegisterProperty<float>( "FogDensity", &GetFogDensity, &SetFogDensity);
		RegisterProperty<Vec3>( "FogColor", &GetFogColor, &SetFogColor);
		RegisterProperty<Vec3>( "AmbientColor", &GetAmbientColor, &SetAmbientColor);
		RegisterProperty<std::string>("SceneManagerType", &GetSceneManagerType, &SetSceneManagerType);
		RegisterProperty<std::string>("SkyboxMaterial", &GetSkyboxMaterial, &SetSkyboxMaterial);
		RegisterProperty<bool> ("SelfShadowing", &GetSelfShadowing ,&SetSelfShadowing );
		RegisterProperty<float> ("FarShadowDistance", &GetFarShadowDistance,&SetFarShadowDistance);
		RegisterProperty<float> ("OptimalAdjustFactor", &GetOptimalAdjustFactor,&SetOptimalAdjustFactor);
		RegisterProperty<int>("NumShadowTextures",&GetNumShadowTextures,&SetNumShadowTextures);
		RegisterProperty<int>("TextureShadowSize",&GetTextureShadowSize,&SetTextureShadowSize);
		RegisterProperty<std::string>("ShadowProjType",&GetShadowProjType,&SetShadowProjType);
		RegisterProperty<std::string>("ShadowType",&GetShadowType,&SetShadowType);
		RegisterProperty<std::string>("ShadowCasterMaterial",&GetShadowCasterMaterial,&SetShadowCasterMaterial);
	}

	void OgreGraphicsSceneManager::OnCreate()
	{
		int address = (int) this;
		m_GFXSystem = SimEngine::GetPtr()->GetSystemManager()->GetFirstSystem<OgreGraphicsSystem>();
		
		m_Scene->RegisterForMessage(ScenarioScene::SCENARIO_MESSAGE_LOAD_SCENE_OBJECT, MESSAGE_FUNC( OgreGraphicsSceneManager::OnLoadSceneObject ),ScenarioScene::GFX_COMPONENT_LOAD_PRIORITY);
		m_Scene->RegisterForMessage(ScenarioScene::SCENARIO_MESSAGE_LOAD_SCENE_MANAGERS,  MESSAGE_FUNC( OgreGraphicsSceneManager::OnLoad ),ScenarioScene::GFX_SYSTEM_LOAD_PRIORITY);
		m_Scene->RegisterForMessage(ScenarioScene::SCENARIO_MESSAGE_UNLOAD_SCENE_MANAGERS,  MESSAGE_FUNC(OgreGraphicsSceneManager::OnUnload));
		m_Scene->RegisterForMessage(ScenarioScene::SCENARIO_MESSAGE_CHANGE_CAMERA,  MESSAGE_FUNC(OgreGraphicsSceneManager::OnChangeCamera));
		
	}

	void OgreGraphicsSceneManager::OnUnload(MessagePtr message)
	{
		if(m_SceneMgr)
		{
			m_SceneMgr->clearScene();
			//m_MainWindow->removeAllViewports();
			Root::getSingleton().destroySceneManager(m_SceneMgr);
		}
	}
	
	void OgreGraphicsSceneManager::OnLoad(MessagePtr message)
	{
		m_SceneMgr = Root::getSingleton().createSceneManager(m_SceneManagerType, m_Name);
		if(m_SceneMgr == NULL) 
			GASS::Log::Error("SceneManager %s not found",m_SceneManagerType.c_str());
		UpdateShadowSettings();
		UpdateSkySettings();
		UpdateLightSettings();
		UpdateFogSettings();
		OgreGraphicsSystemPtr(m_GFXSystem)->SetActiveSceneManger(m_SceneMgr);

		// Try to load default camera
		SceneObjectPtr scene_object = m_Scene->GetObjectManager()->LoadFromTemplate("FreeCameraObject");

		if(!scene_object) //If no FreeCameraObject template found, create one
		{
			SceneObjectTemplatePtr fre_cam_template (new SceneObjectTemplate);
			fre_cam_template->SetName("FreeCameraObject");
			ComponentPtr location_comp (ComponentFactory::Get().Create("LocationComponent"));
			location_comp->SetName("LocationComp");
	
			ComponentPtr camera_comp (ComponentFactory::Get().Create("CameraComponent"));
			camera_comp->SetName("FreeCameraComp");

			ComponentPtr cc_comp (ComponentFactory::Get().Create("FreeCamControlComponent"));
			cc_comp->SetName("FreeCameraCtrlComp");

			fre_cam_template->AddComponent(location_comp);
			fre_cam_template->AddComponent(camera_comp);
			fre_cam_template->AddComponent(cc_comp);

			SimEngine::Get().GetSimObjectManager()->AddTemplate(fre_cam_template);

			scene_object = m_Scene->GetObjectManager()->LoadFromTemplate("FreeCameraObject");

		}
			
		
		assert(scene_object);
		//SceneObject* scene_object = SimEngine::Get().GetSceneObjectTemplateManager()->CreateFromTemplate("FreeCameraObject");
		//OgreCameraComponent* cam_comp = scene_object->GetFirstComponent<OgreCameraComponent>();
		//OgreLocationComponent* loc_comp = scene_object->GetFirstComponent<OgreLocationComponent>();
		//loc_comp->SetPosition(GetOwner()->GetStartPos());
		//loc_comp->SetEulerRotation(GetOwner()->GetStartRot());
		//m_Scene->GetObjectManager()->LoadObject(scene_object);
		

		//Send message to load all gfx components
		
		//m_GFXSystem->m_Window->getViewport(0)->setCamera(cam_comp->GetOgreCamera());
		MessagePtr camera_msg(new Message(ScenarioScene::SCENARIO_MESSAGE_CHANGE_CAMERA,(int) this));
		camera_msg->SetData("CameraObject",scene_object);
		m_Scene->SendImmediate(camera_msg);

		//move camera to spawn position
		MessagePtr pos_msg(new Message(SceneObject::OBJECT_MESSAGE_POSITION,(int) this));
		pos_msg->SetData("Position",GetOwner()->GetStartPos());
		scene_object->SendImmediate(pos_msg);
		
		/*MessagePtr rot_msg(new Message(ScenarioScene::OBJECT_MESSAGE_EULER_ROTATION,(int) this));
		rot_msg->SetData("EulerRotation",GetOwner()->GetStartRot());
		scene_object->SendImmediate(rot_msg);*/

		MessagePtr loaded_msg(new Message(SimSystemManager::SYSTEM_MESSAGE_GFX_SM_LOADED,(int) this));
		
		//Give hook to 3dparty plugins to attach, maybee send other info
		void* root = static_cast<void*>(m_SceneMgr->getRootSceneNode());
		loaded_msg->SetData("RootNode",boost::any(root));
		loaded_msg->SetData("RenderSystem",boost::any(std::string("Ogre3D")));
		SimSystemManager* sim_sm = static_cast<SimSystemManager*>(OgreGraphicsSystemPtr(m_GFXSystem)->GetOwner());
		sim_sm->SendImmediate(loaded_msg);
	
	}

	void OgreGraphicsSceneManager::OnChangeCamera(MessagePtr message)
	{
		SceneObjectPtr cam_obj = boost::any_cast<SceneObjectPtr>(message->GetData("CameraObject"));
		OgreCameraComponentPtr cam_comp = cam_obj->GetFirstComponent<OgreCameraComponent>();
		OgreGraphicsSystemPtr(m_GFXSystem)->m_Window->getViewport(0)->setCamera(cam_comp->GetOgreCamera());

		OgreGraphicsSystemPtr(m_GFXSystem)->GetPostProcess()->Update(cam_comp->GetOgreCamera());
	}

	void OgreGraphicsSceneManager::OnLoadSceneObject(MessagePtr message)
	{
		//Initlize all gfx components and send scene mananger as argument
		SceneObjectPtr obj = boost::any_cast<SceneObjectPtr>(message->GetData("SceneObject"));
		assert(obj);
		MessagePtr gfx_msg(new Message(SceneObject::OBJECT_MESSAGE_LOAD_GFX_COMPONENTS,(int) this));
		gfx_msg->SetData("GraphicsSceneManager",boost::any(this));
		gfx_msg->SetData("OgreSceneManager",boost::any(m_SceneMgr));
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
		//else if(m_FogMode == "None")
		//Set Light color
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
		/*for(int i = 0; i < m_GFXComponents.size(); i++)
		{
			m_GFXComponents[i]->OnUpdate(delta_time);
		}*/
		//update scene settings
	}


	void OgreGraphicsSceneManager::UpdateShadowSettings()
	{
		if(m_SceneMgr == NULL) return;
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
			}
			else if (m_ShadowProjType == "Uniform")
			{
				currentShadowCameraSetup = ShadowCameraSetupPtr(new DefaultShadowCameraSetup());
				m_SceneMgr->setShadowCasterRenderBackFaces(false);
			}
			else if (m_ShadowProjType == "UniformFocused")
				currentShadowCameraSetup = ShadowCameraSetupPtr(new Ogre::FocusedShadowCameraSetup());
			else
				Log::Error("Undefined projection %s",m_ShadowProjType.c_str());
		//	else if (m_ShadowProjType == "PlaneOptimal")
		//		currentShadowCameraSetup = Ogre::ShadowCameraSetupPtr(new Ogre::PlaneOptimalShadowCameraSetup(mPlane));
			m_SceneMgr->setShadowCameraSetup(currentShadowCameraSetup);
		}
		m_SceneMgr->setShadowFarDistance(m_FarShadowDistance);
	}
}




