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
#include "Sim/GASS.h"
#include "Plugins/OSG/IOSGCameraManipulator.h"
#include <osg/Camera>

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
		osg::ref_ptr<osgGA::CameraManipulator> GetManipulator() const {return m_Manipulator;}
	protected:
		std::string GetManipulatorName() const {return m_ManName;}
		void SetManipulatorName(const std::string &name) {m_ManName = name;}

		std::string m_ManName;// m_OrthoWindowHeight;
		osg::ref_ptr<osgGA::CameraManipulator> m_Manipulator;
	};
	typedef boost::shared_ptr<OSGCameraManipulatorComponent> OSGCameraManipulatorComponentPtr;
	typedef boost::weak_ptr<OSGCameraManipulatorComponent> OSGCameraManipulatorComponentWeakPtr;
}

