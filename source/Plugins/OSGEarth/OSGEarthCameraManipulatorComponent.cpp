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

#include "OSGEarthCommonIncludes.h"
#include "OSGEarthCameraManipulatorComponent.h"
#include "OSGEarthSceneManager.h"
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/IOSGGraphicsSceneManager.h"
#include "Plugins/OSG/IOSGCamera.h"
#include "Plugins/OSG/IOSGNode.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/Interface/GASSIGraphicsSceneManager.h"

namespace GASS
{
	OSGEarthCameraManipulatorComponent::OSGEarthCameraManipulatorComponent() : 
		m_CurrentPos(0,0,0)
		
	{

	}

	OSGEarthCameraManipulatorComponent::~OSGEarthCameraManipulatorComponent()
	{

	}

	void OSGEarthCameraManipulatorComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<OSGEarthCameraManipulatorComponent>();
	}

	void OSGEarthCameraManipulatorComponent::OnInitialize()
	{
		RegisterForPreUpdate<IGraphicsSceneManager>();
	
		GASS_SHARED_PTR<OSGEarthSceneManager> earth_sm = GASS_DYNAMIC_PTR_CAST<OSGEarthSceneManager>(GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGEarthSceneManager>());
		if (!earth_sm)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find OSGEarthSceneManager", "OSGEarthCameraManipulatorComponent::OnInitialize");

		osgEarth::Util::EarthManipulator* manip = earth_sm->GetManipulator().get();

		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGEarthCameraManipulatorComponent::OnTransformationChanged, TransformationChangedEvent, 0));

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
		//options.clear();
		//options.add( osgEarth::Util::EarthManipulator::OPTION_GOTO_RANGE_FACTOR, 0.4 );
		//settings->bindMouseDoubleClick( osgEarth::Util::EarthManipulator::ACTION_GOTO, osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON, 0L, options );

		// double click the right button (or CTRL-left button) to zoom out to a point
		options.clear();
		//options.add( osgEarth::Util::EarthManipulator::OPTION_GOTO_RANGE_FACTOR, 2.5 );
		options.add(osgEarth::Util::EarthManipulator::OPTION_GOTO_RANGE_FACTOR, 0.2);
		settings->bindMouseDoubleClick( osgEarth::Util::EarthManipulator::ACTION_GOTO, osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON, 0L, options );
		//settings->bindMouseDoubleClick( osgEarth::Util::EarthManipulator::ACTION_GOTO, osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON, osgGA::GUIEventAdapter::MODKEY_CTRL, options );

		//settings->setThrowingEnabled( false );
		settings->setLockAzimuthWhilePanning( true );
		manip->applySettings(settings);

		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGEarthCameraManipulatorComponent::OnCameraFlyToObject, CameraFlyToObjectRequest,0));
	}

	void OSGEarthCameraManipulatorComponent::OnTransformationChanged(TransformationChangedEventPtr event)
	{
		if (m_UpdateCameraFromLocation)
		{
			_SetPosition(event->GetPosition());
			_SetRotation(event->GetRotation());
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
					if (focalPoint.fromWorld(mapSRS, osg_location))
					{
						osgEarth::Viewpoint vp;
						vp.focalPoint() = focalPoint;
						vp.pitch()->set(-90.0, osgEarth::Units::DEGREES);
						vp.range()->set(rad * 3, osgEarth::Units::METERS);
						m_Manipulator->setViewpoint(vp, 2.0);
					}
				}
			}
		}
	}

	void OSGEarthCameraManipulatorComponent::SceneManagerTick(double /*delta_time*/)
	{
		//update location
		if(m_Manipulator.valid())
		{
			m_Manipulator->setIntersectTraversalMask(~0u);
			osg::Matrixd vm = m_Manipulator->getMatrix();

			osg::Vec3d translation,scale;
			osg::Quat rotation,so;
			vm.decompose(translation, rotation, scale, so );

			const int id = GASS_PTR_TO_INT(this);
			const GASS::Vec3 pos = OSGConvert::ToGASS(translation);
			const GASS::Quaternion rot = OSGConvert::ToGASS(rotation);

			m_UpdateCameraFromLocation = false;
			//only update if position changed
			if (!pos.Equal(m_CurrentPos, 0.0001))
			{
				GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->SetWorldPosition(pos);
				m_CurrentPos = pos;
			}

			//only update if rotation changed
			if (rot != m_CurrentRot)
			{
				GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->SetWorldRotation(rot);
				m_CurrentRot = rot;
			}
			m_UpdateCameraFromLocation = true;

			

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

	double OSGEarthCameraManipulatorComponent::GetPitch() const
	{
		return _GetVP().pitch()->as(osgEarth::Units::DEGREES);
	}

	void OSGEarthCameraManipulatorComponent::SetPitch(double value)
	{
		osgEarth::Viewpoint vp = _GetVP();
		vp.setPitch(osgEarth::Angle(value, osgEarth::Units::DEGREES));
		_SetVP(vp);
	}

	double OSGEarthCameraManipulatorComponent::GetHeading() const
	{
		return _GetVP().heading()->as(osgEarth::Units::DEGREES);
	}

	void OSGEarthCameraManipulatorComponent::SetHeading(double value)
	{
		osgEarth::Viewpoint vp = _GetVP();
		vp.setHeading(osgEarth::Angle(value, osgEarth::Units::DEGREES));
		_SetVP(vp);
	}

	double OSGEarthCameraManipulatorComponent::GetRange() const
	{
		return _GetVP().range()->as(osgEarth::Units::METERS);
	}

	void OSGEarthCameraManipulatorComponent::SetRange(double value)
	{
		osgEarth::Viewpoint vp = _GetVP();
		vp.setRange(osgEarth::Distance(value, osgEarth::Units::METERS));
		_SetVP(vp);
	}

	osgEarth::Viewpoint OSGEarthCameraManipulatorComponent::_GetVP() const
	{
		return m_Manipulator->getViewpoint();
	}

	void OSGEarthCameraManipulatorComponent::_SetVP(const osgEarth::Viewpoint &vp)
	{
		m_Manipulator->setViewpoint(vp, 1.0);
	}
}
