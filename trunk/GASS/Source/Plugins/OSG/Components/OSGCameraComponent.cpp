/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgGA/NodeTrackerManipulator>

#include "Plugins/OSG/Components/OSGCameraComponent.h"

#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/SimEngine.h"

#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/Components/OSGLocationComponent.h"


namespace GASS
{
	OSGCameraComponent::OSGCameraComponent() 
	{

	}

	OSGCameraComponent::~OSGCameraComponent()
	{

	}

	void OSGCameraComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("CameraComponent",new Creator<OSGCameraComponent, IComponent>);
	}

	void OSGCameraComponent::OnCreate()
	{
		int obj_id = (int) this;
		MessageManager * mm = GetMessageManager();
		mm->RegisterForMessage(ScenarioScene::SM_MESSAGE_LOAD_GFX_COMPONENTS, obj_id,  boost::bind( &OSGCameraComponent::OnLoad, this, _1 ),1);
		//mm->RegisterForMessage(ScenarioScene::SM_MESSAGE_UPDATE, address,  boost::bind( &OSGCameraComponent::OnUpdate, this, _1 ),1);
	}

	bool OSGCameraComponent::GetCameraToViewportRay(float screenx, float screeny, Vec3 &ray_start, Vec3 &ray_dir) const
	{
		if(m_OSGCamera)
		{
			osg::Vec3d origin; 
			osg::Vec3d direction;
	 	    /*osg::Matrix mat = osg::Matrix::inverse(m_OSGCamera->getProjectionMatrix()); 
			osg::Matrix cameramatrix = m_OSGCamera->getViewMatrix(); 
			osg::Matrix cameramatrixnotrans = cameramatrix; 
			cameramatrixnotrans.setTrans(0,0,0); 
			osg::Vec4d d = osg::Vec4( 
                screenx * 2.0 - 1.0, 
                -screeny * 2.0 - 1.0, 
                0.0,1.0); 
			d = d * mat; 
			d = d * cameramatrixnotrans; 
			direction.set(d[0],d[1],d[2]); 
	        direction.normalize(); 
		    origin = cameramatrix.getTrans(); */
			ray_start = OSGConvert::ToGASS(origin);
			ray_dir = OSGConvert::ToGASS(direction);
			return true;
		}
		else 
		return false;
	}

	void OSGCameraComponent::OnLoad(MessagePtr message)
	{
		OSGGraphicsSceneManager* osg_sm = boost::any_cast<OSGGraphicsSceneManager*>(message->GetData("GraphicsSceneManager"));
		assert(osg_sm);

		OSGGraphicsSystemPtr gfx_sys = SimEngine::GetPtr()->GetSystemManager()->GetFirstSystem<OSGGraphicsSystem>();

		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponent<OSGLocationComponent>();

		osgViewer::ViewerBase::Views views;
		gfx_sys->GetViewer()->getViews(views);
		//set same scene in all viewports for the moment 
		
		for(int i = 0; i < views.size(); i++)
		{
			osgGA::NodeTrackerManipulator* tm = new osgGA::NodeTrackerManipulator;
			tm->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER_AND_ROTATION);
				tm->setRotationMode(osgGA::NodeTrackerManipulator::ELEVATION_AZIM);
			tm->setTrackNode(lc->GetOSGNode().get());
			//uhh?
			m_OSGCamera = views[i]->getCamera();
			
			//views[i]->getCamera()->addParent(lc->GetOSGNode());
			views[i]->setCameraManipulator(tm);
			//->addChild(views[i]->getCamera());
		}

		//Ogre::SceneManager* sm = ogsm->GetSceneManger();
		//OSGLocationComponent * lc = GetOwner()->GetFirstComponent<OSGLocationComponent>();
		/*m_Camera = sm->createCamera(m_Name);
		m_Camera->setNearClipDistance(1.0);
		m_Camera->setFarClipDistance(5000);
		lc->GetOgreNode()->attachObject(m_Camera);*/
	}
}

