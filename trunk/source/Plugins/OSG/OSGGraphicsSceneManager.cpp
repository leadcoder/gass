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


#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowVolume>
#include <osgShadow/ShadowTexture>
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgShadow/LightSpacePerspectiveShadowMap>
#include <osgShadow/StandardShadowMap>

#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/Components/OSGCameraComponent.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/OSGNodeMasks.h"



namespace GASS
{
	OSGGraphicsSceneManager::OSGGraphicsSceneManager(void):  m_Fog(new osg::Fog()),
		m_FogStart (200),
		m_FogEnd (400),
		m_UseFog(1),
		m_FogMode ("Linear"),
		m_FogDensity ( 0.01),
		m_FogColor(1,1,1),
		m_AmbientColor(1,1,1)
	{
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
	}

	void OSGGraphicsSceneManager::OnCreate()
	{
		m_GFXSystem = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<OSGGraphicsSystem>();
		ScenarioPtr scenario = GetScenario();
		if(scenario)
		{
			scenario->RegisterForMessage(REG_TMESS(OSGGraphicsSceneManager::OnSceneObjectCreated,SceneObjectCreatedNotifyMessage,Scenario::GFX_COMPONENT_LOAD_PRIORITY));
			scenario->RegisterForMessage(REG_TMESS(OSGGraphicsSceneManager::OnLoad,LoadSceneManagersMessage,Scenario::GFX_SYSTEM_LOAD_PRIORITY));
			scenario->RegisterForMessage(REG_TMESS(OSGGraphicsSceneManager::OnUnload,UnloadSceneManagersMessage,0));
			scenario->RegisterForMessage(REG_TMESS(OSGGraphicsSceneManager::OnChangeCamera,ChangeCameraMessage,0));
		}
		else
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Scenario not present", "OSGGraphicsSceneManager::OnCreate");
		}
	}


	void OSGGraphicsSceneManager::OnLoad(MessagePtr message)
	{
		ScenarioPtr scenario = GetScenario();
		assert(scenario);
		//if(scenario->GetSceneUp().z > 0)
		//OSGConvert::Get().m_FlipYZ = false;
		//std::cout << "OSGGraphicsSceneManager::OnLoad Entered" << std::endl;

		m_RootNode = new osg::PositionAttitudeTransform();
		m_RootNode->setName("GASSRootNode");

		OSGGraphicsSystemPtr gfx_sys = OSGGraphicsSystemPtr(m_GFXSystem);
		osg::ref_ptr<osgShadow::ShadowTechnique>  st = gfx_sys->GetShadowTechnique();
		if(st.valid())
		{
			m_ShadowedScene = new osgShadow::ShadowedScene;
			m_ShadowedScene->setName("ShadowRootNode");
			m_ShadowedScene->setReceivesShadowTraversalMask(NM_RECEIVE_SHADOWS);
			m_ShadowedScene->setCastsShadowTraversalMask(NM_CAST_SHADOWS);
			m_ShadowedScene->setShadowTechnique(st);
			m_RootNode->addChild(m_ShadowedScene);
		}
		UpdateFogSettings();
		//add and enable fog
		osg::StateSet* state = m_RootNode->getOrCreateStateSet();
		state->setAttributeAndModes(m_Fog.get());
		short attr = osg::StateAttribute::ON;
		state->setMode(GL_FOG, attr);
		GetOSGShadowRootNode()->setStateSet(state);

		// Load default camera ect
		//std::cout << "OSGGraphicsSceneManager::OnLoad Create freecamera" << std::endl;
		/*SceneObjectPtr scene_object = scene->GetObjectManager()->LoadFromTemplate("FreeCameraObject");
		MessagePtr camera_msg(new ChangeCameraMessage(scene_object,));
		scene->SendImmediate(camera_msg);
		MessagePtr pos_msg(new PositionMessage(scene->GetStartPos()));
		scene_object->SendImmediate(pos_msg);*/

		void* root = static_cast<void*>(m_RootNode.get());
		void* shadow_node = static_cast<void*>(GetOSGShadowRootNode().get());

		MessagePtr loaded_msg(new GFXSceneManagerLoadedNotifyMessage(std::string("OSG"),root,shadow_node));
		SimSystemManagerPtr sim_sm = boost::shared_dynamic_cast<SimSystemManager>(OSGGraphicsSystemPtr(m_GFXSystem)->GetOwner());
		sim_sm->SendImmediate(loaded_msg);
		OSGGraphicsSystemPtr(m_GFXSystem)->SetActiveData(m_RootNode.get());


		OSGGraphicsSystemPtr system =  SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<OSGGraphicsSystem>();
		if(system == NULL)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find OSGGraphicsSystem", "OSGGraphicsSceneManager::OnLoad");
		system->Register(shared_from_this());
	}

	void OSGGraphicsSceneManager::OnUnload(MessagePtr message)
	{

	}

	void OSGGraphicsSceneManager::OnChangeCamera(ChangeCameraMessagePtr message)
	{
		SceneObjectPtr cam_obj = message->GetCamera();
		OSGCameraComponentPtr cam_comp = cam_obj->GetFirstComponentByClass<OSGCameraComponent>();

		OSGGraphicsSystemPtr(m_GFXSystem)->ChangeCamera(message->GetViewport(), cam_comp);
		/*osgViewer::ViewerBase::Views views;
		OSGGraphicsSystemPtr(m_GFXSystem)->GetViewer()->getViews(views);
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
		}*/

		MessagePtr cam_message(new CameraChangedNotifyMessage(cam_obj,cam_comp->GetOSGCamera()));
		GetScenario()->PostMessage(cam_message);
	}


	

	

	void OSGGraphicsSceneManager::OnSceneObjectCreated(SceneObjectCreatedNotifyMessagePtr message)
	{
		//Initlize all gfx components and send scene mananger as argument
		SceneObjectPtr obj = message->GetSceneObject();
		assert(obj);
		void* root = static_cast<void*>(m_RootNode.get());

		MessagePtr gfx_msg(new LoadGFXComponentsMessage(shared_from_this(),root));
		obj->SendImmediate(gfx_msg);
		
		//update scene data
		//OSGGraphicsSystemPtr(m_GFXSystem)->SetActiveData(m_RootNode.get());
	}

	void OSGGraphicsSceneManager::UpdateFogSettings()
	{
		m_Fog->setColor(osg::Vec4(m_FogColor.x, m_FogColor.y, m_FogColor.z,1));
		m_Fog->setDensity(m_FogDensity);
		m_Fog->setEnd(m_FogEnd);
		m_Fog->setStart(m_FogStart);
		if(m_FogMode == "Linear")
			m_Fog->setMode(osg::Fog::LINEAR);
		else if(m_FogMode == "Exp")
			m_Fog->setMode(osg::Fog::EXP);
		else if(m_FogMode == "Exp2")
			m_Fog->setMode(osg::Fog::EXP2);

	}


	osg::ref_ptr<osg::Group> OSGGraphicsSceneManager::GetOSGShadowRootNode()
	{
		//return osg::ref_ptr<osg::Group>(static_cast<osg::Group*>(m_ShadowedScene.get()));
		if(m_ShadowedScene.valid())
			return m_ShadowedScene;
		return m_RootNode;
	}


	void OSGGraphicsSceneManager::UpdateNodeMask(osg::Node* node, GeometryCategory category)
	{
		//reset 
		
		node->setNodeMask(~(NM_REGULAR_GEOMETRY | NM_TERRAIN_GEOMETRY | NM_GIZMO_GEOMETRY)  &  node->getNodeMask());
		switch(category.Get())
		{
		case GT_REGULAR:
			node->setNodeMask(NM_REGULAR_GEOMETRY | node->getNodeMask());
			break;
		case GT_TERRAIN:
			node->setNodeMask(NM_TERRAIN_GEOMETRY | node->getNodeMask());
			break;
		case GT_GIZMO:
			node->setNodeMask(NM_GIZMO_GEOMETRY | node->getNodeMask());
			break;
		}
	}

}




