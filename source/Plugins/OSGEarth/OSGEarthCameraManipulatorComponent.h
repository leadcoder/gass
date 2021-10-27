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
#include "Plugins/OSG/IOSGCameraManipulator.h"
#include "Sim/Interface/GASSIMapCameraComponent.h"

namespace GASS
{
	class OSGEarthCameraManipulatorComponent : public Reflection<OSGEarthCameraManipulatorComponent,Component> ,  public  IOSGCameraManipulator, public IMapCameraComponent
	{
	public:
		OSGEarthCameraManipulatorComponent();
		~OSGEarthCameraManipulatorComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;

		//IMapCameraComponent
		double GetPitch() const override;
		void SetPitch(double value) override;
		double GetHeading() const override;
		void SetHeading(double value) override;
		double GetRange() const override;
		void SetRange(double value) override;

		osg::ref_ptr<osgGA::CameraManipulator> GetManipulator() const override {return m_Manipulator;}
		void SceneManagerTick(double delta_time) override;
	protected:
		void SetPosition(const GASS::Vec3 &pos);
		void SetRotation(const GASS::Quaternion &rot);
		osgEarth::Viewpoint GetViewpoint() const;
		void SetViewpoint(const osgEarth::Viewpoint &vp);

		void OnCameraFlyToObject(CameraFlyToObjectRequestPtr message);
		void OnTransformationChanged(TransformationChangedEventPtr event);

		osg::ref_ptr<osgEarth::Util::EarthManipulator> m_Manipulator;
		osg::Fog* m_Fog{nullptr};

		Vec3 m_CurrentPos;
		Quaternion m_CurrentRot;
		bool m_UpdateCameraFromLocation{true};
	};
}

