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
#include <osgEarthUtil/ViewFitter>

#include <osg/Camera>
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/IOSGGraphicsSceneManager.h"
#include "Plugins/OSG/IOSGCamera.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/Interface/GASSIGraphicsSceneManager.h"
#include "OSGEarthCameraManipulatorComponent.h"
#include "OSGEarthSceneManager.h"
#include "Plugins/OSG/IOSGNode.h"


namespace GASS
{
	OSGEarthCameraManipulatorComponent::OSGEarthCameraManipulatorComponent() : m_Fog(NULL),
		m_CurrentPos(0,0,0)
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

		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGEarthCameraManipulatorComponent::OnWorldPositionRequest, WorldPositionRequest, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGEarthCameraManipulatorComponent::OnWorldRotationRequest, WorldRotationRequest, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGEarthCameraManipulatorComponent::OnPositionRequest, PositionRequest, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGEarthCameraManipulatorComponent::OnRotationRequest, RotationRequest, 0));

		if (!manip)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed get osgEarth::Util::EarthManipulator from OSGEarthSceneManager", "OSGEarthCameraManipulatorComponent::OnInitialize");
		
		IOSGGraphicsSceneManagerPtr iosg_sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<IOSGGraphicsSceneManager>();
		osg::ref_ptr<osg::Group> fog_root = iosg_sm->GetOSGRootNode();
		osg::StateSet* state = fog_root->getOrCreateStateSet();
		m_Fog = (osg::Fog *) state->getAttribute(osg::StateAttribute::FOG);
		m_Manipulator = manip;

		osgEarth::Util::EarthManipulator::Settings* settings = new osgEarth::Util::EarthManipulator::Settings();

		// install default action bindings:
		osgEarth::Util::EarthManipulator::ActionOptions options;

		settings->bindKey( osgEarth::Util::EarthManipulator::ACTION_HOME, osgGA::GUIEventAdapter::KEY_Space );

		settings->bindMouse( osgEarth::Util::EarthManipulator::ACTION_PAN, osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON);
		settings->bindMouse( osgEarth::Util::EarthManipulator::ACTION_PAN, osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON, osgGA::GUIEventAdapter::MODKEY_CTRL);
		//settings->bindMouse( osgEarth::Util::EarthManipulator::ACTION_PAN, osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON);

		settings->bindMouse( osgEarth::Util::EarthManipulator::ACTION_ROTATE, osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON );

		// zoom as you hold the right button:
		options.clear();
		options.add( osgEarth::Util::EarthManipulator::OPTION_CONTINUOUS, true );
		//settings->bindMouse( osgEarth::Util::EarthManipulator::ACTION_ROTATE, osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON, 0L, options );

		// zoom with the scroll wheel:
		settings->bindScroll( osgEarth::Util::EarthManipulator::ACTION_ZOOM_IN,  osgGA::GUIEventAdapter::SCROLL_DOWN );
		settings->bindScroll( osgEarth::Util::EarthManipulator::ACTION_ZOOM_OUT, osgGA::GUIEventAdapter::SCROLL_UP );

		settings->bindMouse(osgEarth::Util::EarthManipulator::ACTION_ZOOM, osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON, osgGA::GUIEventAdapter::MODKEY_CTRL);

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

		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGEarthCameraManipulatorComponent::OnCameraFlyToObject, CameraFlyToObjectRequest,0));
	}


	void OSGEarthCameraManipulatorComponent::OnWorldPositionRequest(WorldPositionRequestPtr message)
	{
		int id = GASS_PTR_TO_INT(this);
		if (message->GetSenderID() != id)
		{
			_SetPosition(message->GetPosition());
		}
	}

	void OSGEarthCameraManipulatorComponent::OnWorldRotationRequest(WorldRotationRequestPtr message)
	{
		const int id = GASS_PTR_TO_INT(this);
		if (message->GetSenderID() != id)
		{
			_SetRotation(message->GetRotation());
		}
	}

	void OSGEarthCameraManipulatorComponent::OnPositionRequest(PositionRequestPtr message)
	{
		int id = GASS_PTR_TO_INT(this);
		if (message->GetSenderID() != id)
		{
			_SetPosition(message->GetPosition());
		}
	}

	void OSGEarthCameraManipulatorComponent::OnRotationRequest(RotationRequestPtr message)
	{
		int id = GASS_PTR_TO_INT(this);
		if (message->GetSenderID() != id)
		{
			_SetRotation(message->GetRotation());
		}
	}

	void OSGEarthCameraManipulatorComponent::_SetPosition(const GASS::Vec3 &pos)
	{
		if (m_Manipulator)
		{
			osg::Matrix mat = m_Manipulator->getMatrix();
			mat.setTrans(OSGConvert::ToOSG(pos));
			m_Manipulator->setByMatrix(mat);

			m_CurrentPos = pos;
		}
	}

	void OSGEarthCameraManipulatorComponent::_SetRotation(const GASS::Quaternion &rot)
	{
		if (m_Manipulator)
		{

			osg::Matrixd vm = m_Manipulator->getMatrix();
			vm.setRotate(OSGConvert::ToOSG(rot));
			m_Manipulator->setByMatrix(vm);
			m_CurrentRot = rot;
		}
	}

	bool GetObjectPosAndSize(SceneObjectPtr obj, AABox &object_bounds, Vec3 &object_pos)
	{
		GeometryComponentPtr gc = obj->GetFirstComponentByClass<IGeometryComponent>(true);
		if (gc)
		{
			object_bounds = gc->GetBoundingBox();
			LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
			if (lc)
			{
				
				object_pos = lc->GetWorldPosition();
				Mat4 trans(lc->GetWorldRotation(), object_pos);
				object_bounds.Min = trans*object_bounds.Min;
				object_bounds.Max = trans*object_bounds.Max;
			}
			else
				return false;
		}
		else
		{
			LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
			if (lc)
				object_pos = lc->GetWorldPosition();
			else
				return false;
		}
		return true;
	}

	void OSGEarthCameraManipulatorComponent::OnCameraFlyToObject(CameraFlyToObjectRequestPtr message)
	{
		if (m_Manipulator.valid())
		{
			SceneObjectPtr obj = message->GetTargetObject();
			AABox object_bounds;
			Vec3 object_pos(0, 0, 0);
			if (GetObjectPosAndSize(obj, object_bounds, object_pos))
			{

				const double rad = object_bounds.GetBoundingSphere().m_Radius;
				

				osgEarth::MapNode* mapNode = osgEarth::MapNode::findMapNode(m_Manipulator->getNode());
				if (mapNode)
				{
					const osgEarth::SpatialReference* mapSRS = mapNode->getMapSRS();
					const osg::Vec3d osg_location = OSGConvert::ToOSG(object_pos);
					osgEarth::GeoPoint focalPoint(mapSRS, 0, 0, 0, osgEarth::ALTMODE_ABSOLUTE);
					bool status = focalPoint.fromWorld(mapSRS, osg_location);
					osgEarth::Viewpoint vp;
					vp.focalPoint() = focalPoint;
					vp.pitch() = -90;
					vp.range() = rad * 3;
					m_Manipulator->setViewpoint(vp, 2.0);
				}
			}
		}
	}

	void OSGEarthCameraManipulatorComponent::SceneManagerTick(double delta_time)
	{
		//update location
		if(m_Manipulator.valid())
		{
			m_Manipulator->setIntersectTraversalMask(~0);
			osg::Matrixd vm = m_Manipulator->getMatrix();

			osg::Vec3d translation,scale;
			osg::Quat rotation,so;
			vm.decompose(translation, rotation, scale, so );

			const int id = GASS_PTR_TO_INT(this);
			const GASS::Vec3 pos = OSGConvert::ToGASS(translation);
			const GASS::Quaternion rot = OSGConvert::ToGASS(rotation);

			//only update if position changed
			if (!pos.Equal(m_CurrentPos, 0.0001))
			{
				GetSceneObject()->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(pos, id)));
				m_CurrentPos = pos;
			}

			//only update if rotation changed
			if (rot != m_CurrentRot)
			{
				GetSceneObject()->PostRequest(WorldRotationRequestPtr(new WorldRotationRequest(rot, id)));
				m_CurrentRot = rot;
			}

			//use height based fog
		/*	double visibiliy = 40000;
			double hazeDensity = 1.0 / visibiliy;
			double height = translation.z();
			static const double H = 8435.0; // Pressure scale height of Earth's atmosphere
			double isothermalEffect = exp(-(height / H));
			if (isothermalEffect <= 0) isothermalEffect = 1E-9;
			if (isothermalEffect > 1.0) isothermalEffect = 1.0;
			hazeDensity *= isothermalEffect;
			m_Fog->setDensity(hazeDensity);*/
		}
	}
}
