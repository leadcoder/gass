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

#pragma once
#include "Sim/Common.h"
#include "Core/Math/Vector.h"
#include "Sim/Components/Graphics/ICameraComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Sim/Scenario/Scene/ScenarioSceneMessages.h"
#include "Core/MessageSystem/IMessage.h"
#include <osg/Camera>

namespace GASS
{
	class OSGCameraComponent : public Reflection<OSGCameraComponent,BaseSceneComponent>, public ICameraComponent
	{
	public:
		OSGCameraComponent();
		virtual ~OSGCameraComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		//ICameraComponent interface
		virtual bool GetCameraToViewportRay(float screenx, float screeny, Vec3 &ray_start, Vec3 &ray_dir) const;
		osg::ref_ptr<osg::Camera> GetOSGCamera() const {return m_OSGCamera;}
		void SetOSGCamera(osg::ref_ptr<osg::Camera> camera);
		void SetUpdateCameraFromLocation(bool value) {m_UpdateCameraFromLocation = value;}
	protected:
		void OnParameter(CameraParameterMessagePtr message);
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		//void OnPositionChanged(PositionMessagePtr message);
		//void OnRotationChanged(RotationMessagePtr message);
		void OnLoad(LoadGFXComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnChangeCamera(ChangeCameraMessagePtr message);
	
		float GetFarClipDistance() const;
		void SetFarClipDistance(float value);
		float GetNearClipDistance() const;
		void SetNearClipDistance(float value);
		float GetFov() const;
		void SetFov(float value);
		bool GetOrtho() const;
		void SetOrtho(bool value);

		void UpdateFromLocation();
		void UpdateProjection();
	
		osg::ref_ptr<osg::Camera> m_OSGCamera;
		//cameras to reflect
		//std::map<std::string,osg::ref_ptr<osg::Camera> > m_OSGCameras;

		float m_NearClip;
		float m_FarClip;
		float m_Fov;
		bool m_Ortho;
		float m_OrthoWindowHeight;
		bool m_UpdateCameraFromLocation;
	};

	typedef boost::shared_ptr<OSGCameraComponent> OSGCameraComponentPtr;
	typedef boost::weak_ptr<OSGCameraComponent> OSGCameraComponentWeakPtr;
}

