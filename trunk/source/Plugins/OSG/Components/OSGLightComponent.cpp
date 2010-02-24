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

#include <boost/bind.hpp>


#include "OSGLightComponent.h"
#include "Core/Math/Quaternion.h"
#include "Core/Utils/Log.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/Components/OSGLocationComponent.h"


namespace GASS
{

	OSGLightComponent::OSGLightComponent(): 	m_LightType (LT_DIRECTIONAL),
		m_Diffuse(1,1,1),
		m_Specular(0,0,0),
		m_AttenuationParams(0,1,0,20),
		m_CastShadow(true),
		m_SpotParams(1,30,40),
		m_LightId(0)
	{

	}

	OSGLightComponent::~OSGLightComponent()
	{

	}

	void OSGLightComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("LightComponent",new Creator<OSGLightComponent, IComponent>);
		RegisterProperty<LightType>("LightType", &GASS::OSGLightComponent::GetLightType, &GASS::OSGLightComponent::SetLightType);
		

		RegisterProperty<Vec4>("AttenuationParmas", &GASS::OSGLightComponent::GetAttenuationParams, &GASS::OSGLightComponent::SetAttenuationParams);
		RegisterProperty<Vec3>("SpotlightParams", &GASS::OSGLightComponent::GetSpotParams, &GASS::OSGLightComponent::SetSpotParams);
		RegisterProperty<bool>("CastShadow", &GASS::OSGLightComponent::GetCastShadow, &GASS::OSGLightComponent::SetCastShadow);
		RegisterProperty<int>("LightId", &GASS::OSGLightComponent::GetLightId, &GASS::OSGLightComponent::SetLightId);

		RegisterProperty<Vec3>("DiffuseLightColor", &GASS::OSGLightComponent::GetDiffuse, &GASS::OSGLightComponent::SetDiffuse);
		RegisterProperty<Vec3>("SpecularLightColor", &GASS::OSGLightComponent::GetSpecular, &GASS::OSGLightComponent::SetSpecular);
	}

	void OSGLightComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLightComponent::OnLoad,LoadGFXComponentsMessage,1));
	}


	void OSGLightComponent::SetLightType(LightType lt)
	{
		m_LightType = lt;
		/*if(m_OgreLight)
		{
		if(lt == LT_DIRECTIONAL)
		m_OgreLight->setType(Ogre::Light::LT_DIRECTIONAL);
		else if(m_LightType == LT_POINT)
		m_OgreLight->setType(Ogre::Light::LT_POINT);
		else if(m_LightType == LT_SPOT)
		m_OgreLight->setType(Ogre::Light::LT_SPOTLIGHT);
		}*/
	}

	void OSGLightComponent::SetAttenuationParams(const Vec4 &params)
	{
		m_AttenuationParams = params;
		/*if(m_OgreLight)
		{
		m_OgreLight->setAttenuation(params.x, params.y, params.z, params.w);
		}*/
	}

	void OSGLightComponent::SetSpotParams(const Vec3 &params)
	{
		m_SpotParams = params;

		if(m_OSGLight.valid() && m_LightType == LT_SPOT)
		{
			m_OSGLight->setSpotCutoff(params.x);
			//if(m_OSGLight->setSpotlightRange(Ogre::Radian(Math::Deg2Rad(params.x)), Ogre::Radian(Math::Deg2Rad(params.y)), params.z);
		}
	}

	void OSGLightComponent::SetDiffuse(const Vec3 &diffuse)
	{
		m_Diffuse = diffuse;
		if(m_OSGLight.valid())
			m_OSGLight->setDiffuse(osg::Vec4(m_Diffuse.x,m_Diffuse.y,m_Diffuse.z,1.0f));

	}
	void OSGLightComponent::SetSpecular(const Vec3 &specular)
	{
		m_Specular = specular;
		//if(m_OgreLight)
		//	m_OgreLight->setSpecularColour(m_Specular.x,m_Specular.y,m_Specular.z);

	}
	void OSGLightComponent::SetCastShadow(bool value)
	{
		m_CastShadow = value;
		//if(m_OgreLight)
		//	m_OgreLight->setCastShadows(m_CastShadow);
	}

	void OSGLightComponent::SetLightId(int id)
	{
		m_LightId = id;
		if(m_OSGLight.valid())
			m_OSGLight->setLightNum(m_LightId);
	}

	void OSGLightComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		osg::ref_ptr<osg::LightSource> lightS2 = new osg::LightSource;    
		  //m_OSGLight = new osg::Light;
			m_OSGLight = lightS2->getLight();
		  SetLightId(m_LightId);

		  m_OSGLight->setPosition(osg::Vec4(0.0,0.0,0.0,0.0f));
		  m_OSGLight->setAmbient(osg::Vec4(0.1f,0.1f,0.1f,0.1f));
		  m_OSGLight->setSpecular(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
		//  m_OSGLight->setConstantAttenuation(1.0f);
		  
		  SetDiffuse(m_Diffuse);
		  
		  //m_OSGLight->setDirection(osg::Vec3(0.0f,1.0f,0.0f));


		  
		  //lightS2->setLight(m_OSGLight);
		  //lightS2->setLocalStateSetModes(osg::StateAttribute::ON); 

		  OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponent<OSGLocationComponent>();
		  //lc->GetOSGNode()->addChild(lightS2);

		  OSGGraphicsSceneManagerPtr  scene_man = boost::shared_dynamic_cast<OSGGraphicsSceneManager>(message->GetGFXSceneManager());
			osg::ref_ptr<osg::PositionAttitudeTransform> root_node = scene_man->GetOSGRootNode();
			root_node ->addChild(lightS2);
		
    
    //myLight2->setDiffuse(osg::Vec4(0.0f,1.0f,1.0f,1.0f));
    //myLight2->setConstantAttenuation(1.0f);
    //myLight2->setLinearAttenuation(2.0f/modelSize);
    //myLight2->setQuadraticAttenuation(2.0f/osg::square(modelSize));

		/*OSGGraphicsSceneManagerPtr ogsm = boost::shared_static_cast<OgreGraphicsSceneManager>(message->GetGFXSceneManager());
		//assert(ogsm);
		Ogre::SceneManager* sm = ogsm->GetSceneManger();

		OgreLocationComponentPtr lc = GetSceneObject()->GetFirstComponent<OgreLocationComponent>();
		m_OgreLight = sm->createLight(m_Name);
		lc->GetOgreNode()->attachObject(m_OgreLight);


		SetLightType(m_LightType);
		SetAttenuationParams(m_AttenuationParams);
		SetCastShadow(m_CastShadow);
		SetDiffuse(m_Diffuse);
		SetSpecular(m_Specular);
		SetSpotParams(m_SpotParams);

		m_OgreLight->setVisible(true);
		m_OgreLight->setPosition(Ogre::Vector3::ZERO);
		//m_OgreLight->setDirection(0,1,0);*/

	}
}
