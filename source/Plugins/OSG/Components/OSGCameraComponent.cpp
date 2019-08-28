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

#include "Plugins/OSG/Components/OSGCameraComponent.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/IOSGCameraManipulator.h"
#include "Plugins/OSG/Components/OSGLocationComponent.h"
#include "Core/Math/GASSMath.h"

namespace GASS
{
	OSGCameraComponent::OSGCameraComponent() : m_NearClip(0.5),
		m_FarClip(1000),
		m_NearFarRatio(0),
		m_Fov(45.0),
		m_LODScale(1.0),
		m_Ortho(false),
		m_OSGCamera(NULL),
		m_UpdateCameraFromLocation(true),
		m_OrthoWindowHeight(0)
	{

	}

	OSGCameraComponent::~OSGCameraComponent()
	{

	}

	void OSGCameraComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<OSGCameraComponent>("CameraComponent");
		ADD_DEPENDENCY("OSGLocationComponent")
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("CameraComponent", OF_VISIBLE)));

		RegisterProperty<float>("FarClipDistance", &GASS::OSGCameraComponent::GetFarClipDistance, &GASS::OSGCameraComponent::SetFarClipDistance,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Far clipping plane distance",PF_VISIBLE  | PF_EDITABLE)));
		RegisterProperty<float>("NearClipDistance", &GASS::OSGCameraComponent::GetNearClipDistance, &GASS::OSGCameraComponent::SetNearClipDistance,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Near clipping plane distance",PF_VISIBLE  | PF_EDITABLE)));
		RegisterProperty<float>("NearFarRatio", &GASS::OSGCameraComponent::GetNearFarRatio, &GASS::OSGCameraComponent::SetNearFarRatio,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Near Far Ratio", PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<float>("Fov", &GASS::OSGCameraComponent::GetFov, &GASS::OSGCameraComponent::SetFov,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Field Of View in Degres",PF_VISIBLE  | PF_EDITABLE)));
		
		RegisterProperty<bool>("Ortho", &GASS::OSGCameraComponent::GetOrtho, &GASS::OSGCameraComponent::SetOrtho,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Use orthographic projection",PF_VISIBLE  | PF_EDITABLE)));
		RegisterProperty<float>("LODScale", &GASS::OSGCameraComponent::GetLODScale, &GASS::OSGCameraComponent::SetLODScale,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("OSG LODScale",PF_VISIBLE  | PF_EDITABLE)));
	}

	void OSGCameraComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraComponent::OnLocationLoaded,LocationLoadedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraComponent::OnParameter,CameraParameterRequest,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraComponent::OnTransformationChanged,TransformationChangedEvent,10));
	}

	void OSGCameraComponent::OnParameter(CameraParameterRequestPtr message)
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
				m_OrthoWindowHeight = message->GetValue1();
				_UpdateProjection();
				//if(m_Camera)
				//m_Camera->setOrthoWindowHeight(value);
			}
			break;
		case CameraParameterRequest::CAMERA_CLIP_DISTANCE:
			{
				SetFarClipDistance(message->GetValue1());
				SetNearClipDistance(message->GetValue2());
			}
			break;
		}
	}

	void OSGCameraComponent::ShowInViewport(const std::string &viewport_name)
	{
		GraphicsSystemPtr gfx_system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IGraphicsSystem>();
		RenderWindowVector windows = gfx_system->GetRenderWindows();
		CameraComponentPtr camera_comp = GASS_DYNAMIC_PTR_CAST<ICameraComponent>(shared_from_this());
		for (size_t i = 0; i < windows.size(); i++)
		{
			ViewportVector  viewports = windows[i]->GetViewports();
			for (size_t j = 0; j < viewports.size(); j++)
			{
				if (viewport_name == "" || viewports[j]->GetName() == viewport_name)
					viewports[j]->SetCamera(camera_comp);
			}
		}
	}

	void OSGCameraComponent::OnTransformationChanged(TransformationChangedEventPtr message)
	{
		if(m_UpdateCameraFromLocation)
			_UpdateFromLocation();
	}

	void OSGCameraComponent::_UpdateProjection()
	{
		if(m_OSGCamera.valid())
		{
			if(m_Ortho)
			{
				double aspectRatio = 1.0;
				if(m_OSGCamera->getViewport())
					aspectRatio = m_OSGCamera->getViewport()->width()/m_OSGCamera->getViewport()->height();

				const double  h_size = m_OrthoWindowHeight/2.0;
				const double  w_size = h_size*aspectRatio;
				m_OSGCamera->setProjectionMatrixAsOrtho(-w_size,w_size,-h_size,h_size,m_NearClip,m_FarClip);
			}
			else
			{
				//m_OSGCamera->setProjectionResizePolicy(osg::Camera::FIXED);
				m_OSGCamera->setProjectionResizePolicy(osg::Camera::HORIZONTAL);
				double aspectRatio = 4.0/3.0;

				if(m_OSGCamera->getViewport())
					aspectRatio = m_OSGCamera->getViewport()->width()/m_OSGCamera->getViewport()->height();

				if(m_FarClip > 0.0)
				{
					m_OSGCamera->setProjectionMatrixAsPerspective(m_Fov, aspectRatio, m_NearClip, m_FarClip);
					m_OSGCamera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
				}
				else
				{
					//just intialize to default values and autocompute far distance
					m_OSGCamera->setProjectionMatrixAsPerspective(m_Fov, aspectRatio, m_NearClip, 10000);
					//m_OSGCamera->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
				}
			}
		}
	}

	void OSGCameraComponent::_UpdateFromLocation()
	{
		if(!m_OSGCamera.valid())
			return;
		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
	
		const osg::Vec3d pos = OSGConvert::ToOSG(lc->GetWorldPosition());
		osg::Quat rot = OSGConvert::ToOSG(lc->GetWorldRotation());

		osg::Matrixd cameraMatrix;
		cameraMatrix.identity();

		osg::Quat q = osg::Quat(Math::Deg2Rad(90.0f),osg::Vec3(1,0,0));
		rot = q * rot;

		cameraMatrix.setTrans(pos);
		cameraMatrix.setRotate(rot);

		const osg::Matrixd final_cam_view= cameraMatrix.inverse(cameraMatrix);
		m_OSGCamera->setViewMatrix(final_cam_view);
	}

	void OSGCameraComponent::SetOSGCamera(osg::ref_ptr<osg::Camera> camera)
	{
		//update osg camera with gass camera attributes?
		m_OSGCamera = camera;
		_UpdateFromLocation();
		_UpdateProjection();
		SetLODScale(m_LODScale);
		if (m_NearFarRatio > 0)
			SetNearFarRatio(m_NearFarRatio);
	}

	bool OSGCameraComponent::GetCameraToViewportRay(float screenx, float screeny, Ray &ray) const
	{
		if(m_OSGCamera.valid())
		{
			//use same method as ogre3d
			osg::Matrixd inverseVP = osg::Matrix::inverse(m_OSGCamera->getViewMatrix()*m_OSGCamera->getProjectionMatrix()); 

			double nx = screenx * 2.0f - 1.0f;
			double ny = (1.0f-screeny) * 2.0f - 1.0f;

			osg::Vec3d nearPoint(nx, ny, -1.0f);
			// Use midPoint rather than far point to avoid issues with infinite projection
			osg::Vec3d midPoint (nx, ny,  1.0f);

			// Get ray origin and ray target on near plane in world space
			osg::Vec3d rayOrigin, rayTarget;

			rayOrigin = nearPoint*inverseVP;
			rayTarget = midPoint*inverseVP;

			osg::Vec3d rayDirection = rayTarget - rayOrigin;
			rayDirection.normalize();

			ray.m_Origin = OSGConvert::ToGASS(rayOrigin);
			ray.m_Dir = OSGConvert::ToGASS(rayDirection);

			return true;
		}
		else 
			return false;
	}

	void OSGCameraComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		OSGCameraManipulatorPtr camera_man = GetSceneObject()->GetFirstComponentByClass<IOSGCameraManipulator>();
		if(camera_man)
			m_UpdateCameraFromLocation = false;
	}

	float OSGCameraComponent::GetLODScale() const
	{
		return  m_LODScale;
	}

	void OSGCameraComponent::SetLODScale(float value)
	{
		m_LODScale = value;
		if(m_OSGCamera.valid())
		{
			m_OSGCamera->setLODScale(value);
		}
	}

	float OSGCameraComponent::GetFarClipDistance() const
	{
		return  m_FarClip;
	}

	void OSGCameraComponent::SetFarClipDistance(float value)
	{
		m_FarClip = value;
		_UpdateProjection();
	}

	float OSGCameraComponent::GetNearClipDistance() const
	{
		return  m_NearClip;
	}

	void OSGCameraComponent::SetNearClipDistance(float value)
	{
		m_NearClip = value;
		_UpdateProjection();
	}


	float OSGCameraComponent::GetNearFarRatio() const
	{
		return m_NearFarRatio;
	}

	void OSGCameraComponent::SetNearFarRatio(float value)
	{
		m_NearFarRatio = value;
		if (m_OSGCamera.valid())
		{
			m_OSGCamera->setNearFarRatio(value);
		}
	}


	float OSGCameraComponent::GetFov() const
	{
		return  m_Fov;
	}

	void OSGCameraComponent::SetFov(float value)
	{
		m_Fov = value;
		_UpdateProjection();
	}

	bool OSGCameraComponent::GetOrtho() const
	{
		return  m_Ortho;
	}

	void OSGCameraComponent::SetOrtho(bool value)
	{
		m_Ortho = value;
		_UpdateProjection();
	}
}

