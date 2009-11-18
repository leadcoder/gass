/****************************************************************************
*                                                                           *
* HiFiEngine                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: hifiengine@gmail.com                                               *
* Web page: http://n00b.dyndns.org/HiFiEngine                               *
*                                                                           *
* HiFiEngine is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the HiFiEngine license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 

#if !defined PHYSX_GEOMETRY_H
#define PHYSX_GEOMETRY_H

#include "NxPhysics.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Core/Math/AABox.h"
#include "Core/Math/Quaternion.h"

class NxActor;

namespace GASS
{
	class PhysXPhysicsSceneManager;
	class PhysXBodyComponent;
	class IGeometryComponent;
	typedef boost::shared_ptr<IGeometryComponent> GeometryComponentPtr;
	typedef boost::shared_ptr<PhysXBodyComponent> PhysXBodyComponentPtr;
	class PhysXGeometryComponent : public Reflection<PhysXGeometryComponent,BaseSceneComponent>
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

		PhysXGeometryComponent();
		virtual ~PhysXGeometryComponent();
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
		PhysXPhysicsSceneManager* m_SceneManager;
	};

	typedef boost::shared_ptr<PhysXGeometryComponent> PhysXGeometryComponentPtr;
}
#endif 
