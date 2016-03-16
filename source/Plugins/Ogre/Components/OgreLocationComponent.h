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
#include "Sim/GASSCommon.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "OgreSceneNodeSource.h"
#include <OgreNode.h>

namespace Ogre
{
	class SceneNode;
}

namespace GASS
{
	class OgreLocationComponent;
	typedef GASS_SHARED_PTR<OgreLocationComponent> OgreLocationComponentPtr;

	class GASSPluginExport OgreLocationComponent : public Reflection<OgreLocationComponent,BaseSceneComponent>, public ILocationComponent, public Ogre::Node::Listener, public OgreSceneNodeSource
	{
	public:
		OgreLocationComponent();
		virtual ~OgreLocationComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		virtual void SetPosition(const Vec3 &value);
		virtual Vec3 GetPosition() const;
		virtual Vec3 GetWorldPosition() const;
		virtual void SetEulerRotation(const Vec3 &value);
		virtual Vec3 GetEulerRotation() const;
		virtual void SetScale(const Vec3 &value);
		virtual Vec3 GetScale() const {return m_Scale;}
		virtual void SetRotation(const Quaternion &value);
		virtual Quaternion GetRotation() const;
		virtual Quaternion GetWorldRotation() const;

		/** Set if position should be relative to first LocationComponent that is 
		* found in component hierarchy (true) or absolut position (false) */
		virtual void SetAttachToParent(bool value);
		/** Get value indicating if position is relative to first LocationComponent that is found in component hierarchy */
		virtual bool GetAttachToParent() const;

		inline Ogre::SceneNode* GetOgreNode(){return m_OgreNode;}
		void SetVisibility(bool visibility);

		//Ogre node listener interface
		virtual void nodeUpdated(const Ogre::Node* node);
		/** Node is being destroyed */
		virtual void nodeDestroyed(const Ogre::Node* node) {}
		/** Node has been attached to a parent */
		virtual void nodeAttached(const Ogre::Node* node) {}
		/** Node has been detached from a parent */
		virtual void nodeDetached(const Ogre::Node* node) {}

	protected:
		void SetWorldRotation(const Quaternion &rot);
		void SetWorldPosition(const Vec3 &pos);
		void OnScaleMessage(ScaleRequestPtr message);
		void PositionRequest(PositionRequestPtr message);
		void RotationRequest(RotationRequestPtr message);
		void VisibilityRequest(LocationVisibilityRequestPtr message);
		void ParentChangedEvent(ParentChangedEventPtr message);
		void WorldPositionRequest(WorldPositionRequestPtr message);
		void WorldRotationRequest(WorldRotationRequestPtr message);
		void BoundingInfoRequest(BoundingInfoRequestPtr message);
		void OnUpdateEulerAngles(UpdateEulerAnglesRequestPtr message);
		void OnAttachToParent(AttachToParentRequestPtr message);

		//helper function to get first parent with location component
		OgreLocationComponentPtr GetParentLocation();

		Vec3 m_Pos;
		Vec3 m_LastPos;

		//! relative rotation of the scene node.
		Vec3 m_Rot;
		Vec3 m_LastRot;

		//! relative scale of the scene node.
		Vec3 m_Scale;
		Ogre::SceneNode* m_OgreNode;
		Quaternion m_QRot;

		// Should this location node be relative to parent's location? First with a LocationComponent
		bool m_AttachToParent;
	};
}