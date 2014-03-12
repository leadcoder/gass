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

#include "Sim/GASSCommon.h"
#include "Plugins/ODE/ODESuspensionComponent.h"
#include "Plugins/ODE/ODEBodyComponent.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
//#include "Plugins/ODE/Collision/ODECollisionSystem.h"
#include "Plugins/ODE/ODESphereGeometryComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/ComponentSystem/GASSIComponentContainer.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"

#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"


namespace GASS
{
	ODESuspensionComponent::ODESuspensionComponent() : m_Body1 (NULL),
		m_Body2 (NULL),
		m_SwayForce  (0),
		m_Strength(1),
		m_Damping(2),
		m_Anchor (0,0,0),
		m_Axis1 (0,0,0),
		m_Axis2 (0,0,0),
		m_ODEJoint (0),
		m_HighStop(0),
		m_LowStop(0)
	{

	}

	ODESuspensionComponent::~ODESuspensionComponent()
	{
		if(m_ODEJoint) dJointDestroy(m_ODEJoint);
	}

	void ODESuspensionComponent::RegisterReflection()
	{
		RegisterProperty<float>("MaxSteerTorque", &GASS::ODESuspensionComponent::GetMaxSteerTorque, &GASS::ODESuspensionComponent::SetMaxSteerTorque);
		RegisterProperty<float>("MaxDriveTorque", &GASS::ODESuspensionComponent::GetMaxDriveTorque, &GASS::ODESuspensionComponent::SetMaxDriveTorque);
		RegisterProperty<float>("Damping", &GASS::ODESuspensionComponent::GetDamping, &GASS::ODESuspensionComponent::SetDamping);
		RegisterProperty<float>("Strength", &GASS::ODESuspensionComponent::GetStrength, &GASS::ODESuspensionComponent::SetStrength);
		RegisterProperty<float>("HighStop", &GASS::ODESuspensionComponent::GetHighStop, &GASS::ODESuspensionComponent::SetHighStop);
		RegisterProperty<float>("LowStop", &GASS::ODESuspensionComponent::GetLowStop, &GASS::ODESuspensionComponent::SetLowStop);
		RegisterProperty<float>("SwayForce", &GASS::ODESuspensionComponent::GetSwayForce, &GASS::ODESuspensionComponent::SetSwayForce);
		RegisterProperty<Vec3>("Axis1", &GASS::ODESuspensionComponent::GetAxis1, &GASS::ODESuspensionComponent::SetAxis1);
		RegisterProperty<Vec3>("Axis2", &GASS::ODESuspensionComponent::GetAxis2, &GASS::ODESuspensionComponent::SetAxis2);
		RegisterProperty<Vec3>("Anchor", &GASS::ODESuspensionComponent::GetAnchor, &GASS::ODESuspensionComponent::SetAnchor);
	}

