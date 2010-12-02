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

#include "Plugins/ODE/ODEBodyComponent.h"
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
	ODEBodyComponent::ODEBodyComponent()
		:m_ODESpaceID(NULL),
		m_ODESecondarySpaceID(NULL),
		m_ODEBodyComponent(0),
		m_AutoDisable(true),
		m_FastRotation(true),
		m_MassRepresentation(MR_GEOMETRY),
		m_Mass(1),
		m_CGPosition(0,0,0),
		m_SymmetricInertia(0,0,0),
		m_AssymetricInertia(0,0,0),
		m_EffectJoints(true),
		m_Active(true)
	{
	}

	ODEBodyComponent::~ODEBodyComponent()
	{
		if(m_ODEBodyComponent) dBodyDestroy(m_ODEBodyComponent);
	}

	void ODEBodyComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsBodyComponent",new Creator<ODEBodyComponent, IComponent>);
		RegisterProperty<float>("Mass", &ODEBodyComponent::GetMass, &ODEBodyComponent::SetMass);
		RegisterProperty<Vec3>("CGPosition",&ODEBodyComponent::GetCGPosition, &ODEBodyComponent::SetCGPosition);
		RegisterProperty<Vec3>("SymmetricInertia",&ODEBodyComponent::GetSymmetricInertia, &ODEBodyComponent::SetSymmetricInertia);
		RegisterProperty<Vec3>("AssymetricInertia",&ODEBodyComponent::GetAssymetricInertia, &ODEBodyComponent::SetAssymetricInertia);
		RegisterProperty<bool>("EffectJoints",&ODEBodyComponent::GetEffectJoints, &ODEBodyComponent::SetEffectJoints);
		RegisterProperty<bool>("Active",&ODEBodyComponent::GetActive, &ODEBodyComponent::SetActive);

	}

	void ODEBodyComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBodyComponent::OnLoad,LoadPhysicsComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBodyComponent::OnPositionChanged,PositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBodyComponent::OnWorldPositionChanged,WorldPositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBodyComponent::OnRotationChanged,RotationMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBodyComponent::OnWorldRotationChanged,WorldRotationMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBodyComponent::OnParameterMessage,PhysicsBodyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEBodyComponent::OnMassMessage,PhysicsMassMessage,0));
	}

	void ODEBodyComponent::OnPositionChanged(PositionMessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}


	void ODEBodyComponent::OnWorldPositionChanged(WorldPositionMessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}

	void ODEBodyComponent::OnWorldRotationChanged(WorldRotationMessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Quaternion rot = message->GetRotation();
			SetRotation(rot);
		}
	}

	void ODEBodyComponent::OnRotationChanged(RotationMessagePtr message)
	{
		int this_id = (int)this; //we used address as id
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

	void ODEBodyComponent::OnParameterMessage(PhysicsBodyMessagePtr message)
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

	void ODEBodyComponent::OnMassMessage(PhysicsMassMessagePtr message)
	{
		SetMass(message->GetMass());
	}

	void ODEBodyComponent::OnLoad(LoadPhysicsComponentsMessagePtr message)
	{
		ODEPhysicsSceneManagerPtr scene_manager = boost::shared_static_cast<ODEPhysicsSceneManager> (message->GetPhysicsSceneManager());
		assert(scene_manager);
		m_SceneManager = scene_manager;

		Vec3 abs_pos;
		m_ODEBodyComponent = dBodyCreate(scene_manager->GetWorld());
		//From car world
		//Set the auto-disable flag of a body. If the do_auto_disable is nonzero the body will be automatically disabled when it has been idle for long enough.
		dBodySetAutoDisableDefaults(m_ODEBodyComponent);
		if(m_AutoDisable) dBodySetAutoDisableFlag(m_ODEBodyComponent, 1);
		else dBodySetAutoDisableFlag(m_ODEBodyComponent, 0);
		//Set whether the body is influenced by the world's gravity or not. If mode is nonzero it is, if mode is zero, it isn't. Newly created bodies are always influenced by the world's gravity.
		dBodySetGravityMode(m_ODEBodyComponent, 1);

		//SetBodyTransformation(m_ODEBodyComponent,m_Owner);
		//From ode doc
		//0: An ``infinitesimal'' orientation update is used. This is fast to compute, but it can occasionally cause inaccuracies for bodies that are rotating at high speed, especially when those bodies are joined to other bodies. This is the default for every new body that is created.
		//1: A ``finite'' orientation update is used. This is more costly to compute, but will be more accurate for high speed rotations. Note however that high speed rotations can result in many types of error in a simulation, and this mode will only fix one of those sources of error.
		if(m_FastRotation) dBodySetFiniteRotationMode(m_ODEBodyComponent, 1 );
		/*
		// Create mass and inertia matrix based on bounding box if nothing else specified
		Vec3 box = GetOwner()->GetFirstGeometry()->GetBoundingBox().GetSize();
		if (box.x != 0 && box.y != 0 && box.z != 0) {
		dMassSetBoxTotal(&m_ODEMass,m_Mass,box.x,box.y,box.z);
		Log::Print("ODEBodyComponent BBox mass %.2f x %.2f y %.2f z %.2f",m_Mass,box.x,box.y,box.z);
		dBodySetMass(m_ODEBodyComponent,&m_ODEMass);
		} else
		Log::Warning("ODEBodyComponent BBox with zero thickness: mass %.2f x %.2f y %.2f z %.2f",m_Mass,box.x,box.y,box.z);
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
		dBodySetData(m_ODEBodyComponent, (void*)this);

		if(!scene_manager->IsActive())
		{
			//Check if parent has physics
		}

		boost::shared_ptr<ILocationComponent> location = GetSceneObject()->GetFirstComponent<ILocationComponent>();
		SetPosition(location->GetPosition());

		dBodySetMovedCallback (m_ODEBodyComponent, &BodyMovedCallback);
	}

	void ODEBodyComponent::BodyMovedCallback(dBodyID id)
	{
		ODEBodyComponent* ode_body = (ODEBodyComponent*) dBodyGetData(id);
		ode_body->BodyMoved();
	}

	void ODEBodyComponent::BodyMoved()
	{
		int from_id = (int)this; //use address as id
		Vec3 pos = GetPosition();
		MessagePtr pos_msg(new PositionMessage(pos,from_id));
		GetSceneObject()->PostMessage(pos_msg);

		MessagePtr rot_msg(new RotationMessage(GetRotation(),from_id));
		GetSceneObject()->PostMessage(rot_msg);

		MessagePtr physics_msg(new VelocityNotifyMessage(GetVelocity(true),GetAngularVelocity(true),from_id));
		GetSceneObject()->PostMessage(physics_msg);
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
		dBodySetMass(m_ODEBodyComponent, &m_ODEMass);
	}

	Vec3 ODEBodyComponent::GetForce(bool rel)
	{
		Vec3 force(0,0,0);
		if (m_ODEBodyComponent) {
			const dReal *f_p = dBodyGetForce(m_ODEBodyComponent);
			if (rel) {
				dVector3 vec;
				dBodyVectorFromWorld(m_ODEBodyComponent,f_p[0],f_p[1],f_p[2],vec);
				force.Set(vec[0],vec[1],vec[2]);
			} else
				force.Set(f_p[0],f_p[1],f_p[2]);
		}
		return force;
	}

	void ODEBodyComponent::SetForce(const Vec3 &force)
	{
		if(m_ODEBodyComponent)
		{
			dBodySetForce(m_ODEBodyComponent,force.x,force.y,force.z);
		}
	}

	void ODEBodyComponent::SetTorque(const Vec3 &torque)
	{
		if(m_ODEBodyComponent)
		{
			dBodySetTorque(m_ODEBodyComponent,torque.x,torque.y,torque.z);
		}
	}

	void ODEBodyComponent::AddTorque(const Vec3 &torque_vec, bool rel)
	{
		if(m_ODEBodyComponent)
		{
			if (rel)
				dBodyAddRelTorque(m_ODEBodyComponent, torque_vec.x,torque_vec.y,torque_vec.z);
			else
				dBodyAddTorque(m_ODEBodyComponent, torque_vec.x,torque_vec.y,torque_vec.z);
		}
	}

	void ODEBodyComponent::SetVelocity(const Vec3 &vel, bool rel)
	{
		if(m_ODEBodyComponent)
		{
			if (rel) {
				dVector3 vec;
				dBodyVectorToWorld(m_ODEBodyComponent,vel.x,vel.y,vel.z,vec);
				dBodySetLinearVel(m_ODEBodyComponent,vec[0],vec[1],vec[2]);
			} else
				dBodySetLinearVel(m_ODEBodyComponent,vel.x,vel.y,vel.z);
		}
	}

	void ODEBodyComponent::SetAngularVelocity(const Vec3 &vel, bool rel)
	{
		if(m_ODEBodyComponent)
		{
			if (rel) {
				dVector3 vec;
				dBodyVectorToWorld(m_ODEBodyComponent,vel.x,vel.y,vel.z,vec);
				dBodySetAngularVel(m_ODEBodyComponent,vec[0],vec[1],vec[2]);
			} else
				dBodySetAngularVel(m_ODEBodyComponent,vel.x,vel.y,vel.z);
		}
	}


	Vec3 ODEBodyComponent::GetAngularVelocity(bool rel)
	{
		Vec3 vel(0,0,0);
		if(m_ODEBodyComponent)
		{
			dReal const * vel_p = dBodyGetAngularVel( m_ODEBodyComponent);
			if (rel) {
				dVector3 vec;
				dBodyVectorFromWorld(m_ODEBodyComponent,vel_p[0],vel_p[1],vel_p[2],vec);
				vel.Set(vec[0],vec[1],vec[2]);
			} else
				vel.Set(vel_p[0],vel_p[1],vel_p[2]);
		}
		return vel;
	}

	void ODEBodyComponent::SetActive(bool value)
	{
		m_Active = value;
		if(m_ODEBodyComponent)
		{
			if(m_Active)
				dBodyEnable(m_ODEBodyComponent);
			else
				dBodyDisable(m_ODEBodyComponent);
		}
	}
	bool ODEBodyComponent::GetActive() const
	{
		return m_Active;
	}

	Vec3 ODEBodyComponent::GetTorque(bool rel)
	{
		Vec3 torque(0,0,0);
		if (m_ODEBodyComponent) {
			const dReal *f_p = dBodyGetTorque(m_ODEBodyComponent);
			if (rel) {
				dVector3 vec;
				dBodyVectorFromWorld(m_ODEBodyComponent,f_p[0],f_p[1],f_p[2],vec);
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
		if(m_ODEBodyComponent)
		{
			if(rel)
				dBodyAddRelForce(m_ODEBodyComponent, force_vec.x,force_vec.y,force_vec.z);
			else
				dBodyAddForce(m_ODEBodyComponent, force_vec.x,force_vec.y,force_vec.z);
		}
	}

	void ODEBodyComponent::AddForceAtPos(const Vec3 &force_vec, const Vec3& pos_vec, bool rel_force, bool rel_pos)
	{
		if (m_ODEBodyComponent)
		{
			if (rel_force) {
				if (rel_pos)
					dBodyAddRelForceAtRelPos(m_ODEBodyComponent, force_vec.x, force_vec.y, force_vec.z,
					pos_vec.x, pos_vec.y, pos_vec.z);
				else
					dBodyAddRelForceAtPos(m_ODEBodyComponent, force_vec.x, force_vec.y, force_vec.z,
					pos_vec.x, pos_vec.y, pos_vec.z);
			} else {
				if (rel_pos)
					dBodyAddForceAtRelPos(m_ODEBodyComponent, force_vec.x, force_vec.y, force_vec.z,
					pos_vec.x, pos_vec.y, pos_vec.z);
				else
					dBodyAddForceAtPos(m_ODEBodyComponent, force_vec.x, force_vec.y, force_vec.z,
					pos_vec.x, pos_vec.y, pos_vec.z);
			}
		}
	}

	void ODEBodyComponent::SetMass(float mass)
	{
		m_Mass = mass;

		if(m_ODEBodyComponent)
		{
			dMassAdjust(&m_ODEMass, m_Mass);
			dBodySetMass(m_ODEBodyComponent, &m_ODEMass);
			m_CGPosition = Vec3(m_ODEMass.c[0],m_ODEMass.c[1],m_ODEMass.c[2]);
		}
		// TODO: update m_SymmetricInertia and m_AssymetricInertia
	}

	void ODEBodyComponent::SetODEMass(dMass mass)
	{
		m_ODEMass = mass;
		dBodySetMass(m_ODEBodyComponent, &m_ODEMass);
		m_Mass = mass.mass;
		m_CGPosition = Vec3(mass.c[0],mass.c[1],mass.c[2]);
		// TODO: update m_SymmetricInertia and m_AssymetricInertia

	}

	Vec3 ODEBodyComponent::GetVelocity(bool rel)
	{
		Vec3 vel(0,0,0);
		if (m_ODEBodyComponent) {
			const dReal *vel_p = dBodyGetLinearVel  (m_ODEBodyComponent);
			if (rel) {
				dVector3 vec;
				dBodyVectorFromWorld(m_ODEBodyComponent,vel_p[0],vel_p[1],vel_p[2],vec);
				vel.Set(vec[0],vec[1],vec[2]);
			} else
				vel.Set(vel_p[0],vel_p[1],vel_p[2]);
		}
		return vel;
	}

	void ODEBodyComponent::SetPosition(const Vec3 &value)
	{
		if(m_ODEBodyComponent)
		{
			Vec3 trans_vec = value - GetPosition();
			dBodySetPosition(m_ODEBodyComponent, value.x, value.y, value.z);

			if(m_EffectJoints)
			{
				int num_joints = dBodyGetNumJoints(m_ODEBodyComponent);
				for(int i = 0 ; i < num_joints ;i++)
				{
					dJointID joint = dBodyGetJoint(m_ODEBodyComponent,i);
					dBodyID b2 = dJointGetBody (joint, 1);
					ODEBodyComponent* child_body = (ODEBodyComponent*) dBodyGetData(b2);
					if(child_body && child_body != this)
					{
						const dReal *p = dBodyGetPosition(b2);
						Vec3 pos(p[0],p[1],p[2]);
						pos = pos + trans_vec;
						child_body->SetPosition(pos);

						//send position message in case of paused physics
						int from_id = (int) this;
						MessagePtr pos_msg(new PositionMessage(pos,from_id));
						child_body->GetSceneObject()->PostMessage(pos_msg);

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
		if(m_ODEBodyComponent)
		{
			const dReal *p = dBodyGetPosition(m_ODEBodyComponent);
			pos = Vec3(p[0],p[1],p[2]);
		}
		return pos;
	}

	void ODEBodyComponent::SetRotation(const Quaternion &rot)
	{
		if(m_ODEBodyComponent)
		{
			dReal ode_rot_mat[12];
			Mat4 rot_mat;
			rot_mat.Identity();
			rot.ToRotationMatrix(rot_mat);
			ODEPhysicsSceneManager::CreateODERotationMatrix(rot_mat,ode_rot_mat);
			dBodySetRotation(m_ODEBodyComponent, ode_rot_mat);
		}
	}

	Quaternion ODEBodyComponent::GetRotation()
	{
		Quaternion q;

		if(m_ODEBodyComponent)
		{

			const dReal *ode_rot_mat = dBodyGetRotation(m_ODEBodyComponent);
			Mat4 rot;
			ODEPhysicsSceneManager::CreateGASSRotationMatrix(ode_rot_mat,rot);
			q.FromRotationMatrix(rot);
		}
		return q;
	}
}
