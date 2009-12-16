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
#include <boost/bind.hpp>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgUtil/Optimizer>
#include <osgDB/ReadFile> 

#include "Core/Common.h"
#include "Core/System/SystemFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"

#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"

#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/Components/OSGCameraComponent.h"




namespace GASS
{
	OSGGraphicsSceneManager::OSGGraphicsSceneManager(void)
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
		m_SceneTransform = new osg::PositionAttitudeTransform();
	}

	OSGGraphicsSceneManager::~OSGGraphicsSceneManager(void)
	{
	
	}

	void OSGGraphicsSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register("OSGGraphicsSceneManager",new GASS::Creator<OSGGraphicsSceneManager, ISceneManager>);
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

	void OSGGraphicsSceneManager::OnCreate()
	{
		m_GFXSystem = SimEngine::GetPtr()->GetSystemManager()->GetFirstSystem<OSGGraphicsSystem>();
		ScenarioScenePtr scene = GetScenarioScene();
		if(scene)
		{
			scene->RegisterForMessage(SCENARIO_MESSAGE_CLASS(OSGGraphicsSceneManager::OnSceneObjectCreated,SceneObjectCreatedNotifyMessage,ScenarioScene::GFX_COMPONENT_LOAD_PRIORITY));
			scene->RegisterForMessage(SCENARIO_RM_LOAD_SCENE_MANAGERS, MESSAGE_FUNC(OSGGraphicsSceneManager::OnLoad),ScenarioScene::GFX_SYSTEM_LOAD_PRIORITY);
			scene->RegisterForMessage(SCENARIO_RM_UNLOAD_SCENE_MANAGERS, MESSAGE_FUNC(OSGGraphicsSceneManager::OnUnload),0);
			scene->RegisterForMessage(SCENARIO_MESSAGE_CLASS(OSGGraphicsSceneManager::OnChangeCamera,ChangeCameraMessage,0));
		}
		else
		{
			Log::Error("Scenario Scene not present in OSGGraphicsSceneManager::OnCreate");
		}

		m_SceneTransform->setAttitude(osg::Quat(Math::Deg2Rad(-90),osg::Vec3(1,0,0),
									     Math::Deg2Rad(180),osg::Vec3(0,1,0),
										 Math::Deg2Rad(0),osg::Vec3(0,0,1)));
	}

	void OSGGraphicsSceneManager::OnChangeCamera(ChangeCameraMessagePtr message)
	{
		SceneObjectPtr cam_obj = message->GetCamera();
		OSGCameraComponentPtr cam_comp = cam_obj->GetFirstComponent<OSGCameraComponent>();
		

		osgViewer::ViewerBase::Views views;
		OSGGraphicsSystemPtr(m_GFXSystem)->GetViewer()->getViews(views);
		//set same scene in all viewports for the moment 
		for(int i = 0; i < views.size(); i++)
		{
			if(views[i]->getCamera() != cam_comp->GetOSGCamera().get())
			{
				cam_comp->GetOSGCamera()->setGraphicsContext(views[i]->getCamera()->getGraphicsContext());
				cam_comp->GetOSGCamera()->setViewport(views[i]->getCamera()->getViewport());
				views[i]->getCamera()->setViewport(NULL);
				views[i]->getCamera()->setGraphicsContext(NULL);
				views[i]->setCamera(cam_comp->GetOSGCamera());
			}
		}

		MessagePtr cam_message(new CameraChangedNotifyMessage(cam_obj,cam_comp->GetOSGCamera()));
		GetScenarioScene()->PostMessage(cam_message);
	}



	void OSGGraphicsSceneManager::OnUnload(MessagePtr message)
	{

	}
	
	void OSGGraphicsSceneManager::OnLoad(MessagePtr message)
	{
		std::cout << "OSGGraphicsSceneManager::OnLoad Entered" << std::endl;
		m_RootNode = new osg::PositionAttitudeTransform();//new osg::Group();

	/*	m_RootNode->setAttitude(osg::Quat(Math::Deg2Rad(-90),osg::Vec3(1,0,0),
									     Math::Deg2Rad(180),osg::Vec3(0,1,0),
										 Math::Deg2Rad(0),osg::Vec3(0,0,1)));*/
		// Create green Irish sky
		osg::ClearNode* backdrop = new osg::ClearNode;
		backdrop->setClearColor(osg::Vec4(0.0f,0.8f,0.0f,1.0f));
		m_RootNode->addChild(backdrop);

		


		UpdateShadowSettings();
		UpdateSkySettings();
		UpdateLightSettings();
		UpdateFogSettings();

		ScenarioScenePtr scene = GetScenarioScene();
		assert(scene);
		//m_GFXSystem->SetActiveSceneManger(m_SceneMgr);

		// Load default camera ect
		std::cout << "OSGGraphicsSceneManager::OnLoad Create freecamera" << std::endl;
		SceneObjectPtr scene_object = scene->GetObjectManager()->LoadFromTemplate("FreeCameraObject");

		MessagePtr camera_msg(new ChangeCameraMessage(scene_object));
		scene->SendImmediate(camera_msg);
		MessagePtr pos_msg(new PositionMessage(scene->GetStartPos()));
		scene_object->SendImmediate(pos_msg);
		
		void* root = static_cast<void*>(m_RootNode.get());
		MessagePtr loaded_msg(new GFXSceneManagerLoadedNotifyMessage(std::string("OSG"),root));
		SimSystemManager* sim_sm = static_cast<SimSystemManager*>(OSGGraphicsSystemPtr(m_GFXSystem)->GetOwner());
		sim_sm->SendImmediate(loaded_msg);
		//osg::ref_ptr<osg::Group> mesh = (osg::Group*) osgDB::readNodeFile("C:/Root/Repo/filtema4/MSI-Projects/GASSData/scenarios/osg/nkpg/models/terrain/tempmaster.ive");
		//m_RootNode->addChild(mesh);
		OSGGraphicsSystemPtr(m_GFXSystem)->SetActiveData(m_RootNode.get());
	}

	void OSGGraphicsSceneManager::OnSceneObjectCreated(SceneObjectCreatedNotifyMessagePtr message)
	{
		//Initlize all gfx components and send scene mananger as argument
		SceneObjectPtr obj = message->GetSceneObject();
		assert(obj);
		MessagePtr gfx_msg(new LoadGFXComponentsMessage(shared_from_this(),NULL));
		obj->SendImmediate(gfx_msg);
		//update scene data
		//OSGGraphicsSystemPtr(m_GFXSystem)->SetActiveData(m_RootNode.get());
	}

	void OSGGraphicsSceneManager::UpdateFogSettings()
	{
		/*if(m_SceneMgr == NULL) return;
		ColourValue fogColour(m_FogColor.x, m_FogColor.y, m_FogColor.z);
		if(m_FogMode == "Linear")
		//	m_SceneMgr->setFog(Ogre::FOG_LINEAR, fogColour, m_FogDensity, m_FogStart, m_FogEnd);
		else if(m_FogMode == "Exp")
			m_SceneMgr->setFog(Ogre::FOG_EXP, fogColour, m_FogDensity, m_FogStart, m_FogEnd);
		else if(m_FogMode == "Exp2")
			m_SceneMgr->setFog(Ogre::FOG_EXP2, fogColour, m_FogDensity, m_FogStart, m_FogEnd);
		//else if(m_FogMode == "None")
		//Set Light color*/
	}

	void OSGGraphicsSceneManager::UpdateSkySettings()
	{
		/*if(m_SceneMgr == NULL) return;
		if(m_SkyboxMaterial != "")
			m_SceneMgr->setSkyBox(true, m_SkyboxMaterial, 50);*/
	}

	void OSGGraphicsSceneManager::UpdateLightSettings()
	{
		//if(m_SceneMgr == NULL) return;
		//	m_SceneMgr->setAmbientLight(ColourValue(m_AmbientColor.x, m_AmbientColor.y, m_AmbientColor.z));

	}

	void OSGGraphicsSceneManager::Update(double delta_time)
	{
		//boost::shared_ptr<UpdateMessage> update_msg = boost::static_pointer_cast<UpdateMessage>( message);
		//if(((int) update_msg->m_Tick) % 10 == 0)
		static int tick = 0;
		std::cout << " OSGGraphicsSceneManager -- tick:" << tick++ <<std::endl;
	}


	void OSGGraphicsSceneManager::UpdateShadowSettings()
	{
		/*if(m_SceneMgr == NULL) return;
		bool isOpenGL;
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
		m_SceneMgr->setShadowFarDistance(m_FarShadowDistance);*/
	}
}