	void ODESuspensionComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODESuspensionComponent::OnBodyLoaded,PhysicsBodyLoadedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODESuspensionComponent::OnDriveVelocityRequest,PhysicsSuspensionJointDriveVelocityRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODESuspensionComponent::OnSteerVelocityRequest,PhysicsSuspensionJointSteerVelocityRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODESuspensionComponent::OnMaxDriveTorqueRequest,PhysicsSuspensionJointMaxDriveTorqueRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODESuspensionComponent::OnMaxSteerTorqueRequest,PhysicsSuspensionJointMaxSteerTorqueRequest,0));
	}

	void ODESuspensionComponent::OnDriveVelocityRequest(PhysicsSuspensionJointDriveVelocityRequestPtr message)
	{
		if(m_Body1)
		{
			m_Body1->Wake();
			SetDriveVelocity(message->GetVelocity());
		}
	}

	void ODESuspensionComponent::OnMaxDriveTorqueRequest(PhysicsSuspensionJointMaxDriveTorqueRequestPtr message)
	{
		if(m_Body1)
		{
			m_Body1->Wake();
			SetMaxDriveTorque(message->GetMaxTorque());
		}
	}

	void ODESuspensionComponent::OnSteerVelocityRequest(PhysicsSuspensionJointSteerVelocityRequestPtr message)
	{
		if(m_Body1)
		{
			m_Body1->Wake();
			
			SetAngularSteerVelocity(message->GetVelocity());
		}
	}

	void ODESuspensionComponent::OnMaxSteerTorqueRequest(PhysicsSuspensionJointMaxSteerTorqueRequestPtr message)
	{
		if(m_Body1)
		{
			m_Body1->Wake();
			SetMaxSteerTorque(message->GetMaxTorque());
		}
	}
	
	void ODESuspensionComponent::OnBodyLoaded(PhysicsBodyLoadedEventPtr message)
	{
		ODEPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<ODEPhysicsSceneManager>();
		assert(scene_manager);
		m_SceneManager = scene_manager;

		CreateJoint();
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODESuspensionComponent::SendJointUpdate,PhysicsVelocityEvent,0));
	}

	void ODESuspensionComponent::CreateJoint()
	{
		dWorldID world = ODEPhysicsSceneManagerPtr(m_SceneManager)->GetWorld();

		
		m_Body1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<ODEBodyComponent>().get();
		if(m_Body1)
		{
			m_Body2 = GetSceneObject()->GetFirstComponentByClass<ODEBodyComponent>().get();
			assert(m_Body2);

			dBodyID b1 = m_Body1->GetODEBodyComponent();
			dBodyID b2 = m_Body2->GetODEBodyComponent();

			if(m_ODEJoint)
				dJointDestroy(m_ODEJoint);

			m_ODEJoint = dJointCreateHinge2(world, 0);
			//GetSceneObject()->RegisterForMessage(REG_TMESS(ODESuspensionComponent::UpdateSwayBars,PhysicsVelocityEvent,0));

			dJointAttach(m_ODEJoint, b1,b2);
			UpdateAnchor();
			UpdateJointAxis();
			UpdateLimits();
			//SetSteerForce(m_JointForce);
			//SetDriveForce(m_JointForce);
			SetDriveVelocity(0);
			SetAngularSteerVelocity(0);
			UpdateSuspension();
		}
	}

	void ODESuspensionComponent::SetAxis1(const Vec3 &axis)
	{
		m_Axis1 = axis;
		if(m_ODEJoint)
			UpdateJointAxis();
	}

	void ODESuspensionComponent::SetAxis2(const Vec3 &axis)
	{
		m_Axis2 = axis;
		if(m_ODEJoint)
			UpdateJointAxis();
	}

	void ODESuspensionComponent::UpdateJointAxis()
	{
		LocationComponentPtr location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		Quaternion rot = location1->GetRotation();

		dReal ode_rot_mat[12];
		Mat4 rot_mat;
		rot_mat.Identity();
		rot.ToRotationMatrix(rot_mat);
		ODEPhysicsSceneManager::CreateODERotationMatrix(rot_mat,ode_rot_mat);

		if (m_Axis1.Length() != 0)
			dJointSetHinge2Axis1(m_ODEJoint,m_Axis1.x,m_Axis1.y,m_Axis1.z);
		else
			dJointSetHinge2Axis1(m_ODEJoint,ode_rot_mat[4],ode_rot_mat[5],ode_rot_mat[6]);
		dJointSetHinge2Axis2(m_ODEJoint,    ode_rot_mat[0],    ode_rot_mat[1],    ode_rot_mat[2]);
	}

	void ODESuspensionComponent::SetAnchor(const Vec3 &value)
	{
		m_Anchor = value;
		if(m_ODEJoint)
			UpdateAnchor();
	}

	void ODESuspensionComponent::UpdateAnchor()
	{
		LocationComponentPtr location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		LocationComponentPtr location2 = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();

		Vec3 pos_b1 = location1->GetPosition();
		Vec3 pos_b2 = location2->GetPosition();

		Vec3  world_anchor;

	
		world_anchor = m_Anchor + pos_b2;
		dJointSetHinge2Anchor(m_ODEJoint,world_anchor.x,world_anchor.y,world_anchor.z);
			
	}

	void ODESuspensionComponent::SetLowStop(float value)
	{
		m_LowStop = value;
		UpdateLimits();
	}

	void ODESuspensionComponent::SetHighStop(float value)
	{
		m_HighStop = value;
		UpdateLimits();
	}


	void ODESuspensionComponent::UpdateLimits()
	{
		if(m_ODEJoint)
		{
			dJointSetHinge2Param(m_ODEJoint,dParamLoStop,m_LowStop);
			dJointSetHinge2Param(m_ODEJoint,dParamHiStop,m_HighStop);
			dJointSetHinge2Param(m_ODEJoint,dParamLoStop,m_LowStop);
			dJointSetHinge2Param(m_ODEJoint,dParamHiStop,m_HighStop);
		}
	}

	void ODESuspensionComponent::SetAngularSteerVelocity(float velocity)
	{
		if(m_ODEJoint)
		{
			dJointSetHinge2Param(m_ODEJoint,dParamVel,velocity);
		}
	}

	void ODESuspensionComponent::SetDriveVelocity(float value)
	{
		if(m_ODEJoint)
		{
			dJointSetHinge2Param(m_ODEJoint, dParamVel2,value);
		}
	}

	void ODESuspensionComponent::SetMaxDriveTorque(float value)
	{
		if(m_ODEJoint)
		{
			dJointSetHinge2Param(m_ODEJoint, dParamFMax2,value);
		}
	}


	float ODESuspensionComponent::GetMaxDriveTorque() const
	{
		if(m_ODEJoint)
		{
			return dJointGetHinge2Param(m_ODEJoint,dParamFMax2);
		}
		return 0;
	}

	void ODESuspensionComponent::SetMaxSteerTorque(float value)
	{
		if(m_ODEJoint)
		{
			dJointSetHinge2Param(m_ODEJoint, dParamFMax,value);
		}
	}


	float ODESuspensionComponent::GetMaxSteerTorque() const 
	{
		if(m_ODEJoint)
		{
			return dJointGetHinge2Param(m_ODEJoint,dParamFMax);
		}
		return 0;
	}

	void ODESuspensionComponent::SetDamping(float value)
	{
		m_Damping= value;
		UpdateSuspension();
	}

	void ODESuspensionComponent::SetStrength(float value)
	{
		m_Strength = value;
		UpdateSuspension();
	}

	void ODESuspensionComponent::UpdateSuspension()
	{
		if(m_ODEJoint)
		{
			float s = 1.0f;
			float suspensionERP = s*m_Strength / (s*m_Strength + m_Damping);
			float suspensionCFM = 1.0f / (s*m_Strength + m_Damping);
			dJointSetHinge2Param (m_ODEJoint,dParamSuspensionERP, suspensionERP);
			dJointSetHinge2Param (m_ODEJoint,dParamSuspensionCFM, suspensionCFM);
		}
	}

	void ODESuspensionComponent::SetSwayForce(float value)
	{
		m_SwayForce = value;
	}


	void ODESuspensionComponent::SendJointUpdate(PhysicsVelocityEventPtr message)
	{
		ODEPhysicsHingeJointEventPtr joint_message;
		if(m_ODEJoint)
		{
			float angle = dJointGetHinge2Angle1(m_ODEJoint);
			float angle_rate = dJointGetHinge2Angle1Rate (m_ODEJoint);
			joint_message = ODEPhysicsHingeJointEventPtr(new ODEPhysicsHingeJointEvent(angle,angle_rate));
			if(joint_message)
				GetSceneObject()->SendImmediateEvent(joint_message);
		}
	}

	/*void ODESuspensionComponent::UpdateSwayBars(PhysicsVelocityEventPtr message)
	{
		//Hack to keep vehicles from flipping upside down
		if(m_SwayForce > 0)
		{
			dBodyID b1 = m_Body1->GetODEBodyComponent();
			dBodyID b2 = m_Body2->GetODEBodyComponent();

			Vec3 bodyPoint;
			Vec3 hingePoint;
			Vec3 axis2;
			float displacement;
			dReal temp[3];
			dJointGetHinge2Anchor2( m_ODEJoint, temp);
			bodyPoint.Set(temp[0],temp[1],temp[2]);
			dJointGetHinge2Anchor( m_ODEJoint, temp);
			hingePoint.Set(temp[0],temp[1],temp[2]);
			dJointGetHinge2Axis1( m_ODEJoint, temp);
			axis2.Set(temp[0],temp[1],temp[2]);
			displacement = Math::Dot((hingePoint - bodyPoint) ,axis2);
			float amt = displacement * m_SwayForce;


			ODESphereGeometryComponentPtr sphere = GetSceneObject()->GetFirstComponentByClass<ODESphereGeometryComponent>();
			Float radius = 0;
			if(sphere) 
			{
				radius = sphere->GetRadius();
			}

			GASS::ODECollisionSystemPtr ode_col_sys = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<GASS::ODECollisionSystem>();
			Float height_above_ground = ode_col_sys->GetHeight(GetSceneObject()->GetScene(),hingePoint,false);
			//std::cout << "height" <<  height_above_ground << "\n";

			//get terrain height

			if( displacement > 0 && height_above_ground < radius*2) 
			{
				if( amt > 15 )
				{
					amt = 15;
				}
				dBodyAddForce( b2, -axis2.x * amt, -axis2.y * amt, -axis2.z * amt );
				dReal const * wp = dBodyGetPosition( b2 );

				dBodyAddForceAtPos( b1, axis2.x*amt, axis2.y*amt, axis2.z*amt, wp[0], wp[1], wp[2] );
				//dBodyAddForce( wheelBody_[ix^1], axis.x * amt, axis.y * amt, axis.z * amt );
				wp = dBodyGetPosition( b2 );
				dBodyAddForceAtPos( b1, -axis2.x*amt, -axis2.y*amt, -axis2.z*amt, wp[0], wp[1], wp[2] );
			}
		}
	}*/

	void ODESuspensionComponent::JointCorrectHinge2()
	{

		// A fix to correct wheel bending, it works by turning a hinge2's axis 1
		// back within the specified dParamLoStop and dParamHiStop limits.
		// Call this function in EVERY step with each car wheel hinge2 joint.
		// By: Mike Bailey - Radish Works (mike@radishworks.com)

		// Just checking for valid passed params
		//assert(joint);
		//assert(joint->vtable == &__dhinge2_vtable,"joint is not a hinge2");

		//if(joint->node[1].body)
		{
			// Get the current turn angle of the joint
			dReal rAng=dJointGetHinge2Angle1(m_ODEJoint);

			// Get the high and low turning stops for the m_ODEJoint
			dReal rLoStop = dJointGetHinge2Param(m_ODEJoint,dParamLoStop);
			dReal rHiStop = dJointGetHinge2Param(m_ODEJoint,dParamHiStop);

			// Get the (transformed) axis the wheel left/right turn.
			dVector3 Axis;
			dJointGetHinge2Axis1(m_ODEJoint,Axis);

			// Calculate the angle the wheel has be turned PAST its limit.
			if(rAng<rLoStop)
			{
				rAng=rAng-rLoStop;
			}
			else if(rAng>rHiStop)
			{
				rAng=rAng-rHiStop;
			}
			else
			{
				rAng=0.0;
			}

			if(rAng)
			{
				// Here's the fix!
				// If there's a value in rAng, this angle is the angle PAST the wheel stops
				// Create a matrix the that's the inverse of the turn angle we DON'T want.
				dMatrix3 matODE;
				dRFromAxisAndAngle(matODE,Axis[0],Axis[1],Axis[2],rAng);

				// Multiply this matrix by the wheels body.
				// The wheel body is now in the range we want.
				// This works quite well on a cars un-turning rear wheels.

				//const dReal * dBodyGetPosition(dBodyID);
				dBodyID body = m_Body2->GetODEBodyComponent();
				const dReal *body_rot =  dBodyGetRotation(body);

				dReal res_rot[3][3];

				dMultiply0(&res_rot[0][0],matODE,body_rot,3,3,3);
				dBodySetRotation(body, &res_rot[0][0]);
			}
		}
	}

}






