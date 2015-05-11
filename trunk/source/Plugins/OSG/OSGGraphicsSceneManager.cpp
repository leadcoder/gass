/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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


#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
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
#include <osgGA/StateSetManipulator>
#include <osgGA/GUIEventHandler>




#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/Components/OSGCameraComponent.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGRenderWindow.h"
#include "Plugins/OSG/OSGViewport.h"
#include "Plugins/OSG/OSGDebugDraw.h"




namespace GASS
{
	OSGGraphicsSceneManager::OSGGraphicsSceneManager(void):  m_Fog(new osg::Fog()),
		m_FogStart (200),
		m_FogEnd (400),
		m_UseFog(1),
		m_FogMode ("Linear"),
		m_FogDensity ( 0.01),
		m_FogColor(1,1,1),
		m_AmbientColor(0.5,0.5,0.5)
	{
	}

	OSGGraphicsSceneManager::~OSGGraphicsSceneManager(void)
	{
		if(m_ShadowedScene.valid())
			m_ShadowedScene->setShadowTechnique(0);
	}

	void OSGGraphicsSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register("OSGGraphicsSceneManager",new GASS::Creator<OSGGraphicsSceneManager, ISceneManager>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("OSG Scene Manager", OF_VISIBLE)));
		RegisterProperty<std::string>( "FogMode", &GetFogMode, &SetFogMode,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>( "FogStart", &GetFogStart, &SetFogStart,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>( "FogEnd", &GetFogEnd, &SetFogEnd,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>( "FogDensity", &GetFogDensity, &SetFogDensity,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec3>( "FogColor", &GetFogColor, &SetFogColor,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec3>( "AmbientColor", &GetAmbientColor, &SetAmbientColor,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
	}

	void OSGGraphicsSceneManager::OnCreate()
	{
		m_GFXSystem = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<OSGGraphicsSystem>();
		ScenePtr scene = GetScene();
		if(scene)
		{
			//scene->RegisterForMessage(REG_TMESS(OSGGraphicsSceneManager::OnSceneObjectCreated,SceneObjectCreatedNotifyMessage,Scene::GFX_COMPONENT_LOAD_PRIORITY));
			//scene->RegisterForMessage(REG_TMESS(OSGGraphicsSceneManager::OnChangeCamera,ChangeCameraRequest,0));
		}
		else
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Scene not present", "OSGGraphicsSceneManager::OnInitialize");
		}

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

			//only used by ViewDependentShadowMap
			osgShadow::ShadowSettings* settings = m_ShadowedScene->getShadowSettings();
			settings->setReceivesShadowTraversalMask(NM_RECEIVE_SHADOWS);
			settings->setCastsShadowTraversalMask(NM_CAST_SHADOWS);
			settings->setComputeNearFarModeOverride(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
			settings->setMaximumShadowMapDistance(200.0);
			
			//settings->setShadowMapProjectionHint(osgShadow::ShadowSettings::PERSPECTIVE_SHADOW_MAP);

			unsigned int unit=6;
			settings->setBaseShadowTextureUnit(unit);

			double n=0.8;
			settings->setMinimumShadowMapNearFarRatio(n);
			

			unsigned int numShadowMaps = 2;
			settings->setNumShadowMapsPerLight(numShadowMaps);

			//settings->setMultipleShadowMapHint(osgShadow::ShadowSettings::PARALLEL_SPLIT);
			settings->setMultipleShadowMapHint(osgShadow::ShadowSettings::CASCADED);

			int mapres = 2048;
			settings->setTextureSize(osg::Vec2s(mapres,mapres));
			

			m_ShadowedScene->setShadowTechnique(st);
			m_RootNode->addChild(m_ShadowedScene);
		}

		osg::StateSet* state = m_RootNode->getOrCreateStateSet();
		state->setAttributeAndModes(m_Fog.get());

		UpdateFogSettings();

		//add debug node
		m_DebugDraw = new OSGDebugDraw();
		m_RootNode->addChild(m_DebugDraw->GetNode());
	}

	void OSGGraphicsSceneManager::OnInit()
	{
		//support debug messages
		GetScene()->RegisterForMessage(REG_TMESS(OSGGraphicsSceneManager::OnDrawLine,DrawLineRequest ,0));

		void* root = static_cast<void*>(m_RootNode.get());
		void* shadow_node = static_cast<void*>(GetOSGShadowRootNode().get());

		SystemMessagePtr loaded_msg(new GraphicsSceneManagerLoadedEvent(std::string("OSG"),root,shadow_node));
		SimSystemManagerPtr sim_sm = OSGGraphicsSystemPtr(m_GFXSystem)->GetSimSystemManager();
		sim_sm->SendImmediate(loaded_msg);
	
		OSGGraphicsSystemPtr system =  SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<OSGGraphicsSystem>();
		if(system == NULL)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find OSGGraphicsSystem", "OSGGraphicsSceneManager::OnLoad");
		system->Register(shared_from_this());
	}

	void OSGGraphicsSceneManager::OnShutdown()
	{

	}

	void OSGGraphicsSceneManager::UpdateFogSettings()
	{
		m_Fog->setColor(osg::Vec4(m_FogColor.x, m_FogColor.y, m_FogColor.z,1));
		m_Fog->setDensity(m_FogDensity);
		m_Fog->setEnd(m_FogEnd);
		m_Fog->setStart(m_FogStart);

		if(m_RootNode)
		{
			osg::StateSet* state = m_RootNode->getOrCreateStateSet();
			short attr = osg::StateAttribute::ON;
			state->setMode(GL_FOG, attr);
		}

		if(m_FogMode == "Linear")
		{
			m_Fog->setMode(osg::Fog::LINEAR);
		}
		else if(m_FogMode == "Exp")
		{
			m_Fog->setMode(osg::Fog::EXP);
		}
		else if(m_FogMode == "Exp2")
		{
			m_Fog->setMode(osg::Fog::EXP2);
		}
		else if(m_FogMode == "None")
		{
			if(m_RootNode)
			{
				osg::StateSet* state = m_RootNode->getOrCreateStateSet();
				short attr = osg::StateAttribute::OFF;
				state->setMode(GL_FOG, attr);
				m_RootNode->setStateSet(state);
			}
		}
			
	}

	void OSGGraphicsSceneManager::DrawLine(const Vec3 &start_point, const Vec3 &end_point, const ColorRGBA &start_color , const ColorRGBA &end_color)
	{
		m_DebugDraw->DrawLine(start_point, end_point, start_color, end_color);
	}
	void OSGGraphicsSceneManager::OnDrawLine(DrawLineRequestPtr message)
	{
		DrawLine(message->GetStart(), message->GetEnd(), message->GetColorStart(), message->GetColorEnd());
	}

	void OSGGraphicsSceneManager::SystemTick(double delta_time)
	{
		m_DebugDraw->Clear();
		//update tick subscribers
		BaseSceneManager::SystemTick(delta_time);
	}


	osg::ref_ptr<osg::Group> OSGGraphicsSceneManager::GetOSGShadowRootNode()
	{
		//return osg::ref_ptr<osg::Group>(static_cast<osg::Group*>(m_ShadowedScene.get()));
		if(m_ShadowedScene.valid())
			return m_ShadowedScene;
		return m_RootNode;
	}

}




