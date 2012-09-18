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

#include "Plugins/PhysX3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"

namespace GASS
{
	PhysXBodyComponent::PhysXBodyComponent() :	m_MassRepresentation(MR_GEOMETRY),
		m_Mass(1),
		m_Actor(NULL)
	{

	}

	PhysXBodyComponent::~PhysXBodyComponent()
	{
		
	}

	void PhysXBodyComponent::OnMassMessage(PhysicsMassMessagePtr message)
	{
		SetMass(message->GetMass());
	}


	void PhysXBodyComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsBodyComponent",new Creator<PhysXBodyComponent, IComponent>);
		RegisterProperty<float>("Mass", &PhysXBodyComponent::GetMass, &PhysXBodyComponent::SetMass);
		RegisterProperty<Vec3>("CGPosition",&PhysXBodyComponent::GetCGPosition, &PhysXBodyComponent::SetCGPosition);
		RegisterProperty<Vec3>("SymmetricInertia",&PhysXBodyComponent::GetSymmetricInertia, &PhysXBodyComponent::SetSymmetricInertia);
		RegisterProperty<Vec3>("AssymetricInertia",&PhysXBodyComponent::GetAssymetricInertia, &PhysXBodyComponent::SetAssymetricInertia);
		RegisterProperty<bool>("EffectJoints",&PhysXBodyComponent::GetEffectJoints, &PhysXBodyComponent::SetEffectJoints);
	}

	void PhysXBodyComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBodyComponent::OnLocationLoaded,LocationLoadedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBodyComponent::OnPositionChanged,PositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBodyComponent::OnWorldPositionChanged,WorldPositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBodyComponent::OnRotationChanged,RotationMessage,0 ));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBodyComponent::OnParameterMessage,PhysicsBodyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBodyComponent::OnMassMessage,PhysicsMassMessage,0));
	}

	void PhysXBodyComponent::OnPositionChanged(PositionMessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}
	void PhysXBodyComponent::OnWorldPositionChanged(WorldPositionMessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}
	void PhysXBodyComponent::OnRotationChanged(RotationMessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Quaternion rot = message->GetRotation();
			SetRotation(rot);
		}
	}

	void PhysXBodyComponent::OnParameterMessage(PhysicsBodyMessagePtr message)
	{

		PhysicsBodyMessage::PhysicsBodyParameterType type = message->GetParameter();
		Vec3 value = message->GetValue();

		//wake body!!
		Enable();
		switch(type)
		{
		case PhysicsBodyMessage::FORCE:
			{
				AddForce(value,true);
			}
			break;
		case PhysicsBodyMessage::TORQUE:
			{
				AddTorque(value,true);
				break;
			}
		case PhysicsBodyMessage::VELOCITY:
			{
				SetVelocity(value,true);
				break;
			}
		}
	}

	void PhysXBodyComponent::OnLocationLoaded(LocationLoadedMessagePtr message)
	{
		LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		Vec3 pos = location->GetPosition();
		
		PhysXPhysicsSceneManagerPtr sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		m_SceneManager = sm;
		
		physx::PxReal density = 1.0f;
		physx::PxTransform transform(physx::PxVec3(pos.x, pos.y, pos.z), physx::PxQuat::createIdentity());
		
		physx::PxVec3 dimensions(0.5,0.5,0.5);
		physx::PxBoxGeometry geometry(dimensions);

		//m_ActorDesc.globalPose.t= NxVec3(pos.x,pos.y,pos.z);
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<PhysXPhysicsSystem>();
		//physx::PxMaterial* aterial = system->GetPxSDK()->createMaterial(0.5,0.5,0.5);
		//m_Actor = PxCreateDynamic(*system->GetPxSDK(), transform, geometry, *mMaterial, density);
		m_Actor = system->GetPxSDK()->createRigidDynamic(transform);
		//m_Actor->setAngularDamping(0.75);
		//m_Actor->setLinearVelocity(physx::PxVec3(0,0,0)); 
		sm->GetPxScene()->addActor(*m_Actor);
		GetSceneObject()->SendImmediate(MessagePtr(new BodyLoadedMessage()));
	}


	void PhysXBodyComponent::SendTransformation()
	{
		int from_id = (int)this; //use address as id

		MessagePtr pos_msg(new PositionMessage(GetPosition(),from_id));
		GetSceneObject()->PostMessage(pos_msg);

		MessagePtr rot_msg(new RotationMessage(GetRotation(),from_id));
		GetSceneObject()->PostMessage(rot_msg);
		
		MessagePtr physics_msg(new VelocityNotifyMessage(GetVelocity(true),GetAngularVelocity(true),from_id));
		GetSceneObject()->PostMessage(physics_msg);
	}

	void PhysXBodyComponent::SetMassProperties(float mass, Vec3 &CGPosition, Vec3 &symmetricInertia, Vec3 &assymmetricInertia)
	{
		m_Mass = mass;
		m_CGPosition = CGPosition;
		m_SymmetricInertia = symmetricInertia;
		m_AssymetricInertia = assymmetricInertia;
/*		dMassSetParameters(&m_ODEMass,mass,
			CGPosition.x,CGPosition.y,CGPosition.z,
			symmetricInertia.x,symmetricInertia.y,symmetricInertia.z,
			assymmetricInertia.x,assymmetricInertia.y,assymmetricInertia.z);
		dBodySetMass(m_PhysXBodyComponent, &m_ODEMass);*/
	}
	
	void PhysXBodyComponent::AddTorque(const Vec3 &torque_vec, bool rel)
	{
		if(m_Actor)
		{
			//if (rel)
			//	m_Actor->addLocalTorque(NxVec3(torque_vec.x,torque_vec.y,torque_vec.z));
			//else
				m_Actor->addTorque(physx::PxVec3(torque_vec.x,torque_vec.y,torque_vec.z));
		}
	}

	void PhysXBodyComponent::SetVelocity(const Vec3 &vel, bool rel)
	{
		if(m_Actor)
		{
			if (rel) 
			{
				//dVector3 vec;
				//dBodyVectorToWorld(m_PhysXBodyComponent,vel.x,vel.y,vel.z,vec);
				//dBodySetLinearVel(m_PhysXBodyComponent,vec[0],vec[1],vec[2]);
			} 
			else
			{
				m_Actor->setLinearVelocity(physx::PxVec3(vel.x,vel.y,vel.z));
			}
		}
	}

	void PhysXBodyComponent::SetAngularVelocity(const Vec3 &vel, bool rel)
	{
		if(m_Actor)
		{
			if (rel) 
			{
			} 
			else
				m_Actor->setAngularVelocity(physx::PxVec3(vel.x,vel.y,vel.z));
		}
	}


	Vec3 PhysXBodyComponent::GetAngularVelocity(bool rel)
	{
		Vec3 vel(0,0,0);
		if(m_Actor)
		{
			physx::PxVec3 nxvel = m_Actor->getAngularVelocity();
			if (rel) 
			{
				//dVector3 vec;
				//dBodyVectorFromWorld(m_PhysXBodyComponent,vel_p[0],vel_p[1],vel_p[2],vec);
				vel.Set(nxvel.x,nxvel.y,nxvel.z);
			} else
				vel.Set(nxvel.x,nxvel.y,nxvel.z);
		}
		return vel;
	}

	void PhysXBodyComponent::Enable()
	{
		if(m_Actor)
		{
		}
	}
	bool PhysXBodyComponent::IsEnabled()
	{
		return true;
	}

	void PhysXBodyComponent::Disable()
	{
		if(m_Actor)
		{
		}
	}


	void PhysXBodyComponent::AddForce(const Vec3 &force_vec, bool rel)
	{
		if(m_Actor)
		{
			if(rel)
				physx::PxRigidBodyExt::addLocalForceAtPos(m_Actor, physx::PxVec3(force_vec.x,force_vec.y,force_vec.z));
			else
				m_Actor->addForce(physx::PxVec3(force_vec.x,force_vec.y,force_vec.z));
		}
	}

	void PhysXBodyComponent::AddForceAtPos(const Vec3 &force_vec, const Vec3& pos_vec, bool rel_force, bool rel_pos)
	{
		if(m_Actor)
		{
			if (rel_force) 
			{
				if (rel_pos)
					physx::PxRigidBodyExt::addLocalForceAtLocalPos(*m_Actor,physx::PxVec3(force_vec.x,force_vec.y,force_vec.z),physx::PxVec3(pos_vec.x,pos_vec.y,pos_vec.z));
				else
					physx::PxRigidBodyExt::addLocalForceAtPos(*m_Actor,physx::PxVec3(force_vec.x,force_vec.y,force_vec.z),physx::PxVec3(pos_vec.x,pos_vec.y,pos_vec.z));
			} else 
			{
				if (rel_pos)
					physx::PxRigidBodyExt::addForceAtLocalPos(*m_Actor,physx::PxVec3(force_vec.x,force_vec.y,force_vec.z),physx::PxVec3(pos_vec.x,pos_vec.y,pos_vec.z));
				else
					physx::PxRigidBodyExt::addForceAtPos(*m_Actor,physx::PxVec3(force_vec.x,force_vec.y,force_vec.z),physx::PxVec3(pos_vec.x,pos_vec.y,pos_vec.z));
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

	Vec3 PhysXBodyComponent::GetVelocity(bool rel)
	{
		Vec3 vel(0,0,0);
		if (m_Actor) 
		{
			if (rel) 
				vel = PxConvert::ToGASS(physx::PxRigidBodyExt::getLocalVelocityAtLocalPos(*m_Actor,physx::PxVec3(0,0,0)));
			else
				vel = PxConvert::ToGASS(m_Actor->getLinearVelocity());
		}
		return vel;
	}

	void PhysXBodyComponent::SetPosition(const Vec3 &value)
	{
		if(m_Actor)
		{
			Vec3 trans_vec = value - GetPosition();
			m_Actor->setGlobalPose(physx::PxTransform(physx::PxVec3(value.x, value.y, value.z),m_Actor->getGlobalPose().q));

			if(m_EffectJoints)
			{
				IComponentContainer::ComponentVector components;
				GetSceneObject()->GetComponentsByClass(components,"PhysXBodyComponent");
				
				for(int i = 0 ; i < components.size(); i++)
				{
					PhysXBodyComponentPtr body = boost::shared_static_cast<PhysXBodyComponent>(components[i]);
					if(body.get() != this)
					{
						LocationComponentPtr location = body->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
						Vec3 pos = location->GetPosition();
						pos = pos + trans_vec;
						body->SetPosition(pos);
					}
				}

				/*components.clear();
				GetSceneObject()->GetComponentsByClass(components,"PhysXJoint");
				
				for(int i = 0 ; i < components.size(); i++)
				{
					PhysXJointPtr joint = boost::shared_static_cast<PhysXJoint>(components[i]);
					//if(joint.get() != this)
					{
						LocationComponentPtr location = joint->GetSceneObject()->GetFirstComponent<ILocationComponent>();
						Vec3 pos = location->GetPosition();
						pos = pos + trans_vec;
						joint->SetPosition(pos);
					}
				}*/

				components.clear();
				GetSceneObject()->GetComponentsByClass(components,"PhysXSuspensionComponent");
				
				/*for(int i = 0 ; i < components.size(); i++)
				{
					PhysXSuspensionComponentPtr joint = boost::shared_static_cast<PhysXSuspensionComponent>(components[i]);
					//if(joint.get() != this)
					{
						LocationComponentPtr location = joint->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
						Vec3 pos = location->GetPosition();
						pos = pos + trans_vec;
						joint->SetPosition(pos);
					}
				}*/


				/*int num_joints = dBodyGetNumJoints(m_PhysXBodyComponent);
				for(int i = 0 ; i < num_joints ;i++)
				{
					dJointID joint = dBodyGetJoint(m_PhysXBodyComponent,i);
					dBodyID b2 = dJointGetBody (joint, 1);
					PhysXBodyComponent* child_body = (PhysXBodyComponent*) dBodyGetData(b2);
					if(child_body && child_body != this)
					{
						const dReal *p = dBodyGetPosition(b2);
						Vec3 pos(p[0],p[1],p[2]);
						pos = pos + trans_vec;
						child_body->SetPosition(pos);
					}

				}*/
			}
		}
	}

	Vec3  PhysXBodyComponent::GetPosition() const
	{
		Vec3 pos(0,0,0);
		if(m_Actor)
		{
			pos = PxConvert::ToGASS(m_Actor->getGlobalPose().p);
		}
		return pos;
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
	
}
