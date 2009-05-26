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
	OgreCameraComponent::OgreCameraComponent()
	{
		m_Camera = NULL;
	}

	OgreCameraComponent::~OgreCameraComponent()
	{
		
	}

	void OgreCameraComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("CameraComponent",new Creator<OgreCameraComponent, IComponent>);
	}

	void OgreCameraComponent::OnCreate()
	{
		int obj_id = (int) this;
		MessageManager * mm = GetMessageManager();
		mm->RegisterForMessage(ScenarioScene::SM_MESSAGE_LOAD_GFX_COMPONENTS, obj_id,  boost::bind( &OgreCameraComponent::OnLoad, this, _1 ),1);
	}


	void OgreCameraComponent::OnLoad(MessagePtr message)
	{
		OgreGraphicsSceneManager* ogsm = boost::any_cast<OgreGraphicsSceneManager*>(message->GetData("GraphicsSceneManager"));
		assert(ogsm);
		Ogre::SceneManager* sm = ogsm->GetSceneManger();
		OgreLocationComponentPtr lc = GetSceneObject()->GetFirstComponent<OgreLocationComponent>();
		m_Camera = sm->createCamera(m_Name);
		m_Camera->setNearClipDistance(1.0);
		m_Camera->setFarClipDistance(1150);
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



}
