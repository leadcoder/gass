/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
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

#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include <ode/ode.h>
#include "Core/MessageSystem/IMessage.h"
#include "Core/Math/AABox.h"
#include "Core/Math/Quaternion.h"
#include "ODEPhysicsSceneManager.h"
#include "ODEGeometry.h"

namespace GASS
{
	class ITerrainComponent;
	class IGeometryComponent;
	class ODEBodyComponent;
	class ODEPhysicsSceneManager;
	typedef boost::weak_ptr<ODEPhysicsSceneManager> ODEPhysicsSceneManagerWeakPtr;
	typedef boost::shared_ptr<IGeometryComponent> GeometryComponentPtr;

	class ODEBoxGeometryComponent : public Reflection<ODEBoxGeometryComponent,BaseSceneComponent> , public IPhysicsGeometry
	{
	friend class ODEPhysicsSceneManager;
	public:
		ODEBoxGeometryComponent();
		virtual ~ODEBoxGeometryComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	protected:
		//Message functions
		void OnLoad(LoadPhysicsComponentsMessagePtr message);
		void OnCollisionSettings(CollisionSettingsMessagePtr message);
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		void OnGeometryChanged(GeometryChangedMessagePtr message);
		
		
		void UpdateODEGeom();

		void Disable();
		void Enable();
		void SetScale(const Vec3 &value);
		void SetFriction(float value){m_Friction = value;}
		float GetFriction() const {return m_Friction;}
		void Reset();
		
		void SetPosition(const Vec3 &pos);
		void SetRotation(const Quaternion &rot);
		dSpaceID GetSpace();
		
		//void SetScale(const Vec3 &value);
		void SetOffset(const Vec3 &value);
		Vec3 GetOffset() const {return m_Offset;}
		
		//void SetSlip(float value){m_Friction = value;}
		//float GetSlip() const {return m_Friction;}
		
		long int GetCollisionBits() const;
		void SetCollisionBits(long int value);

		long int GetCollisionCategory() const;
		void SetCollisionCategory(long int value);

		bool GetSizeFromMesh() const;
		void SetSizeFromMesh(bool value);



		void SetSize(const Vec3 &size);
		Vec3 GetSize() const;

		GeometryComponentPtr GetGeometry() const;
		void UpdateBodyMass();
		bool IsInitialized() const;

		//debug functions
		void CreateDebugBox(const Vec3 &size,const Vec3 &offset);
		SceneObjectPtr GetDebugObject();
		void UpdateDebug();
		void OnPhysicsDebug(PhysicsDebugMessagePtr message);
		void OnDebugTransformation(TransformationNotifyMessagePtr message);

		void SetDebug(bool value);
		bool GetDebug() const;
	protected:
		dGeomID m_GeomID;
		dGeomID m_TransformGeomID;
		dSpaceID m_ODESpaceID;
		ODEBodyComponent* m_Body; //pointer to body!
		Vec3 m_Size; //bounding box start size
		std::string m_GeometryTemplate;
		std::string m_AddToBody;
		Vec3 m_Offset;
		Vec3 m_BBOffset;
		Vec3 m_CollisionGeomScale;
		float m_Friction;
		float m_Slip;
		ODEPhysicsSceneManagerWeakPtr m_SceneManager;
		long int m_CollisionCategory;
		long int m_CollisionBits;
		bool m_SizeFromMesh;
		bool m_Debug;
	};
}
