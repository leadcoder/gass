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
	class OSGCameraComponent : public Reflection<OSGCameraComponent, ICameraComponent>, public IOSGCamera
	{
	public:
		OSGCameraComponent();
		~OSGCameraComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;

		//ICameraComponent interface
		bool GetCameraToViewportRay(float screenx, float screeny, Ray &ray) const override;
		void ShowInViewport(const std::string &viewport_name) override;
		float GetFarClipDistance() const override;
		void SetFarClipDistance(float value) override;
		float GetNearClipDistance() const override;
		void SetNearClipDistance(float value) override;
		float GetNearFarRatio() const override;
		void SetNearFarRatio(float value) override;
		float GetFov() const override;
		void SetFov(float value) override;
		bool GetOrtho() const override;
		void SetOrtho(bool value) override;
		float GetLODScale() const override;
		void SetLODScale(float value) override;

		
		osg::ref_ptr<osg::Camera> GetOSGCamera() const override {return m_OSGCamera;}
		void SetOSGCamera(osg::ref_ptr<osg::Camera> camera);
		void SetUpdateCameraFromLocation(bool value) {m_UpdateCameraFromLocation = value;}
	protected:
		void OnTransformationChanged(TransformationChangedEventPtr message);
		void OnLocationLoaded(LocationLoadedEventPtr message);
	
		
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

