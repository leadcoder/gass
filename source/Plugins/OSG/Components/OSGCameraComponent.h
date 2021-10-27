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

#pragma once
#include "Sim/GASS.h"
#include "Plugins/OSG/OSGCommon.h"
#include "Plugins/OSG/IOSGCamera.h"

namespace GASS
{
	class OSGCameraComponent : public Reflection<OSGCameraComponent,Component>, public ICameraComponent, public IOSGCamera
	{
	public:
		OSGCameraComponent();
		~OSGCameraComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;

		//ICameraComponent interface
		bool GetCameraToViewportRay(float screenx, float screeny, Ray &ray) const override;
		void ShowInViewport(const std::string &viewport_name) override;
		
		osg::ref_ptr<osg::Camera> GetOSGCamera() const override {return m_OSGCamera;}
		void SetOSGCamera(osg::ref_ptr<osg::Camera> camera);
		void SetUpdateCameraFromLocation(bool value) {m_UpdateCameraFromLocation = value;}
	protected:
		void OnParameter(CameraParameterRequestPtr message);
		void OnTransformationChanged(TransformationChangedEventPtr message);
		void OnLocationLoaded(LocationLoadedEventPtr message);
	
		float GetFarClipDistance() const;
		void SetFarClipDistance(float value);
		float GetNearClipDistance() const;
		void SetNearClipDistance(float value);
		float GetNearFarRatio() const;
		void SetNearFarRatio(float value);
		float GetFov() const;
		void SetFov(float value);
		bool GetOrtho() const;
		void SetOrtho(bool value);
		float GetLODScale() const;
		void SetLODScale(float value);

		//helpers
		void UpdateFromLocation();
		void UpdateProjection();
	
		osg::ref_ptr<osg::Camera> m_OSGCamera;
		float m_NearClip{0.5};
		float m_FarClip{1000};
		float m_NearFarRatio{0};
		float m_Fov{45.0};
		float m_LODScale{1.0};
		bool m_Ortho{false};
		float m_OrthoWindowHeight{0};
		bool m_UpdateCameraFromLocation{true};
	};

	using OSGCameraComponentPtr = std::shared_ptr<OSGCameraComponent>;
	using OSGCameraComponentWeakPtr = std::weak_ptr<OSGCameraComponent>;
}

