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

#include "Sim/Common.h"
#include "Plugins/ODE/ODEJoint.h"
#include "Plugins/ODE/ODEBody.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/ComponentSystem/IComponentContainer.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include <boost/bind.hpp>
//#include "Main/SceneNodes/BaseObject.h"
//#include "Main/SceneNodes/TransformationNode.h"
//#include "Main/Helpers/Font.h"

namespace GASS
{
	ODEJoint::ODEJoint() : m_Body1 (NULL),
		m_Body2 (NULL),
		m_JointForce (0),
		m_SwayForce  (0),
		m_Strength(1),
		m_Damping(2),
		m_Anchor (0,0,0),
		m_Axis1 (0,0,0),
		m_Axis2 (0,0,0),
		m_ODEJoint (0),
		m_HighStop(0),
		m_LowStop(0),
		m_Type(BALL_JOINT),
		m_SceneManager(NULL)
	{
	}

	ODEJoint::~ODEJoint()
	{
		if(m_ODEJoint) dJointDestroy(m_ODEJoint);
	}

	void ODEJoint::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("ODEJoint",new Creator<ODEJoint, IComponent>);

		RegisterProperty<std::string>("Type", &GASS::ODEJoint::GetType, &GASS::ODEJoint::SetType);
		RegisterProperty<std::string>("Body1Name", &GASS::ODEJoint::GetBody1Name, &GASS::ODEJoint::SetBody1Name);
		RegisterProperty<std::string>("Body2Name", &GASS::ODEJoint::GetBody2Name, &GASS::ODEJoint::SetBody2Name);
		RegisterProperty<float>("Axis1Force", &GASS::ODEJoint::GetAxis1Force, &GASS::ODEJoint::SetAxis1Force);
		RegisterProperty<float>("Axis2Force", &GASS::ODEJoint::GetAxis2Force, &GASS::ODEJoint::SetAxis2Force);
		RegisterProperty<float>("Damping", &GASS::ODEJoint::GetDamping, &GASS::ODEJoint::SetDamping);
		RegisterProperty<float>("Strength", &GASS::ODEJoint::GetStrength, &GASS::ODEJoint::SetStrength);
		RegisterProperty<float>("HighStop", &GASS::ODEJoint::GetHighStop, &GASS::ODEJoint::SetHighStop);
		RegisterProperty<float>("LowStop", &GASS::ODEJoint::GetLowStop, &GASS::ODEJoint::SetLowStop);
		RegisterProperty<float>("SwayForce", &GASS::ODEJoint::GetSwayForce, &GASS::ODEJoint::SetSwayForce);
		RegisterProperty<Vec3>("Axis1", &GASS::ODEJoint::GetAxis1, &GASS::ODEJoint::SetAxis1);
		RegisterProperty<Vec3>("Axis2", &GASS::ODEJoint::GetAxis2, &GASS::ODEJoint::SetAxis2);
		RegisterProperty<Vec3>("Anchor", &GASS::ODEJoint::GetAnchor, &GASS::ODEJoint::SetAnchor);
	}

	std::string ODEJoint::GetType()const
	{
		switch(m_Type)
		{
		case BALL_JOINT:
			return "ball";
		case HINGE_JOINT:
			return "hinge";
		case UNIVERSAL_JOINT:
			return "universal";
		case SLIDER_JOINT:
			return "slider";
		case SUSPENSION_JOINT:
			return "suspension";
		default:
			return "unknown joint type";
		}
	}

	void ODEJoint::SetType(const std::string &type)
	{
		bool recreate = false;
		if(type != GetType())
			recreate = true;
		std::string jointTypeNameLowerCase = Misc::ToLower(type);
		if(jointTypeNameLowerCase.compare("ball") == 0)
		{
			m_Type = BALL_JOINT;
		}
		//else if(strcmpi(m_JointTypeName.c_str(), "Hinge")== 0)
		else if(jointTypeNameLowerCase.compare("hinge")== 0)
		{
			m_Type = HINGE_JOINT;
		}
		else if(jointTypeNameLowerCase.compare("universal") == 0)
		{
			m_Type = UNIVERSAL_JOINT;
		}
		else if(jointTypeNameLowerCase.compare("slider") == 0)
		{
			m_Type = SLIDER_JOINT;
		}
		else if(jointTypeNameLowerCase.compare("suspension")== 0)
		{
			m_Type = SUSPENSION_JOINT;
		}
		if(m_ODEJoint && recreate) //recreate if we have changed type
			CreateJoint();
	}


	void ODEJoint::OnCreate()
	{
		REGISTER_OBJECT_MESSAGE_CLASS(ODEJoint::OnLoad,LoadPhysicsComponentsMessage,0);
		REGISTER_OBJECT_MESSAGE_CLASS(ODEJoint::OnParameterMessage,PhysicsJointMessage,0);
	}


	void ODEJoint::OnParameterMessage(PhysicsJointMessagePtr message)
	{
		PhysicsJointMessage::PhysicsJointParameterType type = message->GetParameter();

		float value = message->GetValue();

		//wake body!!

		m_Body1->Enable();
		switch(type)
		{
		case PhysicsJointMessage::AXIS1_VELOCITY:
			{
				SetAxis1Vel(value);
			}
			break;
		case PhysicsJointMessage::AXIS2_VELOCITY:
			{
				SetAxis2Vel(value);
			}
			break;
		case PhysicsJointMessage::AXIS1_FORCE:
			{
				SetAxis1Force(value);

			}
			break;
		case PhysicsJointMessage::AXIS2_FORCE:
			{
				SetAxis2Force(value);
			}
			break;
		}
	}

	void ODEJoint::OnLoad(LoadPhysicsComponentsMessagePtr message)
	{
		m_SceneManager = static_cast<ODEPhysicsSceneManager*>(message->GetPhysicsSceneManager());
		assert(m_SceneManager);

		CreateJoint();
	}

	void ODEJoint::CreateJoint()
	{
		dWorldID world = m_SceneManager->GetWorld();

		m_Body1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponent<ODEBody>().get();
		m_Body2 = GetSceneObject()->GetFirstComponent<ODEBody>().get();

		dBodyID b1 = m_Body1->GetODEBody();
		dBodyID b2 = m_Body2->GetODEBody();

		if(m_ODEJoint)
			dJointDestroy(m_ODEJoint);

		switch(m_Type)
		{
		case BALL_JOINT:
			m_ODEJoint = dJointCreateBall(world,0);
			break;
		case SLIDER_JOINT:
			m_ODEJoint = dJointCreateSlider(world,0);
			break;
		case HINGE_JOINT:
			m_ODEJoint = dJointCreateHinge(world,0);
			GetSceneObject()->RegisterForMessage(OBJECT_NM_PHYSICS_VELOCITY, TYPED_MESSAGE_FUNC(ODEJoint::SendJointUpdate,VelocityNotifyMessage));
			break;
		case UNIVERSAL_JOINT:
			m_ODEJoint = dJointCreateUniversal(world,0);
			break;
		case SUSPENSION_JOINT:
			m_ODEJoint = dJointCreateHinge2(world, 0);
			//Update suspension
			GetSceneObject()->RegisterForMessage(OBJECT_NM_PHYSICS_VELOCITY, TYPED_MESSAGE_FUNC(ODEJoint::UpdateSwayBars,VelocityNotifyMessage));
			break;
		}

		dJointAttach(m_ODEJoint, b1,b2);
		UpdateAnchor();
		UpdateJointAxis();
		UpdateLimits();
		SetAxis1Force(m_JointForce);
		SetAxis2Force(m_JointForce);
		SetAxis1Vel(0);
		SetAxis2Vel(0);
		UpdateSuspension();

	}

	void ODEJoint::SetAxis1(const Vec3 &axis)
	{
		m_Axis1 = axis;
		if(m_ODEJoint)
			UpdateJointAxis();
	}

	void ODEJoint::SetAxis2(const Vec3 &axis)
	{
		m_Axis2 = axis;
		if(m_ODEJoint)
			UpdateJointAxis();
	}

	void ODEJoint::UpdateJointAxis()
	{
		ILocationComponent *location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponent<ILocationComponent>().get();
		ILocationComponent *location2 = GetSceneObject()->GetFirstComponent<ILocationComponent>().get();

		Quaternion rot = location1->GetRotation();

		dReal ode_rot_mat[12];
		Mat4 rot_mat;
		rot_mat.Identity();
		rot.ToRotationMatrix(rot_mat);
		ODEPhysicsSceneManager::CreateODERotationMatrix(rot_mat,ode_rot_mat);

		Vec3 pos_b1 = location1->GetPosition();
		Vec3 pos_b2 = location2->GetPosition();

		//Vec3 pos_b1(p1[0],p1[1],p1[2]);
		//Vec3 pos_b2(p2[0],p2[1],p2[2]);

		switch(m_Type)
		{
		case BALL_JOINT:
			break;
		case SLIDER_JOINT:
			if (m_Axis1.Length() != 0)
				dJointSetSliderAxis(m_ODEJoint,m_Axis1.x,m_Axis1.y,m_Axis1.z);
			else
			{
				Vec3 axis = pos_b2 - pos_b1;
				axis.Normalize();
				dJointSetSliderAxis(m_ODEJoint,axis.x,axis.y,axis.z);
			}
			break;
		case HINGE_JOINT:
			if (m_Axis1.Length() != 0)
				dJointSetHingeAxis(m_ODEJoint,m_Axis1.x,m_Axis1.y,m_Axis1.z);
			else
				dJointSetHingeAxis(m_ODEJoint,ode_rot_mat[4],ode_rot_mat[5],ode_rot_mat[6]);
			break;
		case UNIVERSAL_JOINT:
			if (m_Axis1.Length() != 0)
				dJointSetUniversalAxis1(m_ODEJoint,m_Axis1.x,m_Axis1.y,m_Axis1.z);
			else
				dJointSetUniversalAxis1(m_ODEJoint,ode_rot_mat[4],ode_rot_mat[5],ode_rot_mat[6]);
			if (m_Axis2.Length() != 0)
				dJointSetUniversalAxis2(m_ODEJoint,m_Axis2.x,m_Axis2.y,m_Axis2.z);
			else
				dJointSetUniversalAxis2(m_ODEJoint,-ode_rot_mat[6],ode_rot_mat[5],ode_rot_mat[4]); //rotate 90 in xz-plane
			break;
		case SUSPENSION_JOINT:
			if (m_Axis1.Length() != 0)
				dJointSetHinge2Axis1(m_ODEJoint,m_Axis1.x,m_Axis1.y,m_Axis1.z);
			else
				dJointSetHinge2Axis1(m_ODEJoint,ode_rot_mat[4],ode_rot_mat[5],ode_rot_mat[6]);
			dJointSetHinge2Axis2(m_ODEJoint,    ode_rot_mat[0],    ode_rot_mat[1],    ode_rot_mat[2]);
			break;
		}
	}

	void ODEJoint::SetAnchor(const Vec3 &value)
	{
		m_Anchor = value;
		if(m_ODEJoint)
			UpdateAnchor();
	}

	void ODEJoint::UpdateAnchor()
	{
		ILocationComponent *location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponent<ILocationComponent>().get();
		ILocationComponent *location2 = GetSceneObject()->GetFirstComponent<ILocationComponent>().get();

		Vec3 pos_b1 = location1->GetPosition();
		Vec3 pos_b2 = location2->GetPosition();

		Vec3  world_anchor;

		if(m_ODEJoint)
		{
			switch(m_Type)
			{
			case BALL_JOINT:
				world_anchor = m_Anchor + pos_b2;
				dJointSetBallAnchor(m_ODEJoint,world_anchor.x,world_anchor.y,world_anchor.z);
				break;
			case SLIDER_JOINT:
				break;
			case HINGE_JOINT:
				world_anchor = m_Anchor + pos_b2;
				dJointSetHingeAnchor(m_ODEJoint,world_anchor.x,world_anchor.y,world_anchor.z);
				break;
			case UNIVERSAL_JOINT:
				world_anchor = m_Anchor + pos_b2;
				dJointSetUniversalAnchor(m_ODEJoint,world_anchor.x,world_anchor.y,world_anchor.z);
				break;
			case SUSPENSION_JOINT:
				world_anchor = m_Anchor + pos_b2;
				dJointSetHinge2Anchor(m_ODEJoint,world_anchor.x,world_anchor.y,world_anchor.z);
				break;
			}
		}
	}

	void ODEJoint::SetLowStop(float value)
	{
		m_LowStop = value;
		UpdateLimits();
	}

	void ODEJoint::SetHighStop(float value)
	{
		m_HighStop = value;
		UpdateLimits();
	}


	void ODEJoint::UpdateLimits()
	{
		if(m_ODEJoint)
		{
			switch(m_Type)
			{
			case BALL_JOINT:
				break;
			case SLIDER_JOINT:
				dJointSetSliderParam(m_ODEJoint, dParamHiStop, m_HighStop);
				dJointSetSliderParam(m_ODEJoint, dParamLoStop, m_LowStop);
				break;
			case HINGE_JOINT:
				dJointSetHingeParam(m_ODEJoint, dParamHiStop, m_HighStop);
				dJointSetHingeParam(m_ODEJoint, dParamLoStop, m_LowStop);
				break;
			case UNIVERSAL_JOINT:
				break;
			case SUSPENSION_JOINT:
				dJointSetHinge2Param(m_ODEJoint,dParamLoStop,m_LowStop);
				dJointSetHinge2Param(m_ODEJoint,dParamHiStop,m_HighStop);
				dJointSetHinge2Param(m_ODEJoint,dParamLoStop,m_LowStop);
				dJointSetHinge2Param(m_ODEJoint,dParamHiStop,m_HighStop);
				break;
			}
		}
	}

	/*float ODEJoint::GetAngle()
	{
		switch(m_Type)
		{
		case BALL_JOINT:
			return 0.0;
			break;
		case SLIDER_JOINT:
			return dJointGetSliderPosition(m_ODEJoint);
			break;
		case HINGE_JOINT:
			return dJointGetHingeAngle(m_ODEJoint);
			break;
		case SUSPENSION_JOINT:
			return dJointGetHinge2Angle1(m_ODEJoint);
			break;
		}
		return 0.0;
	}*/

	/*float ODEJoint::GetAngleRate()
	{
		switch(m_Type)
		{
		case BALL_JOINT:
			return 0.0;
			break;
		case SLIDER_JOINT:
			return dJointGetSliderPositionRate(m_ODEJoint);
			break;
		case HINGE_JOINT:
			return dJointGetHingeAngleRate(m_ODEJoint);
			break;
		case UNIVERSAL_JOINT:
			return dJointGetUniversalAngle1Rate(m_ODEJoint);
			break;
		case SUSPENSION_JOINT:
			return dJointGetHinge2Angle1Rate(m_ODEJoint);
			break;
		}
		return 0.0;
	}*/

	void ODEJoint::SetAxis1Vel(float velocity)
	{
		if(m_ODEJoint)
		{
			switch(m_Type)
			{
			case SLIDER_JOINT:
				dJointSetSliderParam(m_ODEJoint,dParamVel,velocity);
				break;
			case HINGE_JOINT:
				dJointSetHingeParam(m_ODEJoint,dParamVel,velocity);
				break;
			case UNIVERSAL_JOINT:
				dJointSetUniversalParam(m_ODEJoint,dParamVel,velocity);
				break;
			case SUSPENSION_JOINT:
				dJointSetHinge2Param(m_ODEJoint,dParamVel,velocity);
				break;
			}
		}
	}

	void ODEJoint::SetAxis2Vel(float value)
	{
		if(m_ODEJoint)
		{
			switch(m_Type)
			{
			case UNIVERSAL_JOINT:
				dJointSetUniversalParam(m_ODEJoint, dParamVel2,value);
				break;
			case SUSPENSION_JOINT:
				dJointSetHinge2Param(m_ODEJoint, dParamVel2,value);
				break;
			}
		}
	}

	void ODEJoint::SetAxis2Force(float value)
	{
		if(m_ODEJoint)
		{
			switch(m_Type)
			{
			case UNIVERSAL_JOINT:
				dJointSetUniversalParam(m_ODEJoint, dParamFMax2,value);
				break;
			case SUSPENSION_JOINT:
				dJointSetHinge2Param(m_ODEJoint, dParamFMax2,value);
				break;
			}
		}
	}


	float ODEJoint::GetAxis2Force() const
	{
		if(m_ODEJoint)
		{
			switch(m_Type)
			{
			case UNIVERSAL_JOINT:
				return dJointGetUniversalParam(m_ODEJoint, dParamFMax2);
				break;
			case SUSPENSION_JOINT:
				return dJointGetHinge2Param(m_ODEJoint,dParamFMax2);
				break;
			}
		}
		return 0;
	}

	void ODEJoint::SetAxis1Force(float value)
	{
		m_JointForce = value;
		if(m_ODEJoint)
		{
			switch(m_Type)
			{
			case SLIDER_JOINT:
				dJointSetSliderParam(m_ODEJoint, dParamFMax,value);
				break;
			case HINGE_JOINT:
				dJointSetHingeParam(m_ODEJoint, dParamFMax,value);
				break;
			case UNIVERSAL_JOINT:
				dJointSetUniversalParam(m_ODEJoint, dParamFMax,value);
				break;
			case SUSPENSION_JOINT:
				dJointSetHinge2Param(m_ODEJoint, dParamFMax,value);
				break;
			}
		}
	}


	void ODEJoint::SetDamping(float value)
	{
		m_Damping= value;
		UpdateSuspension();
	}

	void ODEJoint::SetStrength(float value)
	{
		m_Strength = value;
		UpdateSuspension();
	}

	void ODEJoint::UpdateSuspension()
	{
		if(m_ODEJoint && m_Type == SUSPENSION_JOINT)
		{
			float s = 1.0f;
			float suspensionERP = s*m_Strength / (s*m_Strength + m_Damping);
			float suspensionCFM = 1.0f / (s*m_Strength + m_Damping);
			dJointSetHinge2Param (m_ODEJoint,dParamSuspensionERP, suspensionERP);
			dJointSetHinge2Param (m_ODEJoint,dParamSuspensionCFM, suspensionCFM);
		}
	}

	void ODEJoint::SetSwayForce(float value)
	{
		m_SwayForce = value;
	}

