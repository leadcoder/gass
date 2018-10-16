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

#include "Plugins/ODE/ODEBodyComponent.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Core/Math/GASSAABox.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Sim/GASSScene.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSceneObject.h"

namespace GASS
{
	ODEBodyComponent::ODEBodyComponent()
		:m_ODESpaceID(NULL),
		m_ODESecondarySpaceID(NULL),
		m_ODEBodyID(0),
		m_AutoDisable(true),
		m_FastRotation(true),
		m_MassRepresentation(MR_GEOMETRY),
		m_Mass(1),
		m_CGPosition(0,0,0),
		m_SymmetricInertia(0,0,0),
		m_AssymetricInertia(0,0,0),
		m_EffectJoints(true),
		m_Active(true),
		m_Debug(false)
	{
	}

	ODEBodyComponent::~ODEBodyComponent()
	{
		if(m_ODEBodyID) dBodyDestroy(m_ODEBodyID);
	}

	void ODEBodyComponent::RegisterReflection()
	{
		RegisterProperty<float>("Mass", &ODEBodyComponent::GetMass, &ODEBodyComponent::SetMass);
		RegisterProperty<Vec3>("CGPosition",&ODEBodyComponent::GetCGPosition, &ODEBodyComponent::SetCGPosition);
		RegisterProperty<Vec3>("SymmetricInertia",&ODEBodyComponent::GetSymmetricInertia, &ODEBodyComponent::SetSymmetricInertia);
		RegisterProperty<Vec3>("AssymetricInertia",&ODEBodyComponent::GetAssymetricInertia, &ODEBodyComponent::SetAssymetricInertia);
		RegisterProperty<bool>("EffectJoints",&ODEBodyComponent::GetEffectJoints, &ODEBodyComponent::SetEffectJoints);
		RegisterProperty<bool>("Active",&ODEBodyComponent::GetActive, &ODEBodyComponent::SetActive);

	}

