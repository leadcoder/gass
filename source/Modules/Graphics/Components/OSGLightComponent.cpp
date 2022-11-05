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

#include "OSGLightComponent.h"

#include <memory>
#include "Modules/Graphics/OSGGraphicsSceneManager.h"
#include "Modules/Graphics/Components/OSGLocationComponent.h"

namespace GASS
{
	OSGLightComponent::OSGLightComponent(): m_LightType (LT_DIRECTIONAL),
		m_Diffuse(1,1,1),
		m_Specular(1,1,1),
		m_Ambient(1,1,1),
		m_AttenuationParams(0,1,0,20),
		
		m_SpotParams(1,30,40)
		
	{

	}

	OSGLightComponent::~OSGLightComponent()
	{

	}

	void OSGLightComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<OSGLightComponent>("LightComponent");
		ADD_DEPENDENCY("OSGLocationComponent")
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("Component used for lights", OF_VISIBLE));

		RegisterGetSet("LightType", &GASS::OSGLightComponent::GetLightType, &GASS::OSGLightComponent::SetLightType, PF_VISIBLE | PF_EDITABLE, "Light Type");
		RegisterGetSet("AttenuationParmas", &GASS::OSGLightComponent::GetAttenuationParams, &GASS::OSGLightComponent::SetAttenuationParams,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("SpotlightParams", &GASS::OSGLightComponent::GetSpotParams, &GASS::OSGLightComponent::SetSpotParams,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("CastShadow", &GASS::OSGLightComponent::GetCastShadow, &GASS::OSGLightComponent::SetCastShadow,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("LightId", &GASS::OSGLightComponent::GetLightId, &GASS::OSGLightComponent::SetLightId,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("DiffuseColor", &GASS::OSGLightComponent::GetDiffuse, &GASS::OSGLightComponent::SetDiffuse,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("SpecularColor", &GASS::OSGLightComponent::GetSpecular, &GASS::OSGLightComponent::SetSpecular,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("AmbientColor", &GASS::OSGLightComponent::GetAmbient, &GASS::OSGLightComponent::SetAmbient,PF_VISIBLE | PF_EDITABLE,"");
	}

	void OSGLightComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLightComponent::OnLocationLoaded,LocationLoadedEvent,1));
	}

	void OSGLightComponent::SetLightType(LightTypeBinder lt)
	{
		m_LightType = lt;
	}

	void OSGLightComponent::SetAttenuationParams(const Vec4 &params)
	{
		m_AttenuationParams = params;
	}

	void OSGLightComponent::SetSpotParams(const Vec3 &params)
	{
		m_SpotParams = params;

		if(m_OSGLight.valid() && m_LightType == LT_SPOT)
		{
			m_OSGLight->setSpotCutoff(static_cast<float>(params.x));
			//TODO set more parameters like OGRE  "m_OSGLight->setSpotlightRange(Ogre::Radian(Math::Deg2Rad(params.x)), Ogre::Radian(Math::Deg2Rad(params.y)), params.z);"
		}
	}

	void OSGLightComponent::SetDiffuse(const ColorRGB &diffuse)
	{
		m_Diffuse = diffuse;
		if(m_OSGLight.valid())
			m_OSGLight->setDiffuse(osg::Vec4(static_cast<float>(m_Diffuse.r), static_cast<float>(m_Diffuse.g), static_cast<float>(m_Diffuse.b),1.0f));

	}
	void OSGLightComponent::SetSpecular(const ColorRGB &specular)
	{
		m_Specular = specular;
		if(m_OSGLight)
			m_OSGLight->setSpecular(osg::Vec4(static_cast<float>(m_Specular.r), static_cast<float>(m_Specular.g), static_cast<float>(m_Specular.b), 1.0f));
	}

	void OSGLightComponent::SetAmbient(const ColorRGB &ambient)
	{
		m_Ambient = ambient;
		if(m_OSGLight)
			m_OSGLight->setAmbient(osg::Vec4(static_cast<float>(m_Ambient.r), static_cast<float>(m_Ambient.g), static_cast<float>(m_Ambient.b), 1.0f));

	}
	void OSGLightComponent::SetCastShadow(bool value)
	{
		m_CastShadow = value;
	}

	void OSGLightComponent::SetLightId(int id)
	{
		m_LightId = id;
		if(m_OSGLight.valid())
			m_OSGLight->setLightNum(m_LightId);
	}

	void OSGLightComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		m_OSGLight = new osg::Light;
		m_OSGLightSource = new osg::LightSource;

		//udpate osg
		SetLightId(m_LightId);
		SetDiffuse(m_Diffuse);
		SetAmbient(m_Ambient);
		SetSpecular(m_Specular);

		if(m_LightType == LT_POINT)
		{
			 m_OSGLight->setConstantAttenuation(1.0f);
			 m_OSGLight->setLinearAttenuation(2.0f/1000);
			 m_OSGLight->setQuadraticAttenuation(2.0f/osg::square(1000));
			 m_OSGLight->setPosition(osg::Vec4(0.0,0.0,0.0,1.0f));
		}

		m_OSGLightSource->setLight(m_OSGLight);
		m_OSGLightSource->setLocalStateSetModes(osg::StateAttribute::ON); 

		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		lc->GetOSGNode()->addChild(m_OSGLightSource);

		//Always global light?
		OSGGraphicsSceneManagerPtr  scene_man = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>();
		osg::ref_ptr<osg::Group> root_node = scene_man->GetOSGRootNode();
		root_node->getOrCreateStateSet()->setAssociatedModes(m_OSGLight, osg::StateAttribute::ON);
	}
}