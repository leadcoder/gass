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
	using OSGLocationComponentPtr = std::shared_ptr<OSGLocationComponent>;
	using OSGGraphicsSceneManagerWeakPtr = std::weak_ptr<OSGGraphicsSceneManager>;

	class OSGLocationComponent : public Reflection<OSGLocationComponent, Component>, public ILocationComponent, public IOSGNode, public osg::NodeCallback
	{
	public:
		OSGLocationComponent();
		~OSGLocationComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;

		//ILocationComponent
		Vec3 GetPosition() const override;
		void SetPosition(const Vec3 &value) override;

		Vec3 GetWorldPosition() const override;
		void SetWorldPosition(const Vec3 &value) override;

		void SetEulerRotation(const EulerRotation &value) override;
		EulerRotation GetEulerRotation() const override;

		Quaternion GetRotation() const override;
		void SetRotation(const Quaternion& value) override;

		Quaternion GetWorldRotation() const override;
		void SetWorldRotation(const Quaternion& value) override;

		Vec3 GetScale() const override;
		void SetScale(const Vec3 &value) override;

		bool GetAttachToParent() const override;
		void SetAttachToParent(bool value) override;

		void SetVisible(bool value) override;
		bool GetVisible() const override;

		bool HasParentLocation() const override;
		//end ILocationComponent
			
		//IOSGNode interface
		osg::ref_ptr<osg::Node> GetNode() override {return m_TransformNode;}

		//move this to private
		osg::ref_ptr<osg::PositionAttitudeTransform> GetOSGNode() const {return m_TransformNode;}
		void operator()(osg::Node* node, osg::NodeVisitor* nv) override;
	protected:
		void OnParentChanged(ParentChangedEventPtr message);
		
		//remove this?
		void SetOSGNode(osg::ref_ptr<osg::PositionAttitudeTransform> node) { m_TransformNode = node; }

		//Internal functions
		OSGLocationComponent* GetParentLocation() const { return m_ParentLocation; }
		void OnPositionUpdateRecursive(SceneObjectPtr scene_object);
		void OnRotationUpdateRecursive(SceneObjectPtr scene_object);
		void OnParentPositionUpdated();
		void OnParentRotationUpdated();
		void NotifyTransformationChange() const;
		Vec3 LocalToWorld(const Vec3 &world_pos) const;
		Vec3 WorldToLocal(const Vec3 &local_pos) const;
		Quaternion WorldToLocal(const Quaternion &world_rot) const;
		Quaternion LocalToWorld(const Quaternion &local_rot) const;
		OSGLocationComponentPtr GetFirstParentLocation() const;
		osg::ref_ptr<osg::Group> GetOsgRootGroup();
		
		//! relative position of the scene node.
		Vec3 m_Position;
		Vec3 m_WorldPosition;
		//! relative rotation of the scene node.
		Quaternion m_Rotation;
		//! relative rotation of the scene node in euler angles.
		EulerRotation m_EulerRotation;
		Quaternion m_WorldRotation;
		//! relative scale of the scene node.
		Vec3 m_Scale;
		bool m_AttachToParent{false};
		OSGLocationComponent* m_ParentLocation{nullptr};
		osg::ref_ptr<osg::PositionAttitudeTransform> m_TransformNode;
		OSGGraphicsSceneManagerWeakPtr m_GFXSceneManager;
		unsigned int m_NodeMask{0};
	};

	using OSGLocationComponentWeakPtr = std::weak_ptr<OSGLocationComponent>;
	using OSGLocationComponentPtr = std::shared_ptr<OSGLocationComponent>;	
}