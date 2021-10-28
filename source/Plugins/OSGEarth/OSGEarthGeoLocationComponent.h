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
#include "Plugins/OSG/IOSGNode.h"
#include "Plugins/OSG/IOSGGraphicsSceneManager.h"

namespace GASS
{
	class OSGGraphicsSceneManager;
	class OSGEarthGeoLocationComponent;
	using OSGEarthGeoLocationComponentPtr = std::shared_ptr<OSGEarthGeoLocationComponent>;
	using OSGGraphicsSceneManagerWeakPtr = std::weak_ptr<OSGGraphicsSceneManager>;

	class OSGEarthGeoLocationComponent : public Reflection<OSGEarthGeoLocationComponent,Component>, public ILocationComponent,  public IOSGNode, public osg::NodeCallback
	{
	public:
		OSGEarthGeoLocationComponent();
		~OSGEarthGeoLocationComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;

		void SetScale(const Vec3 &value) override;
		void SetPosition(const Vec3 &value) override;
		Vec3 GetPosition() const override;
		void SetWorldPosition(const Vec3 &value) override;
		Vec3 GetWorldPosition() const override;
		
		void SetEulerRotation(const EulerRotation &value) override;
		EulerRotation GetEulerRotation() const override;
		void SetRotation(const Quaternion &value) override;
		Quaternion GetRotation() const override;
		void SetWorldRotation(const Quaternion &value) override;
		Quaternion GetWorldRotation() const override;
		Vec3 GetScale() const override {return m_Scale;}

		void SetVisible(bool value) override;
		bool GetVisible() const override;
		bool HasParentLocation() const override;

			
		//IOSGNode interface
		osg::ref_ptr<osg::Node> GetNode() override {return m_TransformNode;}

		//move this to private
		osg::ref_ptr<osg::PositionAttitudeTransform> GetOSGNode() const {return m_TransformNode;}
		void SetOSGNode(osg::ref_ptr<osg::PositionAttitudeTransform> node) {m_TransformNode = node;}
		void operator()(osg::Node* node, osg::NodeVisitor* nv) override;
		bool GetAttachToParent() const override;
	protected:
		void OnParentChanged(ParentChangedEventPtr message);
		void SetAttachToParent(bool value) override;
		
		//helper
		OSGEarthGeoLocationComponentPtr GetParentLocation();
		void SendTransMessage();
		//! relative position of the scene node.
		Vec3 m_Pos;
		//! relative rotation of the scene node.
		EulerRotation m_Rot;
		Quaternion m_QRot;
		//! relative scale of the scene node.
		Vec3 m_Scale;
		bool m_AttachToParent{false};
		osg::ref_ptr<osg::PositionAttitudeTransform> m_TransformNode;
		osg::ref_ptr<osgEarth::GeoTransform> m_GeoTransform;
		IOSGGraphicsSceneManagerWeakPtr m_GFXSceneManager;
		int m_NodeMask{0};
		osg::ref_ptr<osgEarth::MapNode> m_Map;
	};

	using OSGEarthGeoLocationComponentWeakPtr = std::weak_ptr<OSGEarthGeoLocationComponent>;
	using OSGEarthGeoLocationComponentPtr = std::shared_ptr<OSGEarthGeoLocationComponent>;	
}