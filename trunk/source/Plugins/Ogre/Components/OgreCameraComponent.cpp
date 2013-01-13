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
#include "Plugins/Ogre/Components/OgreCameraComponent.h"
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreRay.h>

//SSAO
#include <OgreCompositorManager.h>

#include "Core/Utils/GASSLogManager.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/Utils/GASSException.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"

#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/OgreConvert.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"


namespace GASS
{

	OgreCameraComponent::OgreCameraComponent(): 
		m_NearClip(0.5),
		m_FarClip(1000),
		m_Fov(45.0),
		m_Ortho(false),
		m_Camera(NULL),
		m_ClipToFog(false),
		m_PolygonMode(Ogre::PM_SOLID)
	{
		
	}

	OgreCameraComponent::~OgreCameraComponent()
	{
		
	}

	void OgreCameraComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("CameraComponent",new Creator<OgreCameraComponent, IComponent>);
		RegisterProperty<float>("FarClipDistance", &GASS::OgreCameraComponent::GetFarClipDistance, &GASS::OgreCameraComponent::SetFarClipDistance);
		RegisterProperty<float>("NearClipDistance", &GASS::OgreCameraComponent::GetNearClipDistance, &GASS::OgreCameraComponent::SetNearClipDistance);
		RegisterProperty<float>("Fov", &GASS::OgreCameraComponent::GetFov, &GASS::OgreCameraComponent::SetFov);
		RegisterProperty<bool>("Ortho", &GASS::OgreCameraComponent::GetOrtho, &GASS::OgreCameraComponent::SetOrtho);
		RegisterProperty<bool>("ClipToFog", &GASS::OgreCameraComponent::GetClipToFog, &GASS::OgreCameraComponent::SetClipToFog);
		RegisterEnumProperty<PolygonModeWrapper>("PolygonMode", &GASS::OgreCameraComponent::GetPolygonMode, &GASS::OgreCameraComponent::SetPolygonMode);
		RegisterVectorProperty<std::string>("PostFilters", &GASS::OgreCameraComponent::GetPostFilters, &GASS::OgreCameraComponent::SetPostFilters);
	}

	void OgreCameraComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreCameraComponent::OnLocationLoaded,LocationLoadedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreCameraComponent::OnParameter,CameraParameterMessage,0));
	}

	void OgreCameraComponent::OnLocationLoaded(LocationLoadedMessagePtr message)
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
	
	void OgreCameraComponent::OnParameter(CameraParameterMessagePtr message)
	{
		CameraParameterMessage::CameraParameterType type = message->GetParameter();
		switch(type)
		{
		case CameraParameterMessage::CAMERA_FOV:
			{
				float value = message->GetValue1();
				SetFov(value);
			}
			break;
		case CameraParameterMessage::CAMERA_ORTHO_WIN_SIZE:
			{
				float value = message->GetValue1();
				if(m_Camera)
					m_Camera->setOrthoWindowHeight(value);
			}
			break;
		case CameraParameterMessage::CAMERA_CLIP_DISTANCE:
			{
				float farc = message->GetValue1();
				SetFarClipDistance(farc);
				float nearc = message->GetValue2();
				SetFarClipDistance(nearc);
			}
			break;
		}
	}

	bool OgreCameraComponent::GetCameraToViewportRay(float screenx, float screeny, Vec3 &ray_start, Vec3 &ray_dir) const
	{
		if(m_Camera)
		{
			Ogre::Ray ray = m_Camera->getCameraToViewportRay(screenx, screeny);
			ray_start = Convert::ToGASS(ray.getOrigin());
			ray_dir = Convert::ToGASS(ray.getDirection());
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
			m_Camera->setFOVy(Ogre::Radian(Math::Deg2Rad(value)));
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
