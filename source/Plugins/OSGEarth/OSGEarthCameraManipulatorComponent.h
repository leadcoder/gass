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
	class OSGEarthCameraManipulatorComponent : public Reflection<OSGEarthCameraManipulatorComponent,BaseSceneComponent> ,  public  IOSGCameraManipulator, public IMapCameraComponent
	{
	public:
		OSGEarthCameraManipulatorComponent();
		~OSGEarthCameraManipulatorComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;

		//IMapCameraComponent
		double GetPitch() const;
		void SetPitch(double value);
		double GetHeading() const;
		void SetHeading(double value);
		double GetRange() const;
		void SetRange(double value);

		osg::ref_ptr<osgGA::CameraManipulator> GetManipulator() const override {return m_Manipulator;}
		void SceneManagerTick(double delta_time) override;
	protected:
		void _SetPosition(const GASS::Vec3 &pos);
		void _SetRotation(const GASS::Quaternion &rot);
		osgEarth::Viewpoint _GetVP() const;
		void _SetVP(const osgEarth::Viewpoint &vp);

		void OnCameraFlyToObject(CameraFlyToObjectRequestPtr message);
		void OnTransformationChanged(TransformationChangedEventPtr event);

		osg::ref_ptr<osgEarth::Util::EarthManipulator> m_Manipulator;
		osg::Fog* m_Fog;

		Vec3 m_CurrentPos;
		Quaternion m_CurrentRot;
		bool m_UpdateCameraFromLocation;
	};
}

