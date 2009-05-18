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
#include "OgreLightComponent.h"
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreLight.h>


#include "Core/Math/Quaternion.h"
#include "Core/Utils/Log.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/OgreConvert.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"


namespace GASS
{

	OgreLightComponent::OgreLightComponent():
		m_OgreLight (NULL),
		m_LightType (LT_DIRECTIONAL),
		m_Diffuse(1,1,1),
		m_Specular(0,0,0),
		m_AttenuationParams(0,1,0,20),
		m_CastShadow(true),
		m_SpotParams(1,30,40)
	{

	}

	OgreLightComponent::~OgreLightComponent() 
	{

	}

	void OgreLightComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("LightComponent",new Creator<OgreLightComponent, IComponent>);
		RegisterProperty<LightType>("LightType", &GetLightType, &SetLightType);

		RegisterProperty<Vec4>("AttenuationParmas", &GetAttenuationParams, &SetAttenuationParams);
		RegisterProperty<Vec3>("SpotlightParams", &GetSpotParams, &SetSpotParams);
		RegisterProperty<bool>("CastShadow", &GetCastShadow, &SetCastShadow);

		RegisterProperty<Vec3>("DiffuseLightColor", &GetDiffuse, &SetDiffuse);
		RegisterProperty<Vec3>("SpecularLightColor", &GetSpecular, &SetSpecular);
	}

	void OgreLightComponent::OnCreate()
	{
		int obj_id = (int) this;
		MessageManager * mm = GetMessageManager();
		mm->RegisterForMessage(ScenarioScene::SM_MESSAGE_LOAD_GFX_COMPONENTS, obj_id,  boost::bind( &OgreLightComponent::OnLoad, this, _1 ),1);
	}


	void OgreLightComponent::SetLightType(LightType lt)
	{
		m_LightType = lt;
		if(m_OgreLight)
		{
			if(lt == LT_DIRECTIONAL)
				m_OgreLight->setType(Ogre::Light::LT_DIRECTIONAL);
			else if(m_LightType == LT_POINT)
				m_OgreLight->setType(Ogre::Light::LT_POINT);
			else if(m_LightType == LT_SPOT)
				m_OgreLight->setType(Ogre::Light::LT_SPOTLIGHT);
		}
	}

	void OgreLightComponent::SetAttenuationParams(const Vec4 &params)
	{
		m_AttenuationParams = params;
		if(m_OgreLight)
		{
			m_OgreLight->setAttenuation(params.x, params.y, params.z, params.w);
		}
	}

	void OgreLightComponent::SetSpotParams(const Vec3 &params)
	{
		m_SpotParams = params;

		if(m_OgreLight && m_LightType == LT_SPOT)
		{
			m_OgreLight->setSpotlightRange(Ogre::Radian(Math::Deg2Rad(params.x)), Ogre::Radian(Math::Deg2Rad(params.y)), params.z);
		}
	}

	void OgreLightComponent::SetDiffuse(const Vec3 &diffuse)
	{
		m_Diffuse = diffuse;
		if(m_OgreLight)
			m_OgreLight->setDiffuseColour(m_Diffuse.x,m_Diffuse.y,m_Diffuse.z);

	}
	void OgreLightComponent::SetSpecular(const Vec3 &specular)
	{
		m_Specular = specular;
		if(m_OgreLight)
			m_OgreLight->setSpecularColour(m_Specular.x,m_Specular.y,m_Specular.z);

	}
	void OgreLightComponent::SetCastShadow(bool value)
	{
		m_CastShadow = value;
		if(m_OgreLight) 
			m_OgreLight->setCastShadows(m_CastShadow);
	}

	void OgreLightComponent::OnLoad(MessagePtr message)
	{
		OgreGraphicsSceneManager* ogsm = boost::any_cast<OgreGraphicsSceneManager*>(message->GetData("GraphicsSceneManager"));
		assert(ogsm);
		Ogre::SceneManager* sm = ogsm->GetSceneManger();

		OgreLocationComponent * lc = GetSceneObject()->GetFirstComponent<OgreLocationComponent>().get();
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
		//m_OgreLight->setDirection(0,1,0);
		
	}
}
