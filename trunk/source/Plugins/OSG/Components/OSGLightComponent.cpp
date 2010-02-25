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
		m_Specular(1,1,1),
		m_Ambient(0.7,0.7,0.7),
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

		RegisterProperty<Vec3>("DiffuseColor", &GASS::OSGLightComponent::GetDiffuse, &GASS::OSGLightComponent::SetDiffuse);
		RegisterProperty<Vec3>("SpecularColor", &GASS::OSGLightComponent::GetSpecular, &GASS::OSGLightComponent::SetSpecular);
		RegisterProperty<Vec3>("AmbientColor", &GASS::OSGLightComponent::GetAmbient, &GASS::OSGLightComponent::SetAmbient);
	}

	void OSGLightComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLightComponent::OnLoad,LoadGFXComponentsMessage,1));
	}

	void OSGLightComponent::SetLightType(LightType lt)
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
		if(m_OSGLight)
			m_OSGLight->setSpecular(osg::Vec4(m_Specular.x,m_Specular.y,m_Specular.z,1));
	}

	void OSGLightComponent::SetAmbient(const Vec3 &ambient)
	{
		m_Ambient = ambient;
		if(m_OSGLight)
			m_OSGLight->setAmbient(osg::Vec4(m_Ambient.x,m_Ambient.y,m_Ambient.z,1));

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

	void OSGLightComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{

		m_OSGLight = new osg::Light;
		m_OSGLightSource = new osg::LightSource;


		//udpate osg
		SetLightId(m_LightId);
		SetDiffuse(m_Diffuse);
		SetAmbient(m_Ambient);
		SetSpecular(m_Specular);

		m_OSGLightSource->setLight(m_OSGLight);
		m_OSGLightSource->setLocalStateSetModes(osg::StateAttribute::ON); 

		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponent<OSGLocationComponent>();
		lc->GetOSGNode()->addChild(m_OSGLightSource);

		//Always global light?
		OSGGraphicsSceneManagerPtr  scene_man = boost::shared_dynamic_cast<OSGGraphicsSceneManager>(message->GetGFXSceneManager());
		osg::ref_ptr<osg::PositionAttitudeTransform> root_node = scene_man->GetOSGRootNode();
		root_node->getOrCreateStateSet()->setAssociatedModes(m_OSGLight, osg::StateAttribute::ON);

	}
}
