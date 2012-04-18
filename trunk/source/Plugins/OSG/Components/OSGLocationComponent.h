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
#   pragma warning (disable : 4541)
#include "Sim/GASS.h"
#include <osg/PositionAttitudeTransform>

namespace GASS
{
	class OSGGraphicsSceneManager;
	class OSGLocationComponent;
	typedef boost::shared_ptr<OSGLocationComponent>  OSGLocationComponentPtr;
	typedef boost::weak_ptr<OSGGraphicsSceneManager> OSGGraphicsSceneManagerWeakPtr;

	class OSGLocationComponent : public Reflection<OSGLocationComponent,BaseSceneComponent>, public ILocationComponent,  public osg::NodeCallback
	{
	public:
		OSGLocationComponent();
		virtual ~OSGLocationComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	
		virtual void SetScale(const Vec3 &value);
		virtual void SetPosition(const Vec3 &value);
		virtual Vec3 GetPosition() const;
		virtual void SetWorldPosition(const Vec3 &value);
		virtual Vec3 GetWorldPosition() const;
		
		virtual void SetEulerRotation(const Vec3 &value);
		virtual Vec3 GetEulerRotation() const;
		virtual void SetRotation(const Quaternion &value);
		virtual Quaternion GetRotation() const;
		virtual void SetWorldRotation(const Quaternion &value);
		virtual Quaternion GetWorldRotation() const;
		virtual Vec3 GetScale() const {return m_Scale;}
		osg::ref_ptr<osg::PositionAttitudeTransform> GetOSGNode() {return m_TransformNode;}

		 virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

	protected:
		void OnLoad(LoadGFXComponentsMessagePtr message);
		void OnPositionMessage(PositionMessagePtr message);
		void OnRotationMessage(RotationMessagePtr  message);
		void OnScaleMessage(ScaleMessagePtr message);
		void OnWorldPositionMessage(WorldPositionMessagePtr message);
		void OnWorldRotationMessage(WorldRotationMessagePtr message);
		void OnParentChangedMessage(ParentChangedMessagePtr message);
		void OnAttachToParent(AttachToParentMessagePtr message);

		void SetAttachToParent(bool value);
		bool GetAttachToParent() const;

		//helper
		OSGLocationComponentPtr GetParentLocation();
		
		
		void OnVisibilityMessage(VisibilityMessagePtr message);
		void SendTransMessage();
		Vec3 m_Pos;
		//Vec3 m_LastPos;

		//! relative rotation of the scene node.
		Vec3 m_Rot;
		//Vec3 m_LastRot;

		bool m_AttachToParent;

		//! relative scale of the scene node.
		Vec3 m_Scale;
		osg::ref_ptr<osg::PositionAttitudeTransform> m_TransformNode;
		//osg::ref_ptr<osg::PositionAttitudeTransform> m_RotTransformNode;
		OSGGraphicsSceneManagerWeakPtr m_GFXSceneManager;

		int m_NodeMask;
		
	};

	typedef boost::weak_ptr<OSGLocationComponent> OSGLocationComponentWeakPtr;
	typedef boost::shared_ptr<OSGLocationComponent> OSGLocationComponentPtr;
	
}

