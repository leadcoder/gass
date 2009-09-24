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

#include "Plugins/PhysX/PhysXBody.h"
#include "Plugins/PhysX/PhysXPhysicsSceneManager.h"
#include "Core/Math/AABox.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include <boost/bind.hpp>


namespace GASS
{
	PhysXBody::PhysXBody() :
		
		m_Center(0,0,0),
		m_MassOffset(0,0,0),
		m_AutoDisable(true),
		m_FastRotation(true),
		m_SceneManager(NULL),
		m_MassRepresentation(MR_GEOMETRY),
		m_Mass(1)
	{

	}

	PhysXBody::~PhysXBody()
	{
		
	}

	void PhysXBody::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysXBody",new Creator<PhysXBody, IComponent>);
		RegisterProperty<float>("Mass", &PhysXBody::GetMass, &PhysXBody::SetMass);
		RegisterProperty<Vec3>("CGPosition",&PhysXBody::GetCGPosition, &PhysXBody::SetCGPosition);
		RegisterProperty<Vec3>("SymmetricInertia",&PhysXBody::GetSymmetricInertia, &PhysXBody::SetSymmetricInertia);
		RegisterProperty<Vec3>("AssymetricInertia",&PhysXBody::GetAssymetricInertia, &PhysXBody::SetAssymetricInertia);
		RegisterProperty<bool>("EffectJoints",&PhysXBody::GetEffectJoints, &PhysXBody::SetEffectJoints);
		
	}

	void PhysXBody::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_LOAD_PHYSICS_COMPONENTS, MESSAGE_FUNC( PhysXBody::OnLoad ));
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_POSITION,				MESSAGE_FUNC( PhysXBody::OnPositionChanged));
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_ROTATION,				MESSAGE_FUNC( PhysXBody::OnRotationChanged ));
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_PHYSICS_BODY_PARAMETER,  MESSAGE_FUNC(PhysXBody::OnParameterMessage));
	}

	void PhysXBody::OnPositionChanged(MessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = boost::any_cast<Vec3>(message->GetData("Position"));
			SetPosition(pos);
		}
	}

	void PhysXBody::OnRotationChanged(MessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Quaternion rot = boost::any_cast<Quaternion>(message->GetData("Rotation"));
			SetRotation(rot);
		}
	}

	void PhysXBody::OnParameterMessage(MessagePtr message)
	{
		SceneObject::PhysicsBodyParameterType type = boost::any_cast<SceneObject::PhysicsBodyParameterType>(message->GetData("Parameter"));
		//wake body!!
		Enable();
		switch(type)
		{
		case SceneObject::FORCE:
			{
				Vec3 value = boost::any_cast<Vec3>(message->GetData("Value"));
				AddForce(value,true);
			}
			break;
		case SceneObject::TORQUE:
			{
				Vec3 value = boost::any_cast<Vec3>(message->GetData("Value"));
				AddTorque(value,true);
				break;
			}
		case SceneObject::VELOCITY:
			{
				Vec3 value = boost::any_cast<Vec3>(message->GetData("Value"));
				SetVelocity(value,true);
				break;
			}

		}
	}

	void PhysXBody::OnLoad(MessagePtr message)
	{
		m_SceneManager = boost::any_cast<PhysXPhysicsSceneManager*>(message->GetData("PhysicsSceneManager"));
		assert(m_SceneManager);
		NxBodyDesc bodyDesc;
		NxActorDesc actorDesc;

		//Get all geometries
		IComponentContainer::ComponentIterator comp_iter = GetSceneObject->GetComponents();
		while(comp_iter.hasMoreElements())
		{
			BaseSceneComponentPtr comp = boost::shared_static_cast<BaseSceneComponent>(comp_iter.getNext());
			PhysXGeometryPtr geom = boost::shared_dynamic_cast<PhysXGeometry>(comp)
			if(geom)
			{				
				actorDesc.shapes.pushBack(geom->GetShape());		
			}
		}
		actorDesc.body			= &bodyDesc;
		actorDesc.density		= 10.0f;
		actorDesc.globalPose.t	= NxVec3(0,0,0);	
		m_Actor = m_SceneManager->GetNxScene()->createActor(actorDesc);	
	}


	void PhysXBody::BodyMoved()
	{
		int from_id = (int)this; //use address as id
		MessagePtr pos_msg(new Message(SceneObject::OBJECT_RM_POSITION,from_id));
		Vec3 pos = GetPosition();
		
		pos_msg->SetData("Position",pos);
		GetSceneObject()->PostMessage(pos_msg);

		MessagePtr rot_msg(new Message(SceneObject::OBJECT_RM_ROTATION,from_id));
		rot_msg->SetData("Rotation",GetRotation());
		GetSceneObject()->PostMessage(rot_msg);
		
		MessagePtr physics_msg(new Message(SceneObject::OBJECT_NM_PHYSICS_VELOCITY,from_id));
		physics_msg->SetData("Velocity",GetVelocity(true));
		physics_msg->SetData("AngularVelocity",GetAngularVelocity(true));
		GetSceneObject()->PostMessage(physics_msg);
		//msg->SetData("Rotation",Vec3(0,0,0));
		
	}

	void PhysXBody::SetMassProperties(float mass, Vec3 &CGPosition, Vec3 &symmetricInertia, Vec3 &assymmetricInertia)
	{
		m_Mass = mass;
		m_CGPosition = CGPosition;
		m_SymmetricInertia = symmetricInertia;
		m_AssymetricInertia = assymmetricInertia;
/*		dMassSetParameters(&m_ODEMass,mass,
			CGPosition.x,CGPosition.y,CGPosition.z,
			symmetricInertia.x,symmetricInertia.y,symmetricInertia.z,
			assymmetricInertia.x,assymmetricInertia.y,assymmetricInertia.z);
		dBodySetMass(m_PhysXBody, &m_ODEMass);*/
	}

	Vec3 PhysXBody::GetForce(bool rel)
	{
		Vec3 force(0,0,0);
		if (m_PhysXBody) {
			const dReal *f_p = dBodyGetForce(m_PhysXBody);
			if (rel) {
				dVector3 vec;
				dBodyVectorFromWorld(m_PhysXBody,f_p[0],f_p[1],f_p[2],vec);
				force.Set(vec[0],vec[1],vec[2]);
			} else
				force.Set(f_p[0],f_p[1],f_p[2]);
		}
		return force;
	}

	void PhysXBody::SetForce(const Vec3 &force)
	{
		if(m_PhysXBody)
		{
			dBodySetForce(m_PhysXBody,force.x,force.y,force.z);
		}
	}

	void PhysXBody::SetTorque(const Vec3 &torque)
	{
		if(m_PhysXBody)
		{
			dBodySetTorque(m_PhysXBody,torque.x,torque.y,torque.z);
		}
	}

	void PhysXBody::AddTorque(const Vec3 &torque_vec, bool rel)
	{
		if(m_PhysXBody)
		{
			if (rel)
				dBodyAddRelTorque(m_PhysXBody, torque_vec.x,torque_vec.y,torque_vec.z);
			else
				dBodyAddTorque(m_PhysXBody, torque_vec.x,torque_vec.y,torque_vec.z);
		}
	}

	void PhysXBody::SetVelocity(const Vec3 &vel, bool rel)
	{
		if(m_PhysXBody)
		{
			if (rel) {
				dVector3 vec;
				dBodyVectorToWorld(m_PhysXBody,vel.x,vel.y,vel.z,vec);
				dBodySetLinearVel(m_PhysXBody,vec[0],vec[1],vec[2]);
			} else
				dBodySetLinearVel(m_PhysXBody,vel.x,vel.y,vel.z);
		}
	}

	void PhysXBody::SetAngularVelocity(const Vec3 &vel, bool rel)
	{
		if(m_PhysXBody)
		{
			if (rel) {
				dVector3 vec;
				dBodyVectorToWorld(m_PhysXBody,vel.x,vel.y,vel.z,vec);
				dBodySetAngularVel(m_PhysXBody,vec[0],vec[1],vec[2]);
			} else
				dBodySetAngularVel(m_PhysXBody,vel.x,vel.y,vel.z);
		}
	}


	Vec3 PhysXBody::GetAngularVelocity(bool rel)
	{
		Vec3 vel(0,0,0);
		if(m_PhysXBody)
		{
			dReal const * vel_p = dBodyGetAngularVel( m_PhysXBody);
			if (rel) {
				dVector3 vec;
				dBodyVectorFromWorld(m_PhysXBody,vel_p[0],vel_p[1],vel_p[2],vec);
				vel.Set(vec[0],vec[1],vec[2]);
			} else
				vel.Set(vel_p[0],vel_p[1],vel_p[2]);
		}
		return vel;
	}

	void PhysXBody::Enable()
	{
		if(m_PhysXBody)
		{
			dBodyEnable(m_PhysXBody);
		}
	}
	bool PhysXBody::IsEnabled()
	{
		if(dBodyIsEnabled(m_PhysXBody) == 0) return false;
		return true;
	}

	void PhysXBody::Disable()
	{
		if(m_PhysXBody)
		{
			dBodyDisable(m_PhysXBody);
		}
	}

	Vec3 PhysXBody::GetTorque(bool rel)
	{
		Vec3 torque(0,0,0);
		if (m_PhysXBody) {
			const dReal *f_p = dBodyGetTorque(m_PhysXBody);
			if (rel) {
				dVector3 vec;
				dBodyVectorFromWorld(m_PhysXBody,f_p[0],f_p[1],f_p[2],vec);
				torque.Set(vec[0],vec[1],vec[2]);
			} else
				torque.Set(f_p[0],f_p[1],f_p[2]);
		}
		return torque;
	}



	void PhysXBody::DampenBody( dBodyID body, float vScale, float aScale )
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
	}

	dSpaceID PhysXBody::GetSpace()
	{
		if(m_SceneManager && m_ODESpaceID == NULL)
		{
			m_ODESpaceID = m_SceneManager->GetPhysicsSpace();//dSimpleSpaceCreate(ODEPhysicsManager::m_Space);
		}
		return m_ODESpaceID;
	}

	dSpaceID PhysXBody::GetSecondarySpace()
	{

		if(m_ODESecondarySpaceID == 0)
		{
			m_ODESecondarySpaceID = dSimpleSpaceCreate(m_SceneManager->GetCollisionSpace());
		}
		return m_ODESecondarySpaceID;
	}



	void PhysXBody::AddForce(const Vec3 &force_vec, bool rel)
	{
		if(m_PhysXBody)
		{
			if(rel)
				dBodyAddRelForce(m_PhysXBody, force_vec.x,force_vec.y,force_vec.z);
			else
				dBodyAddForce(m_PhysXBody, force_vec.x,force_vec.y,force_vec.z);
		}
	}

	void PhysXBody::AddForceAtPos(const Vec3 &force_vec, const Vec3& pos_vec, bool rel_force, bool rel_pos)
	{
		if (m_PhysXBody)
		{
			if (rel_force) {
				if (rel_pos)
					dBodyAddRelForceAtRelPos(m_PhysXBody, force_vec.x, force_vec.y, force_vec.z,
					pos_vec.x, pos_vec.y, pos_vec.z);
				else
					dBodyAddRelForceAtPos(m_PhysXBody, force_vec.x, force_vec.y, force_vec.z,
					pos_vec.x, pos_vec.y, pos_vec.z);
			} else {
				if (rel_pos)
					dBodyAddForceAtRelPos(m_PhysXBody, force_vec.x, force_vec.y, force_vec.z,
					pos_vec.x, pos_vec.y, pos_vec.z);
				else
					dBodyAddForceAtPos(m_PhysXBody, force_vec.x, force_vec.y, force_vec.z,
					pos_vec.x, pos_vec.y, pos_vec.z);
			}
		}
	}

	void PhysXBody::SetMass(float mass)
	{
		m_Mass = mass;

		if(m_PhysXBody)
		{
			dMassAdjust(&m_ODEMass, m_Mass);
			dBodySetMass(m_PhysXBody, &m_ODEMass);
			m_CGPosition = Vec3(m_ODEMass.c[0],m_ODEMass.c[1],m_ODEMass.c[2]);
		}
		// TODO: update m_SymmetricInertia and m_AssymetricInertia
	}

	void PhysXBody::SetODEMass(dMass mass)
	{
		m_ODEMass = mass;
		dBodySetMass(m_PhysXBody, &m_ODEMass);
		m_Mass = mass.mass;
		m_CGPosition = Vec3(mass.c[0],mass.c[1],mass.c[2]);
		// TODO: update m_SymmetricInertia and m_AssymetricInertia

	}

	Vec3 PhysXBody::GetVelocity(bool rel)
	{
		Vec3 vel(0,0,0);
		if (m_PhysXBody) {
			const dReal *vel_p = dBodyGetLinearVel  (m_PhysXBody);
			if (rel) {
				dVector3 vec;
				dBodyVectorFromWorld(m_PhysXBody,vel_p[0],vel_p[1],vel_p[2],vec);
				vel.Set(vec[0],vec[1],vec[2]);
			} else
				vel.Set(vel_p[0],vel_p[1],vel_p[2]);
		}
		return vel;
	}

	void PhysXBody::SetPosition(const Vec3 &value)
	{
		if(m_PhysXBody)
		{
			Vec3 trans_vec = value - GetPosition();
			dBodySetPosition(m_PhysXBody, value.x, value.y, value.z);

			if(m_EffectJoints)
			{
				int num_joints = dBodyGetNumJoints(m_PhysXBody);
				for(int i = 0 ; i < num_joints ;i++)
				{
					dJointID joint = dBodyGetJoint(m_PhysXBody,i);
					dBodyID b2 = dJointGetBody (joint, 1);
					PhysXBody* child_body = (PhysXBody*) dBodyGetData(b2);
					if(child_body && child_body != this)
					{
						const dReal *p = dBodyGetPosition(b2);
						Vec3 pos(p[0],p[1],p[2]);
						pos = pos + trans_vec;
						child_body->SetPosition(pos);
					}

					/*const dReal *p = dBodyGetPosition(b2);
					Vec3 pos(p[0],p[1],p[2]);
					pos = pos + trans_vec;
					dBodySetPosition(b2, pos.x, pos.y, pos.z);*/
				}
			}
		}
	}

	Vec3  PhysXBody::GetPosition() const
	{
		Vec3 pos(0,0,0);
		if(m_PhysXBody)
		{
			const dReal *p = dBodyGetPosition(m_PhysXBody);
			pos = Vec3(p[0],p[1],p[2]);
		}
		return pos;
	}

	void PhysXBody::SetRotation(const Quaternion &rot)
	{
		if(m_PhysXBody)
		{
			dReal ode_rot_mat[12];
			Mat4 rot_mat;
			rot_mat.Identity();
			rot.ToRotationMatrix(rot_mat);
			ODEPhysicsSceneManager::CreateODERotationMatrix(rot_mat,ode_rot_mat);
			dBodySetRotation(m_PhysXBody, ode_rot_mat);
		}
	}

	Quaternion PhysXBody::GetRotation()
	{
		Quaternion q;

		if(m_PhysXBody)
		{
			
			const dReal *ode_rot_mat = dBodyGetRotation(m_PhysXBody);
			Mat4 rot;
			ODEPhysicsSceneManager::CreateGASSRotationMatrix(ode_rot_mat,rot);
			q.FromRotationMatrix(rot);
		}
		return q;
	}
}
