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



#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/TrackballManipulator>
#include <osg/MatrixTransform>
#include <osgShadow/ShadowTechnique>
#include <osgEarth/MapNode>

#include <osg/Camera>
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/IOSGGraphicsSceneManager.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/Interface/GASSIGraphicsSceneManager.h"
#include "OSGEarthCameraManipulatorComponent.h"
#include "OSGEarthSceneManager.h"
//#include "OSGEarthPlaceNodeComponent.h"
#include "Plugins/OSG/IOSGNode.h"


namespace GASS
{
	OSGEarthCameraManipulatorComponent::OSGEarthCameraManipulatorComponent() 
	{

	}

	OSGEarthCameraManipulatorComponent::~OSGEarthCameraManipulatorComponent()
	{

	}

	void OSGEarthCameraManipulatorComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("OSGEarthCameraManipulatorComponent",new Creator<OSGEarthCameraManipulatorComponent, Component>);
	}

	void OSGEarthCameraManipulatorComponent::OnInitialize()
	{
		SceneManagerListenerPtr listener = shared_from_this();
		GASS_SHARED_PTR<BaseSceneManager> osg_sm = GASS_DYNAMIC_PTR_CAST<BaseSceneManager>(GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<IGraphicsSceneManager>());
		
		if(!osg_sm)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find IGraphicsSceneManager", "OSGEarthCameraManipulatorComponent::OnInitialize");
		
		osg_sm->Register(listener);

		GASS_SHARED_PTR<OSGEarthSceneManager> earth_sm = GASS_DYNAMIC_PTR_CAST<OSGEarthSceneManager>(GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGEarthSceneManager>());
		if (!earth_sm)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find OSGEarthSceneManager", "OSGEarthCameraManipulatorComponent::OnInitialize");

		osgEarth::Util::EarthManipulator* manip = earth_sm->GetManipulator().get();

		if (!manip)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed get osgEarth::Util::EarthManipulator from OSGEarthSceneManager", "OSGEarthCameraManipulatorComponent::OnInitialize");
		
		m_Manipulator = manip;

		osgEarth::Util::EarthManipulator::Settings* settings = new osgEarth::Util::EarthManipulator::Settings();

		// install default action bindings:
		osgEarth::Util::EarthManipulator::ActionOptions options;

		settings->bindKey( osgEarth::Util::EarthManipulator::ACTION_HOME, osgGA::GUIEventAdapter::KEY_Space );

		settings->bindMouse( osgEarth::Util::EarthManipulator::ACTION_PAN, osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON);
		//settings->bindMouse( osgEarth::Util::EarthManipulator::ACTION_PAN, osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON);

		settings->bindMouse( osgEarth::Util::EarthManipulator::ACTION_ROTATE, osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON );

		// zoom as you hold the right button:
		options.clear();
		options.add( osgEarth::Util::EarthManipulator::OPTION_CONTINUOUS, true );
		//settings->bindMouse( osgEarth::Util::EarthManipulator::ACTION_ROTATE, osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON, 0L, options );

		// zoom with the scroll wheel:
		settings->bindScroll( osgEarth::Util::EarthManipulator::ACTION_ZOOM_IN,  osgGA::GUIEventAdapter::SCROLL_DOWN );
		settings->bindScroll( osgEarth::Util::EarthManipulator::ACTION_ZOOM_OUT, osgGA::GUIEventAdapter::SCROLL_UP );

		// pan around with arrow keys:
		settings->bindKey( osgEarth::Util::EarthManipulator::ACTION_PAN_LEFT,  osgGA::GUIEventAdapter::KEY_Left );
		settings->bindKey( osgEarth::Util::EarthManipulator::ACTION_PAN_RIGHT, osgGA::GUIEventAdapter::KEY_Right );
		settings->bindKey( osgEarth::Util::EarthManipulator::ACTION_PAN_UP,    osgGA::GUIEventAdapter::KEY_Up );
		settings->bindKey( osgEarth::Util::EarthManipulator::ACTION_PAN_DOWN,  osgGA::GUIEventAdapter::KEY_Down );

		// double click the left button to zoom in on a point:
		options.clear();
		options.add( osgEarth::Util::EarthManipulator::OPTION_GOTO_RANGE_FACTOR, 0.4 );
		//settings->bindMouseDoubleClick( osgEarth::Util::EarthManipulator::ACTION_GOTO, osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON, 0L, options );

		// double click the right button (or CTRL-left button) to zoom out to a point
		options.clear();
		options.add( osgEarth::Util::EarthManipulator::OPTION_GOTO_RANGE_FACTOR, 2.5 );
		settings->bindMouseDoubleClick( osgEarth::Util::EarthManipulator::ACTION_GOTO, osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON, 0L, options );
		settings->bindMouseDoubleClick( osgEarth::Util::EarthManipulator::ACTION_GOTO, osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON, osgGA::GUIEventAdapter::MODKEY_CTRL, options );

		//settings->setThrowingEnabled( false );
		settings->setLockAzimuthWhilePanning( true );
		manip->applySettings(settings);

		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGEarthCameraManipulatorComponent::OnTrackRequest,CameraTrackObjectRequest,0));
	}

	void OSGEarthCameraManipulatorComponent::OnTrackRequest(CameraTrackObjectRequestPtr message)
	{
		if(m_Manipulator.valid())
		{
			SceneObjectPtr so = message->GetTrackObject();
			if(so)
			{
				//OSGEarthPlaceNodeComponentPtr place_comp = so->GetFirstComponentByClass<OSGEarthPlaceNodeComponent>();
				OSGNodePtr node_wrapper = so->GetFirstComponentByClass<IOSGNode>();
				if(node_wrapper)
				{
					m_Manipulator->setViewpoint(osgEarth::Viewpoint(
						"Home",
						-71.0763, 42.34425, 0,   // longitude, latitude, altitude
						24.261, -21.6, 3450.0), // heading, pitch, range
						5.0);
					m_Manipulator->setTetherNode(node_wrapper->GetNode());
				}
			}
			else
				m_Manipulator->setTetherNode(NULL);
		}
	}

	void OSGEarthCameraManipulatorComponent::SceneManagerTick(double delta_time)
	{
		//update location
		if(m_Manipulator.valid())
		{
			osg::Matrixd vm = m_Manipulator->getMatrix();
			osg::Vec3d translation,scale;
			osg::Quat rotation;
			osg::Quat so;
			vm.decompose(translation,rotation, scale, so );
			GetSceneObject()->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(Vec3(translation.x(),translation.z(),-translation.y()))));
		}
	}
	
}

