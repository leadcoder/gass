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
#include "Sim/GASS.h"
#include "Plugins/Havok/HavokBodyComponent.h"
#include "Plugins/Havok/HavokPhysicsSceneManager.h"
#include "HavokSphereGeometryComponent.h"
#include "HavokBaseGeometryComponent.h"
#include <Physics/Collide/Filter/Group/hkpGroupFilter.h>
#include <Physics/Collide/Filter/Group/hkpGroupFilterSetup.h>


namespace GASS
{
	HavokBodyComponent::HavokBodyComponent()
		:m_AutoDisable(true),
		m_FastRotation(true),
		m_MassRepresentation(MR_GEOMETRY),
		m_Mass(1),
		m_CGPosition(0,0,0),
		m_SymmetricInertia(0,0,0),
		m_AssymetricInertia(0,0,0),
		m_EffectJoints(true),
		m_Active(true),
		m_RigidBody(NULL),
		m_Shape(NULL),
		m_SystemCollisionGroup(-1),
		m_DeltaTime(0)
	{
	}

	HavokBodyComponent::~HavokBodyComponent()
	{
	}

	void HavokBodyComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsBodyComponent",new Creator<HavokBodyComponent, IComponent>);
		RegisterProperty<float>("Mass", &HavokBodyComponent::GetMass, &HavokBodyComponent::SetMass);
		RegisterProperty<Vec3>("CGPosition",&HavokBodyComponent::GetCGPosition, &HavokBodyComponent::SetCGPosition);
		RegisterProperty<Vec3>("SymmetricInertia",&HavokBodyComponent::GetSymmetricInertia, &HavokBodyComponent::SetSymmetricInertia);
		RegisterProperty<Vec3>("AssymetricInertia",&HavokBodyComponent::GetAssymetricInertia, &HavokBodyComponent::SetAssymetricInertia);
		RegisterProperty<bool>("EffectJoints",&HavokBodyComponent::GetEffectJoints, &HavokBodyComponent::SetEffectJoints);
		RegisterProperty<bool>("Active",&HavokBodyComponent::GetActive, &HavokBodyComponent::SetActive);

	}

	void HavokBodyComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokBodyComponent::OnLoad,LoadComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokBodyComponent::OnPositionChanged,PositionRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokBodyComponent::OnWorldPositionChanged,WorldPositionRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokBodyComponent::OnRotationChanged,RotationRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokBodyComponent::OnWorldRotationChanged,WorldRotationRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokBodyComponent::OnParameterMessage,PhysicsBodyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokBodyComponent::OnMassMessage,PhysicsMassMessage,0));
	}

	void HavokBodyComponent::OnPositionChanged(PositionRequestPtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}


	void HavokBodyComponent::OnWorldPositionChanged(WorldPositionRequestPtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}

	void HavokBodyComponent::OnWorldRotationChanged(WorldRotationRequestPtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Quaternion rot = message->GetRotation();
			SetRotation(rot);
		}
	}

	void HavokBodyComponent::OnRotationChanged(RotationRequestPtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Quaternion rot = message->GetRotation();
			SetRotation(rot);
		}
	}

	void HavokBodyComponent::Wake()
	{
		if(GetActive())
			SetActive(true);
	}

	void HavokBodyComponent::OnParameterMessage(PhysicsBodyMessagePtr message)
	{
		PhysicsBodyMessage::PhysicsBodyParameterType type = message->GetParameter();
		//wake body!!
		Wake();
		switch(type)
		{
		case PhysicsBodyMessage::FORCE:
			{
				Vec3 value = message->GetValue();
				AddForce(value,true);
			}
			break;
		case PhysicsBodyMessage::TORQUE:
			{
				Vec3 value = message->GetValue();
				AddTorque(value,true);
				break;
			}
		case PhysicsBodyMessage::VELOCITY:
			{
				Vec3 value = message->GetValue();
				SetVelocity(value,true);
				break;
			}
		case PhysicsBodyMessage::ENABLE:
			{
				SetActive(true);
				break;
			}
		case PhysicsBodyMessage::DISABLE:
			{
				SetActive(false);
				break;
			}
		}
	}

	void HavokBodyComponent::OnMassMessage(PhysicsMassMessagePtr message)
	{
		SetMass(message->GetMass());
	}

	void HavokBodyComponent::OnLoad(LoadComponentsMessagePtr message)
	{

		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<HavokPhysicsSceneManager>()->Register(listener);
		
		HavokPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<HavokPhysicsSceneManager>();
		m_DeltaTime = scene_manager->GetSimulationUpdateInterval();
		assert(scene_manager);

		Vec3 abs_pos;
		hkpWorld* pPhysicsWorld = scene_manager->GetWorld();

		m_World =  pPhysicsWorld;

		//create box shape using given dimensions
		hkReal fhkConvexShapeRadius=0.05;


		/*m_Shape = new hkpCompoundShape(cs);
		//Get All shapes!
		IComponentContainer::ComponentVector comps;
		GetSceneObject()->GetComponentsByClass<HavokBaseGeometryComponent>(comps,false);
		for(int i =0; i < comps.size(); i++)
		{
		HavokBaseGeometryComponentPtr geom = boost::shared_dynamic_cast<HavokBaseGeometryComponent>(comps[i]);
		if(geom)
		{
		hkpShape* shape = geom->GetShape();
		}
		}*/

		IComponentContainer::ComponentVector comps;
		hkpMotion::MotionType mt = hkpMotion::MOTION_BOX_INERTIA;
		GetSceneObject()->GetComponentsByClass<HavokBaseGeometryComponent>(comps,false);
		for(int i =0; i < comps.size(); i++)
		{
			HavokBaseGeometryComponentPtr geom = boost::shared_dynamic_cast<HavokBaseGeometryComponent>(comps[i]);
			if(geom)
			{
				m_Shape = geom->GetShape();
				//classify
				HavokSphereGeometryComponentPtr sphere = boost::shared_dynamic_cast<HavokSphereGeometryComponent>(comps[i]);
				if(sphere)
					mt = hkpMotion::MOTION_SPHERE_INERTIA;			
				break;
			}
		}


		// Compute the inertia tensor from the shape
		hkpMassProperties massProperties;
		hkpInertiaTensorComputer::computeShapeVolumeMassProperties(m_Shape, m_Mass, massProperties);

		//create rigid body information structure 
		hkpRigidBodyCinfo rigidBodyInfo;

		boost::shared_ptr<ILocationComponent> location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		Vec3 pos = location->GetPosition();
		hkVector4 position(pos.x,pos.y,pos.z,1);


		// Assign the rigid body properties
		rigidBodyInfo.m_position = position;
		rigidBodyInfo.m_mass = massProperties.m_mass;
		rigidBodyInfo.m_centerOfMass = massProperties.m_centerOfMass;
		rigidBodyInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
		rigidBodyInfo.m_shape = m_Shape;
		rigidBodyInfo.m_motionType = mt;
		rigidBodyInfo.m_friction = 1.0f;

		//create new rigid body with supplied info
		m_RigidBody = new hkpRigidBody(rigidBodyInfo);

		m_RigidBody->setUserData(hkUlong(this));

		

		//add rigid body to physics world
		pPhysicsWorld->lock();

		int group = GetCollisionGroup();

		m_RigidBody->setCollisionFilterInfo(hkpGroupFilter::calcFilterInfo( hkpGroupFilterSetup::LAYER_DYNAMIC,  group) );
		
		pPhysicsWorld->addEntity(m_RigidBody);

		//decerase reference counter for rigid body and shape
		m_RigidBody->removeReference();
		m_Shape->removeReference();

		pPhysicsWorld->unlock();

	}

	void HavokBodyComponent::UpdateMass()
	{
		// Compute the inertia tensor from the shape
		hkpMassProperties massProperties;
		if(m_Shape && m_RigidBody)
		{
			hkpInertiaTensorComputer::computeShapeVolumeMassProperties(m_Shape, m_Mass, massProperties);
			m_RigidBody->setCenterOfMassLocal(massProperties.m_centerOfMass);
			m_RigidBody->setMass(massProperties.m_mass);
			m_RigidBody->setInertiaLocal(massProperties.m_inertiaTensor);
		}

	}

	void HavokBodyComponent::Update(double delta_time)
	{
		int from_id =(int) this;
		Vec3 pos = GetPosition();
		MessagePtr pos_msg(new WorldPositionRequest(pos,from_id));
		GetSceneObject()->PostMessage(pos_msg);

		MessagePtr rot_msg(new WorldRotationRequest(GetRotation(),from_id));
		GetSceneObject()->PostMessage(rot_msg);

		MessagePtr physics_msg(new PhysicsVelocityEvent(GetVelocity(true),GetAngularVelocity(true),from_id));
		GetSceneObject()->PostMessage(physics_msg);
	}

	TaskGroup HavokBodyComponent::GetTaskGroup() const
	{
		return "MAIN_TASK_GROUP";
	}

	void HavokBodyComponent::BodyMoved()
	{
		int from_id = (int)this; //use address as id
		Vec3 pos = GetPosition();
		MessagePtr pos_msg(new WorldPositionRequest(pos,from_id));
		GetSceneObject()->PostMessage(pos_msg);

		MessagePtr rot_msg(new WorldRotationRequest(GetRotation(),from_id));
		GetSceneObject()->PostMessage(rot_msg);

		MessagePtr physics_msg(new PhysicsVelocityEvent(GetVelocity(true),GetAngularVelocity(true),from_id));
		GetSceneObject()->PostMessage(physics_msg);
	}



	void HavokBodyComponent::SetMassProperties(float mass, Vec3 &CGPosition, Vec3 &symmetricInertia, Vec3 &assymmetricInertia)
	{
		m_Mass = mass;
		m_CGPosition = CGPosition;
		m_SymmetricInertia = symmetricInertia;
		m_AssymetricInertia = assymmetricInertia;
	}

	Vec3 HavokBodyComponent::GetForce(bool rel)
	{
		Vec3 force(0,0,0);

		return force;
	}

	void HavokBodyComponent::SetForce(const Vec3 &force)
	{

	}

	void HavokBodyComponent::SetTorque(const Vec3 &torque)
	{

	}

	void HavokBodyComponent::AddTorque(const Vec3 &torque_vec, bool rel)
	{
		if(m_RigidBody)
		{

			m_RigidBody->markForWrite();
			if(rel)
			{
				hkVector4 local_rot(torque_vec.x,0,0);
				hkVector4 rotateWorld;
				rotateWorld.setRotatedDir( m_RigidBody->getTransform().getRotation(), local_rot);
				m_RigidBody->applyTorque(m_DeltaTime, hkVector4(rotateWorld(0),rotateWorld(1),rotateWorld(2)));
				local_rot.set(0,torque_vec.y,0);
				rotateWorld.setRotatedDir(m_RigidBody->getTransform().getRotation(), local_rot);
				m_RigidBody->applyTorque(m_DeltaTime, hkVector4(rotateWorld(0),rotateWorld(1),rotateWorld(2)));
				//m_RigidBody->applyAngularImpulse(rotateWorld);

				if(fabs(torque_vec.y) > 0)
				{
					std::stringstream ss;
					ss << "torq"<< torque_vec.y << "\n";
					std::string engine_data = ss.str();
					MessagePtr debug_msg(new DebugPrintMessage(engine_data));
					SimEngine::Get().GetSimSystemManager()->SendImmediate(debug_msg);
				}
			}
			else
				m_RigidBody->applyAngularImpulse(hkVector4(torque_vec.x,torque_vec.y,torque_vec.z));

			m_RigidBody->unmarkForWrite();
		}
	}

	void HavokBodyComponent::SetVelocity(const Vec3 &vel, bool rel)
	{

	}

	void HavokBodyComponent::SetAngularVelocity(const Vec3 &vel, bool rel)
	{

	}


	Vec3 HavokBodyComponent::GetAngularVelocity(bool rel)
	{
		Vec3 velocity(0,0,0);
		if(m_RigidBody)
		{
			hkVector4 vel = m_RigidBody->getAngularVelocity();
			if(rel)
			{
				hkVector4 rotateWorld;
				rotateWorld.setRotatedInverseDir( m_RigidBody->getTransform().getRotation(), vel);
				vel = rotateWorld;
			}
			velocity.Set(vel(0),vel(1),vel(2));
		}
		return velocity;
	}

	void HavokBodyComponent::SetActive(bool value)
	{
		m_Active = value;

	}

	bool HavokBodyComponent::GetActive() const
	{
		return m_Active;
	}

	Vec3 HavokBodyComponent::GetTorque(bool rel)
	{
		Vec3 torque(0,0,0);
		return torque;
	}


	void HavokBodyComponent::AddForce(const Vec3 &force_vec, bool rel)
	{
		if(m_RigidBody)
		{
			m_RigidBody->markForWrite();
			if(rel)
			{
				hkVector4 local_rot(force_vec.x,force_vec.y,force_vec.z);
				hkVector4 rotateWorld;
				rotateWorld.setRotatedDir( m_RigidBody->getTransform().getRotation(), local_rot);
				m_RigidBody->applyLinearImpulse(rotateWorld);
			}
			else
				m_RigidBody->applyLinearImpulse(hkVector4(force_vec.x,force_vec.y,force_vec.z));
			m_RigidBody->unmarkForWrite();
		}
	}

	void HavokBodyComponent::AddForceAtPos(const Vec3 &force_vec, const Vec3& pos_vec, bool rel_force, bool rel_pos)
	{

	}

	void HavokBodyComponent::SetMass(float mass)
	{
		m_Mass = mass;


	}



	Vec3 HavokBodyComponent::GetVelocity(bool rel)
	{
		Vec3 velocity(0,0,0);
		if(m_RigidBody)
		{
			hkVector4 vel = m_RigidBody->getLinearVelocity();
			if(rel)
			{
				hkVector4 rotateWorld;
				rotateWorld.setRotatedInverseDir( m_RigidBody->getTransform().getRotation(), vel);
				vel = rotateWorld;
			}
			velocity.Set(vel(0),vel(1),vel(2));
		}
		return velocity;
	}

	void HavokBodyComponent::SetPosition(const Vec3 &value)
	{
		if(m_RigidBody)
		{
			//m_World->markForWrite();
			m_World->lock();
			hkVector4 h_pos(value.x,value.y,value.z);
			Vec3 trans_vec = value - GetPosition();
			hkVector4 h_trans_vec(trans_vec.x,trans_vec.y,trans_vec.z);
			//m_RigidBody->setPositionAndRotationAsCriticalOperation(h_pos,hkQuaternion (0,0,0,1));
			//m_RigidBody->getRigidMotion()->setPosition(h_pos);
			m_RigidBody->setPosition(h_pos);
			m_RigidBody->updateCachedAabb();

			// Get its constraints
			const int numConstraints = m_RigidBody->getNumConstraints();
			for (int ci = 0; ci < numConstraints; ci++)
			{
				hkpRigidBody *child_rb = NULL;
				hkpConstraintInstance* constraint = const_cast<hkpConstraintInstance*>(m_RigidBody->getConstraint(ci));
				if(m_RigidBody != constraint->getRigidBodyA())
				{
					child_rb = constraint->getRigidBodyA();
					
					hkVector4 child_pos = child_rb->getPosition();
					child_pos.add(h_trans_vec);
					//child_rb->getRigidMotion()->setPosition(child_pos);

					//recursive!
					HavokBodyComponent* child_body = (HavokBodyComponent*) child_rb->getUserData();
					if(child_body)
						child_body->SetPosition(Vec3(child_pos(0),child_pos(1),child_pos(2)));

				}
									//const hkpConstraintData* data = constraint->getData();
			}
			m_World->unlock();
		}
	}

	Vec3  HavokBodyComponent::GetPosition() const
	{
		Vec3 pos(0,0,0);
		if(m_RigidBody)
		{
			hkVector4 h_pos = m_RigidBody->getPosition();
			pos.Set(h_pos(0),h_pos(1),h_pos(2));
		}

		return pos;
	}

	void HavokBodyComponent::SetRotation(const Quaternion &rot)
	{

		if(m_RigidBody)
		{
			m_World->lock();
			hkQuaternion h_rot(rot.x,rot.y,rot.z,-rot.w);
			m_RigidBody->getRigidMotion()->setRotation(h_rot);
			m_RigidBody->updateCachedAabb();
			m_World->unlock();
		}

	}

	Quaternion HavokBodyComponent::GetRotation()
	{
		Quaternion rot;
		if(m_RigidBody)
		{
			
			hkQuaternion h_rot = m_RigidBody->getRotation();
			rot.x = h_rot(0);
			rot.y = h_rot(1);
			rot.z = h_rot(2);
			rot.w = -h_rot(3);
		}
		return rot;
	}

	int HavokBodyComponent::GetCollisionGroup()
	{
		HavokBodyComponentPtr comp = GetSceneObject()->GetObjectUnderRoot()->GetFirstComponentByClass<HavokBodyComponent>();
		if(comp)
		{
			if(m_SystemCollisionGroup == -1)
			{

				hkpGroupFilter* filter = new hkpGroupFilter();
				hkpGroupFilterSetup::setupGroupFilter( filter );
				m_SystemCollisionGroup  = filter->getNewSystemGroup();
				m_World->setCollisionFilter( filter );
				filter->removeReference();
				
			}
		}
		return  m_SystemCollisionGroup;
	}
}