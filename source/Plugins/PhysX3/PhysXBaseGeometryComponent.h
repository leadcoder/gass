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
	typedef GASS_SHARED_PTR<IGeometryComponent> GeometryComponentPtr;
	typedef GASS_SHARED_PTR<PhysXBodyComponent> PhysXBodyComponentPtr;
	typedef GASS_WEAK_PTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;
	
	
	enum
	{
		/*COLLISION_FLAG_GROUND			=	1 << 0,
		COLLISION_FLAG_WHEEL			=	1 << 1,
		COLLISION_FLAG_CHASSIS			=	1 << 2,
		COLLISION_FLAG_OBSTACLE			=	1 << 3,
		COLLISION_FLAG_DRIVABLE_OBSTACLE=	1 << 4,
		*/
		COLLISION_FLAG_GROUND_AGAINST	=																	GEOMETRY_FLAG_VEHICLE_CHASSIS | GEOMETRY_FLAG_DYNAMIC_OBJECT,
		COLLISION_FLAG_WHEEL_AGAINST	=								   GEOMETRY_FLAG_RAY_CAST_WHEEL |	GEOMETRY_FLAG_VEHICLE_CHASSIS | GEOMETRY_FLAG_DYNAMIC_OBJECT,
		COLLISION_FLAG_CHASSIS_AGAINST	=			GEOMETRY_FLAG_GROUND | GEOMETRY_FLAG_RAY_CAST_WHEEL |	GEOMETRY_FLAG_VEHICLE_CHASSIS | GEOMETRY_FLAG_DYNAMIC_OBJECT,
		COLLISION_FLAG_OBSTACLE_AGAINST	=			GEOMETRY_FLAG_GROUND | GEOMETRY_FLAG_RAY_CAST_WHEEL |	GEOMETRY_FLAG_VEHICLE_CHASSIS | GEOMETRY_FLAG_DYNAMIC_OBJECT,
		COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST =	GEOMETRY_FLAG_GROUND 								|	GEOMETRY_FLAG_VEHICLE_CHASSIS | GEOMETRY_FLAG_DYNAMIC_OBJECT,



	/*	COLLISION_FLAG_GROUND_AGAINST	=									COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS  | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
		COLLISION_FLAG_WHEEL_AGAINST	=			COLLISION_FLAG_GROUND    											   | COLLISION_FLAG_OBSTACLE,
		COLLISION_FLAG_CHASSIS_AGAINST	=			COLLISION_FLAG_GROUND						 |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
		COLLISION_FLAG_OBSTACLE_AGAINST	=			COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
		COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST=	COLLISION_FLAG_GROUND 						 |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	*/
	};
	
	class PhysXBaseGeometryComponent : public Reflection<PhysXBaseGeometryComponent,BaseSceneComponent>
	{
	public:
		PhysXBaseGeometryComponent();
		virtual ~PhysXBaseGeometryComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		virtual void Disable(){};
		virtual void Enable();
		virtual void SetScale(const Vec3 &value);
	protected:
		ADD_PROPERTY(bool,SimulationCollision);
		//messages functions
		void OnLoad(MessagePtr message);
		void OnTransformationChanged(TransformationChangedEventPtr message);
		void OnCollisionSettings(CollisionSettingsRequestPtr message);
		
		virtual physx::PxShape* CreateShape() = 0;
		void SetPosition(const Vec3 &pos);
		void SetRotation(const Quaternion &rot);
		virtual void SetOffset(const Vec3 &value){m_Offset = value;}
		virtual Vec3 GetOffset() const {return m_Offset;}
		virtual bool GetSizeFromMesh() const;
		virtual void SetSizeFromMesh(bool value);
		GeometryComponentPtr GetGeometry() const;

		
		PhysXBodyComponentPtr m_Body;		
		Vec3 m_Offset;
		bool m_SizeFromMesh;
		physx::PxRigidStatic *m_StaticActor;
		physx::PxShape *m_Shape;
	};
	typedef GASS_SHARED_PTR<PhysXBaseGeometryComponent> PhysXBaseGeometryComponentPtr;
}
#endif 
