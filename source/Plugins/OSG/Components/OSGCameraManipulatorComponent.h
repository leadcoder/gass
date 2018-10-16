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
	class OSGCameraManipulatorComponent : public Reflection<OSGCameraManipulatorComponent,BaseSceneComponent> , public IOSGCameraManipulator
	{
	public:
		OSGCameraManipulatorComponent();
		virtual ~OSGCameraManipulatorComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void SceneManagerTick(double delta);
		//IOSGCameraManipulator
		osg::ref_ptr<osgGA::CameraManipulator> GetManipulator() const {return m_OrbitMan;}
	protected:
		void SetRotation(const Quaternion& rot);
		void SetPosition(const Vec3& pos);
		void OnTransformationChanged(TransformationChangedEventPtr event);
		std::string GetManipulatorName() const {return m_ManName;}
		void SetManipulatorName(const std::string &name) {m_ManName = name;}
	private:
		static void _ExtractTransformationFromOrbitManipulator(osgGA::OrbitManipulator* man, GASS::Vec3 &pos, GASS::Quaternion &rot);
		static void _SetOrbitManRotation(osgGA::OrbitManipulator* man, const GASS::Quaternion &rot);
		static void _SetOrbitManPosition(osgGA::OrbitManipulator* man, const GASS::Vec3 &pos);
		std::string m_ManName;
		osg::ref_ptr<osgGA::OrbitManipulator> m_OrbitMan;
		bool m_ReadyToRun;
		bool m_UpdateCameraFromLocation;
	};
	typedef GASS_SHARED_PTR<OSGCameraManipulatorComponent> OSGCameraManipulatorComponentPtr;
	typedef GASS_WEAK_PTR<OSGCameraManipulatorComponent> OSGCameraManipulatorComponentWeakPtr;
}

