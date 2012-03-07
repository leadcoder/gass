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

#include <boost/bind.hpp>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/TrackballManipulator>
#include <osg/MatrixTransform>
#include <osgShadow/ShadowTechnique>

#include <GL/gl.h>
#include <GL/glu.h>

#include "Plugins/OSG/Components/OSGCameraManipulatorComponent.h"
#include "Plugins/OSG/Components/OSGCameraComponent.h"
#include "Plugins/OSG/Utils/MyTrackballManipulator.h"



#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include "Sim/Systems/SimSystemManager.h"
//#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Plugins/OSG/OSGNodeMasks.h"

#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/Components/OSGLocationComponent.h"


namespace GASS
{
	OSGCameraManipulatorComponent::OSGCameraManipulatorComponent() 
	{

	}

	OSGCameraManipulatorComponent::~OSGCameraManipulatorComponent()
	{

	}

	void OSGCameraManipulatorComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("OSGCameraManipulatorComponent",new Creator<OSGCameraManipulatorComponent, IComponent>);
		RegisterProperty<std::string>("Manipulator", &GASS::OSGCameraManipulatorComponent::GetManipulatorName, &GASS::OSGCameraManipulatorComponent::SetManipulatorName);
	}

	void OSGCameraManipulatorComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraManipulatorComponent::OnLoad,LoadGFXComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraManipulatorComponent::OnUnload,UnloadComponentsMessage,1));
		//GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraManipulatorComponent::OnParameter,CameraParameterMessage,1));
		//GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraManipulatorComponent::OnTransformationChanged,TransformationNotifyMessage,10));
	}

	/*void OSGCameraManipulatorComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		UpdateFromLocation();
	}*/

	void OSGCameraManipulatorComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
			SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
	}

	void OSGCameraManipulatorComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Register(this);

		/*OSGCameraComponentPtr camera = GetSceneObject()->GetFirstComponentByClass<OSGCameraComponent>();
		osg::ref_ptr<osg::Camera> osg_camera = camera->GetOSGCamera();
		camera->SetUpdateCameraFromLocation(false);*/
		
		//osgGA::TrackballManipulator *Tman1 = new osgGA::TrackballManipulator();
		m_Manipulator = new osgGA::MyTrackballManipulator();
		
		//osgViewer::View* view = (osgViewer::View*) osg_camera->getView();
		//view->setCameraManipulator(Tman1);

		//m_OSGCamera = new osg::Camera(*views[vp_id]->getCamera());
	}

	void OSGCameraManipulatorComponent::Update(double delta)
	{
		//update location
		if(m_Manipulator.valid())
		{
			osg::Matrixd vm = m_Manipulator->getMatrix();
			osg::Vec3d translation,scale;
			osg::Quat rotation;
			osg::Quat so;
			vm.decompose(translation,rotation, scale, so );
			GetSceneObject()->PostMessage(MessagePtr(new WorldPositionMessage(OSGConvert::Get().ToGASS(translation))));
			//GetSceneObject()->PostMessage(MessagePtr(new WorldPositionMessage(OSGConvert::Get().ToGASS(translation))));
		}
	}

	TaskGroup OSGCameraManipulatorComponent::GetTaskGroup() const
	{
		return MAIN_TASK_GROUP;
	}
}

