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
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include <ode/ode.h>
#include "Core/Math/GASSAABox.h"
#include "Core/Math/GASSQuaternion.h"
#include "ODEPhysicsSceneManager.h"
#include "IODEGeometryComponent.h"
#include "Sim/Interface/GASSIPhysicsGeometryComponent.h"

namespace GASS
{
	class IGeometryComponent;
	class ODEBodyComponent;
	class ODEPhysicsSceneManager;
	using ODEPhysicsSceneManagerWeakPtr = std::weak_ptr<ODEPhysicsSceneManager>;
	using GeometryComponentPtr = std::shared_ptr<IGeometryComponent>;

	/**
	An ODE geometry-object has a lot of common operations/functionality like, transformations, 
	collision settings and friction. 
	This class implements that common ODE-geometry functionality to reduce the implementation 
	load of derived components. This class is not registred to the component manager and can 
	not be instantiated in configuration files
	*/

	class ODEBaseGeometryComponent : public Reflection<ODEBaseGeometryComponent,BaseSceneComponent> , public IPhysicsGeometryComponent, public IODEGeometryComponent
	{
	friend class ODEPhysicsSceneManager;
	public:
		ODEBaseGeometryComponent();
		~ODEBaseGeometryComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
	protected:

		//Message functions
		void OnBodyLoaded(PhysicsBodyLoadedEventPtr message);
		void OnLocationLoaded(LocationLoadedEventPtr message);
		void OnCollisionSettings(CollisionSettingsRequestPtr message);
		void OnTransformationChanged(TransformationChangedEventPtr message);
		void OnGeometryChanged(GeometryChangedEventPtr message);
		
		//virtual functions that derived geometry have to implement
		virtual dGeomID CreateODEGeom() {return nullptr;}
		virtual void SetSizeFromMesh(bool /*value*/) {};
		virtual void UpdateBodyMass(){};

		//common ode geometry functionality, 
		//all functions are virtual for max flexibility
		virtual void UpdateODEGeom();
		virtual void Disable();
		virtual void Enable();
		virtual void SetFriction(float value){m_Friction = value;}
		float GetFriction() const override {return m_Friction;}
		virtual void Reset();
		virtual void SetPosition(const Vec3 &pos);
		virtual void SetRotation(const Quaternion &rot);
		virtual dSpaceID GetSpace();
		virtual void SetOffset(const Vec3 &value);
		virtual Vec3 GetOffset() const {return m_Offset;}
		virtual unsigned long GetCollisionBits() const;
		virtual void SetCollisionBits(unsigned long value);
		virtual unsigned long GetCollisionCategory() const;
		virtual void SetCollisionCategory(unsigned long value);
		virtual bool GetSizeFromMesh() const;
		virtual GeometryComponentPtr GetGeometry() const;
		virtual bool IsInitialized() const;

		//debug fucntions
		virtual void UpdateDebug() {};
		virtual void OnPhysicsDebug(PhysicsDebugRequestPtr message);
		virtual void OnDebugTransformation(TransformationChangedEventPtr message);
		virtual void SetDebug(bool value);
		virtual bool GetDebug() const;
		virtual SceneObjectPtr GetDebugObject();
	protected:
		dGeomID m_GeomID{0};
		//dGeomID m_TransformGeomID;
		dSpaceID m_ODESpaceID{NULL};
		ODEBodyComponent* m_Body{NULL}; //pointer to body!
		std::string m_GeometryTemplate;
		std::string m_AddToBody;
		Vec3 m_Offset;
		float m_Friction{1};
		unsigned long m_CollisionCategory{1};
		unsigned long m_CollisionBits{1};
		bool m_SizeFromMesh{true};
		bool m_Debug{false};
		ODEPhysicsSceneManagerWeakPtr m_SceneManager;

	};
}
