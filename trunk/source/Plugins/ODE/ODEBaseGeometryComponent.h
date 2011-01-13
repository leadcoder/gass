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
	class IGeometryComponent;
	class ODEBodyComponent;
	class ODEPhysicsSceneManager;
	typedef boost::weak_ptr<ODEPhysicsSceneManager> ODEPhysicsSceneManagerWeakPtr;
	typedef boost::shared_ptr<IGeometryComponent> GeometryComponentPtr;

	/**
	An ODE geometry-object has a lot of common operations/functionality like, transformations, 
	collision settings and friction. 
	This class implements that common ODE geometry functionality to reduce the implementation 
	load of derived components. This class have some (by purpose) pure virtual functions that must me 
	implemented by derived components.
	*/

	class ODEBaseGeometryComponent : public Reflection<ODEBaseGeometryComponent,BaseSceneComponent> , public IPhysicsGeometry
	{
	friend class ODEPhysicsSceneManager;
	public:
		ODEBaseGeometryComponent();
		virtual ~ODEBaseGeometryComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	protected:

		//Message functions
		void OnLoad(LoadPhysicsComponentsMessagePtr message);
		void OnCollisionSettings(CollisionSettingsMessagePtr message);
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		void OnGeometryChanged(GeometryChangedMessagePtr message);
		
		//virtual functions that derived geometry have to implement
		virtual dGeomID CreateODEGeom() {return 0;}
		virtual void SetSizeFromMesh(bool value) {};
		virtual void UpdateBodyMass(){};

	
		//common ode geometry functionality, 
		//all functions are virtual for max flexibility
		virtual void UpdateODEGeom();
		virtual void Disable();
		virtual void Enable();
		virtual void SetFriction(float value){m_Friction = value;}
		virtual float GetFriction() const {return m_Friction;}
		virtual void Reset();
		virtual void SetPosition(const Vec3 &pos);
		virtual void SetRotation(const Quaternion &rot);
		virtual dSpaceID GetSpace();
		virtual void SetOffset(const Vec3 &value);
		virtual Vec3 GetOffset() const {return m_Offset;}
		
		virtual long int GetCollisionBits() const;
		virtual void SetCollisionBits(long int value);

		virtual long int GetCollisionCategory() const;
		virtual void SetCollisionCategory(long int value);

		virtual bool GetSizeFromMesh() const;
		
		virtual GeometryComponentPtr GetGeometry() const;
		virtual bool IsInitialized() const;

		//debug fucntions
		virtual void UpdateDebug() {};
		virtual void OnPhysicsDebug(PhysicsDebugMessagePtr message);
		virtual void OnDebugTransformation(TransformationNotifyMessagePtr message);
		virtual void SetDebug(bool value);
		virtual bool GetDebug() const;
		virtual SceneObjectPtr GetDebugObject();
	protected:
		dGeomID m_GeomID;
		dGeomID m_TransformGeomID;
		dSpaceID m_ODESpaceID;
		ODEBodyComponent* m_Body; //pointer to body!
		std::string m_GeometryTemplate;
		std::string m_AddToBody;
		Vec3 m_Offset;
		float m_Friction;
		long int m_CollisionCategory;
		long int m_CollisionBits;
		bool m_SizeFromMesh;
		bool m_Debug;
		ODEPhysicsSceneManagerWeakPtr m_SceneManager;
	};
}
