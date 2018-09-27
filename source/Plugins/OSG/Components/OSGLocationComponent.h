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
#include "Sim/GASSEulerRotation.h"
#include "Plugins/OSG/OSGCommon.h"
#include "Plugins/OSG/IOSGNode.h"

namespace GASS
{
	class OSGGraphicsSceneManager;
	class OSGLocationComponent;
	typedef GASS_SHARED_PTR<OSGLocationComponent>  OSGLocationComponentPtr;
	typedef GASS_WEAK_PTR<OSGGraphicsSceneManager> OSGGraphicsSceneManagerWeakPtr;

	class OSGLocationComponent : public Reflection<OSGLocationComponent, BaseSceneComponent>, public ILocationComponent, public IOSGNode, public osg::NodeCallback
	{
	public:
		OSGLocationComponent();
		virtual ~OSGLocationComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();

		//ILocationComponent
		virtual Vec3 GetPosition() const;
		virtual void SetPosition(const Vec3 &value);

		virtual Vec3 GetWorldPosition() const;
		virtual void SetWorldPosition(const Vec3 &value);

		virtual void SetEulerRotation(const EulerRotation &value);
		virtual EulerRotation GetEulerRotation() const;

		virtual Quaternion GetRotation() const;
		virtual void SetRotation(const Quaternion& value);

		virtual Quaternion GetWorldRotation() const;
		virtual void SetWorldRotation(const Quaternion& value);

		virtual Vec3 GetScale() const { return m_Scale; }
		virtual void SetScale(const Vec3 &value);

		virtual bool GetAttachToParent() const;
		virtual void SetAttachToParent(bool value);
		//end ILocationComponent

			
		//IOSGNode interface
		virtual osg::ref_ptr<osg::Node> GetNode() {return m_TransformNode;}

		//move this to private
		osg::ref_ptr<osg::PositionAttitudeTransform> GetOSGNode() const {return m_TransformNode;}
		void SetOSGNode(osg::ref_ptr<osg::PositionAttitudeTransform> node) {m_TransformNode = node;}
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
	protected:
		void OnPositionMessage(PositionRequestPtr message);
		void OnRotationMessage(RotationRequestPtr  message);
		void OnScaleMessage(ScaleRequestPtr message);
		void OnWorldPositionRequest(WorldPositionRequestPtr message);
		void OnWorldRotationMessage(WorldRotationRequestPtr message);
		void OnParentChangedMessage(ParentChangedEventPtr message);
		void OnAttachToParent(AttachToParentRequestPtr message);
		void OnVisibilityMessage(LocationVisibilityRequestPtr message);
		
		//helper
		OSGLocationComponentPtr _GetParentLocation();
		void _SendTransMessage();
		//! relative position of the scene node.
		Vec3 m_Pos;
		//! relative rotation of the scene node.
		EulerRotation m_EulerRot;
		Quaternion m_QRot;
		//! relative scale of the scene node.
		Vec3 m_Scale;
		bool m_AttachToParent;
		osg::ref_ptr<osg::PositionAttitudeTransform> m_TransformNode;
		OSGGraphicsSceneManagerWeakPtr m_GFXSceneManager;
		int m_NodeMask;
	};

	typedef GASS_WEAK_PTR<OSGLocationComponent> OSGLocationComponentWeakPtr;
	typedef GASS_SHARED_PTR<OSGLocationComponent> OSGLocationComponentPtr;	
}