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


#if !defined PHYSX_GEOMETRY_H
#define PHYSX_GEOMETRY_H

#include "PxPhysics.h"
#include "PhysXCommon.h"

namespace physx
{
	class PxActor;
}

namespace GASS
{
	class PhysXPhysicsSceneManager;
	class PhysXBodyComponent;
	class IGeometryComponent;
	typedef boost::shared_ptr<IGeometryComponent> GeometryComponentPtr;
	typedef boost::shared_ptr<PhysXBodyComponent> PhysXBodyComponentPtr;
	typedef boost::weak_ptr<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;
	
	
	enum
	{
		COLLISION_FLAG_GROUND			=	1 << 0,
		COLLISION_FLAG_WHEEL			=	1 << 1,
		COLLISION_FLAG_CHASSIS			=	1 << 2,
		COLLISION_FLAG_OBSTACLE			=	1 << 3,
		COLLISION_FLAG_DRIVABLE_OBSTACLE=	1 << 4,

		COLLISION_FLAG_GROUND_AGAINST	=									COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS  | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
		COLLISION_FLAG_WHEEL_AGAINST	=			COLLISION_FLAG_GROUND    											   | COLLISION_FLAG_OBSTACLE,
		COLLISION_FLAG_CHASSIS_AGAINST	=			COLLISION_FLAG_GROUND						 |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
		COLLISION_FLAG_OBSTACLE_AGAINST	=			COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
		COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST=	COLLISION_FLAG_GROUND 						 |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	};
	
	class PhysXBaseGeometryComponent : public Reflection<PhysXBaseGeometryComponent,BaseSceneComponent>
	{
	public:
		PhysXBaseGeometryComponent();
		virtual ~PhysXBaseGeometryComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void Disable(){};
		virtual void Enable();
		virtual void SetScale(const Vec3 &value);
		void SetFriction(float value){m_Friction = value;}
		float GetFriction() const {return m_Friction;}
	protected:

		//messages functions
		void OnBodyLoaded(BodyLoadedMessagePtr message);
		void OnLocationLoaded(LocationLoadedMessagePtr message);
		void OnGeometryChanged(GeometryChangedMessagePtr message);
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		void OnCollisionSettings(CollisionSettingsMessagePtr message);
		virtual physx::PxShape* CreateShape(){return NULL;}
		
		void SetPosition(const Vec3 &pos);
		void SetRotation(const Quaternion &rot);
		
		virtual void SetOffset(const Vec3 &value){m_Offset = value;}
		virtual Vec3 GetOffset() const {return m_Offset;}
		virtual unsigned long GetCollisionBits() const;
		virtual void SetCollisionBits(unsigned long value);
		virtual unsigned long GetCollisionCategory() const;
		virtual void SetCollisionCategory(unsigned long value);
		virtual bool GetSizeFromMesh() const;
		virtual void SetSizeFromMesh(bool value);
		GeometryComponentPtr GetGeometry() const;
		
		PhysXBodyComponentPtr m_Body;		
		Vec3 m_Offset;
		float m_Friction;
		unsigned long m_CollisionCategory;
		unsigned long m_CollisionBits;
		bool m_SizeFromMesh;
		physx::PxRigidStatic *m_StaticActor;
		physx::PxShape *m_Shape;
		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
		std::string m_GeometryTemplate;
	};
	typedef boost::shared_ptr<PhysXBaseGeometryComponent> PhysXBaseGeometryComponentPtr;
}
#endif 
