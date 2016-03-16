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

#include "Plugins/OSG/OSGGraphicsSceneManager.h"
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
		m_FogMode (FM_LINEAR),
		m_FogDensity ( 0.01f),
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
		RegisterProperty<FogModeBinder>( "FogMode", &GASS::OSGGraphicsSceneManager::GetFogMode, &GASS::OSGGraphicsSceneManager::SetFogMode,
			EnumerationProxyPropertyMetaDataPtr(new EnumerationProxyPropertyMetaData("Fog type",PF_VISIBLE,&FogModeBinder::GetStringEnumeration)));
		RegisterProperty<float>( "FogStart", &OSGGraphicsSceneManager::GetFogStart, &OSGGraphicsSceneManager::SetFogStart,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>( "FogEnd", &OSGGraphicsSceneManager::GetFogEnd, &OSGGraphicsSceneManager::SetFogEnd,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>( "FogDensity", &OSGGraphicsSceneManager::GetFogDensity, &OSGGraphicsSceneManager::SetFogDensity,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec3>( "FogColor", &OSGGraphicsSceneManager::GetFogColor, &OSGGraphicsSceneManager::SetFogColor,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec3>( "AmbientColor", &OSGGraphicsSceneManager::GetAmbientColor, &OSGGraphicsSceneManager::SetAmbientColor,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
	}

	void OSGGraphicsSceneManager::OnCreate()
	{
		m_GFXSystem = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<OSGGraphicsSystem>();
		ScenePtr scene = GetScene();
		if(!scene)
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

			short mapres = 2048;
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
		m_Fog->setColor(osg::Vec4(static_cast<float>(m_FogColor.x), static_cast<float>(m_FogColor.y), static_cast<float>(m_FogColor.z),1));
		m_Fog->setDensity(m_FogDensity);
		m_Fog->setEnd(m_FogEnd);
		m_Fog->setStart(m_FogStart);

		if(m_RootNode)
		{
			osg::StateSet* state = m_RootNode->getOrCreateStateSet();
			short attr = osg::StateAttribute::ON;
			state->setMode(GL_FOG, attr);
		}

		switch(m_FogMode.GetValue())
		{
		case FM_LINEAR:
			m_Fog->setMode(osg::Fog::LINEAR);
			break;
		case FM_EXP:
			m_Fog->setMode(osg::Fog::EXP);
			break;
		case FM_EXP2:
			m_Fog->setMode(osg::Fog::EXP2);
			break;
		case FM_NONE:
			if(m_RootNode)
			{
				osg::StateSet* state = m_RootNode->getOrCreateStateSet();
				short attr = osg::StateAttribute::OFF;
				state->setMode(GL_FOG, attr);
				m_RootNode->setStateSet(state);
			}
			break;
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
		if(m_ShadowedScene.valid())
			return m_ShadowedScene;
		return m_RootNode;
	}
}

