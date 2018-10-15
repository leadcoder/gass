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

#include "Plugins/PhysX3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXSuspensionComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"

namespace GASS
{
	PhysXBodyComponent::PhysXBodyComponent() :	m_MassRepresentation(MR_GEOMETRY),
		m_Mass(1),
		m_Actor(NULL),
		m_Kinematic(false),
		m_DisableGravity(false),
		m_EffectJoints(false),
		m_PositionIterCount(4),
		m_VelocityIterCount(4),
		m_ForceReport(false),
		m_LocationComponent(NULL),
		m_TrackTransformation(true),
		m_Active(true)
	{

	}

	PhysXBodyComponent::~PhysXBodyComponent()
	{
		
	}

	void PhysXBodyComponent::OnDelete()
	{
		if(m_Actor)
			m_Actor->release();
		m_Actor = NULL;
	}

	void PhysXBodyComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsBodyComponent",new Creator<PhysXBodyComponent, Component>);
		REG_PROPERTY(float,Mass,PhysXBodyComponent);
		REG_PROPERTY(bool,Kinematic,PhysXBodyComponent);
		REG_PROPERTY(bool,DisableGravity,PhysXBodyComponent);
		REG_PROPERTY(int,PositionIterCount,PhysXBodyComponent)
		REG_PROPERTY(int,VelocityIterCount,PhysXBodyComponent)
	}

	void PhysXBodyComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBodyComponent::OnLocationLoaded,LocationLoadedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBodyComponent::OnTransformationChanged, TransformationChangedEvent, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBodyComponent::OnVelocity,PhysicsBodyVelocityRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBodyComponent::OnAddForce,PhysicsBodyAddForceRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBodyComponent::OnAddTorque,PhysicsBodyAddTorqueRequest,0));
	}

	void PhysXBodyComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
		m_LocationComponent = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>().get();
		Vec3 pos = m_LocationComponent->GetPosition();
		Quaternion rot = m_LocationComponent->GetRotation();
		
		PhysXPhysicsSceneManagerPtr sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		m_SceneManager = sm;
		physx::PxTransform transform(PxConvert::ToPx(pos + sm->GetOffset()), PxConvert::ToPx(rot));
		m_Actor = system->GetPxSDK()->createRigidDynamic(transform);
		m_Actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, m_DisableGravity);
		//transfer loaded attributes to actor
		SetKinematic(m_Kinematic);
		m_Actor->setSolverIterationCounts(m_PositionIterCount,m_VelocityIterCount);
		
		sm->GetPxScene()->addActor(*m_Actor);
		GetSceneObject()->SendImmediateEvent(PhysicsBodyLoadedEventPtr(new PhysicsBodyLoadedEvent()));

		SceneManagerListenerPtr listener = shared_from_this();
		sm->Register(listener);
	}

	void PhysXBodyComponent::SetKinematic(bool value)
	{
		m_Kinematic = value;
		if(m_Actor)
		{
			m_Actor->setRigidDynamicFlag(physx::PxRigidDynamicFlag::eKINEMATIC, m_Kinematic);
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

	void PhysXBodyComponent::OnVelocity(PhysicsBodyVelocityRequestPtr message)
	{
		SetVelocity(message->GetVelocity());
	}

	void PhysXBodyComponent::OnAngularVelocity(PhysicsBodyAngularVelocityRequestPtr message)
	{
		SetAngularVelocity(message->GetAngularVelocity());
	}

	void PhysXBodyComponent::OnAddForce(PhysicsBodyAddForceRequestPtr message)
	{
		AddForce(message->GetForce(),false);
	}

	void PhysXBodyComponent::OnAddTorque(PhysicsBodyAddTorqueRequestPtr message)
	{
		AddTorque(message->GetTorque());
	}

	Vec3 PhysXBodyComponent::GetPosition() const
	{
		Vec3 pos(0,0,0);
		if(m_Actor)
		{
			Vec3 offset = PhysXPhysicsSceneManagerPtr(m_SceneManager)->GetOffset();
			pos = PxConvert::ToGASS(m_Actor->getGlobalPose().p) - offset;
		}
		return pos;
	}

	void PhysXBodyComponent::SetPosition(const Vec3 &value)
	{
		if(m_Actor)
		{
			Vec3 trans_vec = value - GetPosition();
			//get scene offset used to support double precision
			Vec3 offset = PhysXPhysicsSceneManagerPtr(m_SceneManager)->GetOffset();
			m_Actor->setGlobalPose(physx::PxTransform(PxConvert::ToPx(value + offset), m_Actor->getGlobalPose().q));

			if(m_EffectJoints)
			{
				ComponentContainer::ComponentVector components;
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
						pos = pos + trans_vec;
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
