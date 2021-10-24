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

#include <memory>

#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Math/GASSMath.h"
#include "Sim/Utils/GASSCollisionHelper.h"
#include "Plugins/ODE/ODESphereGeometryComponent.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Plugins/ODE/ODESuspensionComponent.h"
#include "Plugins/ODE/ODEBodyComponent.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"


namespace GASS
{
	ODESuspensionComponent::ODESuspensionComponent() : 
		m_Anchor (0,0,0),
		m_Axis1 (0,0,0),
		m_Axis2 (0,0,0)
		
	{

	}

	ODESuspensionComponent::~ODESuspensionComponent()
	{
		if(m_ODEJoint) dJointDestroy(m_ODEJoint);
	}

	void ODESuspensionComponent::RegisterReflection()
	{
		RegisterGetSet("MaxSteerTorque", &GASS::ODESuspensionComponent::GetMaxSteerTorque, &GASS::ODESuspensionComponent::SetMaxSteerTorque);
		RegisterGetSet("MaxDriveTorque", &GASS::ODESuspensionComponent::GetMaxDriveTorque, &GASS::ODESuspensionComponent::SetMaxDriveTorque);
		RegisterGetSet("Damping", &GASS::ODESuspensionComponent::GetDamping, &GASS::ODESuspensionComponent::SetDamping);
		RegisterGetSet("Strength", &GASS::ODESuspensionComponent::GetStrength, &GASS::ODESuspensionComponent::SetStrength);
		RegisterGetSet("HighStop", &GASS::ODESuspensionComponent::GetHighStop, &GASS::ODESuspensionComponent::SetHighStop);
		RegisterGetSet("LowStop", &GASS::ODESuspensionComponent::GetLowStop, &GASS::ODESuspensionComponent::SetLowStop);
		RegisterGetSet("SwayForce", &GASS::ODESuspensionComponent::GetSwayForce, &GASS::ODESuspensionComponent::SetSwayForce);
		RegisterGetSet("Axis1", &GASS::ODESuspensionComponent::GetAxis1, &GASS::ODESuspensionComponent::SetAxis1);
		RegisterGetSet("Axis2", &GASS::ODESuspensionComponent::GetAxis2, &GASS::ODESuspensionComponent::SetAxis2);
		RegisterGetSet("Anchor", &GASS::ODESuspensionComponent::GetAnchor, &GASS::ODESuspensionComponent::SetAnchor);
	}

	void ODESuspensionComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODESuspensionComponent::OnBodyLoaded,PhysicsBodyLoadedEvent,0));
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

			m_ODEJoint = dJointCreateHinge2(world, nullptr);
			GetSceneObject()->RegisterForMessage(REG_TMESS(ODESuspensionComponent::UpdateSwayBars,PhysicsVelocityEvent,0));

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
		Mat4 rot_mat(rot);
		ODEPhysicsSceneManager::CreateODERotationMatrix(rot_mat,ode_rot_mat);

#ifndef WIN32 //we use older ode version in linux...
		if (m_Axis1.Length() != 0)
			dJointSetHinge2Axis1(m_ODEJoint,m_Axis1.x,m_Axis1.y,m_Axis1.z);
		else
			dJointSetHinge2Axis1(m_ODEJoint,ode_rot_mat[4],ode_rot_mat[5],ode_rot_mat[6]);
		dJointSetHinge2Axis2(m_ODEJoint,    ode_rot_mat[0],    ode_rot_mat[1],    ode_rot_mat[2]);
#else
		dReal axis1[3], axis2[3];
		if (m_Axis1.Length() != 0)
		{
			axis1[0] = m_Axis1.x; axis1[1] = m_Axis1.y; axis1[2] = m_Axis1.z;
		}
		else
		{
			axis1[0] = ode_rot_mat[4]; axis1[1] = ode_rot_mat[5]; axis1[2] = ode_rot_mat[6];
		}
		axis2[0] = ode_rot_mat[0]; axis2[1] = ode_rot_mat[1]; axis2[2] = ode_rot_mat[2];
		dJointSetHinge2Axes(m_ODEJoint, axis1, axis2);
