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
#include "OgreCameraComponent.h"
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreRay.h>

//SSAO
#include <OgreCompositorManager.h>


#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"

#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/OgreConvert.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"


namespace GASS
{
	OgreCameraComponent::OgreCameraComponent(): 
		m_NearClip(0.5),
		m_FarClip(1000),
		m_Camera(NULL)
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
	}

	void OgreCameraComponent::OnCreate()
	{
		
		//priorty = 1 -> load this one after nodes
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_LOAD_GFX_COMPONENTS, MESSAGE_FUNC(OgreCameraComponent::OnLoad),1);
	}


	void OgreCameraComponent::OnLoad(MessagePtr message)
	{
		OgreGraphicsSceneManager* ogsm = boost::any_cast<OgreGraphicsSceneManager*>(message->GetData("GraphicsSceneManager"));
		assert(ogsm);
		Ogre::SceneManager* sm = ogsm->GetSceneManger();
		OgreLocationComponentPtr lc = GetSceneObject()->GetFirstComponent<OgreLocationComponent>();

		static unsigned int obj_id = 0;
		obj_id++;
		std::stringstream ss;
		std::string name;
		ss << GetName() << obj_id;
		ss >> name;

		m_Camera = sm->createCamera(name);
		SetNearClipDistance(m_NearClip);
		SetFarClipDistance(m_FarClip);
        lc->GetOgreNode()->attachObject(m_Camera);
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



}
