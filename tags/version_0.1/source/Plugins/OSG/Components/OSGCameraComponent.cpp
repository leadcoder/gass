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
#include <osgShadow/ShadowTechnique>

#include <GL/gl.h>
#include <GL/glu.h>

#include "Plugins/OSG/Components/OSGCameraComponent.h"

#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include "Sim/Systems/SimSystemManager.h"
//#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/SimEngine.h"
#include "Plugins/OSG/OSGNodeMasks.h"

#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/IOSGCameraManipulator.h"

#include "Plugins/OSG/Components/OSGLocationComponent.h"


namespace GASS
{
	OSGCameraComponent::OSGCameraComponent() :m_NearClip(0.5),
		m_FarClip(1000),
		m_Fov(45.0),
		m_Ortho(false),
		m_OSGCamera(NULL),
		m_UpdateCameraFromLocation(true)
	{

	}

	OSGCameraComponent::~OSGCameraComponent()
	{

	}

	void OSGCameraComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("CameraComponent",new Creator<OSGCameraComponent, IComponent>);
		RegisterProperty<float>("FarClipDistance", &GASS::OSGCameraComponent::GetFarClipDistance, &GASS::OSGCameraComponent::SetFarClipDistance);
		RegisterProperty<float>("NearClipDistance", &GASS::OSGCameraComponent::GetNearClipDistance, &GASS::OSGCameraComponent::SetNearClipDistance);
		RegisterProperty<float>("Fov", &GASS::OSGCameraComponent::GetFov, &GASS::OSGCameraComponent::SetFov);
		RegisterProperty<bool>("Ortho", &GASS::OSGCameraComponent::GetOrtho, &GASS::OSGCameraComponent::SetOrtho);
	}

	void OSGCameraComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraComponent::OnLoad,LoadGFXComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraComponent::OnUnload,UnloadComponentsMessage,1));
		//GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraComponent::OnPositionChanged,PositionMessage,10));
		//GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraComponent::OnRotationChanged,RotationMessage,10));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraComponent::OnParameter,CameraParameterMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraComponent::OnTransformationChanged,TransformationNotifyMessage,10));
	}


	void OSGCameraComponent::OnChangeCamera(ChangeCameraMessagePtr message)
	{
		SceneObjectPtr cam_obj = message->GetCamera();
		
		OSGCameraComponentPtr cam_comp = cam_obj->GetFirstComponentByClass<OSGCameraComponent>();
		OSGGraphicsSystemPtr gfx_sys = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<OSGGraphicsSystem>();
		osgViewer::ViewerBase::Views views;
		gfx_sys->GetViewer()->getViews(views);

		bool this_camera = false;
		if(cam_obj == GetSceneObject())
		{
			this_camera = true;
		}
		if(this_camera)
		{
			
		}
		else
		{

		}

		for(int i = 0; i < views.size(); i++)
		{
			//if(views[i]->getCamera() != cam_comp->GetOSGCamera().get())
			{
				/*cam_comp->GetOSGCamera()->setGraphicsContext(views[i]->getCamera()->getGraphicsContext());
				cam_comp->GetOSGCamera()->setViewport(views[i]->getCamera()->getViewport());
				views[i]->getCamera()->setViewport(NULL);
				views[i]->getCamera()->setGraphicsContext(NULL);
				views[i]->setCamera(cam_comp->GetOSGCamera());*/
			}
		}
	}

	void OSGCameraComponent::OnParameter(CameraParameterMessagePtr message)
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
				m_OrthoWindowHeight = message->GetValue1();
				UpdateProjection();
				//if(m_Camera)
				//m_Camera->setOrthoWindowHeight(value);
			}
			break;
		case CameraParameterMessage::CAMERA_CLIP_DISTANCE:
			{
				SetFarClipDistance(message->GetValue1());
				SetNearClipDistance(message->GetValue2());
			}
			break;
		}
	}

	void OSGCameraComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		if(m_UpdateCameraFromLocation)
			UpdateFromLocation();
	}

	void OSGCameraComponent::UpdateProjection()
	{
		if(m_OSGCamera.valid())
		{
			if(m_Ortho)
			{
				double aspectRatio = 1.0;
				if(m_OSGCamera->getViewport())
					aspectRatio = m_OSGCamera->getViewport()->width()/m_OSGCamera->getViewport()->height();

				double  h_size = m_OrthoWindowHeight/2.0;
				double  w_size = h_size*aspectRatio;
				m_OSGCamera->setProjectionMatrixAsOrtho(-w_size,w_size,-h_size,h_size,m_NearClip,m_FarClip);
			}
			else
			{
				//double fovy, aspectRatio, zNear, zFar;
				//m_OSGCamera->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
				double aspectRatio = 4.0/3.0;
				if(m_OSGCamera->getViewport())
					aspectRatio = m_OSGCamera->getViewport()->width()/m_OSGCamera->getViewport()->height();
				m_OSGCamera->setProjectionMatrixAsPerspective(m_Fov, aspectRatio, m_NearClip, m_FarClip);
			}
		}
	}

	/*void OSGCameraComponent::OnRotationChanged(RotationMessagePtr message)
	{
		UpdateFromLocation();
	}*/

	void OSGCameraComponent::UpdateFromLocation()
	{
		if(!m_OSGCamera.valid())
			return;
		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		//lc->GetOSGNode()->getAttitude();

		//osg::Vec3d pos = lc->GetOSGNode()->getPosition();
		//osg::Quat rot = lc->GetOSGNode()->getAttitude();

		Vec3 w_pos = lc->GetWorldPosition();
		osg::Vec3d pos = OSGConvert::Get().ToOSG(lc->GetWorldPosition());
		osg::Quat rot = OSGConvert::Get().ToOSG(lc->GetWorldRotation());

		/*Vec3 up = GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->GetSceneUp();
		if(up.z ==1)
		{
			osg::Quat q = osg::Quat(Math::Deg2Rad(90),osg::Vec3(1,0,0));
			rot = rot*q;
		}*/
		//osg::Transform* trans = lc->GetOSGNode()->asTransform();
		//osg::MatrixTransform* trans2 = lc->GetOSGNode()->asTransform()->asMatrixTransform();

		osg::Matrixd cameraMatrix;
		cameraMatrix.identity();

		osg::Quat q = osg::Quat(Math::Deg2Rad(90),osg::Vec3(1,0,0));
		rot = q*rot;

		cameraMatrix.setTrans(pos);
		cameraMatrix.setRotate(rot);

		osg::Matrixd final_cam_view= cameraMatrix.inverse(cameraMatrix);
		
		m_OSGCamera->setViewMatrix(final_cam_view);
	
		//m_OSGCamera->getInverseViewMatrix().setRotate(lc->GetOSGNode()->getAttitude());
	}

	void OSGCameraComponent::SetOSGCamera(osg::ref_ptr<osg::Camera> camera)
	{
		//update osg camera with gass camera attributes?
		m_OSGCamera = camera;
		UpdateProjection();
	}

	bool OSGCameraComponent::GetCameraToViewportRay(float screenx, float screeny, Vec3 &ray_start, Vec3 &ray_dir) const
	{
		if(m_OSGCamera.valid())
		{
		/*	osg::Vec3d origin; 
			osg::Vec3d direction;
			osg::Matrix mat = osg::Matrix::inverse(m_OSGCamera->getProjectionMatrix()); 
			osg::Matrix cameramatrix = m_OSGCamera->getInverseViewMatrix(); 
			osg::Matrix cameramatrixnotrans = cameramatrix; 
			cameramatrixnotrans.setTrans(0,0,0); */

			/*if(!m_Ortho)
			{
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
			}
			else*/
			{
				//use same method as ogre3d
				osg::Matrixd inverseVP = osg::Matrix::inverse(m_OSGCamera->getViewMatrix()*m_OSGCamera->getProjectionMatrix()); 
				double nx = screenx * 2.0f - 1.0f;
				double ny = (1.0f-screeny) * 2.0f - 1.0f;
				osg::Vec3d nearPoint(nx, ny, 0.0f);
				// Use midPoint rather than far point to avoid issues with infinite projection
				osg::Vec3d midPoint (nx, ny,  1.0f);

				// Get ray origin and ray target on near plane in world space
				osg::Vec3d rayOrigin, rayTarget;
		
				rayOrigin = nearPoint*inverseVP;
				rayTarget = midPoint*inverseVP;

				osg::Vec3d rayDirection = rayTarget - rayOrigin;
				rayDirection.normalize();

				//ray_start.Set(rayOrigin.x(),rayOrigin.y(),rayOrigin.z());
				//ray_dir.Set(rayDirection.x(),rayDirection.y(),rayDirection.z());
				ray_start = OSGConvert::Get().ToGASS(rayOrigin);
				ray_dir = OSGConvert::Get().ToGASS(rayDirection);
			}
			return true;
		}
		else 
			return false;
	}

	void OSGCameraComponent::OnUnload(UnloadComponentsMessagePtr message)
	{

	}

	void OSGCameraComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{


		OSGCameraManipulatorPtr camera_man = GetSceneObject()->GetFirstComponentByClass<IOSGCameraManipulator>();
		if(camera_man)
			m_UpdateCameraFromLocation = false;


		return;

		OSGGraphicsSystemPtr gfx_sys = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<OSGGraphicsSystem>();

		//GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->RegisterForMessage(REG_TMESS(OSGGraphicsSceneManager::OnChangeCamera,ChangeCameraMessage,0));

		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();

		
		osgViewer::ViewerBase::Views views;
		gfx_sys->GetViewer()->getViews(views);

		int vp_id = 0;
		
			
		//set same scene in all viewports for the moment 
		//for(int i = 0; i < views.size(); i++)
		if(vp_id < views.size())
		{
		
			//uhh?

			//m_OSGCamera = new osg::Camera;
			//m_OSGCamera->setClearColor(osg::Vec4(1,1,1,1));

			double fovy, aspectRatio, zNear, zFar;
			m_OSGCamera = new osg::Camera(*views[vp_id]->getCamera());
			//m_OSGCamera = views[vp_id]->getCamera();
			//m_OSGCamera->setCullMask(NM_VISIBLE);
			
			m_OSGCamera->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
			m_OSGCamera->setProjectionMatrixAsPerspective(m_Fov, aspectRatio, m_NearClip, m_FarClip);

			//m_OSGCamera->setReadBuffer(GL_BACK);
			//m_OSGCamera->setDrawBuffer(GL_BACK);
			//m_OSGCamera = views[i]->getCamera();
			m_OrthoWindowHeight = m_OSGCamera->getViewport()->height();
			UpdateProjection();
			//m_OSGCamera->setViewMatrix(

			//views[i]->getCamera()->addParent(lc->GetOSGNode());
			//views[i]->setsetCameraManipulator(tm);
			//->addChild(views[i]->getCamera());
		}

		//Ogre::SceneManager* sm = ogsm->GetSceneManger();
		//OSGLocationComponent * lc = GetOwner()->GetFirstComponentByClass<OSGLocationComponent>();
		/*m_Camera = sm->createCamera(m_Name);
		m_Camera->setNearClipDistance(1.0);
		m_Camera->setFarClipDistance(5000);
		lc->GetOgreNode()->attachObject(m_Camera);*/
	}

	float OSGCameraComponent::GetFarClipDistance() const
	{
		return  m_FarClip;
	}
	void OSGCameraComponent::SetFarClipDistance(float value)
	{
		m_FarClip = value;
		UpdateProjection();
	}

	float OSGCameraComponent::GetNearClipDistance() const
	{
		return  m_NearClip;
	}

	void OSGCameraComponent::SetNearClipDistance(float value)
	{
		m_NearClip = value;
		UpdateProjection();
	}

	float OSGCameraComponent::GetFov() const
	{
		return  m_Fov;
	}

	void OSGCameraComponent::SetFov(float value)
	{
		m_Fov = value;
		UpdateProjection();
	}

	bool OSGCameraComponent::GetOrtho() const
	{
		return  m_Ortho;
	}

	void OSGCameraComponent::SetOrtho(bool value)
	{
		m_Ortho = value;
		UpdateProjection();
	}
}
