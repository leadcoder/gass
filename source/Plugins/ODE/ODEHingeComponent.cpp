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

#include "Sim/GASSCommon.h"
#include "Plugins/ODE/ODEHingeComponent.h"
#include "Plugins/ODE/ODEBodyComponent.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSILocationComponent.h"

namespace GASS
{
	ODEHingeComponent::ODEHingeComponent() : m_MaxTorque (0),
		m_Anchor (0,0,0),
		m_Axis (0,0,0),
		m_ODEJoint (0),
		m_HighStop(0),
		m_LowStop(0),
		m_Body1Loaded(0),
		m_Body2Loaded(0),
		m_ODEBody1(0),
		m_ODEBody2(0)
		//m_SwayForce(0),
		//m_Strength(0),
		//m_Damping(0)
	{
	}

	ODEHingeComponent::~ODEHingeComponent()
	{
		if(m_ODEJoint) dJointDestroy(m_ODEJoint);
	}

	void ODEHingeComponent::RegisterReflection()
	{
		RegisterGetSet("MaxTorque", &GASS::ODEHingeComponent::GetMaxTorque, &GASS::ODEHingeComponent::SetMaxTorque);
		RegisterGetSet("HighStop", &GASS::ODEHingeComponent::GetHighStop, &GASS::ODEHingeComponent::SetHighStop);
		RegisterGetSet("LowStop", &GASS::ODEHingeComponent::GetLowStop, &GASS::ODEHingeComponent::SetLowStop);
		RegisterGetSet("Axis", &GASS::ODEHingeComponent::GetAxis, &GASS::ODEHingeComponent::SetAxis);
		RegisterGetSet("Anchor", &GASS::ODEHingeComponent::GetAnchor, &GASS::ODEHingeComponent::SetAnchor);
		RegisterGetSet("Body1", &GASS::ODEHingeComponent::GetBody1, &GASS::ODEHingeComponent::SetBody1);
		RegisterGetSet("Body2", &GASS::ODEHingeComponent::GetBody2, &GASS::ODEHingeComponent::SetBody2);
	}
	
	void ODEHingeComponent::OnInitialize()
	{
		ODEPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<ODEPhysicsSceneManager>();
		m_SceneManager = scene_manager;
		
//		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEHingeComponent::OnBodyLoaded,PhysicsBodyLoadedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEHingeComponent::OnVelocityRequest,PhysicsHingeJointVelocityRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEHingeComponent::OnMaxTorqueRequest,PhysicsHingeJointMaxTorqueRequest,0));

		if(!(m_Body1.IsValid() && m_Body2.IsValid()))
		{
			//Check if this hinge should link this parent with this node
			ODEBodyComponentPtr bc1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<ODEBodyComponent>();
			ODEBodyComponentPtr bc2 = GetSceneObject()->GetFirstComponentByClass<ODEBodyComponent>();
			if(bc1 && bc2)
			{
				m_ODEBody1  = bc1->GetODEBodyComponent();
				m_ODEBody2  = bc2->GetODEBodyComponent();
				m_Body1 = SceneObjectRef(bc1->GetSceneObject());
				m_Body2 = SceneObjectRef(bc2->GetSceneObject());

				if(!m_ODEBody1)
					m_Body1->RegisterForMessage(REG_TMESS(ODEHingeComponent::OnBody1Loaded,PhysicsBodyLoadedEvent,0));
				else
					m_Body1Loaded = true;
				if(!m_ODEBody2) 
					m_Body2->RegisterForMessage(REG_TMESS(ODEHingeComponent::OnBody2Loaded,PhysicsBodyLoadedEvent,0));
				else
					m_Body2Loaded = true;

				if(m_Body2Loaded && m_Body2Loaded)
					CreateJoint();
			}
		}
	}

	void ODEHingeComponent::SetBody1(SceneObjectRef value) 
	{
		 m_Body1 = value;
		 if(m_Body1.IsValid())
		 {
			 m_ODEBody1  = m_Body1->GetFirstComponentByClass<ODEBodyComponent>()->GetODEBodyComponent();
			 if(m_ODEBody1)
				 m_Body1Loaded = true;
			 else
				 m_Body1Loaded = false;
			 if(m_Body1Loaded && m_Body2Loaded)
			 	CreateJoint();
		 }
		 else
			 m_Body1Loaded = false;
	}

	void ODEHingeComponent::SetBody2(SceneObjectRef value) 
	{
		 m_Body2 = value;
		 if(m_Body2.IsValid())
		 {
			 m_ODEBody2  = m_Body2->GetFirstComponentByClass<ODEBodyComponent>()->GetODEBodyComponent();
			 if(m_ODEBody2)
				 m_Body2Loaded = true;
			 else
				 m_Body2Loaded = false;
			 if(m_Body1Loaded && m_Body2Loaded)
				CreateJoint();
		 }
		 else
			m_Body2Loaded = false;
	}

	void ODEHingeComponent::OnBody1Loaded(PhysicsBodyLoadedEventPtr message)
	{
		m_Body1Loaded = true;
		m_ODEBody1  = m_Body1->GetFirstComponentByClass<ODEBodyComponent>()->GetODEBodyComponent();
		if(m_Body2Loaded)
			CreateJoint();
	}

