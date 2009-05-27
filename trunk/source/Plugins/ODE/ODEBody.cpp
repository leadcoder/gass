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

#include "Plugins/ODE/ODEBody.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
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
	ODEBody::ODEBody() 
		:m_ODESpaceID(NULL),
		m_ODESecondarySpaceID(NULL),
		m_ODEBody(0),
		m_Center(0,0,0),
		m_MassOffset(0,0,0),
		m_AutoDisable(true),
		m_FastRotation(true),
		m_SceneManager(NULL),
		m_MassRepresentation(MR_GEOMETRY),
		m_Mass(1)
	{

	}

	ODEBody::~ODEBody()
	{
		if(m_ODEBody) dBodyDestroy(m_ODEBody);
	}

	void ODEBody::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("ODEBody",new Creator<ODEBody, IComponent>);
		RegisterProperty<float>("Mass", &ODEBody::GetMass, &ODEBody::SetMass);
		RegisterProperty<Vec3>("CGPosition",&ODEBody::GetCGPosition, &ODEBody::SetCGPosition);
		RegisterProperty<Vec3>("SymmetricInertia",&ODEBody::GetSymmetricInertia, &ODEBody::SetSymmetricInertia);
		RegisterProperty<Vec3>("AssymetricInertia",&ODEBody::GetAssymetricInertia, &ODEBody::SetAssymetricInertia);
		RegisterProperty<bool>("EffectJoints",&ODEBody::GetEffectJoints, &ODEBody::SetEffectJoints);
		
	}

	void ODEBody::OnCreate()
	{
		int obj_id = (int) this;
		MessageManager * mm = GetMessageManager();
		// Todo: 
		mm->RegisterForMessage(SceneObject::OBJECT_MESSAGE_LOAD_PHYSICS_COMPONENTS, obj_id,  boost::bind( &ODEBody::OnLoad, this, _1 ),0);
		mm->RegisterForMessage(SceneObject::OBJECT_MESSAGE_POSITION, obj_id,  boost::bind( &ODEBody::OnPositionChanged, this, _1 ),0);
		mm->RegisterForMessage(SceneObject::OBJECT_MESSAGE_ROTATION, obj_id,  boost::bind( &ODEBody::OnRotationChanged, this, _1 ),0);
		mm->RegisterForMessage(SceneObject::OBJECT_MESSAGE_PHYSICS_BODY_PARAMETER, obj_id,  boost::bind( &ODEBody::OnParameterMessage, this, _1 ),0);
	}

	void ODEBody::OnPositionChanged(MessagePtr message)
	{
		void* address = (void*) message->m_FromID;
		if(address != this)
		{
			Vec3 pos = boost::any_cast<Vec3>(message->GetData("Position"));
			SetPosition(pos);
		}
	}

	void ODEBody::OnRotationChanged(MessagePtr message)
	{
		void* address = (void*) message->m_FromID;
		if(address != this)
		{
			Quaternion rot = boost::any_cast<Quaternion>(message->GetData("Rotation"));
			SetRotation(rot);
		}
	}

	void ODEBody::OnParameterMessage(MessagePtr message)
	{
		SceneObject::PhysicsParameterType type = boost::any_cast<SceneObject::PhysicsParameterType>(message->GetData("Parameter"));
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
			}
		}
	}

	void ODEBody::OnLoad(MessagePtr message)
	{
		m_SceneManager = boost::any_cast<ODEPhysicsSceneManager*>(message->GetData("PhysicsSceneManager"));
		assert(m_SceneManager);

		Vec3 abs_pos;
		m_ODEBody = dBodyCreate(m_SceneManager->GetWorld());
		//From car world
		//Set the auto-disable flag of a body. If the do_auto_disable is nonzero the body will be automatically disabled when it has been idle for long enough.
		dBodySetAutoDisableDefaults(m_ODEBody);
		if(m_AutoDisable) dBodySetAutoDisableFlag(m_ODEBody, 1);
		else dBodySetAutoDisableFlag(m_ODEBody, 0);
		//Set whether the body is influenced by the world's gravity or not. If mode is nonzero it is, if mode is zero, it isn't. Newly created bodies are always influenced by the world's gravity.
		dBodySetGravityMode(m_ODEBody, 1);

		//SetBodyTransformation(m_ODEBody,m_Owner);
		//From ode doc
		//0: An ``infinitesimal'' orientation update is used. This is fast to compute, but it can occasionally cause inaccuracies for bodies that are rotating at high speed, especially when those bodies are joined to other bodies. This is the default for every new body that is created.
		//1: A ``finite'' orientation update is used. This is more costly to compute, but will be more accurate for high speed rotations. Note however that high speed rotations can result in many types of error in a simulation, and this mode will only fix one of those sources of error.
		if(m_FastRotation) dBodySetFiniteRotationMode(m_ODEBody, 1 );
		/*
		// Create mass and inertia matrix based on bounding box if nothing else specified
		Vec3 box = GetOwner()->GetFirstGeometry()->GetBoundingBox().GetSize();
		if (box.x != 0 && box.y != 0 && box.z != 0) {
		dMassSetBoxTotal(&m_ODEMass,m_Mass,box.x,box.y,box.z);
		Log::Print("ODEBody BBox mass %.2f x %.2f y %.2f z %.2f",m_Mass,box.x,box.y,box.z);
		dBodySetMass(m_ODEBody,&m_ODEMass);
		} else
		Log::Warning("ODEBody BBox with zero thickness: mass %.2f x %.2f y %.2f z %.2f",m_Mass,box.x,box.y,box.z);
		*/
		if (m_MassRepresentation != MR_GEOMETRY) 
		{
			//assert(m_MassRepresentation != MR_USER); // TODO: implement box, sphere, cylinder mass geometries

			if(m_MassRepresentation == MR_USER)
				SetMassProperties(m_Mass,m_CGPosition,m_SymmetricInertia,m_AssymetricInertia);
			else
			{
				dMass ode_mass;
				switch(m_MassRepresentation)
				{
				case MR_BOX:
					dMassSetBoxTotal(&ode_mass, m_Mass, 1,1,1);
					break;
				case MR_CYLINDER:
					dMassSetCylinderTotal(&ode_mass, m_Mass,3, 1, 1);
					break;
				case MR_SPHERE:
					dMassSetSphereTotal(&ode_mass, m_Mass, 1);
					break;
				}
				SetODEMass(ode_mass);
			}

		}
		dBodySetData(m_ODEBody, (void*)this);

		boost::shared_ptr<ILocationComponent> location = GetSceneObject()->GetFirstComponent<ILocationComponent>();
		SetPosition(location->GetPosition());
		
		dBodySetMovedCallback (m_ODEBody, &BodyMovedCallback);
	}

	void ODEBody::BodyMovedCallback(dBodyID id)
	{
		ODEBody* ode_body = (ODEBody*) dBodyGetData(id);
		ode_body->BodyMoved();
	}

	void ODEBody::BodyMoved()
	{
		int from_id = (int)this;
		MessagePtr pos_msg(new Message(SceneObject::OBJECT_MESSAGE_POSITION,from_id));
		Vec3 pos = GetPosition();

		
		pos_msg->SetData("Position",pos);
		GetMessageManager()->SendGlobalMessage(pos_msg);

		MessagePtr rot_msg(new Message(SceneObject::OBJECT_MESSAGE_ROTATION,from_id));
		rot_msg->SetData("Rotation",GetRotation());
		GetMessageManager()->SendGlobalMessage(rot_msg);


		MessagePtr physics_msg(new Message(SceneObject::OBJECT_MESSAGE_PHYSICS,from_id));
		physics_msg->SetData("Velocity",GetVelocity(true));
		physics_msg->SetData("AngularVelocity",GetAngularVelocity(true));
		GetMessageManager()->SendGlobalMessage(physics_msg);
		//msg->SetData("Rotation",Vec3(0,0,0));
		
	}



	/*	bool ODEBody::WantsContact( dContact & contact, IPhysicsObject * other, dGeomID you, dGeomID him, bool firstTest)
	{
	BaseObject* bo = (BaseObject*)m_Owner->GetRoot()		
	if(bo && bo->IsMaster()) return true;
	else return false;
	}
	}*/

	void ODEBody::SetMassProperties(float mass, Vec3 &CGPosition, Vec3 &symmetricInertia, Vec3 &assymmetricInertia)
	{
		m_Mass = mass;
		m_CGPosition = CGPosition;
		m_SymmetricInertia = symmetricInertia;
		m_AssymetricInertia = assymmetricInertia;
		dMassSetParameters(&m_ODEMass,mass,
			CGPosition.x,CGPosition.y,CGPosition.z,
			symmetricInertia.x,symmetricInertia.y,symmetricInertia.z,
			assymmetricInertia.x,assymmetricInertia.y,assymmetricInertia.z);
		dBodySetMass(m_ODEBody, &m_ODEMass);
	}

	Vec3 ODEBody::GetForce(bool rel)
	{
		Vec3 force(0,0,0);
		if (m_ODEBody) {
			const dReal *f_p = dBodyGetForce(m_ODEBody);
			if (rel) {
				dVector3 vec;
				dBodyVectorFromWorld(m_ODEBody,f_p[0],f_p[1],f_p[2],vec);
				force.Set(vec[0],vec[1],vec[2]);
			} else
				force.Set(f_p[0],f_p[1],f_p[2]);
		}
		return force;
	}

	void ODEBody::SetForce(const Vec3 &force)
	{
		if(m_ODEBody)
		{
			dBodySetForce(m_ODEBody,force.x,force.y,force.z);
		}
	}

	void ODEBody::SetTorque(const Vec3 &torque)
	{
		if(m_ODEBody)
		{
			dBodySetTorque(m_ODEBody,torque.x,torque.y,torque.z);
		}
	}

	void ODEBody::AddTorque(const Vec3 &torque_vec, bool rel)
	{
		if(m_ODEBody)
		{
			if (rel)
				dBodyAddRelTorque(m_ODEBody, torque_vec.x,torque_vec.y,torque_vec.z);
			else
				dBodyAddTorque(m_ODEBody, torque_vec.x,torque_vec.y,torque_vec.z);
		}
	}

	void ODEBody::SetVelocity(const Vec3 &vel, bool rel)
	{
		if(m_ODEBody)
		{
			if (rel) {
				dVector3 vec;
				dBodyVectorToWorld(m_ODEBody,vel.x,vel.y,vel.z,vec);
				dBodySetLinearVel(m_ODEBody,vec[0],vec[1],vec[2]);
			} else
				dBodySetLinearVel(m_ODEBody,vel.x,vel.y,vel.z);
		}
	}

	void ODEBody::SetAngularVelocity(const Vec3 &vel, bool rel)
	{
		if(m_ODEBody)
		{
			if (rel) {
				dVector3 vec;
				dBodyVectorToWorld(m_ODEBody,vel.x,vel.y,vel.z,vec);
				dBodySetAngularVel(m_ODEBody,vec[0],vec[1],vec[2]);
			} else
				dBodySetAngularVel(m_ODEBody,vel.x,vel.y,vel.z);
		}
	}


	Vec3 ODEBody::GetAngularVelocity(bool rel)
	{
		Vec3 vel(0,0,0);
		if(m_ODEBody)
		{
			dReal const * vel_p = dBodyGetAngularVel( m_ODEBody);
			if (rel) {
				dVector3 vec;
				dBodyVectorFromWorld(m_ODEBody,vel_p[0],vel_p[1],vel_p[2],vec);
				vel.Set(vec[0],vec[1],vec[2]);
			} else
				vel.Set(vel_p[0],vel_p[1],vel_p[2]);
		}
		return vel;
	}

	void ODEBody::Enable()
	{
		if(m_ODEBody)
		{
			dBodyEnable(m_ODEBody);
		}
	}
	bool ODEBody::IsEnabled()
	{
		if(dBodyIsEnabled(m_ODEBody) == 0) return false;
		return true;
	}

	void ODEBody::Disable()
	{
		if(m_ODEBody)
		{
			dBodyDisable(m_ODEBody);
		}
	}

	Vec3 ODEBody::GetTorque(bool rel)
	{
		Vec3 torque(0,0,0);
		if (m_ODEBody) {
			const dReal *f_p = dBodyGetTorque(m_ODEBody);
			if (rel) {
				dVector3 vec;
				dBodyVectorFromWorld(m_ODEBody,f_p[0],f_p[1],f_p[2],vec);
				torque.Set(vec[0],vec[1],vec[2]);
			} else
				torque.Set(f_p[0],f_p[1],f_p[2]);
		}
		return torque;
	}



	void ODEBody::DampenBody( dBodyID body, float vScale, float aScale )
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

	dSpaceID ODEBody::GetSpace()
	{
		if(m_SceneManager && m_ODESpaceID == NULL)
		{
			m_ODESpaceID = m_SceneManager->GetPhysicsSpace();//dSimpleSpaceCreate(ODEPhysicsManager::m_Space);
		}
		return m_ODESpaceID;
	}

	dSpaceID ODEBody::GetSecondarySpace()
	{

		if(m_ODESecondarySpaceID == 0)
		{
			m_ODESecondarySpaceID = dSimpleSpaceCreate(m_SceneManager->GetCollisionSpace());
		}
		return m_ODESecondarySpaceID;
	}



	void ODEBody::AddForce(const Vec3 &force_vec, bool rel)
	{
		if(m_ODEBody)
		{
			if(rel)
				dBodyAddRelForce(m_ODEBody, force_vec.x,force_vec.y,force_vec.z);
			else
				dBodyAddForce(m_ODEBody, force_vec.x,force_vec.y,force_vec.z);
		}
	}

	void ODEBody::AddForceAtPos(const Vec3 &force_vec, const Vec3& pos_vec, bool rel_force, bool rel_pos)
	{
		if (m_ODEBody)
		{
			if (rel_force) {
				if (rel_pos)
					dBodyAddRelForceAtRelPos(m_ODEBody, force_vec.x, force_vec.y, force_vec.z,
					pos_vec.x, pos_vec.y, pos_vec.z);
				else
					dBodyAddRelForceAtPos(m_ODEBody, force_vec.x, force_vec.y, force_vec.z,
					pos_vec.x, pos_vec.y, pos_vec.z);
			} else {
				if (rel_pos)
					dBodyAddForceAtRelPos(m_ODEBody, force_vec.x, force_vec.y, force_vec.z,
					pos_vec.x, pos_vec.y, pos_vec.z);
				else
					dBodyAddForceAtPos(m_ODEBody, force_vec.x, force_vec.y, force_vec.z,
					pos_vec.x, pos_vec.y, pos_vec.z);
			}
		}
	}

	void ODEBody::SetMass(float mass)
	{
		m_Mass = mass;

		if(m_ODEBody)
		{
			dMassAdjust(&m_ODEMass, m_Mass);
			dBodySetMass(m_ODEBody, &m_ODEMass);
			m_CGPosition = Vec3(m_ODEMass.c[0],m_ODEMass.c[1],m_ODEMass.c[2]);
		}
		// TODO: update m_SymmetricInertia and m_AssymetricInertia
	}

	void ODEBody::SetODEMass(dMass mass)
	{
		m_ODEMass = mass;
		dBodySetMass(m_ODEBody, &m_ODEMass);
		m_Mass = mass.mass;
		m_CGPosition = Vec3(mass.c[0],mass.c[1],mass.c[2]);
		// TODO: update m_SymmetricInertia and m_AssymetricInertia

	}

	Vec3 ODEBody::GetVelocity(bool rel)
	{
		Vec3 vel(0,0,0);
		if (m_ODEBody) {
			const dReal *vel_p = dBodyGetLinearVel  (m_ODEBody);
			if (rel) {
				dVector3 vec;
				dBodyVectorFromWorld(m_ODEBody,vel_p[0],vel_p[1],vel_p[2],vec);
				vel.Set(vec[0],vec[1],vec[2]);
			} else
				vel.Set(vel_p[0],vel_p[1],vel_p[2]);
		}
		return vel;
	}

	void ODEBody::SetPosition(const Vec3 &value)
	{
		if(m_ODEBody)
		{
			Vec3 trans_vec = value - GetPosition();
			dBodySetPosition(m_ODEBody, value.x, value.y, value.z);

			if(m_EffectJoints)
			{
				int num_joints = dBodyGetNumJoints(m_ODEBody);
				for(int i = 0 ; i < num_joints ;i++)
				{
					dJointID joint = dBodyGetJoint(m_ODEBody,i);
					dBodyID b2 = dJointGetBody (joint, 1);
					const dReal *p = dBodyGetPosition(b2);
					Vec3 pos(p[0],p[1],p[2]);
					pos = pos + trans_vec;
					dBodySetPosition(b2, pos.x, pos.y, pos.z);
				}
			}
		}
	}

	Vec3  ODEBody::GetPosition() const
	{
		Vec3 pos(0,0,0);
		if(m_ODEBody)
		{
			const dReal *p = dBodyGetPosition(m_ODEBody);
			pos = Vec3(p[0],p[1],p[2]);
		}
		return pos;
	}

	void ODEBody::SetRotation(const Quaternion &rot)
	{
		if(m_ODEBody)
		{
			dReal ode_rot_mat[12];
			Mat4 rot_mat;
			rot_mat.Identity();
			rot.ToRotationMatrix(rot_mat);
			ODEPhysicsSceneManager::CreateODERotationMatrix(rot_mat,ode_rot_mat);
			dBodySetRotation(m_ODEBody, ode_rot_mat);
		}
	}

	Quaternion ODEBody::GetRotation()
	{
		Quaternion q;

		if(m_ODEBody)
		{
			
			const dReal *ode_rot_mat = dBodyGetRotation(m_ODEBody);
			Mat4 rot;
			ODEPhysicsSceneManager::CreateGASSRotationMatrix(ode_rot_mat,rot);
			q.FromRotationMatrix(rot);
		}
		return q;
	}
}
