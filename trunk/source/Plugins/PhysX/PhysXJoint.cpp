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

#include "Plugins/PhysX/PhysXJoint.h"
#include "Plugins/PhysX/PhysXBodyComponent.h"
#include "Plugins/PhysX/PhysXPhysicsSceneManager.h"
#include "Core/Math/AABox.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponentContainer.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include <boost/bind.hpp>


namespace GASS
{
	PhysXJoint::PhysXJoint() : m_SceneManager(NULL), 
		m_JointForce1 (0),
		m_JointForce2 (0),
		m_SwayForce  (0),
		m_Strength(1),
		m_Damping(2),
		m_Anchor (0,0,0),
		m_Axis1 (0,0,0),
		m_Axis2 (0,0,0),
		m_PhysXJoint (0),
		m_HighStop(0),
		m_LowStop(0),
		m_Type(BALL_JOINT),
		m_AxisActor(NULL)
	{

	}

	PhysXJoint::~PhysXJoint()
	{
		
	}

	void PhysXJoint::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysXJoint",new Creator<PhysXJoint, IComponent>);

		RegisterProperty<std::string>("Type", &GASS::PhysXJoint::GetType, &GASS::PhysXJoint::SetType);
		RegisterProperty<std::string>("Body1Name", &GASS::PhysXJoint::GetBody1Name, &GASS::PhysXJoint::SetBody1Name);
		RegisterProperty<std::string>("Body2Name", &GASS::PhysXJoint::GetBody2Name, &GASS::PhysXJoint::SetBody2Name);
		//RegisterProperty<float>("Axis1Force", &GASS::PhysXJoint::GetAxis1Force, &GASS::PhysXJoint::SetAxis1Force);
		//RegisterProperty<float>("Axis2Force", &GASS::PhysXJoint::GetAxis2Force, &GASS::PhysXJoint::SetAxis2Force);
		//RegisterProperty<float>("HighStop", &GASS::PhysXJoint::GetHighStop, &GASS::PhysXJoint::SetHighStop);
		//RegisterProperty<float>("LowStop", &GASS::PhysXJoint::GetLowStop, &GASS::PhysXJoint::SetLowStop);
		//RegisterProperty<float>("SwayForce", &GASS::PhysXJoint::GetSwayForce, &GASS::PhysXJoint::SetSwayForce);
		//RegisterProperty<Vec3>("Axis1", &GASS::PhysXJoint::GetAxis1, &GASS::PhysXJoint::SetAxis1);
		//RegisterProperty<Vec3>("Axis2", &GASS::PhysXJoint::GetAxis2, &GASS::PhysXJoint::SetAxis2);
		//RegisterProperty<Vec3>("Anchor", &GASS::PhysXJoint::GetAnchor, &GASS::PhysXJoint::SetAnchor);
		
	}

	void PhysXJoint::OnCreate()
	{

		REGISTER_OBJECT_MESSAGE_CLASS(PhysXJoint::OnLoad,LoadPhysicsComponentsMessage,2);
		REGISTER_OBJECT_MESSAGE_CLASS(PhysXJoint::OnParameterMessage,PhysicsJointMessage,0);
	}

	std::string PhysXJoint::GetType()const
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
		default:
			return "unknown joint type";
		}
	}


	void PhysXJoint::SetType(const std::string &type)
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
		if(m_PhysXJoint && recreate) //recreate if we have changed type
			CreateJoint();
	}

	void PhysXJoint::OnParameterMessage(PhysicsJointMessagePtr message)
	{
		PhysicsJointMessage::PhysicsJointParameterType type = message->GetParameter();

		float value = message->GetValue();

		//wake body!!

		//m_Body1->Enable();
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

	void PhysXJoint::OnLoad(LoadPhysicsComponentsMessagePtr message)
	{
		m_SceneManager = static_cast<PhysXPhysicsSceneManager*>(message->GetPhysicsSceneManager());
		assert(m_SceneManager);
		CreateJoint();
	}

	void PhysXJoint::CreateJoint()
	{
		PhysXBodyComponentPtr body1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponent<PhysXBodyComponent>();
		PhysXBodyComponentPtr body2 = GetSceneObject()->GetFirstComponent<PhysXBodyComponent>();

		NxActor* a1 = body1->GetNxActor();
		NxActor* a2 = body2->GetNxActor();


		//m_SceneManager->GetNxScene()->setActorPairFlags(*a1, *a2, NX_IGNORE_PAIR);

		//ignore collision for all parent bodies 
		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetParentSceneObject()->GetComponentsByClass(components,"PhysXBodyComponent");
			
		for(int i = 0; i < components.size(); i++)
		{
			PhysXBodyComponentPtr body = boost::shared_static_cast<PhysXBodyComponent>(components[i]);
			if(body->GetNxActor() && body->GetNxActor() != a2)
				m_SceneManager->GetNxScene()->setActorPairFlags(*body->GetNxActor(), *a2, NX_IGNORE_PAIR);
		}

		//if(m_PhysXJoint)
		//	dJointDestroy(m_PhysXJoint);

		switch(m_Type)
		{
		case BALL_JOINT:
			//m_PhysXJoint = dJointCreateBall(world,0);
			break;
		case SLIDER_JOINT:
			//m_PhysXJoint = dJointCreateSlider(world,0);
			break;
		case HINGE_JOINT:
			{
			//m_PhysXJoint = dJointCreateHinge(world,0);
			NxRevoluteJointDesc revoluteDesc;
			revoluteDesc.setToDefault();
			revoluteDesc.actor[0] = a1;
			revoluteDesc.actor[1] = a2;
			revoluteDesc.setGlobalAxis(NxVec3(1,0,0));
			m_PhysXJoint =	m_SceneManager->GetNxScene()->createJoint(revoluteDesc);
			}
			break;
		case UNIVERSAL_JOINT:
			//m_PhysXJoint = dJointCreateUniversal(world,0);
			break;
		//case SUSPENSION_JOINT:
			//m_PhysXJoint = dJointCreateHinge2(world, 0);
			//Update suspension
			//GetSceneObject()->RegisterForMessage(OBJECT_NM_PHYSICS_VELOCITY, TYPED_MESSAGE_FUNC(PhysXJoint::UpdateSwayBars,VelocityNotifyMessage));
		//	break;
		}
		UpdateAnchor();
		UpdateJointAxis();
		UpdateLimits();
		SetAxis1Force(m_JointForce1);
		//UpdateSuspension();

	}

	void PhysXJoint::SetAxis1(const Vec3 &axis)
	{
		m_Axis1 = axis;
		if(m_PhysXJoint)
			UpdateJointAxis();
	}

	void PhysXJoint::SetAxis2(const Vec3 &axis)
	{
		m_Axis2 = axis;
		if(m_PhysXJoint)
			UpdateJointAxis();
	}

	void PhysXJoint::SetPosition(const Vec3 &value)
	{
		if(m_AxisActor)
		{
			m_AxisActor->setGlobalPosition(NxVec3(value.x, value.y, value.z));
		}
	}

	void PhysXJoint::UpdateJointAxis()
	{
		//not thread safe!!! fix this
		ILocationComponent *location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponent<ILocationComponent>().get();
		ILocationComponent *location2 = GetSceneObject()->GetFirstComponent<ILocationComponent>().get();

		Quaternion rot = location1->GetRotation();

		//dReal ode_rot_mat[12];
		Mat4 rot_mat;
		rot_mat.Identity();
		rot.ToRotationMatrix(rot_mat);
		//ODEPhysicsSceneManager::CreateODERotationMatrix(rot_mat,ode_rot_mat);

		Vec3 pos_b1 = location1->GetPosition();
		Vec3 pos_b2 = location2->GetPosition();

		//Vec3 pos_b1(p1[0],p1[1],p1[2]);
		//Vec3 pos_b2(p2[0],p2[1],p2[2]);

		switch(m_Type)
		{
		case BALL_JOINT:
			break;
		case SLIDER_JOINT:
			/*if (m_Axis1.Length() != 0)
				dJointSetSliderAxis(m_PhysXJoint,m_Axis1.x,m_Axis1.y,m_Axis1.z);
			else
			{
				Vec3 axis = pos_b2 - pos_b1;
				axis.Normalize();
				dJointSetSliderAxis(m_PhysXJoint,axis.x,axis.y,axis.z);
			}*/
			break;
		case HINGE_JOINT:
			{
			NxRevoluteJoint* joint = static_cast<NxRevoluteJoint*>(m_PhysXJoint);
			if (m_Axis1.Length() != 0)
				joint->setGlobalAxis(NxVec3(m_Axis1.x,m_Axis1.y,m_Axis1.z));
			else
			{
				Vec3 axis = rot_mat.GetRightVector();
				joint->setGlobalAxis(NxVec3(axis.x,axis.y,axis.z));				
			}
			}//dJointSetHingeAxis(m_PhysXJoint,ode_rot_mat[4],ode_rot_mat[5],ode_rot_mat[6]);
			break;
		case UNIVERSAL_JOINT:
			/*if (m_Axis1.Length() != 0)
				dJointSetUniversalAxis1(m_PhysXJoint,m_Axis1.x,m_Axis1.y,m_Axis1.z);
			else
				dJointSetUniversalAxis1(m_PhysXJoint,ode_rot_mat[4],ode_rot_mat[5],ode_rot_mat[6]);
			if (m_Axis2.Length() != 0)
				dJointSetUniversalAxis2(m_PhysXJoint,m_Axis2.x,m_Axis2.y,m_Axis2.z);
			else
				dJointSetUniversalAxis2(m_PhysXJoint,-ode_rot_mat[6],ode_rot_mat[5],ode_rot_mat[4]); //rotate 90 in xz-plane*/
			break;
//		case SUSPENSION_JOINT:
			/*if (m_Axis1.Length() != 0)
				dJointSetHinge2Axis1(m_PhysXJoint,m_Axis1.x,m_Axis1.y,m_Axis1.z);
			else
				dJointSetHinge2Axis1(m_PhysXJoint,ode_rot_mat[4],ode_rot_mat[5],ode_rot_mat[6]);
			dJointSetHinge2Axis2(m_PhysXJoint,    ode_rot_mat[0],    ode_rot_mat[1],    ode_rot_mat[2]);*/
//			break;
		}
	}

	void PhysXJoint::SetAnchor(const Vec3 &value)
	{
		m_Anchor = value;
		if(m_PhysXJoint)
			UpdateAnchor();
	}

	void PhysXJoint::UpdateAnchor()
	{
		ILocationComponent *location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponent<ILocationComponent>().get();
		ILocationComponent *location2 = GetSceneObject()->GetFirstComponent<ILocationComponent>().get();

		Vec3 pos_b1 = location1->GetPosition();
		Vec3 pos_b2 = location2->GetPosition();

		Vec3  world_anchor;

		if(m_PhysXJoint)
		{
			switch(m_Type)
			{
			case BALL_JOINT:
				world_anchor = m_Anchor + pos_b2;
				//dJointSetBallAnchor(m_PhysXJoint,world_anchor.x,world_anchor.y,world_anchor.z);
				break;
			case SLIDER_JOINT:
				break;
			case HINGE_JOINT:
				{
				world_anchor = m_Anchor + pos_b2;
				NxRevoluteJoint* joint = static_cast<NxRevoluteJoint*>(m_PhysXJoint);
				joint->setGlobalAnchor(NxVec3(world_anchor.x,world_anchor.y,world_anchor.z));
				//dJointSetHingeAnchor(m_PhysXJoint,world_anchor.x,world_anchor.y,world_anchor.z);
				}
				break;
			case UNIVERSAL_JOINT:
				world_anchor = m_Anchor + pos_b2;
				//dJointSetUniversalAnchor(m_PhysXJoint,world_anchor.x,world_anchor.y,world_anchor.z);
				break;
			}
		}
	}

	void PhysXJoint::SetLowStop(float value)
	{
		m_LowStop = value;
		UpdateLimits();
	}

	void PhysXJoint::SetHighStop(float value)
	{
		m_HighStop = value;
		UpdateLimits();
	}


	void PhysXJoint::UpdateLimits()
	{
		if(m_PhysXJoint)
		{
			switch(m_Type)
			{
			case BALL_JOINT:
				break;
			case SLIDER_JOINT:
				//dJointSetSliderParam(m_PhysXJoint, dParamHiStop, m_HighStop);
				//dJointSetSliderParam(m_PhysXJoint, dParamLoStop, m_LowStop);
				break;			
			case HINGE_JOINT:
				{
				/*NxRevoluteJoint* joint = static_cast<NxRevoluteJoint*>(m_PhysXJoint);
				NxJointLimitPairDesc limits;
				limits.high.setToDefault();
				limits.high.value = m_HighStop;
				limits.low.setToDefault();
				limits.low.value = m_LowStop;
				joint->setLimits(limits);
				*/
				//dJointSetHingeParam(m_PhysXJoint, dParamHiStop, m_HighStop);
				//dJointSetHingeParam(m_PhysXJoint, dParamLoStop, m_LowStop);
				}
				break;
			case UNIVERSAL_JOINT:
				break;
			}
		}
	}

	float PhysXJoint::GetAngle()
	{
		switch(m_Type)
		{
		case BALL_JOINT:
			return 0.0;
			break;
		case SLIDER_JOINT:
			return 0.0;//dJointGetSliderPosition(m_PhysXJoint);
			break;
		case HINGE_JOINT:
			{
				NxRevoluteJoint* joint = static_cast<NxRevoluteJoint*>(m_PhysXJoint);
				return joint->getAngle();
			}
			break;
		}
		return 0.0;
	}

	float PhysXJoint::GetAngleRate()
	{
		switch(m_Type)
		{
		case BALL_JOINT:
			return 0.0;
			break;
		case SLIDER_JOINT:
			//return dJointGetSliderPositionRate(m_PhysXJoint);
			break;
		case UNIVERSAL_JOINT:
			//return dJointGetUniversalAngle1Rate(m_PhysXJoint);
			break;
		case HINGE_JOINT:
			{
				NxRevoluteJoint* joint = static_cast<NxRevoluteJoint*>(m_PhysXJoint);
				return joint->getVelocity();
			}
			break;
		}
		return 0.0;
	}

	void PhysXJoint::UpdateMotor()
	{
		if(m_PhysXJoint)
		{
			switch(m_Type)
			{
			case SLIDER_JOINT:
			//	dJointSetSliderParam(m_PhysXJoint,dParamVel,velocity);
				break;
			case HINGE_JOINT:			
				{
				NxRevoluteJoint* joint = static_cast<NxRevoluteJoint*>(m_PhysXJoint);
				NxMotorDesc motor;
				motor.velTarget = m_AngularVelocity2;
				motor.maxForce = m_JointForce2;
				joint->setMotor(motor);
				}
			//	dJointSetHingeParam(m_PhysXJoint,dParamVel,velocity);
				break;
			case UNIVERSAL_JOINT:
			//	dJointSetUniversalParam(m_PhysXJoint,dParamVel,velocity);
				break;
			}
		}
	}

	void PhysXJoint::SetAxis1Vel(float velocity)
	{
		m_AngularVelocity1  = velocity;
		UpdateMotor();

	}

	void PhysXJoint::SetAxis2Vel(float value)
	{
		m_AngularVelocity2  = value;
		UpdateMotor();
		
	}

	void PhysXJoint::SetAxis2Force(float value)
	{
		m_JointForce2 = value;
		UpdateMotor();
		
	}

	float PhysXJoint::GetAxis2Force() const
	{
		return m_JointForce2;
		if(m_PhysXJoint)
		{
			switch(m_Type)
			{
			case UNIVERSAL_JOINT:
				//return dJointGetUniversalParam(m_PhysXJoint, dParamFMax2);
				break;
			
				break;
			}
		}
		return 0;
	}

	void PhysXJoint::SetAxis1Force(float value)
	{
		m_JointForce1 = value;
		UpdateMotor();
		
	}


	void PhysXJoint::Enable()
	{
		// no disable for joints
	}

	void PhysXJoint::Disable()
	{
		// no enable for joints
	}

	

}
