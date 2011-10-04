/****************************************************************************
* This file is part of GASS.                                                *
* See http://cHavok.google.com/p/gass/                                 *
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
#include "Sim/Scenario/Scene/Messages/PhysicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/CoreSceneObjectMessages.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Math/AABox.h"
#include "Core/Math/Quaternion.h"
#include "HavokPhysicsSceneManager.h"
#include "Sim/Components/Physics/IPhysicsGeometryComponent.h"

class hkpTransformShape;

namespace GASS
{
	class IGeometryComponent;
	class HavokBodyComponent;
	class HavokPhysicsSceneManager;
	typedef boost::weak_ptr<HavokPhysicsSceneManager> HavokPhysicsSceneManagerWeakPtr;
	typedef boost::shared_ptr<IGeometryComponent> GeometryComponentPtr;

	/**
	An Havok geometry-object has a lot of common operations/functionality like, transformations, 
	collision settings and friction. 
	This class implements that common Havok-geometry functionality to reduce the implementation 
	load of derived components. This class is not registred to the component manager and can 
	not be instantiated in configuration files
	*/

	class HavokBaseGeometryComponent : public Reflection<HavokBaseGeometryComponent,BaseSceneComponent> , public IPhysicsGeometryComponent
	{
	friend class HavokPhysicsSceneManager;
	public:
		HavokBaseGeometryComponent();
		virtual ~HavokBaseGeometryComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual hkpShape* GetShape() const {return (hkpShape*) m_TransformShape;}
	protected:

		//Message functions
		void OnLoad(LoadPhysicsComponentsMessagePtr message);
		void OnCollisionSettings(CollisionSettingsMessagePtr message);
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		void OnGeometryChanged(GeometryChangedMessagePtr message);
		
		//virtual functions that derived geometry have to implement
		virtual void SetSizeFromMesh(bool value) {};
		virtual void UpdateBodyMass(){};

		//common Havok geometry functionality, 
		//all functions are virtual for max flexibility
		//virtual void UpdateHavokGeom();
		virtual hkpShape*  CreateHavokShape() {return NULL;}
		
		virtual void Disable();
		virtual void Enable();
		virtual void SetFriction(float value){m_Friction = value;}
		virtual float GetFriction() const {return m_Friction;}
		//virtual void Reset();
		virtual void SetPosition(const Vec3 &pos);
		virtual void SetRotation(const Quaternion &rot);
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
		HavokBodyComponent* m_Body; //pointer to body!
		std::string m_GeometryTemplate;
		std::string m_AddToBody;
		Vec3 m_Offset;
		float m_Friction;
		long int m_CollisionCategory;
		long int m_CollisionBits;
		bool m_SizeFromMesh;
		bool m_Debug;
		HavokPhysicsSceneManagerWeakPtr m_SceneManager;
		hkpShape* m_Shape;
		hkpTransformShape* m_TransformShape;
	};

	typedef boost::shared_ptr<HavokBaseGeometryComponent> HavokBaseGeometryComponentPtr;
}
