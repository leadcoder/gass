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
#include "Plugins/ODE/ODEHingeComponent.h"
#include "Plugins/ODE/ODEBodyComponent.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/ComponentSystem/GASSIComponentContainer.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSILocationComponent.h"


namespace GASS
{
	ODEHingeComponent::ODEHingeComponent() : m_Body1 (NULL),
		m_Body2 (NULL),
		m_JointForce (0),
		m_Anchor (0,0,0),
		m_Axis (0,0,0),
		m_ODEJoint (0),
		m_HighStop(0),
		m_LowStop(0)
	{
	}

	ODEHingeComponent::~ODEHingeComponent()
	{
		if(m_ODEJoint) dJointDestroy(m_ODEJoint);
	}

	void ODEHingeComponent::RegisterReflection()
	{
		RegisterProperty<float>("AxisForce", &GASS::ODEHingeComponent::GetAxisForce, &GASS::ODEHingeComponent::SetAxisForce);
		RegisterProperty<float>("HighStop", &GASS::ODEHingeComponent::GetHighStop, &GASS::ODEHingeComponent::SetHighStop);
		RegisterProperty<float>("LowStop", &GASS::ODEHingeComponent::GetLowStop, &GASS::ODEHingeComponent::SetLowStop);
		RegisterProperty<Vec3>("Axis", &GASS::ODEHingeComponent::GetAxis, &GASS::ODEHingeComponent::SetAxis);
		RegisterProperty<Vec3>("Anchor", &GASS::ODEHingeComponent::GetAnchor, &GASS::ODEHingeComponent::SetAnchor);
	}
	
	void ODEHingeComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEHingeComponent::OnBodyLoaded,BodyLoadedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEHingeComponent::OnVelocityRequest,PhysicsHingeJointVelocityRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ODEHingeComponent::OnForceRequest,PhysicsHingeJointForceRequest,0));
	}

	void ODEHingeComponent::OnVelocityRequest(PhysicsHingeJointVelocityRequestPtr message)
	{
		if(m_Body1)
		{
			m_Body1->Wake();
			SetAxisVel(message->GetVelocity());
		}
	}

	void ODEHingeComponent::OnForceRequest(PhysicsHingeJointForceRequestPtr message)
	{
		if(m_Body1)
		{
			m_Body1->Wake();
			SetAxisForce(message->GetForce());
		}
	}

	void ODEHingeComponent::OnBodyLoaded(BodyLoadedMessagePtr message)
	{
		ODEPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<ODEPhysicsSceneManager>();
		m_SceneManager = scene_manager;
		assert(scene_manager);
		CreateJoint();
	}

	void ODEHingeComponent::CreateJoint()
	{
		dWorldID world = ODEPhysicsSceneManagerPtr(m_SceneManager)->GetWorld();

		m_Body1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<ODEBodyComponent>().get();
		

		if(m_Body1)
		{
			m_Body2 = GetSceneObject()->GetFirstComponentByClass<ODEBodyComponent>().get();

			dBodyID b1 = m_Body1->GetODEBodyComponent();
			dBodyID b2 = m_Body2->GetODEBodyComponent();

			if(m_ODEJoint)
				dJointDestroy(m_ODEJoint);

			m_ODEJoint = dJointCreateHinge(world,0);
			GetSceneObject()->RegisterForMessage(REG_TMESS(ODEHingeComponent::SendJointUpdate,VelocityNotifyMessage,0));
			dJointAttach(m_ODEJoint, b1,b2);

			dJointSetHingeParam(m_ODEJoint,dParamFudgeFactor,0.5);
			dJointSetHingeParam(m_ODEJoint,dParamBounce,0.5);

			UpdateAnchor();
			UpdateJointAxis();
			UpdateLimits();
			SetAxisForce(m_JointForce);
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
		LocationComponentPtr location1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		LocationComponentPtr location2 = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();

		Quaternion rot = location1->GetRotation();

		dReal ode_rot_mat[12];
		Mat4 rot_mat;
		rot_mat.Identity();
		rot.ToRotationMatrix(rot_mat);
		ODEPhysicsSceneManager::CreateODERotationMatrix(rot_mat,ode_rot_mat);

		//Vec3 pos_b1 = location1->GetPosition();
		//Vec3 pos_b2 = location2->GetPosition();

		if (m_Axis.Length() != 0)
			dJointSetHingeAxis(m_ODEJoint,m_Axis.x,m_Axis.y,m_Axis.z);
			else
				dJointSetHingeAxis(m_ODEJoint,ode_rot_mat[4],ode_rot_mat[5],ode_rot_mat[6]);
			
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
		LocationComponentPtr location2 = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();

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

	void ODEHingeComponent::SetAxisForce(float value)
	{
		m_JointForce = value;
		if(m_ODEJoint)
		{
			dJointSetHingeParam(m_ODEJoint, dParamFMax,value);
		}
	}

	void ODEHingeComponent::SendJointUpdate(VelocityNotifyMessagePtr message)
	{
		MessagePtr joint_message;
		if(m_ODEJoint)
		{
			float angle = dJointGetHingeAngle(m_ODEJoint);
			float angle_rate = dJointGetHingeAngleRate(m_ODEJoint);
			joint_message = HingeJointNotifyMessagePtr(new HingeJointNotifyMessage(angle,angle_rate));
			if(joint_message)
				GetSceneObject()->SendImmediate(joint_message);
		}
	}
}






