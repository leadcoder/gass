/****************************************************************************
*                                                                           *
* HiFiEngine                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: hifiengine@gmail.com                                               *
* Web page: http://n00b.dyndns.org/HiFiEngine                               *
*                                                                           *
* HiFiEngine is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the HiFiEngine license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 

#if !defined ODE_BODY_H
#define ODE_BODY_H

#include "IPhysicsBody.h"
#include "Root.h"
#include "ODEJoint.h"
#include <ode/ode.h>
namespace HiFi
{
	class HFEPluginExport ODEBody : public IPhysicsBody
	{
		friend class ODEJoint;
		friend class ODEGeometry;
		DECLARE_RTTI;
		PHYSICS_DUPLICATE(ODEBody,"PhysicsBody");
	public:
		ODEBody();
		virtual ~ODEBody();
		virtual void Init(BaseObject* owner);
		virtual void Update(float delta);
		virtual void SetPosition(const Vec3 &value);
		virtual void SetRotation(const Vec3 &value);
		virtual void UpdateTransformation();
		virtual void SetTorque(const Vec3 &torque);
		virtual Vec3 GetTorque();
		virtual void SetVelocity(const Vec3 &vel);
		virtual Vec3 GetVelocity();
		virtual void SetAngularVelocity(const Vec3 &vel);
		virtual Vec3 GetAngularVelocity();
		virtual float GetMass(){return m_Mass;}
		virtual void SetMass(float mass);
		virtual void Enable();
		virtual void Disable();
		virtual bool IsEnabled();
		virtual void AddForce(const Vec3 &force_vec);
		virtual void SetForce(const Vec3 &force);
		virtual Vec3 GetForce();
		virtual void AddTorque(const Vec3 &torque_vec);
		virtual bool WantsContact( dContact & contact, IPhysicsObject * other, dGeomID you, dGeomID him, bool firstTest);
	protected:
		dBodyID GetODEBody(){return m_ODEBody;}
		void SetODEMass(dMass mass){m_ODEMass = mass;}
		void DampenBody( dBodyID body, float vScale, float aScale );
		dSpaceID GetSpace();
		dSpaceID GetSecondarySpace();
	protected:
		dBodyID m_ODEBody;
		dSpaceID m_ODESpaceID;
		dSpaceID m_ODESecondarySpaceID;
		dMass m_ODEMass;

		float m_Density;
		bool m_DynamicsEnabled;
		bool m_ReenableIfInteractedWith;
		bool m_CollisionEnabled;
		float m_BounceCoeffRestitution;
		float m_BounceVelocityThreshold;
		float m_LinearVelDisableThreshold;
		float m_AngularVelDisableThreshold;
		float m_DisableTime;
		float m_DisableTimeEnd;
		float m_BodyMass;
		Vec3 m_Center;
		Vec3 m_MassOffset;
		bool m_MassInitialized;
		bool m_Initialized;
		Mat4 m_LastTrans;
	};
}
#endif 
