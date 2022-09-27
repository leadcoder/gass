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

#include <memory>

#include "Plugins/PhysX/PhysXBodyComponent.h"
#include "Plugins/PhysX/PhysXSuspensionComponent.h"
#include "Plugins/PhysX/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX/PhysXPhysicsSystem.h"

namespace GASS
{
	PhysXBodyComponent::PhysXBodyComponent() 
		
	{

	}

	PhysXBodyComponent::~PhysXBodyComponent()
	{
		
	}

	void PhysXBodyComponent::OnDelete()
	{
		if(m_Actor)
			m_Actor->release();
		m_Actor = nullptr;
	}

	void PhysXBodyComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<PhysXBodyComponent>("PhysicsBodyComponent");
		RegisterGetSet("Mass", &PhysXBodyComponent::GetMass, &PhysXBodyComponent::SetMass);
		RegisterGetSet("Kinematic", &PhysXBodyComponent::GetKinematic, &PhysXBodyComponent::SetKinematic);
		RegisterMember("DisableGravity", &PhysXBodyComponent::m_DisableGravity);
		RegisterMember("PositionIterCount", &PhysXBodyComponent::m_PositionIterCount);
		RegisterMember("VelocityIterCount", &PhysXBodyComponent::m_VelocityIterCount);
		RegisterGetSet("EffectJoints", &PhysXBodyComponent::GetEffectJoints, &PhysXBodyComponent::SetEffectJoints);

	}

	void PhysXBodyComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBodyComponent::OnLocationLoaded,LocationLoadedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBodyComponent::OnTransformationChanged, TransformationChangedEvent, 0));
	}

	void PhysXBodyComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
		m_LocationComponent = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>().get();
		Vec3 pos = m_LocationComponent->GetPosition();
		Quaternion rot = m_LocationComponent->GetRotation();
		
		PhysXPhysicsSceneManagerPtr sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		m_SceneManager = sm;
		
		//Transform to initial offset
		//NOTE: We dont use PhysXPhysicsSceneManager::WorldToLocal, 
		//location component is just created and the vehicle is not yet moved to it start location.
		physx::PxTransform transform(PxConvert::ToPx(pos), PxConvert::ToPx(rot));
		m_Actor = system->GetPxSDK()->createRigidDynamic(transform);
		m_Actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, m_DisableGravity);
		//transfer loaded attributes to actor
		SetKinematic(m_Kinematic);
		m_Actor->setSolverIterationCounts(m_PositionIterCount,m_VelocityIterCount);
		
		sm->GetPxScene()->addActor(*m_Actor);
		GetSceneObject()->SendImmediateEvent(std::make_shared<PhysicsBodyLoadedEvent>());

		RegisterForPostUpdate<PhysXPhysicsSceneManager>();
	}

	void PhysXBodyComponent::SetKinematic(bool value)
	{
		m_Kinematic = value;
		if(m_Actor)
		{
			m_Actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, m_Kinematic);
		}
	}

	bool PhysXBodyComponent::GetKinematic() const
	{
		return m_Kinematic;
	}

	void PhysXBodyComponent::OnTransformationChanged(TransformationChangedEventPtr event)
	{
		if(m_TrackTransformation)
		{
			SetPosition(event->GetPosition());
			SetRotation(event->GetRotation());
		}
	}
	
	Vec3 PhysXBodyComponent::GetPosition() const
	{
		Vec3 pos(0,0,0);
		if(m_Actor)
		{
			PhysXPhysicsSceneManagerPtr sm = m_SceneManager.lock();
			pos = sm->LocalToWorld(m_Actor->getGlobalPose().p);
		}
		return pos;
	}

	void PhysXBodyComponent::SetPosition(const Vec3 &value)
	{
		if(m_Actor)
		{
			Vec3 trans_vec = value - GetPosition();
			//get scene offset used to support double precision
			PhysXPhysicsSceneManagerPtr sm = m_SceneManager.lock();
			m_Actor->setGlobalPose(physx::PxTransform(sm->WorldToLocal(value), m_Actor->getGlobalPose().q));

			if(m_EffectJoints)
			{
				SceneObject::ComponentVector components;
				GetSceneObject()->GetComponentsByClassName(components,"PhysXBodyComponent");
				
				for(int i = 0 ; i < components.size(); i++)
				{
					PhysXBodyComponentPtr body = GASS_STATIC_PTR_CAST<PhysXBodyComponent>(components[i]);
					if(body.get() != this)
					{
						LocationComponentPtr location = body->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
						Vec3 pos = location->GetPosition();
						pos = pos + trans_vec;
						body->SetPosition(pos);
						location->SetWorldPosition(pos);
					}
				}
			

				components.clear();
				GetSceneObject()->GetComponentsByClassName(components,"PhysXSuspensionComponent");
				
				for(int i = 0 ; i < components.size(); i++)
				{
					PhysXSuspensionComponentPtr joint = GASS_STATIC_PTR_CAST<PhysXSuspensionComponent>(components[i]);
					//if(joint.get() != this)
					{
						LocationComponentPtr location = joint->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
						Vec3 pos = location->GetPosition();
						//pos = pos + trans_vec;
						joint->SetPosition(pos);
					}
				}
			}
		}
	}

	void PhysXBodyComponent::SetRotation(const Quaternion &rot)
	{
		if(m_Actor)
		{
			m_Actor->setGlobalPose(physx::PxTransform(m_Actor->getGlobalPose().p,PxConvert::ToPx(rot)));
		}
	}

	Quaternion PhysXBodyComponent::GetRotation()
	{
		Quaternion q;

		if(m_Actor)
		{
			q = PxConvert::ToGASS(m_Actor->getGlobalPose().q);
		}
		return q;
	}

	void PhysXBodyComponent::SceneManagerTick(double /*delta_time*/)
	{
		if (m_Active)
		{
			//skip transformation callbacks from ourself
			m_TrackTransformation = false;
			m_LocationComponent->SetWorldPosition(GetPosition());
			m_LocationComponent->SetWorldRotation(GetRotation());
			m_TrackTransformation = true;
		}
	}

	void PhysXBodyComponent::AddTorque(const Vec3 &torque_vec, bool relative)
	{
		if(m_Actor && m_Active)
		{
			const physx::PxVec3 world_torq = relative ? m_Actor->getGlobalPose().rotate(PxConvert::ToPx(torque_vec)) : PxConvert::ToPx(torque_vec);
			m_Actor->addTorque(world_torq);
		}
	}

	void PhysXBodyComponent::SetVelocity(const Vec3 &vel , bool relative)
	{
		if(m_Actor && m_Active)
		{
			const physx::PxVec3 world_vel = relative ? m_Actor->getGlobalPose().rotate(PxConvert::ToPx(vel)) : PxConvert::ToPx(vel);
			m_Actor->setLinearVelocity(world_vel);
		}
	}

	Vec3 PhysXBodyComponent::GetVelocity(bool relative) const
	{
		Vec3 vel(0, 0, 0);
		if (m_Actor)
			vel = relative ? PxConvert::ToGASS(m_Actor->getGlobalPose().rotate(m_Actor->getLinearVelocity())) : PxConvert::ToGASS(m_Actor->getLinearVelocity());
		return vel;
	}

	void PhysXBodyComponent::SetAngularVelocity(const Vec3 &vel, bool relative)
	{
		if(m_Actor && m_Active)
		{
			m_Actor->setAngularVelocity(PxConvert::ToPx(vel));
		}
	}


	Vec3 PhysXBodyComponent::GetAngularVelocity(bool relative) const
	{
		Vec3 vel(0,0,0);
		if(m_Actor)
		{
			
			physx::PxVec3 pxvel = m_Actor->getAngularVelocity();
			vel.Set(pxvel.x,pxvel.y,pxvel.z);
		}
		return vel;
	}

	void PhysXBodyComponent::WakeUp()
	{
		if(m_Actor)
		{
			m_Actor->wakeUp();
		}
	}
	
	void PhysXBodyComponent::AddForce(const Vec3 &force_vec, bool relative)
	{
		if(m_Actor)
		{
			/*if(relative)
				physx::PxRigidBodyExt::addLocalForceAtPos(*m_Actor, PxConvert::ToPx(force_vec),physx::PxVec3(0,0,0));
			else
				m_Actor->addForce(PxConvert::ToPx(force_vec));*/
			const physx::PxVec3 world_force = relative ? m_Actor->getGlobalPose().rotate(PxConvert::ToPx(force_vec)) : PxConvert::ToPx(force_vec);
			m_Actor->addForce(world_force);
		}
	}

	void PhysXBodyComponent::AddForceAtPos(const Vec3 &force_vec, const Vec3& pos_vec, bool rel_force, bool rel_pos)
	{
		if(m_Actor)
		{
			if (rel_force) 
			{
				if (rel_pos)
					physx::PxRigidBodyExt::addLocalForceAtLocalPos(*m_Actor, PxConvert::ToPx(force_vec), PxConvert::ToPx(pos_vec));
				else
					physx::PxRigidBodyExt::addLocalForceAtPos(*m_Actor, PxConvert::ToPx(force_vec), PxConvert::ToPx(pos_vec));
			} 
			else 
			{
				if (rel_pos)
					physx::PxRigidBodyExt::addForceAtLocalPos(*m_Actor, PxConvert::ToPx(force_vec), PxConvert::ToPx(pos_vec));
				else
					physx::PxRigidBodyExt::addForceAtPos(*m_Actor, PxConvert::ToPx(force_vec), PxConvert::ToPx(pos_vec));
			}
		}
	}

	void PhysXBodyComponent::SetMass(float mass)
	{
		m_Mass = mass;
		if(m_Actor)
		{
			m_Actor->setMass(mass);
		}
	}

	void PhysXBodyComponent::SetActive(bool value)
	{
		m_Active = value;
		if (m_Actor)
		{
			m_Actor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, !value);
		}
	}

	bool PhysXBodyComponent::GetActive() const
	{
		return m_Active;
	}
}