#endif
	}

	void ODESuspensionComponent::SetAnchor(const Vec3 &value)
	{
		m_Anchor = value;
		if(m_ODEJoint)
			UpdateAnchor();
	}

	void ODESuspensionComponent::UpdateAnchor()
	{
		//LocationComponentPtr location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		LocationComponentPtr location2 = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();

		//Vec3 pos_b1 = location1->GetPosition();
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
			if (m_Body1)
				m_Body1->Wake();

			dJointSetHinge2Param(m_ODEJoint,dParamVel,velocity);
		}
	}

	void ODESuspensionComponent::SetDriveVelocity(float value)
	{
		if(m_ODEJoint)
		{
			if (m_Body1)
				m_Body1->Wake();
			dJointSetHinge2Param(m_ODEJoint, dParamVel2,value);
		}
	}

	void ODESuspensionComponent::SetMaxDriveTorque(float value)
	{
		if(m_ODEJoint)
		{
			if(m_Body1)
				m_Body1->Wake();
			dJointSetHinge2Param(m_ODEJoint, dParamFMax2,value);
		}
	}


	float ODESuspensionComponent::GetMaxDriveTorque() const
	{
		if(m_ODEJoint)
		{
			return static_cast<float>(dJointGetHinge2Param(m_ODEJoint,dParamFMax2));
		}
		return 0;
	}

	void ODESuspensionComponent::SetMaxSteerTorque(float value)
	{
		if(m_ODEJoint)
		{
			if (m_Body1)
				m_Body1->Wake();

			dJointSetHinge2Param(m_ODEJoint, dParamFMax,value);
		}
	}


	float ODESuspensionComponent::GetMaxSteerTorque() const
	{
		if(m_ODEJoint)
		{
			return static_cast<float>(dJointGetHinge2Param(m_ODEJoint,dParamFMax));
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
			float angle = static_cast<float>(dJointGetHinge2Angle1(m_ODEJoint));
			float angle_rate = static_cast<float>(dJointGetHinge2Angle1Rate (m_ODEJoint));
			joint_message = std::make_shared<ODEPhysicsHingeJointEvent>(angle,angle_rate);
			if(joint_message)
				GetSceneObject()->SendImmediateEvent(joint_message);
		}
	}

	void ODESuspensionComponent::UpdateSwayBars(PhysicsVelocityEventPtr message)
	{
		//Hack to keep vehicles from flipping upside down
		if(m_SwayForce > 0)
		{
			dBodyID b1 = m_Body1->GetODEBodyComponent();
			dBodyID b2 = m_Body2->GetODEBodyComponent();

			Vec3 bodyPoint;
			Vec3 hingePoint;
			Vec3 axis2;
			Float displacement;
			dReal temp[3];
			dJointGetHinge2Anchor2( m_ODEJoint, temp);
			bodyPoint.Set(temp[0],temp[1],temp[2]);
			dJointGetHinge2Anchor( m_ODEJoint, temp);
			hingePoint.Set(temp[0],temp[1],temp[2]);
			dJointGetHinge2Axis1( m_ODEJoint, temp);
			axis2.Set(temp[0],temp[1],temp[2]);
			displacement = Vec3::Dot((hingePoint - bodyPoint) ,axis2);
			Float amt = displacement * m_SwayForce;


			ODESphereGeometryComponentPtr sphere = GetSceneObject()->GetFirstComponentByClass<ODESphereGeometryComponent>();
			Float radius = 0;
			if(sphere)
			{
				radius = sphere->GetRadius();
			}

//			GASS::ODECollisionSystemPtr ode_col_sys = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<GASS::ODECollisionSystem>();
			GeometryFlags flags =  static_cast<GeometryFlags>(GEOMETRY_FLAG_SCENE_OBJECTS | GEOMETRY_FLAG_PAGED_LOD);
			ScenePtr scene = GetSceneObject()->GetScene();
			Float height_above_ground = hingePoint.y - CollisionHelper::GetHeightAtPosition(scene, hingePoint, flags, true);
			//Float height_above_ground = ode_col_sys->GetHeight(GetSceneObject()->GetScene(),hingePoint,false);
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
	}

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






