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

#include "GASSOgreLightComponent.h"
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreLight.h>
#include <OgreRoot.h>
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSMath.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Plugins/Ogre/GASSOgreGraphicsSceneManager.h"
#include "Plugins/Ogre/GASSOgreConvert.h"
#include "Plugins/Ogre/Components/GASSOgreLocationComponent.h"

namespace GASS
{

	OgreLightComponent::OgreLightComponent(): m_OgreLight (NULL),
		m_LightType (LT_DIRECTIONAL),
		m_Diffuse(1,1,1),
		m_Specular(0,0,0),
		m_AttenuationParams(600, 1.0, 0.007, 0.0002),
		m_CastShadow(true),
		m_SpotParams(30,40,1)
	{

	}

	OgreLightComponent::~OgreLightComponent()
	{

	}

	void OgreLightComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<OgreLightComponent>("LightComponent");
		ADD_DEPENDENCY("OgreLocationComponent")
			GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("CameraComponent", OF_VISIBLE)));
		RegisterGetSet("LightType", &GASS::OgreLightComponent::GetLightType, &GASS::OgreLightComponent::SetLightType, PF_VISIBLE | PF_EDITABLE, "Light type");
		RegisterGetSet("AttenuationParmas", &GASS::OgreLightComponent::GetAttenuationParams, &GASS::OgreLightComponent::SetAttenuationParams,PF_VISIBLE | PF_EDITABLE,"Attenuation Parameters: Distance, Constant, Linear, Quad");
		RegisterGetSet("SpotlightParams", &GASS::OgreLightComponent::GetSpotParams, &GASS::OgreLightComponent::SetSpotParams,PF_VISIBLE | PF_EDITABLE,"Spotlight Parameters: Inner cone, Outer cone, falloff");
		RegisterGetSet("LightDir", &GASS::OgreLightComponent::GetDir, &GASS::OgreLightComponent::SetDir);
		RegisterGetSet("CastShadow", &GASS::OgreLightComponent::GetCastShadow, &GASS::OgreLightComponent::SetCastShadow,PF_VISIBLE | PF_EDITABLE,"CastShadow");
		RegisterGetSet("DiffuseColor", &GASS::OgreLightComponent::GetDiffuse, &GASS::OgreLightComponent::SetDiffuse,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("SpecularColor", &GASS::OgreLightComponent::GetSpecular, &GASS::OgreLightComponent::SetSpecular,PF_VISIBLE | PF_EDITABLE,"");
		RegisterMember("AmbientColor", &GASS::OgreLightComponent::m_AmbientColor);
	}

	void OgreLightComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLightComponent::OnLocationLoaded,LocationLoadedEvent,1));
	}

	void OgreLightComponent::OnDelete()
	{
		Ogre::SceneManager* sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<IOgreSceneManagerProxy>()->GetOgreSceneManager();
		if(sm && m_OgreLight)
			sm->destroyLight(m_OgreLight);
	}

	void OgreLightComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		OgreGraphicsSceneManagerPtr ogsm =  GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OgreGraphicsSceneManager>();
		assert(ogsm);
		Ogre::SceneManager* sm = ogsm->GetOgreSceneManager();

		OgreLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OgreLocationComponent>();

		static unsigned int obj_id = 0;
		obj_id++;
		std::stringstream ss;
		std::string name;
		ss << GetName() << obj_id;
		ss >> name;

		m_OgreLight = sm->createLight(name);

		SetLightType(m_LightType);
		SetAttenuationParams(m_AttenuationParams);
		SetCastShadow(m_CastShadow);
		SetDiffuse(m_Diffuse);
		SetSpecular(m_Specular);
		SetSpotParams(m_SpotParams);

		m_OgreLight->setVisible(true);
		m_OgreLight->setPosition(Ogre::Vector3::ZERO);
		m_OgreLight->setShadowNearClipDistance( 1 );
		m_OgreLight->setShadowFarClipDistance( 3000 );
		//m_OgreLight->setDirection(0,0,-1);
		lc->GetOgreNode()->attachObject(m_OgreLight);
	}


	void OgreLightComponent::SetLightType(LightTypeBinder lt)
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
			m_OgreLight->setAttenuation(static_cast<Ogre::Real>(params.x), static_cast<Ogre::Real>(params.y), static_cast<Ogre::Real>(params.z), static_cast<Ogre::Real>(params.w));
		}
	}

	void OgreLightComponent::SetDir(const Vec3 &dir)
	{
		if(m_OgreLight)
			m_OgreLight->setDirection(OgreConvert::ToOgre(dir));
	}
	Vec3 OgreLightComponent::GetDir() const
	{
		Vec3 dir(0,0,1);
		if(m_OgreLight)
		{
			const Ogre::Vector3 odir = m_OgreLight->getDirection();
			dir.Set(odir.x,odir.y,odir.z);
		}
		return dir;
	}

	void OgreLightComponent::SetSpotParams(const Vec3 &params)
	{
		m_SpotParams = params;

		if(m_OgreLight && m_LightType == LT_SPOT)
		{
			m_OgreLight->setSpotlightRange(Ogre::Radian(static_cast<Ogre::Real>(Math::Deg2Rad(params.x))), 
										   Ogre::Radian(static_cast<Ogre::Real>(Math::Deg2Rad(params.y))), static_cast<Ogre::Real>(params.z));
		}
	}

	void OgreLightComponent::SetDiffuse(const ColorRGB &diffuse)
	{
		m_Diffuse = diffuse;
		if(m_OgreLight)
			m_OgreLight->setDiffuseColour(OgreConvert::ToOgre(m_Diffuse));

	}
	void OgreLightComponent::SetSpecular(const ColorRGB &specular)
	{
		m_Specular = specular;
		if(m_OgreLight)
			m_OgreLight->setSpecularColour(OgreConvert::ToOgre(m_Specular));

	}
	void OgreLightComponent::SetCastShadow(bool value)
	{
		m_CastShadow = value;
		if(m_OgreLight)
			m_OgreLight->setCastShadows(m_CastShadow);
	}
}