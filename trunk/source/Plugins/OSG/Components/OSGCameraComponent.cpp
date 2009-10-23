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
#include <osg/MatrixTransform>

#include <GL/gl.h>
#include <GL/glu.h>

#include "Plugins/OSG/Components/OSGCameraComponent.h"

#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
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
		GetSceneObject()->RegisterForMessage(OBJECT_RM_LOAD_GFX_COMPONENTS, TYPED_MESSAGE_FUNC(OSGCameraComponent::OnLoad,LoadGFXComponentsMessage),1);
		GetSceneObject()->RegisterForMessage(OBJECT_RM_POSITION, TYPED_MESSAGE_FUNC(OSGCameraComponent::OnPositionChanged,PositionMessage),10);
		GetSceneObject()->RegisterForMessage(OBJECT_RM_ROTATION, TYPED_MESSAGE_FUNC(OSGCameraComponent::OnRotationChanged,RotationMessage),10);
	}

	void OSGCameraComponent::OnPositionChanged(PositionMessagePtr message)
	{
		UpdateFromLocation();
	}

	void OSGCameraComponent::OnRotationChanged(RotationMessagePtr message)
	{
		UpdateFromLocation();
	}

	void OSGCameraComponent::UpdateFromLocation()
	{
		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponent<OSGLocationComponent>();
		//lc->GetOSGNode()->getAttitude();

		osg::Vec3d pos = lc->GetOSGNode()->getPosition();
		osg::Quat rot = lc->GetOSGNode()->getAttitude();

		osg::Quat q = osg::Quat(Math::Deg2Rad(90),osg::Vec3(1,0,0));

		rot = q*rot;
		osg::Transform* trans = lc->GetOSGNode()->asTransform();
		osg::MatrixTransform* trans2 = lc->GetOSGNode()->asTransform()->asMatrixTransform();

		osg::Matrixd cameraMatrix;

		cameraMatrix.setTrans(pos);
		cameraMatrix.setRotate(rot);

		osg::Matrixd final_cam_view= cameraMatrix.inverse(cameraMatrix);

		m_OSGCamera->setViewMatrix(final_cam_view);
	
		//m_OSGCamera->getInverseViewMatrix().setRotate(lc->GetOSGNode()->getAttitude());
	}

	bool OSGCameraComponent::GetCameraToViewportRay(float screenx, float screeny, Vec3 &ray_start, Vec3 &ray_dir) const
	{
		if(m_OSGCamera)
		{
			osg::Vec3d origin; 
			osg::Vec3d direction;
			osg::Matrix mat = osg::Matrix::inverse(m_OSGCamera->getProjectionMatrix()); 
			osg::Matrix cameramatrix = m_OSGCamera->getInverseViewMatrix(); 
			osg::Matrix cameramatrixnotrans = cameramatrix; 
			cameramatrixnotrans.setTrans(0,0,0); 
			osg::Vec4 d = osg::Vec4(
				screenx * 2.0f - 1.0f, 
				(1.0f-screeny) * 2.0f - 1.0f, 
				0.0f,1.0f); 
			d = d * mat; 
			d = d * cameramatrixnotrans; 

			direction.set(d[0],d[1],d[2]); 
			direction.normalize(); 
			origin = cameramatrix.getTrans(); 

			ray_start = OSGConvert::ToGASS(origin);
			ray_dir = OSGConvert::ToGASS(direction);
			ray_dir.Normalize();
			return true;
		}
		else 
			return false;
	}

	void OSGCameraComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		OSGGraphicsSceneManager* osgsm = static_cast<OSGGraphicsSceneManager*>(message->GetGFXSceneManager());
		assert(osgsm);

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
			
			//m_OSGCamera->setViewMatrix(

			//views[i]->getCamera()->addParent(lc->GetOSGNode());
			//views[i]->setsetCameraManipulator(tm);
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