	void ODEBodyComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBodyComponent::OnLocationLoaded,LocationLoadedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBodyComponent::OnPositionChanged,PositionRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBodyComponent::OnWorldPositionChanged,WorldPositionRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBodyComponent::OnRotationChanged,RotationRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBodyComponent::OnWorldRotationChanged,WorldRotationRequest,0));
	}

	void ODEBodyComponent::OnPositionChanged(PositionRequestPtr message)
	{
		int this_id = GASS_PTR_TO_INT(this); //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}


	void ODEBodyComponent::OnWorldPositionChanged(WorldPositionRequestPtr message)
	{
		int this_id = GASS_PTR_TO_INT(this); //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}

	void ODEBodyComponent::OnWorldRotationChanged(WorldRotationRequestPtr message)
	{
		int this_id = GASS_PTR_TO_INT(this); //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Quaternion rot = message->GetRotation();
			SetRotation(rot);
		}
	}

	void ODEBodyComponent::OnRotationChanged(RotationRequestPtr message)
	{
		int this_id = GASS_PTR_TO_INT(this); //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Quaternion rot = message->GetRotation();
			SetRotation(rot);
		}
	}

	void ODEBodyComponent::Wake()
	{
		if(GetActive())
			SetActive(true);
	}

	void ODEBodyComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		ODEPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<ODEPhysicsSceneManager>();
		assert(scene_manager);
		m_SceneManager = scene_manager;

		//Vec3 abs_pos;
		m_ODEBodyID = dBodyCreate(scene_manager->GetWorld());
		//From car world
		//Set the auto-disable flag of a body. If the do_auto_disable is nonzero the body will be automatically disabled when it has been idle for long enough.
		dBodySetAutoDisableDefaults(m_ODEBodyID);
		if(m_AutoDisable) dBodySetAutoDisableFlag(m_ODEBodyID, 1);
		else dBodySetAutoDisableFlag(m_ODEBodyID, 0);
		//Set whether the body is influenced by the world's gravity or not. If mode is nonzero it is, if mode is zero, it isn't. Newly created bodies are always influenced by the world's gravity.
		dBodySetGravityMode(m_ODEBodyID, 1);

		//SetBodyTransformation(m_ODEBodyID,m_Owner);
		//From ode doc
		//0: An ``infinitesimal'' orientation update is used. This is fast to compute, but it can occasionally cause inaccuracies for bodies that are rotating at high speed, especially when those bodies are joined to other bodies. This is the default for every new body that is created.
		//1: A ``finite'' orientation update is used. This is more costly to compute, but will be more accurate for high speed rotations. Note however that high speed rotations can result in many types of error in a simulation, and this mode will only fix one of those sources of error.
		if(m_FastRotation) dBodySetFiniteRotationMode(m_ODEBodyID, 1 );
		/*
		// Create mass and inertia matrix based on bounding box if nothing else specified
		Vec3 box = GetOwner()->GetFirstGeometry()->GetBoundingBox().GetSize();
		if (box.x != 0 && box.y != 0 && box.z != 0) {
		dMassSetBoxTotal(&m_ODEMass,m_Mass,box.x,box.y,box.z);
		FileLog::Print("ODEBodyComponent BBox mass %.2f x %.2f y %.2f z %.2f",m_Mass,box.x,box.y,box.z);
		dBodySetMass(m_ODEBodyID,&m_ODEMass);
		} else
		GASS_LOG(LWARNING) << "ODEBodyComponent BBox with zero thickness: mass %.2f x %.2f y %.2f z %.2f",m_Mass,box.x,box.y,box.z);
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
				case MR_GEOMETRY:
				case MR_USER:
					break;
				}
				SetODEMass(ode_mass);
			}

		}
		dBodySetData(m_ODEBodyID, (void*)this);

		GASS_SHARED_PTR<ILocationComponent> location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		SetPosition(location->GetPosition());

		dBodySetMovedCallback (m_ODEBodyID, &BodyMovedCallback);

		SetActive(m_Active);

		GetSceneObject()->SendImmediateEvent(PhysicsBodyLoadedEventPtr(new PhysicsBodyLoadedEvent()));
	}

	void ODEBodyComponent::BodyMovedCallback(dBodyID id)
	{
		ODEBodyComponent* ode_body = (ODEBodyComponent*) dBodyGetData(id);
		ode_body->BodyMoved();
	}

	void ODEBodyComponent::BodyMoved()
	{
		int from_id = GASS_PTR_TO_INT(this); //use address as id
		Vec3 pos = GetPosition();

		GetSceneObject()->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(pos,from_id)));
		GetSceneObject()->PostRequest(WorldRotationRequestPtr(new WorldRotationRequest(GetRotation(),from_id)));
		GetSceneObject()->PostEvent(PhysicsVelocityEventPtr(new PhysicsVelocityEvent(GetVelocity(true),GetAngularVelocity(true),from_id)));
	}


	/*	bool ODEBodyComponent::WantsContact( dContact & contact, IPhysicsObject * other, dGeomID you, dGeomID him, bool firstTest)
	{
	BaseObject* bo = (BaseObject*)m_Owner->GetRoot()
	if(bo && bo->IsMaster()) return true;
	else return false;
	}
	}*/

	void ODEBodyComponent::SetMassProperties(float mass, Vec3 &CGPosition, Vec3 &symmetricInertia, Vec3 &assymmetricInertia)
	{
		m_Mass = mass;
		m_CGPosition = CGPosition;
		m_SymmetricInertia = symmetricInertia;
		m_AssymetricInertia = assymmetricInertia;
		dMassSetParameters(&m_ODEMass,mass,
			CGPosition.x,CGPosition.y,CGPosition.z,
			symmetricInertia.x,symmetricInertia.y,symmetricInertia.z,
			assymmetricInertia.x,assymmetricInertia.y,assymmetricInertia.z);
		dBodySetMass(m_ODEBodyID, &m_ODEMass);
	}

	Vec3 ODEBodyComponent::GetForce(bool rel) const
	{
		Vec3 force(0,0,0);
		if (m_ODEBodyID) {
			const dReal *f_p = dBodyGetForce(m_ODEBodyID);
			if (rel) {
				dVector3 vec;
				dBodyVectorFromWorld(m_ODEBodyID,f_p[0],f_p[1],f_p[2],vec);
				force.Set(vec[0],vec[1],vec[2]);
			} else
				force.Set(f_p[0],f_p[1],f_p[2]);
		}
		return force;
	}

	void ODEBodyComponent::SetForce(const Vec3 &force)
	{
		if(m_ODEBodyID)
		{
			dBodySetForce(m_ODEBodyID,force.x,force.y,force.z);
		}
	}

	void ODEBodyComponent::SetTorque(const Vec3 &torque)
	{
		if(m_ODEBodyID)
		{
			dBodySetTorque(m_ODEBodyID,torque.x,torque.y,torque.z);
		}
	}

	void ODEBodyComponent::AddTorque(const Vec3 &torque_vec, bool rel)
	{
		if(m_ODEBodyID)
		{
			Wake();
			if (rel)
				dBodyAddRelTorque(m_ODEBodyID, torque_vec.x,torque_vec.y,torque_vec.z);
			else
				dBodyAddTorque(m_ODEBodyID, torque_vec.x,torque_vec.y,torque_vec.z);
		}
	}

	void ODEBodyComponent::SetVelocity(const Vec3 &vel, bool rel)
	{
		if(m_ODEBodyID)
		{
			if (rel) {
				dVector3 vec;
				dBodyVectorToWorld(m_ODEBodyID,vel.x,vel.y,vel.z,vec);
				dBodySetLinearVel(m_ODEBodyID,vec[0],vec[1],vec[2]);
			} else
				dBodySetLinearVel(m_ODEBodyID,vel.x,vel.y,vel.z);
		}
	}

	void ODEBodyComponent::SetAngularVelocity(const Vec3 &vel, bool rel)
	{
		if(m_ODEBodyID)
		{
			if (rel) {
				dVector3 vec;
				dBodyVectorToWorld(m_ODEBodyID,vel.x,vel.y,vel.z,vec);
				dBodySetAngularVel(m_ODEBodyID,vec[0],vec[1],vec[2]);
			} else
				dBodySetAngularVel(m_ODEBodyID,vel.x,vel.y,vel.z);
		}
	}


	Vec3 ODEBodyComponent::GetAngularVelocity(bool rel) const
	{
		Vec3 vel(0,0,0);
		if(m_ODEBodyID)
		{
			dReal const * vel_p = dBodyGetAngularVel( m_ODEBodyID);
			if (rel) {
				dVector3 vec;
				dBodyVectorFromWorld(m_ODEBodyID,vel_p[0],vel_p[1],vel_p[2],vec);
				vel.Set(vec[0],vec[1],vec[2]);
			} else
				vel.Set(vel_p[0],vel_p[1],vel_p[2]);
		}
		return vel;
	}

	void ODEBodyComponent::SetActive(bool value)
	{
		m_Active = value;
		if(m_ODEBodyID)
		{
			if(m_Active)
				dBodyEnable(m_ODEBodyID);
			else
				dBodyDisable(m_ODEBodyID);
		}
	}

	bool ODEBodyComponent::GetActive() const
	{
		return m_Active;
	}

	Vec3 ODEBodyComponent::GetTorque(bool rel) const
	{
		Vec3 torque(0,0,0);
		if (m_ODEBodyID) {
			const dReal *f_p = dBodyGetTorque(m_ODEBodyID);
			if (rel) {
				dVector3 vec;
				dBodyVectorFromWorld(m_ODEBodyID,f_p[0],f_p[1],f_p[2],vec);
				torque.Set(vec[0],vec[1],vec[2]);
			} else
				torque.Set(f_p[0],f_p[1],f_p[2]);
		}
		return torque;
	}



	void ODEBodyComponent::DampenBody( dBodyID body, float vScale, float aScale )
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

	dSpaceID ODEBodyComponent::GetSpace()
	{
		ODEPhysicsSceneManagerPtr scene_manager = ODEPhysicsSceneManagerPtr(m_SceneManager);
		if(scene_manager && m_ODESpaceID == NULL)
		{
			m_ODESpaceID = scene_manager->GetPhysicsSpace();//dSimpleSpaceCreate(ODEPhysicsManager::m_Space);
		}
		return m_ODESpaceID;
	}

	dSpaceID ODEBodyComponent::GetSecondarySpace()
	{
		if(m_ODESecondarySpaceID == 0)
		{
			ODEPhysicsSceneManagerPtr scene_manager = ODEPhysicsSceneManagerPtr(m_SceneManager);
			m_ODESecondarySpaceID = dSimpleSpaceCreate(scene_manager->GetCollisionSpace());
		}
		return m_ODESecondarySpaceID;
	}



	void ODEBodyComponent::AddForce(const Vec3 &force_vec, bool rel)
	{
		if(m_ODEBodyID)
		{
			Wake();
			if(rel)
				dBodyAddRelForce(m_ODEBodyID, force_vec.x,force_vec.y,force_vec.z);
			else
				dBodyAddForce(m_ODEBodyID, force_vec.x,force_vec.y,force_vec.z);
		}
	}

	void ODEBodyComponent::AddForceAtPos(const Vec3 &force_vec, const Vec3& pos_vec, bool rel_force, bool rel_pos)
	{
		if (m_ODEBodyID)
		{
			if (rel_force) {
				if (rel_pos)
					dBodyAddRelForceAtRelPos(m_ODEBodyID, force_vec.x, force_vec.y, force_vec.z,
					pos_vec.x, pos_vec.y, pos_vec.z);
				else
					dBodyAddRelForceAtPos(m_ODEBodyID, force_vec.x, force_vec.y, force_vec.z,
					pos_vec.x, pos_vec.y, pos_vec.z);
			} else {
				if (rel_pos)
					dBodyAddForceAtRelPos(m_ODEBodyID, force_vec.x, force_vec.y, force_vec.z,
					pos_vec.x, pos_vec.y, pos_vec.z);
				else
					dBodyAddForceAtPos(m_ODEBodyID, force_vec.x, force_vec.y, force_vec.z,
					pos_vec.x, pos_vec.y, pos_vec.z);
			}
		}
	}

	void ODEBodyComponent::SetMass(float mass)
	{
		m_Mass = mass;

		if(m_ODEBodyID)
		{
			dMassAdjust(&m_ODEMass, m_Mass);
			dBodySetMass(m_ODEBodyID, &m_ODEMass);
			m_CGPosition = Vec3(m_ODEMass.c[0],m_ODEMass.c[1],m_ODEMass.c[2]);
		}
		// TODO: update m_SymmetricInertia and m_AssymetricInertia
	}

	void ODEBodyComponent::SetODEMass(dMass mass)
	{
		m_ODEMass = mass;
		dBodySetMass(m_ODEBodyID, &m_ODEMass);
		m_Mass = static_cast<float>(mass.mass);
		m_CGPosition = Vec3(mass.c[0],mass.c[1],mass.c[2]);
		// TODO: update m_SymmetricInertia and m_AssymetricInertia

	}

	Vec3 ODEBodyComponent::GetVelocity(bool rel) const
	{
		Vec3 vel(0,0,0);
		if (m_ODEBodyID) {
			const dReal *vel_p = dBodyGetLinearVel  (m_ODEBodyID);
			if (rel) {
				dVector3 vec;
				dBodyVectorFromWorld(m_ODEBodyID,vel_p[0],vel_p[1],vel_p[2],vec);
				vel.Set(vec[0],vec[1],vec[2]);
			} else
				vel.Set(vel_p[0],vel_p[1],vel_p[2]);
		}
		return vel;
	}

	void ODEBodyComponent::SetPosition(const Vec3 &value)
	{
		if(m_ODEBodyID)
		{
			Vec3 trans_vec = value - GetPosition();
			dBodySetPosition(m_ODEBodyID, value.x, value.y, value.z);

			if(m_EffectJoints && GetActive())
			{
				int num_joints = dBodyGetNumJoints(m_ODEBodyID);
				for(int i = 0 ; i < num_joints ;i++)
				{
					dJointID joint = dBodyGetJoint(m_ODEBodyID,i);
					dBodyID b2 = dJointGetBody (joint, 1);
					ODEBodyComponent* child_body = (ODEBodyComponent*) dBodyGetData(b2);
					if(child_body && child_body != this)
					{
						const dReal *p = dBodyGetPosition(b2);
						Vec3 pos(p[0],p[1],p[2]);
						pos = pos + trans_vec;
						child_body->SetPosition(pos);

						//send position message in case of paused physics
						int from_id = GASS_PTR_TO_INT(this);

						child_body->GetSceneObject()->PostRequest(PositionRequestPtr(new PositionRequest(pos,from_id)));
					}
					/*const dReal *p = dBodyGetPosition(b2);
					Vec3 pos(p[0],p[1],p[2]);
					pos = pos + trans_vec;
					dBodySetPosition(b2, pos.x, pos.y, pos.z);*/
				}
			}
		}
	}

	Vec3  ODEBodyComponent::GetPosition() const
	{
		Vec3 pos(0,0,0);
		if(m_ODEBodyID)
		{
			const dReal *p = dBodyGetPosition(m_ODEBodyID);
			pos = Vec3(p[0],p[1],p[2]);
		}
		return pos;
	}

	void ODEBodyComponent::SetRotation(const Quaternion &rot)
	{
		if(m_ODEBodyID)
		{
			//Quaternion rel_rot = GetRotation().Inverse()* rot;

			dReal ode_rot_mat[12];
			Mat4 rot_mat(rot);
			ODEPhysicsSceneManager::CreateODERotationMatrix(rot_mat,ode_rot_mat);
			dBodySetRotation(m_ODEBodyID, ode_rot_mat);

			/*if(m_EffectJoints && GetActive())
			{
				int num_joints = dBodyGetNumJoints(m_ODEBodyID);
				for(int i = 0 ; i < num_joints ;i++)
				{
					dJointID joint = dBodyGetJoint(m_ODEBodyID,i);
					dBodyID b2 = dJointGetBody (joint, 1);
					ODEBodyComponent* child_body = (ODEBodyComponent*) dBodyGetData(b2);
					if(child_body && child_body != this)
					{
							const dReal *ode_rot_mat = dBodyGetRotation(b2);
							Mat4 rot;
							ODEPhysicsSceneManager::CreateGASSRotationMatrix(ode_rot_mat,rot);
							Quaternion q;
							q.FromRotationMatrix(rot);
							q = q*rel_rot;
							child_body->SetRotation(q);

							//send position message in case of paused physics
							int from_id = (int) this;
							MessagePtr pos_msg(new RotationRequest(q,from_id));
							child_body->GetSceneObject()->PostMessage(pos_msg);
					}
				}
			}*/
		}
	}

	Quaternion ODEBodyComponent::GetRotation() const
	{
		Quaternion q;

		if(m_ODEBodyID)
		{

			const dReal *ode_rot_mat = dBodyGetRotation(m_ODEBodyID);
			Mat4 rot;
			ODEPhysicsSceneManager::CreateGASSRotationMatrix(ode_rot_mat,rot);
			q.FromRotationMatrix(rot);
		}
		return q;
	}
}