	void ODEHingeComponent::OnBody2Loaded(PhysicsBodyLoadedEventPtr message)
	{
		m_Body2Loaded = true;
		m_ODEBody2  = m_Body2->GetFirstComponentByClass<ODEBodyComponent>()->GetODEBodyComponent();
		if(m_Body1Loaded)
			CreateJoint();
	}

	void ODEHingeComponent::OnVelocityRequest(PhysicsHingeJointVelocityRequestPtr message)
	{
		if(m_ODEBody1)
		{
			//m_ODEBody1->Wake();
			SetAxisVel(static_cast<float>(message->GetVelocity()));
		}
	}

	void ODEHingeComponent::OnMaxTorqueRequest(PhysicsHingeJointMaxTorqueRequestPtr message)
	{
		if(m_ODEBody1)
		{
			//m_ODEBody1->Wake();
			SetMaxTorque(static_cast<float>(message->GetMaxTorque()));
		}
	}

	void ODEHingeComponent::CreateJoint()
	{
		dWorldID world = ODEPhysicsSceneManagerPtr(m_SceneManager)->GetWorld();
	
		if(m_ODEBody1 && m_ODEBody2)
		{
			
			if(m_ODEJoint)
				dJointDestroy(m_ODEJoint);

			m_ODEJoint = dJointCreateHinge(world,0);
			GetSceneObject()->RegisterForMessage(REG_TMESS(ODEHingeComponent::SendJointUpdate,PhysicsVelocityEvent,0));
			dJointAttach(m_ODEJoint, m_ODEBody1,m_ODEBody2);

			dJointSetHingeParam(m_ODEJoint,dParamFudgeFactor,0.5);
			dJointSetHingeParam(m_ODEJoint,dParamBounce,0.5);

			UpdateAnchor();
			UpdateJointAxis();
			UpdateLimits();
			SetMaxTorque(m_MaxTorque);
			SetAxisVel(0);
		}
	}

	void ODEHingeComponent::SetAxis(const Vec3 &axis)
	{
		m_Axis = axis;
		if(m_ODEJoint)
			UpdateJointAxis();
	}

	void ODEHingeComponent::UpdateJointAxis()
	{
		LocationComponentPtr location1 = m_Body1->GetFirstComponentByClass<ILocationComponent>();
		LocationComponentPtr location2 = m_Body2->GetFirstComponentByClass<ILocationComponent>();
		
		Quaternion rot = location1->GetRotation();
		dReal ode_rot_mat[12];
		Mat4 rot_mat(rot);
		ODEPhysicsSceneManager::CreateODERotationMatrix(rot_mat,ode_rot_mat);

		//Vec3 pos_b1 = location1->GetPosition();
		//Vec3 pos_b2 = location2->GetPosition();

		if (m_Axis.Length() != 0)
			dJointSetHingeAxis(m_ODEJoint,m_Axis.x,m_Axis.y,m_Axis.z);
		else
		{
			//const Vec3 axis = rot_mat.GetYAxis();
			dJointSetHingeAxis(m_ODEJoint,ode_rot_mat[4],ode_rot_mat[5],ode_rot_mat[6]);
		}
	}

	void ODEHingeComponent::SetAnchor(const Vec3 &value)
	{
		m_Anchor = value;
		if(m_ODEJoint)
			UpdateAnchor();
	}

	void ODEHingeComponent::UpdateAnchor()
	{
		//LocationComponentPtr location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		LocationComponentPtr location2 = m_Body2->GetFirstComponentByClass<ILocationComponent>();

		//Vec3 pos_b1 = location1->GetPosition();
		Vec3 pos_b2 = location2->GetPosition();

		Vec3  world_anchor;

		if(m_ODEJoint)
		{
			world_anchor = m_Anchor + pos_b2;
			dJointSetHingeAnchor(m_ODEJoint,world_anchor.x,world_anchor.y,world_anchor.z);
		}
	}

	void ODEHingeComponent::SetLowStop(float value)
	{
		m_LowStop = value;
		UpdateLimits();
	}

	void ODEHingeComponent::SetHighStop(float value)
	{
		m_HighStop = value;
		UpdateLimits();
	}


	void ODEHingeComponent::UpdateLimits()
	{
		if(m_ODEJoint)
		{
			dJointSetHingeParam(m_ODEJoint, dParamHiStop, m_HighStop);
			dJointSetHingeParam(m_ODEJoint, dParamLoStop, m_LowStop);
		}
	}


	void ODEHingeComponent::SetAxisVel(float velocity)
	{
		if(m_ODEJoint)
		{
			dJointSetHingeParam(m_ODEJoint,dParamVel,velocity);
		}
	}

	void ODEHingeComponent::SetMaxTorque(float value)
	{
		m_MaxTorque = value;
		if(m_ODEJoint)
		{
			dJointSetHingeParam(m_ODEJoint, dParamFMax,value);
		}
	}

	void ODEHingeComponent::SendJointUpdate(PhysicsVelocityEventPtr message)
	{
		ODEPhysicsHingeJointEventPtr joint_message;
		if(m_ODEJoint)
		{
			float angle = static_cast<float>(dJointGetHingeAngle(m_ODEJoint));
			float angle_rate = static_cast<float>(dJointGetHingeAngleRate(m_ODEJoint));
			joint_message = ODEPhysicsHingeJointEventPtr(new ODEPhysicsHingeJointEvent(angle,angle_rate));
			if(joint_message)
				GetSceneObject()->SendImmediateEvent(joint_message);
		}
	}
}






