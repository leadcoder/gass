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

#include "Plugins/PhysX3/PhysXBaseGeometryComponent.h"
#include "Plugins/PhysX3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"

namespace GASS
{

	PhysXBaseGeometryComponent::PhysXBaseGeometryComponent() :m_StaticActor(NULL),
		m_Shape(NULL),
		m_Offset(0,0,0)
	{
	}

	PhysXBaseGeometryComponent::~PhysXBaseGeometryComponent()
	{
	}

	void PhysXBaseGeometryComponent::Enable()
	{

	}

	void PhysXBaseGeometryComponent::RegisterReflection()
	{
		RegisterProperty<Vec3>("Offset", &GASS::PhysXBaseGeometryComponent::GetOffset, &GASS::PhysXBaseGeometryComponent::SetOffset);
		RegisterProperty<float>("Friction", &GASS::PhysXBaseGeometryComponent::GetFriction, &GASS::PhysXBaseGeometryComponent::SetFriction);
		RegisterProperty<bool>("SizeFromMesh", &GASS::PhysXBaseGeometryComponent::GetSizeFromMesh, &GASS::PhysXBaseGeometryComponent::SetSizeFromMesh);
		RegisterProperty<unsigned long>("CollisionBits", &GASS::PhysXBaseGeometryComponent::GetCollisionBits, &GASS::PhysXBaseGeometryComponent::SetCollisionBits);
		RegisterProperty<unsigned long>("CollisionCategory", &GASS::PhysXBaseGeometryComponent::GetCollisionCategory, &GASS::PhysXBaseGeometryComponent::SetCollisionCategory);
		//RegisterProperty<bool>("Debug", &GASS::PhysXBaseGeometryComponent::GetDebug, &GASS::PhysXBaseGeometryComponent::SetDebug);
	}

	void PhysXBaseGeometryComponent::OnInitialize()
	{
		//Try to figure out when to load
		m_Body = GetSceneObject()->GetFirstComponentByClass<PhysXBodyComponent>();
		LocationComponentPtr location  = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		GeometryComponentPtr geom  = GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();

		if(m_Body)
		{
			if(m_SizeFromMesh)
				GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBaseGeometryComponent::OnGeometryChanged,GeometryChangedMessage,0));
			else
				GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBaseGeometryComponent::OnBodyLoaded,BodyLoadedMessage,1));
		}
		else
		{
			if(m_SizeFromMesh && geom)
				GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBaseGeometryComponent::OnGeometryChanged,GeometryChangedMessage,0));
			else
			{
				if(location)
					GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBaseGeometryComponent::OnLocationLoaded,LocationLoadedMessage,1));
				//else
				//	GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBaseGeometryComponent::OnLoadComponents,LoadComponentsMessage,1));
			}
		}

		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBaseGeometryComponent::OnTransformationChanged,TransformationNotifyMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBaseGeometryComponent::OnCollisionSettings,CollisionSettingsMessage ,0));
	}

	bool  PhysXBaseGeometryComponent::GetSizeFromMesh() const
	{
		return m_SizeFromMesh;
	}

	void PhysXBaseGeometryComponent::SetSizeFromMesh(bool value) 
	{
		m_SizeFromMesh = value;
	}

	unsigned long PhysXBaseGeometryComponent::GetCollisionBits() const 
	{
		return m_CollisionBits;
	}

	void PhysXBaseGeometryComponent::SetCollisionBits(unsigned long value)
	{
		m_CollisionBits = value;

	}

	unsigned long  PhysXBaseGeometryComponent::GetCollisionCategory() const 
	{
		return m_CollisionCategory;
	}

	void PhysXBaseGeometryComponent::SetCollisionCategory(unsigned long value)
	{
		m_CollisionCategory =value;
	}

	void PhysXBaseGeometryComponent::OnBodyLoaded(BodyLoadedMessagePtr message)
	{
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		assert(scene_manager);
		m_SceneManager = scene_manager;
		m_Shape = CreateShape();
		m_Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE,true);
		if(m_Body)
		{
			physx::PxReal mass = m_Body->GetMass();
			physx::PxRigidBodyExt::setMassAndUpdateInertia(*m_Body->GetPxActor(), &mass,1);
		}
	}

	GeometryComponentPtr PhysXBaseGeometryComponent::GetGeometry() const 
	{
		GeometryComponentPtr geom;
		if(m_GeometryTemplate != "")
		{
			geom = boost::shared_dynamic_cast<IGeometryComponent>(GetSceneObject()->GetComponent(m_GeometryTemplate));
		}
		else geom = GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();
		return geom;
	}

	void PhysXBaseGeometryComponent::OnLocationLoaded(LocationLoadedMessagePtr message)
	{

	}

	void PhysXBaseGeometryComponent::OnGeometryChanged(GeometryChangedMessagePtr message)
	{
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		assert(scene_manager);
		m_SceneManager = scene_manager;
		SetSizeFromMesh(m_SizeFromMesh);
		m_Shape = CreateShape();
		m_Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE,true);

		if(m_Body)
		{
			physx::PxReal mass = m_Body->GetMass();
			//physx::PxReal density = 0.001;
			physx::PxRigidBodyExt::setMassAndUpdateInertia(*m_Body->GetPxActor(), &mass,1);
			//physx::PxRigidBodyExt::updateMassAndInertia(*m_Body->GetPxActor(), &density,1);
		}
	}

	void PhysXBaseGeometryComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		Vec3 pos = message->GetPosition();
		SetPosition(pos);

		Quaternion rot = message->GetRotation();
		SetRotation(rot);
	}

	void PhysXBaseGeometryComponent::OnCollisionSettings(CollisionSettingsMessagePtr message)
	{
		bool value = message->EnableCollision();
		if(value)
			Enable();
		else
			Disable();
	}

	void PhysXBaseGeometryComponent::SetPosition(const Vec3 &pos)
	{
		if(m_Body == NULL)
		{

		}
	}

	void PhysXBaseGeometryComponent::SetRotation(const Quaternion &rot)
	{
		if(m_Body == NULL)
		{

		}
	}

	void PhysXBaseGeometryComponent::SetScale(const Vec3 &value)
	{
	}
}
