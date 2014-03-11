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

#include "Plugins/PhysX/PhysXBodyComponent.h"
#include "Plugins/PhysX/PhysXJoint.h"
#include "Plugins/PhysX/PhysXSuspensionComponent.h"


#include "Plugins/PhysX/PhysXPhysicsSceneManager.h"
#include "Core/Math/AABox.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponentContainer.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include <boost/bind.hpp>


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

	void PhysXBodyComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(OBJECT_RM_LOAD_PHYSICS_COMPONENTS, TYPED_MESSAGE_FUNC( PhysXBodyComponent::OnLoad ,LoadPhysicsComponentsMessage),1);
		GetSceneObject()->RegisterForMessage(OBJECT_RM_POSITION,				TYPED_MESSAGE_FUNC( PhysXBodyComponent::OnPositionChanged,PositionRequest));
		GetSceneObject()->RegisterForMessage(OBJECT_RM_ROTATION,				TYPED_MESSAGE_FUNC( PhysXBodyComponent::OnRotationChanged,RotationRequest ));
		GetSceneObject()->RegisterForMessage(OBJECT_RM_PHYSICS_BODY_PARAMETER,  TYPED_MESSAGE_FUNC(PhysXBodyComponent::OnParameterMessage,PhysicsBodyMessage));
		REGISTER_OBJECT_MESSAGE_CLASS(PhysXBodyComponent::OnMassMessage,PhysicsMassMessage,0);
	}

	void PhysXBodyComponent::OnPositionChanged(PositionRequestPtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}

	void PhysXBodyComponent::OnRotationChanged(RotationRequestPtr message)
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

	void PhysXBodyComponent::OnLoad(LoadPhysicsComponentsMessagePtr message)
	{
		m_SceneManager = boost::shared_dynamic_cast<PhysXPhysicsSceneManager>(message->GetPhysicsSceneManager());
		assert(m_SceneManager);
		NxBodyDesc bodyDesc;
		bodyDesc.setToDefault();
		bodyDesc.mass = m_Mass;
		m_ActorDesc.body		= &bodyDesc;
		m_ActorDesc.density		= 0.0f;
		

		

		LocationComponentPtr location = GetSceneObject()->GetFirstComponent<ILocationComponent>();
		Vec3 pos = location->GetPosition();

		m_ActorDesc.globalPose.t= NxVec3(pos.x,pos.y,pos.z);
		m_Actor = PhysXPhysicsSceneManagerPtr(m_SceneManager)->GetNxScene()->createActor(m_ActorDesc);	
	}


	void PhysXBodyComponent::SendTransformation()
	{
		int from_id = (int)this; //use address as id

		MessagePtr pos_msg(new PositionRequest(GetPosition(),from_id));
		GetSceneObject()->PostMessage(pos_msg);

		MessagePtr rot_msg(new RotationRequest(GetRotation(),from_id));
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

	Vec3 PhysXBodyComponent::GetMomentum()
	{
		Vec3 momentum(0,0,0);
		if (m_Actor) 
		{
			NxVec3 temp = m_Actor->getLinearMomentum();
			momentum.Set(temp.x,temp.y,temp.z); 
		}
		return momentum;
	}
	
	void PhysXBodyComponent::AddTorque(const Vec3 &torque_vec, bool rel)
	{
		if(m_Actor)
		{
			if (rel)
				m_Actor->addLocalTorque(NxVec3(torque_vec.x,torque_vec.y,torque_vec.z));
			else
				m_Actor->addTorque(NxVec3(torque_vec.x,torque_vec.y,torque_vec.z));
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
				m_Actor->setLinearVelocity(NxVec3(vel.x,vel.y,vel.z));
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
				m_Actor->setAngularVelocity(NxVec3(vel.x,vel.y,vel.z));
		}
	}


	Vec3 PhysXBodyComponent::GetAngularVelocity(bool rel)
	{
		Vec3 vel(0,0,0);
		if(m_Actor)
		{
			NxVec3 nxvel = m_Actor->getAngularVelocity();
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
			//dBodyEnable(m_PhysXBodyComponent);
		}
	}
	bool PhysXBodyComponent::IsEnabled()
	{
		//if(dBodyIsEnabled(m_PhysXBodyComponent) == 0) return false;
		return true;
	}

	void PhysXBodyComponent::Disable()
	{
		if(m_Actor)
		{
			//dBodyDisable(m_PhysXBodyComponent);
		}
	}

	


	/*void PhysXBodyComponent::DampenBody( dBodyID body, float vScale, float aScale )
	{
		assert( vScale <= 0 && aScale <= 0 );
		if( !dBodyIsEnabled( body ) )
		{
			return;
		}
		dReal const * V = dBodyGetLinearVel( body );
		dBodyAddForce( body, vScale*V[0], vScale*V[1], vScale*V[2] );
		dReal const * A = dBodyGetAngularVel( body );
		dBodyAddTorque( body, aScale*A[0], aScale*A[1], aScale*A[2] );
	}*/

	void PhysXBodyComponent::AddForce(const Vec3 &force_vec, bool rel)
	{
		if(m_Actor)
		{
			if(rel)
				m_Actor->addLocalForce(NxVec3(force_vec.x,force_vec.y,force_vec.z));
			else
				m_Actor->addForce(NxVec3(force_vec.x,force_vec.y,force_vec.z));
		}
	}

	void PhysXBodyComponent::AddForceAtPos(const Vec3 &force_vec, const Vec3& pos_vec, bool rel_force, bool rel_pos)
	{
		if(m_Actor)
		{
			if (rel_force) 
			{
				if (rel_pos)
					m_Actor->addLocalForceAtLocalPos(NxVec3(force_vec.x,force_vec.y,force_vec.z),NxVec3(pos_vec.x,pos_vec.y,pos_vec.z));
				else
					m_Actor->addLocalForceAtPos(NxVec3(force_vec.x,force_vec.y,force_vec.z),NxVec3(pos_vec.x,pos_vec.y,pos_vec.z));
			} else 
			{
				if (rel_pos)
					m_Actor->addForceAtLocalPos(NxVec3(force_vec.x,force_vec.y,force_vec.z),NxVec3(pos_vec.x,pos_vec.y,pos_vec.z));
				else
					m_Actor->addForceAtPos(NxVec3(force_vec.x,force_vec.y,force_vec.z),NxVec3(pos_vec.x,pos_vec.y,pos_vec.z));
			}
		}
	}

	void PhysXBodyComponent::SetMass(float mass)
	{
		m_Mass = mass;
		if(m_Actor)
		{
			m_Actor->setMass(mass);
			//dMassAdjust(&m_ODEMass, m_Mass);
			//dBodySetMass(m_PhysXBodyComponent, &m_ODEMass);
			//m_CGPosition = Vec3(m_ODEMass.c[0],m_ODEMass.c[1],m_ODEMass.c[2]);
		}
		// TODO: update m_SymmetricInertia and m_AssymetricInertia
	}

	Vec3 PhysXBodyComponent::GetVelocity(bool rel)
	{
		Vec3 vel(0,0,0);
		if (m_Actor) 
		{
			NxVec3 nx_vel = m_Actor->getLinearVelocity();
			if (rel) 
			{
				//dVector3 vec;
				//dBodyVectorFromWorld(m_PhysXBodyComponent,vel_p[0],vel_p[1],vel_p[2],vec);
				vel.Set(nx_vel.x,nx_vel.y,nx_vel.z);
			} else
				vel.Set(nx_vel.x,nx_vel.y,nx_vel.z);
		}
		return vel;
	}

	void PhysXBodyComponent::SetPosition(const Vec3 &value)
	{
		if(m_Actor)
		{
			Vec3 trans_vec = value - GetPosition();
			m_Actor->setGlobalPosition(NxVec3(value.x, value.y, value.z));

			if(m_EffectJoints)
			{
				IComponentContainer::ComponentVector components;
				GetSceneObject()->GetComponentsByClass(components,"PhysXBodyComponent");
				
				for(int i = 0 ; i < components.size(); i++)
				{
					PhysXBodyComponentPtr body = boost::shared_static_cast<PhysXBodyComponent>(components[i]);
					if(body.get() != this)
					{
						LocationComponentPtr location = body->GetSceneObject()->GetFirstComponent<ILocationComponent>();
						Vec3 pos = location->GetPosition();
						pos = pos + trans_vec;
						body->SetPosition(pos);
					}
				}

				components.clear();
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
				}

				components.clear();
				GetSceneObject()->GetComponentsByClass(components,"PhysXSuspensionComponent");
				
				for(int i = 0 ; i < components.size(); i++)
				{
					PhysXSuspensionComponentPtr joint = boost::shared_static_cast<PhysXSuspensionComponent>(components[i]);
					//if(joint.get() != this)
					{
						LocationComponentPtr location = joint->GetSceneObject()->GetFirstComponent<ILocationComponent>();
						Vec3 pos = location->GetPosition();
						pos = pos + trans_vec;
						joint->SetPosition(pos);
					}
				}


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
			NxVec3 nx_pos = m_Actor->getGlobalPosition();
			pos = Vec3(nx_pos.x,nx_pos.y,nx_pos.z);
		}
		return pos;
	}

	void PhysXBodyComponent::SetRotation(const Quaternion &rot)
	{
		if(m_Actor)
		{
			NxQuat nx_rot;
			nx_rot.setXYZW(rot.x,rot.y,rot.z,rot.w);
			m_Actor->setGlobalOrientation(nx_rot);
		}
	}

	

	Quaternion PhysXBodyComponent::GetRotation()
	{
		Quaternion q;

		if(m_Actor)
		{
			NxQuat nx_rot = m_Actor->getGlobalOrientationQuat();
		
			q.x = nx_rot.x;
			q.y = nx_rot.y;
			q.z = nx_rot.z;
			q.w = -nx_rot.w;
		}
		return q;
	}

	void PhysXBodyComponent::AddShape(NxShapeDesc* shape)
	{
		m_ActorDesc.shapes.pushBack(shape);
	}
}
