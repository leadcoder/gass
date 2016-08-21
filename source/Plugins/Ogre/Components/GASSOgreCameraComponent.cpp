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

#include "Plugins/Ogre/Components/GASSOgreCameraComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/Utils/GASSException.h"
#include "Core/Math/GASSMath.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Plugins/Ogre/GASSOgreGraphicsSceneManager.h"
#include "Plugins/Ogre/GASSOgreConvert.h"
#include "Plugins/Ogre/Components/GASSOgreLocationComponent.h"

namespace GASS
{

	OgreCameraComponent::OgreCameraComponent(): 
		m_NearClip(0.5),
		m_FarClip(1000),
		m_Fov(45.0),
		m_Ortho(false),
		m_Camera(NULL),
		m_ClipToFog(false),
		m_PolygonMode(Ogre::PM_SOLID),
		m_MaterialScheme("ALM_STD")
	{
		
	}

	OgreCameraComponent::~OgreCameraComponent()
	{
		
	}

	void OgreCameraComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("CameraComponent",new Creator<OgreCameraComponent, Component>);
		ADD_DEPENDENCY("OgreLocationComponent")
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("CameraComponent", OF_VISIBLE)));

		RegisterProperty<float>("FarClipDistance", &GASS::OgreCameraComponent::GetFarClipDistance, &GASS::OgreCameraComponent::SetFarClipDistance,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Far clip plane Distance",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("NearClipDistance", &GASS::OgreCameraComponent::GetNearClipDistance, &GASS::OgreCameraComponent::SetNearClipDistance,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Near clipplane distance",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("Fov", &GASS::OgreCameraComponent::GetFov, &GASS::OgreCameraComponent::SetFov,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Field Of View (degress)",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("Ortho", &GASS::OgreCameraComponent::GetOrtho, &GASS::OgreCameraComponent::SetOrtho,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Orthographic projection (otherwise Perpective)",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("ClipToFog", &GASS::OgreCameraComponent::GetClipToFog, &GASS::OgreCameraComponent::SetClipToFog,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Let far clip plane be locked to end fog distance",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<PolygonModeWrapper>("PolygonMode", &GASS::OgreCameraComponent::GetPolygonMode, &GASS::OgreCameraComponent::SetPolygonMode,	
			EnumerationProxyPropertyMetaDataPtr(new EnumerationProxyPropertyMetaData("Polygon render mode for this camera",PF_VISIBLE,&PolygonModeWrapper::GetStringEnumeration)));
		RegisterProperty< std::vector<std::string> >("PostFilters", &GASS::OgreCameraComponent::GetPostFilters, &GASS::OgreCameraComponent::SetPostFilters);

		std::vector<std::string> scheme_enumeration;
		//hard code for now
		scheme_enumeration.push_back("ALM_STD"); //Additive light masking 
		scheme_enumeration.push_back("FFP_STD"); //Regular fixed function pipe line 
		RegisterProperty<std::string>("MaterialScheme", &GASS::OgreCameraComponent::GetMaterialScheme, &GASS::OgreCameraComponent::SetMaterialScheme,	
			StaticEnumerationPropertyMetaDataPtr(new StaticEnumerationPropertyMetaData("Material scheme for this camera",PF_VISIBLE,scheme_enumeration)));
	}

	void OgreCameraComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreCameraComponent::OnLocationLoaded,LocationLoadedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreCameraComponent::OnParameter,CameraParameterRequest,0));
	}

	void OgreCameraComponent::SetMaterialScheme(const std::string &value) 
	{
		m_MaterialScheme = value;
		if(m_Camera && m_Camera->getViewport()) //we are initilized and have viewport!
			m_Camera->getViewport()->setMaterialScheme(m_MaterialScheme);
	}

	void OgreCameraComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		OgreGraphicsSceneManagerPtr ogsm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OgreGraphicsSceneManager>();
		assert(ogsm);
		Ogre::SceneManager* sm = ogsm->GetOgreSceneManager();
		OgreLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OgreLocationComponent>();
		if(!lc)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find location component", "OgreCameraComponent::OnLoad");
		}

		static unsigned int obj_id = 0;
		obj_id++;
		std::stringstream ss;
		std::string name;
		ss << GetName() << obj_id;
		ss >> name;

		if(m_ClipToFog)
			m_FarClip = ogsm->GetFogEnd();

		m_Camera = sm->createCamera(name);
		//m_Camera->setAspectRatio(16.0/9.0);
		m_Camera->setAutoAspectRatio(true);
		SetNearClipDistance(m_NearClip);
		SetFarClipDistance(m_FarClip);
		SetFov(m_Fov);
		SetOrtho(m_Ortho);
		SetPolygonMode(m_PolygonMode);
		SetMaterialScheme(m_MaterialScheme);
        lc->GetOgreNode()->attachObject(m_Camera);
	}

	std::vector<std::string> OgreCameraComponent::GetPostFilters() const
	{
		return m_PostFilters;
	}

	void OgreCameraComponent::SetPostFilters(const std::vector<std::string> &filters)
	{
		m_PostFilters = filters;
	}

	void OgreCameraComponent::SetPolygonMode(PolygonModeWrapper value) 
	{
		m_PolygonMode= value;
		if(m_Camera)
		{
			m_Camera->setPolygonMode(m_PolygonMode.GetValue());
		}
	}
	
	void OgreCameraComponent::OnParameter(CameraParameterRequestPtr message)
	{
		CameraParameterRequest::CameraParameterType type = message->GetParameter();
		switch(type)
		{
		case CameraParameterRequest::CAMERA_FOV:
			{
				float value = message->GetValue1();
				SetFov(value);
			}
			break;
		case CameraParameterRequest::CAMERA_ORTHO_WIN_SIZE:
			{
				float value = message->GetValue1();
				if(m_Camera)
					m_Camera->setOrthoWindowHeight(value);
			}
			break;
		case CameraParameterRequest::CAMERA_CLIP_DISTANCE:
			{
				float farc = message->GetValue1();
				SetFarClipDistance(farc);
				float nearc = message->GetValue2();
				SetFarClipDistance(nearc);
			}
			break;
		}
	}

	bool OgreCameraComponent::GetCameraToViewportRay(float screenx, float screeny, Ray &ray) const
	{
		if(m_Camera)
		{
			Ogre::Ray ogre_ray = m_Camera->getCameraToViewportRay(screenx, screeny);
			ray.m_Origin = OgreConvert::ToGASS(ogre_ray.getOrigin());
			ray.m_Dir = OgreConvert::ToGASS(ogre_ray.getDirection());
			return true;
		}
		else 
		return false;
	}

	float OgreCameraComponent::GetFarClipDistance() const
	{
		return  m_FarClip;
	}
	void OgreCameraComponent::SetFarClipDistance(float value)
	{
		m_FarClip = value;
		if(m_Camera)
		{
			m_Camera->setFarClipDistance(value);
		}
	}

	float OgreCameraComponent::GetNearClipDistance() const
	{
		return  m_NearClip;
	}

	void OgreCameraComponent::SetNearClipDistance(float value)
	{
		m_NearClip = value;
		if(m_Camera)
		{
			m_Camera->setNearClipDistance(value);
		}
	}

	float OgreCameraComponent::GetFov() const
	{
		return  m_Fov;
	}

	void OgreCameraComponent::SetFov(float value)
	{
		m_Fov = value;
		if(m_Camera && !GetOrtho())
		{
			m_Camera->setFOVy(Ogre::Radian(static_cast<float>(Math::Deg2Rad(value))));
		}
	}

	bool OgreCameraComponent::GetOrtho() const
	{
		return  m_Ortho;
	}

	void OgreCameraComponent::SetOrtho(bool value)
	{
		m_Ortho = value;
		if(m_Camera)
		{
			if(m_Ortho)
				m_Camera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
			else 
				m_Camera->setProjectionType(Ogre::PT_PERSPECTIVE);
		}
	}
}