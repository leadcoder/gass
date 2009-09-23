/****************************************************************************
*                                                                           *
* HiFiEngine                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: hifiengine@gmail.com                                               *
* Web page: http://n00b.dyndns.org/HiFiEngine                               *
*                                                                           *
* HiFiEngine is only used with knowledge from the author. This software     *
* is not al
lowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the HiFiEngine license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 

#include "Windows_PreReq.h"
//#include <Windows.h>
#include "Common.h"
#include "ODEBody.h"
#include "ODEPhysicsManager.h"
#include "IGeometry.h"
#include "Dator.h"
#include "Font.h"
#include "Log.h"
#include "Root.h"
#include "Profiler.h"
#include "BaseObject.h"
#include "MeshGeometry.h"
#include "MeshObject.h"

namespace HiFi
{
	IMPLEMENT_RTTI(ODEBody,IPhysicsBody);
	//const CRTTI ODEBody::m_RTTI("ODEBody",2,&ODEObject::m_RTTI,&IPhysicsObject::m_RTTI);

	ODEBody::ODEBody() :  IPhysicsBody()
	{
		m_ODESpaceID = NULL;
		m_ODESecondarySpaceID = NULL;
		m_ODEBody = 0;
		m_Mass = 10;
		m_Center.Set(0,0,0);
		m_MassOffset = Vec3(0,0,0);
		m_AutoDisable = false;
	}

	ODEBody::~ODEBody()
	{
		if(m_ODEBody) dBodyDestroy(m_ODEBody);
	}

	void ODEBody::Update(float delta)
	{
		bool paused = Root::Get().GetPhysicsManager()->IsPaused();
		
		if(!paused)
		{
			m_Owner->SetUpdateTransform(false);
			ODEPhysicsManager::SetNodeTransformation(m_ODEBody,m_Owner);
		}
		else if(paused)
		{
			m_Owner->SetUpdateTransform(true);
			ODEPhysicsManager::SetBodyTransformation(m_ODEBody,m_Owner);
		}
	
		if(m_Debug && m_ODEBody && !((ODEPhysicsManager*)Root::Get().GetPhysicsManager())->IsRunningInOwnThread())
		{
			m_MassOffset.x = 0;
			/*if(GetAsyncKeyState('O') && GetAsyncKeyState(VK_LSHIFT))
			{
				m_MassOffset.x += 0.01;
				//if(fabs(m_MassOffset.y) < 0.01) m_MassOffset.y -= 0.01;
				//else m_MassOffset.y -= m_MassOffset.y*0.01;
			}
			else if(GetAsyncKeyState('O')) 
			{
				m_MassOffset.x += -0.01;
				//if(fabs(m_MassOffset.y) < 0.01) m_MassOffset.y += 0.01;
				//else m_MassOffset.y += (m_MassOffset.y*0.01);
			}*/


			Font::DebugPrint("Mass offset y %.2f",m_MassOffset.y);
			dMassTranslate(&m_ODEMass,m_MassOffset.x, m_MassOffset.y,m_MassOffset.z);
			/*if(GetAsyncKeyState('G') && GetAsyncKeyState(VK_LSHIFT))
			{
				m_Offset.y += 0.01;
			}
			else if(GetAsyncKeyState('G')) 
			{
				m_Offset.y -= 0.01;
			}*/
			

			float mass = GetMass();
			/*if(GetAsyncKeyState(VK_F4))
			{
				mass += mass*0.05;
				SetMass(mass);
			}
			if(GetAsyncKeyState(VK_F3))
			{

				mass -= mass*0.05;
				if(mass < 0) mass = 0.1;
				SetMass(mass);
			}*/

			//m_Engine->RegisterCommands();
			Font::DebugPrint("Mass %.2f",mass);
		}
	}



	bool ODEBody::WantsContact( dContact & contact, IPhysicsObject * other, dGeomID you, dGeomID him, bool firstTest)
	{
		BaseObject* bo = (BaseObject*)m_Owner->GetRoot();
		
		/*if(m_GeomRep == "wheel")
		{
			if(bo && !bo->IsMaster()) return false;
			//car
			float inv = 1;
			//PlayerControlObject* pco = ((PlayerControlObject*) GetRoot());//dBodyGetLinearVel( m_ODEBody);
			dReal const * vel = dBodyGetLinearVel  (m_ODEBody);
			dBodyID bid = dGeomGetBody( you );
			//dReal const * vel = dBodyGetLinearVel( bid );
			float colVel = Math::Dot(((Vec3 const &)*vel) , (Vec3 &)contact.geom.normal);
			if(bo->GetPosition().x < 0) inv = -1;
			//if( BodyIsWheel( bid, &inv ) ) 
			{
				Vec3 front;
				Vec3 up;
				//  Compute fDir1 if a wheel
				dReal const * R = dBodyGetRotation( bid );
				front = Vec3( R[2] * inv, R[6] * inv, R[10] * inv );
				up = Vec3( R[1] * inv, R[5] * inv, R[9] * inv );
				//  Set Slip2
				contact.surface.mode |= dContactSlip2 | dContactFDir1;
				float v = sqrtf( vel[0]*vel[0] + vel[1]*vel[1] + vel[2]*vel[2] );
				contact.surface.slip2 = m_TireSlip * v;
				//  Turn on Approx1
				contact.surface.mode |= dContactApprox1;
				//  re-tweak mu and mu2
				contact.surface.mu = contact.surface.mu2 = m_Friction;
				//  The theory is that it doesn't matter if "front" points "up" 
				//  because we want fdir2 to be orthogonal to "front" and contact 
				//  normal. Of course, if "front" points in the direction of the 
				//  contact, this can be a problem. In that case, choose "up".
				if( fabsf( dDot( contact.geom.normal, (dReal*) &front.x, 3 ) ) > 0.5f ) {
					((Vec3 &)contact.fdir1) = up;
				}
				else {
					((Vec3 &)contact.fdir1) = front;
				}
				m_Contact = true;
			}
			return true;
		}
		else*/
		{
			if(bo && bo->IsMaster()) return true;
			else return false;
		}
	}


	Vec3 ODEBody::GetForce()
	{
		Vec3 force(0,0,0);
		if(m_ODEBody)
		{
			const dReal *value =  dBodyGetForce(m_ODEBody);
			force.Set(value[0],value[1],value[2]);
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

	void ODEBody::AddTorque(const Vec3 &torque_vec)
	{
		if(m_ODEBody)
		{
			dBodyAddTorque(m_ODEBody, torque_vec.x,torque_vec.y,torque_vec.z);
		}
	}

	void ODEBody::SetVelocity(const Vec3 &vel)
	{
		if(m_ODEBody)
		{
			dBodySetLinearVel(m_ODEBody,vel.x,vel.y,vel.z);
		}
	}

	void ODEBody::SetAngularVelocity(const Vec3 &vel)
	{
		if(m_ODEBody)
		{
			dBodySetAngularVel(m_ODEBody,vel.x,vel.y,vel.z);
		}
	}


	Vec3 ODEBody::GetAngularVelocity()
	{
		Vec3 vel(0,0,0);
		if(m_ODEBody)
		{
			dReal const * vel_p = dBodyGetAngularVel( m_ODEBody);
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

	Vec3 ODEBody::GetTorque()
	{
		Vec3 torque(0,0,0);
		if(m_ODEBody)
		{
			const dReal *value =  dBodyGetTorque (m_ODEBody);
			torque.Set(value[0],value[1],value[2]);
		}
		return torque;
	}

	void ODEBody::Init(BaseObject* owner)
	{
		IPhysicsBody::Init(owner);
		Vec3 abs_pos;
		m_ODEBody = dBodyCreate(((ODEPhysicsManager*)Root::Get().GetPhysicsManager())->m_World);
		//From car world
		//Set the auto-disable flag of a body. If the do_auto_disable is nonzero the body will be automatically disabled when it has been idle for long enough. 
		dBodySetAutoDisableDefaults(m_ODEBody);
		if(m_AutoDisable) dBodySetAutoDisableFlag(m_ODEBody, 1);
		else dBodySetAutoDisableFlag(m_ODEBody, 0);
		//Set whether the body is influenced by the world's gravity or not. If mode is nonzero it is, if mode is zero, it isn't. Newly created bodies are always influenced by the world's gravity. 
		dBodySetGravityMode(m_ODEBody, 1);
		ODEPhysicsManager::SetBodyTransformation(m_ODEBody,m_Owner);

		//We don't want the scenenode::update function to set te transformation from parent 
		//because we set the transformation ourself from the physics body.
		m_Owner->SetUpdateTransform(false);
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
		if(m_ODESpaceID == NULL)
		{
			m_ODESpaceID = ODEPhysicsManager::m_Space;//dSimpleSpaceCreate(ODEPhysicsManager::m_Space);
		}
		return m_ODESpaceID;
	}

	dSpaceID ODEBody::GetSecondarySpace()
	{

		if(m_ODESecondarySpaceID == 0)
		{
			m_ODESecondarySpaceID = dSimpleSpaceCreate(ODEPhysicsManager::m_CollisionSpace);
		}
		return m_ODESecondarySpaceID;
	}


	
	void ODEBody::AddForce(const Vec3 &force_vec)
	{
		if(m_ODEBody)
		{
			float mass = (float) m_ODEMass.mass;
			dBodyAddForce(m_ODEBody, mass*force_vec.x,mass*force_vec.y,mass*force_vec.z);
		}
	}

	void ODEBody::SetMass(float mass)
	{
		m_Mass = mass;
		dMassAdjust(&m_ODEMass, m_Mass);
		dBodySetMass(m_ODEBody, &m_ODEMass);
	}

	Vec3 ODEBody::GetVelocity()
	{
		Vec3 vel;
		const dReal *odevel = dBodyGetLinearVel  (m_ODEBody);
		vel.Set(odevel[0],odevel[1],odevel[2]);
		return vel;
	}

	void ODEBody::SetPosition(const Vec3 &value)
	{
		if(m_ODEBody)
		{
			dBodySetPosition(m_ODEBody, value.x, value.y, value.z);
		}
	}

	void ODEBody::UpdateTransformation()
	{
		if(m_ODEBody)
		{
			ODEPhysicsManager::SetBodyTransformation(m_ODEBody,m_Owner);
		}
	}

	void ODEBody::SetRotation(const Vec3 &value)
	{
		/*ISceneNode::SetRotation(value);

		if(m_ODEBody)
		{
		Mat4 rel_mat;
		rel_mat.SetTransformation(m_Pos,m_Rot,m_Scale);

		if(m_Parent)
		{
		m_AbsoluteTransformation = rel_mat * m_Parent->GetAbsoluteTransformation();
		}
		else
		{
		m_AbsoluteTransformation = rel_mat;
		}
		dReal R[12];
		float *m = m_AbsoluteTransformation.m_Data2;
		R[0] = m[0];
		R[1] = m[4];
		R[2] = m[8];
		R[3] = 0;
		R[4] = m[1];
		R[5] = m[5];
		R[6] = m[9];
		R[7] = 0;
		R[8] = m[2];
		R[9] = m[6];
		R[10]= m[10];
		R[11] = 0;
		dBodySetRotation(m_ODEBody, R);
		}*/
	}
}
