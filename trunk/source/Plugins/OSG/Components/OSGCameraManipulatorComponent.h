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
#include "Plugins/OSG/IOSGCameraManipulator.h"
#include "Sim/Components/Graphics/ICameraComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/Messages/CoreSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/CoreScenarioSceneMessages.h"
#include "Sim/Scenario/Scene/Messages/GraphicsScenarioSceneMessages.h"
#include "Sim/Scheduling/ITaskListener.h"
#include "Core/MessageSystem/IMessage.h"
#include <osg/Camera>

namespace GASS
{
	class OSGCameraManipulatorComponent : public Reflection<OSGCameraManipulatorComponent,BaseSceneComponent> , public IOSGCameraManipulator, public ITaskListener
	{
	public:
		OSGCameraManipulatorComponent();
		virtual ~OSGCameraManipulatorComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		//IOSGCameraManipulator
		osg::ref_ptr<osgGA::MatrixManipulator> GetManipulator() const {return m_Manipulator;}
		
		//ITaskListener
		void Update(double delta);
		TaskGroup GetTaskGroup()const;
		
	protected:
		void OnLoad(LoadGFXComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		std::string GetManipulatorName() const {return m_ManName;}
		void SetManipulatorName(const std::string &name) {m_ManName = name;}

		std::string m_ManName;// m_OrthoWindowHeight;
		osg::ref_ptr<osgGA::MatrixManipulator> m_Manipulator;
	};
	typedef boost::shared_ptr<OSGCameraManipulatorComponent> OSGCameraManipulatorComponentPtr;
	typedef boost::weak_ptr<OSGCameraManipulatorComponent> OSGCameraManipulatorComponentWeakPtr;
}

