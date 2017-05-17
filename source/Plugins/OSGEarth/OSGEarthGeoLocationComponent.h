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
//#   pragma warning (disable : 4541)
#include "Sim/GASS.h"
#include "Plugins/OSG/OSGCommon.h"
#include "Plugins/OSG/IOSGNode.h"
#include "Plugins/OSG/IOSGGraphicsSceneManager.h"
#include <osgEarth/GeoTransform>
#include <osgEarth/MapNode>


namespace GASS
{
	class OSGGraphicsSceneManager;
	class OSGEarthGeoLocationComponent;
	typedef GASS_SHARED_PTR<OSGEarthGeoLocationComponent>  OSGEarthGeoLocationComponentPtr;
	typedef GASS_WEAK_PTR<OSGGraphicsSceneManager> OSGGraphicsSceneManagerWeakPtr;

	class OSGEarthGeoLocationComponent : public Reflection<OSGEarthGeoLocationComponent,BaseSceneComponent>, public ILocationComponent,  public IOSGNode, public osg::NodeCallback
	{
	public:
		OSGEarthGeoLocationComponent();
		virtual ~OSGEarthGeoLocationComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();

		virtual void SetScale(const Vec3 &value);
		virtual void SetPosition(const Vec3 &value);
		virtual Vec3 GetPosition() const;
		virtual void SetWorldPosition(const Vec3 &value);
		virtual Vec3 GetWorldPosition() const;
		
		virtual void SetEulerRotation(const EulerRotation &value);
		virtual EulerRotation GetEulerRotation() const;
		virtual void SetRotation(const Quaternion &value);
		virtual Quaternion GetRotation() const;
		virtual void SetWorldRotation(const Quaternion &value);
		virtual Quaternion GetWorldRotation() const;
		virtual Vec3 GetScale() const {return m_Scale;}
			
		//IOSGNode interface
		virtual osg::ref_ptr<osg::Node> GetNode() {return m_TransformNode;}

		//move this to private
		osg::ref_ptr<osg::PositionAttitudeTransform> GetOSGNode() const {return m_TransformNode;}
		void SetOSGNode(osg::ref_ptr<osg::PositionAttitudeTransform> node) {m_TransformNode = node;}
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
		bool GetAttachToParent() const;
	protected:
		void OnPositionMessage(PositionRequestPtr message);
		void OnRotationMessage(RotationRequestPtr  message);
		void OnScaleMessage(ScaleRequestPtr message);
		void OnWorldPositionRequest(WorldPositionRequestPtr message);
		void OnWorldRotationMessage(WorldRotationRequestPtr message);
		void OnParentChangedMessage(ParentChangedEventPtr message);
		void OnAttachToParent(AttachToParentRequestPtr message);
		void OnVisibilityMessage(LocationVisibilityRequestPtr message);

		void SetAttachToParent(bool value);
		
		//helper
		OSGEarthGeoLocationComponentPtr _GetParentLocation();
		void _SendTransMessage();
		//! relative position of the scene node.
		Vec3 m_Pos;
		//! relative rotation of the scene node.
		EulerRotation m_Rot;
		Quaternion m_QRot;
		//! relative scale of the scene node.
		Vec3 m_Scale;
		bool m_AttachToParent;
		osg::ref_ptr<osg::PositionAttitudeTransform> m_TransformNode;
		osg::ref_ptr<osgEarth::GeoTransform> m_GeoTransform;
		IOSGGraphicsSceneManagerWeakPtr m_GFXSceneManager;
		int m_NodeMask;
		osg::ref_ptr<osgEarth::MapNode> m_Map;
	};

	typedef GASS_WEAK_PTR<OSGEarthGeoLocationComponent> OSGEarthGeoLocationComponentWeakPtr;
	typedef GASS_SHARED_PTR<OSGEarthGeoLocationComponent> OSGEarthGeoLocationComponentPtr;	
}