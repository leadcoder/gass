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
#include "Plugins/OSG/IOSGCameraManipulator.h"


namespace osgGA
{
	class CustomTerrainManipulator;
};


namespace GASS
{
	class OSGCameraManipulatorComponent : public Reflection<OSGCameraManipulatorComponent,Component> , public IOSGCameraManipulator
	{
	public:
		OSGCameraManipulatorComponent();
		~OSGCameraManipulatorComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void SceneManagerTick(double delta) override;
		//IOSGCameraManipulator
		osg::ref_ptr<osgGA::CameraManipulator> GetManipulator() const override {return m_OrbitMan;}
	protected:
		void SetRotation(const Quaternion& rot);
		void SetPosition(const Vec3& pos);
		void OnTransformationChanged(TransformationChangedEventPtr event);
		std::string GetManipulatorName() const {return m_ManName;}
		void SetManipulatorName(const std::string &name) {m_ManName = name;}
	private:
		static void ExtractTransformationFromOrbitManipulator(osgGA::OrbitManipulator* man, Vec3 &pos, Quaternion &rot);
		static void SetOrbitManRotation(osgGA::OrbitManipulator* man, const Quaternion &rot);
		static void SetOrbitManPosition(osgGA::OrbitManipulator* man, const Vec3 &pos);
		std::string m_ManName;
		osg::ref_ptr<osgGA::OrbitManipulator> m_OrbitMan;
		bool m_ReadyToRun{false};
		bool m_UpdateCameraFromLocation{true};
	};
	using OSGCameraManipulatorComponentPtr = std::shared_ptr<OSGCameraManipulatorComponent>;
	using OSGCameraManipulatorComponentWeakPtr = std::weak_ptr<OSGCameraManipulatorComponent>;
}