/*	float ODEJoint::GetAxis2Vel()
	{
		return dJointGetHinge2Angle2Rate(m_ODEJoint);
	}*/

	void ODEJoint::Enable()
	{
		// no disable for joints
	}

	void ODEJoint::Disable()
	{
		// no enable for joints
	}

	void ODEJoint::SendJointUpdate(VelocityNotifyMessagePtr message)
	{
		MessagePtr joint_message;
		if(m_ODEJoint)
		{
			switch(m_Type)
			{
			case SLIDER_JOINT:
				break;
			case HINGE_JOINT:
				{
				float angle = dJointGetHingeAngle(m_ODEJoint);
				float angle_rate = dJointGetHingeAngleRate(m_ODEJoint);
				joint_message = HingeJointNotifyMessagePtr(new HingeJointNotifyMessage(angle,angle_rate));
				break;
				}
			case UNIVERSAL_JOINT:
				break;
			case SUSPENSION_JOINT:
				break;
			}
			if(joint_message)
				GetSceneObject()->PostMessage(joint_message);
		}
	}

	void ODEJoint::UpdateSwayBars(VelocityNotifyMessagePtr message)
	{
		//Hack to keep vehicles from flipping upside down
		if(m_SwayForce > 0)
		{
			dBodyID b1 = m_Body1->GetODEBody();
			dBodyID b2 = m_Body2->GetODEBody();

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
			if( displacement > 0 ) 
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

	void ODEJoint::JointCorrectHinge2()
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
				dBodyID body = m_Body2->GetODEBody();
				const dReal *body_rot =  dBodyGetRotation(body);

				dReal res_rot[3][3];

				dMultiply0(&res_rot[0][0],matODE,body_rot,3,3,3);
				dBodySetRotation(body, &res_rot[0][0]);
			}
		}
	}

}






