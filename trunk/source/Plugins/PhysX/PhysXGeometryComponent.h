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

#include "PxPhysX.h"
#include "Sim/GASS.h"

namespace GASS
{
	class PhysXPhysicsSceneManager;
	class PhysXBodyComponent;
	class IGeometryComponent;
	typedef boost::shared_ptr<IGeometryComponent> GeometryComponentPtr;
	typedef boost::shared_ptr<PhysXBodyComponent> PhysXBodyComponentPtr;
	typedef boost::weak_ptr<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;
	class PhysXBaseGeometryComponent : public Reflection<PhysXBaseGeometryComponent,BaseSceneComponent>
	{
	public:
		enum PhysicsGeometryType
		{
			PGT_MESH,
			PGT_BOX,
			PGT_TERRAIN,
			PGT_CYLINDER,
			PGT_SPHERE
		};

		PhysXBaseGeometryComponent();
		virtual ~PhysXBaseGeometryComponent();
		static void RegisterReflection();
		virtual void OnCreate();

		virtual void Disable();
		virtual void Enable();
		virtual void SetScale(const Vec3 &value);
		void SetFriction(float value){m_Friction = value;}
		float GetFriction() const {return m_Friction;}



	protected:
		void CreateShape(GeometryComponentPtr geom, PhysXBodyComponentPtr body);
		NxActorDesc m_StaticActorDesc;
		NxActor *m_StaticActor;

		void SetPosition(const Vec3 &pos);
		void SetRotation(const Quaternion &rot);

		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		void OnLoad(LoadPhysicsComponentsMessagePtr message);
		void OnCollisionSettings(CollisionSettingsMessagePtr message);
		void SetOffset(const Vec3 &value){m_Offset = value;}
		Vec3 GetOffset() const {return m_Offset;}
		void SetGeometryType(const std::string &geom_type);
		std::string GetGeometryType() const;

		PhysXBodyComponentPtr m_Body;
		Vec3 m_BBSize; // bounding box start size
		float m_BSSize; // bounding sphere start size

		PhysicsGeometryType m_GeometryType;
		std::string m_GeometryTemplate;
		std::string m_AddToBody;
		Vec3 m_Offset;
		Vec3 m_CollisionGeomScale;
		float m_Friction;
		float m_Slip;
		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
	};

	typedef boost::shared_ptr<PhysXBaseGeometryComponent> PhysXBaseGeometryComponentPtr;
}
#endif 
