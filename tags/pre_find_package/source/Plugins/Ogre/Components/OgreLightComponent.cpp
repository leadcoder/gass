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

#include "OgreLightComponent.h"
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreLight.h>
#include <OgreRoot.h>


#include "Core/Math/GASSQuaternion.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/OgreConvert.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include "Plugins/Ogre/IOgreSceneManagerProxy.h"


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

		ComponentFactory::GetPtr()->Register("LightComponent",new Creator<OgreLightComponent, Component>);
		ADD_DEPENDENCY("OgreLocationComponent")
			GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("CameraComponent", OF_VISIBLE)));
		RegisterProperty<LightTypeBinder>("LightType", &GASS::OgreLightComponent::GetLightType, &GASS::OgreLightComponent::SetLightType,
			EnumerationProxyPropertyMetaDataPtr(new EnumerationProxyPropertyMetaData("Light type",PF_VISIBLE,&LightTypeBinder::GetStringEnumeration)));

		RegisterProperty<Vec4>("AttenuationParmas", &GASS::OgreLightComponent::GetAttenuationParams, &GASS::OgreLightComponent::SetAttenuationParams,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Attenuation Parameters: Distance, Constant, Linear, Quad",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<Vec3>("SpotlightParams", &GASS::OgreLightComponent::GetSpotParams, &GASS::OgreLightComponent::SetSpotParams,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Spotlight Parameters: Inner cone, Outer cone, falloff",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<Vec3>("LightDir", &GASS::OgreLightComponent::GetDir, &GASS::OgreLightComponent::SetDir);
		RegisterProperty<bool>("CastShadow", &GASS::OgreLightComponent::GetCastShadow, &GASS::OgreLightComponent::SetCastShadow,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("CastShadow",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<ColorRGB>("DiffuseColor", &GASS::OgreLightComponent::GetDiffuse, &GASS::OgreLightComponent::SetDiffuse,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<ColorRGB>("SpecularColor", &GASS::OgreLightComponent::GetSpecular, &GASS::OgreLightComponent::SetSpecular,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
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
			m_OgreLight->setAttenuation(params.x, params.y, params.z, params.w);
		}
	}

	void OgreLightComponent::SetDir(const Vec3 &dir)
	{
		if(m_OgreLight)
			m_OgreLight->setDirection(dir.x,dir.y,dir.z);
	}
	Vec3 OgreLightComponent::GetDir() const
	{
		Vec3 dir;
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
			m_OgreLight->setSpotlightRange(Ogre::Radian(Math::Deg2Rad(params.x)), Ogre::Radian(Math::Deg2Rad(params.y)), params.z);
		}
	}

	void OgreLightComponent::SetDiffuse(const ColorRGB &diffuse)
	{
		m_Diffuse = diffuse;
		if(m_OgreLight)
			m_OgreLight->setDiffuseColour(m_Diffuse.r,m_Diffuse.g,m_Diffuse.b);

	}
	void OgreLightComponent::SetSpecular(const ColorRGB &specular)
	{
		m_Specular = specular;
		if(m_OgreLight)
			m_OgreLight->setSpecularColour(m_Specular.r,m_Specular.g,m_Specular.b);

	}
	void OgreLightComponent::SetCastShadow(bool value)
	{
		m_CastShadow = value;
		if(m_OgreLight)
			m_OgreLight->setCastShadows(m_CastShadow);
	}
}
