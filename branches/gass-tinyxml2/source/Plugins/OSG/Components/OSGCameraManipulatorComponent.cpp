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
#include "Plugins/OSG/Components/OSGCameraManipulatorComponent.h"
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/TerrainManipulator>

#include <osgGA/TrackballManipulator>
#include <osg/MatrixTransform>
#include <osgShadow/ShadowTechnique>

#include <GL/gl.h>
#include <GL/glu.h>


#include "Plugins/OSG/Components/OSGCameraComponent.h"
#include "Plugins/OSG/Utils/MyTrackballManipulator.h"
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/Components/OSGLocationComponent.h"

namespace GASS
{
	OSGCameraManipulatorComponent::OSGCameraManipulatorComponent() : m_ManName("Terrain")
	{

	}

	OSGCameraManipulatorComponent::~OSGCameraManipulatorComponent()
	{

	}

	void OSGCameraManipulatorComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("OSGCameraManipulatorComponent",new Creator<OSGCameraManipulatorComponent, Component>);
		RegisterProperty<std::string>("Manipulator", &GASS::OSGCameraManipulatorComponent::GetManipulatorName, &GASS::OSGCameraManipulatorComponent::SetManipulatorName);
	}

	void OSGCameraManipulatorComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGCameraManipulatorComponent::OnWorldPositionRequest,WorldPositionRequest,0));

		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>()->Register(shared_from_this());

		//osgGA::TrackballManipulator *Tman1 = new osgGA::TrackballManipulator();
		if(m_ManName == "Trackball")
			m_Manipulator = new osgGA::MyTrackballManipulator();
		else 
			m_Manipulator = new osgGA::TerrainManipulator();
		
		//osgViewer::View* view = (osgViewer::View*) osg_camera->getView();
		//view->setCameraManipulator(Tman1);

		//m_OSGCamera = new osg::Camera(*views[vp_id]->getCamera());
	}

	void OSGCameraManipulatorComponent::OnWorldPositionRequest(WorldPositionRequestPtr message)
	{
		int id = PTR_TO_INT(this);
		if(message->GetSenderID() != id)
		{
			osg::Matrixd vm = m_Manipulator->getMatrix();
			osg::Vec3d translation,scale;
			osg::Quat rotation;
			osg::Quat so;
			vm.decompose(translation,rotation, scale, so );
			vm.setTrans(OSGConvert::Get().ToOSG(message->GetPosition()));
			m_Manipulator->setByMatrix(vm);
		}
	}

	void OSGCameraManipulatorComponent::SceneManagerTick(double delta)
	{
		//update location
		if(m_Manipulator.valid())
		{
			osg::Matrixd vm = m_Manipulator->getMatrix();
			osg::Vec3d translation,scale;
			osg::Quat rotation;
			osg::Quat so;
			int id = PTR_TO_INT(this);
			vm.decompose(translation,rotation, scale, so );
			GetSceneObject()->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(OSGConvert::Get().ToGASS(translation),id)));
			//GetSceneObject()->PostMessage(MessagePtr(new WorldPositionRequest(OSGConvert::Get().ToGASS(translation))));
		}
	}
}
