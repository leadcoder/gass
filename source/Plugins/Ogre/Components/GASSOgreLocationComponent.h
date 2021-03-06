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
#include "Plugins/Ogre/GASSOgreCommon.h"
#include "GASSOgreSceneNodeSource.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSEulerRotation.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"

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
		~OgreLocationComponent() override;
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

		Vec3 GetScale() const override { return m_Scale;}
		void SetScale(const Vec3 &value) override;

		bool GetAttachToParent() const override;
		void SetAttachToParent(bool value) override;

		void SetVisible(bool value) override;
		bool GetVisible() const override;

		bool HasParentLocation() const override;

		//end ILocationComponent
	
		inline Ogre::SceneNode* GetOgreNode() override{return m_OgreNode;}
		
		//Ogre node listener interface
		void nodeUpdated(const Ogre::Node* node) override;
		/** Node is being destroyed */
		void nodeDestroyed(const Ogre::Node* /*node*/) override {}
		/** Node has been attached to a parent */
		void nodeAttached(const Ogre::Node* /*node*/) override {}
		/** Node has been detached from a parent */
		void nodeDetached(const Ogre::Node* /*node*/) override {}

	protected:
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

		void NotifyTransformationChange();
		//helper function to get first parent with location component
		OgreLocationComponentPtr GetParentLocation();

		Vec3 m_Pos;
		//Vec3 m_LastPos;

		//! relative rotation of the scene node.
		EulerRotation m_EulerRot;
		//Vec3 m_LastRot;

		//! relative scale of the scene node.
		Vec3 m_Scale;
		Ogre::SceneNode* m_OgreNode;
		Quaternion m_QRot;

		// Should this location node be relative to parent's location? First with a LocationComponent
		bool m_AttachToParent;
		bool m_Visible;
	};
}