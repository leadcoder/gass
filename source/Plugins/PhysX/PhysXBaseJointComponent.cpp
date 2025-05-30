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

#include "Plugins/PhysX/PhysXBaseJointComponent.h"
#include "Plugins/PhysX/PhysXBodyComponent.h"
#include "Sim/GASS.h"

namespace GASS
{
	PhysXBaseJointComponent::PhysXBaseJointComponent() 
		
	{

	}

	PhysXBaseJointComponent::~PhysXBaseJointComponent()
	{

	}

	void PhysXBaseJointComponent::RegisterReflection()
	{
		RegisterGetSet("Body1",&PhysXBaseJointComponent::GetBody1, &PhysXBaseJointComponent::SetBody1);
		RegisterGetSet("Body2", &PhysXBaseJointComponent::GetBody2, &PhysXBaseJointComponent::SetBody2);
		RegisterMember("Report", &GASS::PhysXBaseJointComponent::m_Report);
	}

	void PhysXBaseJointComponent::OnInitialize()
	{
		if(!(m_Body1.IsValid() && m_Body2.IsValid()))
		{
			//Check if this hinge should link this parent with this node
			PhysXBodyComponentPtr bc1 = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<PhysXBodyComponent>();
			PhysXBodyComponentPtr bc2 = GetSceneObject()->GetFirstComponentByClass<PhysXBodyComponent>();
			if(bc1 && bc2)
			{
				m_Body1 = SceneObjectRef(bc1->GetSceneObject());
				m_Body2 = SceneObjectRef(bc2->GetSceneObject());
			}
			else
				return;
		}

		PhysXBodyPtr b1 = m_Body1->GetFirstComponentByClass<IPhysXRigidDynamic>();
		if(!b1)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"The first body object you try to attach is not derived from IPhysXBody interface", "PhysXBaseJointComponent::OnInitialize");

		PhysXBodyPtr b2 = m_Body2->GetFirstComponentByClass<IPhysXRigidDynamic>();
		if(!b1)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"The second body object you try to attach is not derived from IPhysXBody interface", "PhysXBaseJointComponent::OnInitialize");

		// Check if bodies are already are loaded
		if(b1->GetPxRigidDynamic())
			m_Body1Loaded = true;
		else //wait for body to be loaded
			m_Body1->RegisterForMessage(REG_TMESS(PhysXBaseJointComponent::OnBody1Loaded,PhysicsBodyLoadedEvent,0));

		if(b2->GetPxRigidDynamic())
			m_Body2Loaded = true;
		else //wait for body to be loaded
			m_Body2->RegisterForMessage(REG_TMESS(PhysXBaseJointComponent::OnBody2Loaded,PhysicsBodyLoadedEvent,0));

		//both bodies are loaded -> create joint
		if(m_Body1Loaded && m_Body2Loaded)
			CreateJoint();

		/*if(m_Report)
		{
			PhysXPhysicsSceneManagerPtr sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
			SceneManagerListenerPtr listener = shared_from_this();
			sm->Register(listener);
		}*/
	}

	/*void PhysXBaseJointComponent::SceneManagerTick(double delta_time)
	{
		if(GetJoint())
		{
			//The force is resolved at the origin of actor1's joint frame
			physx::PxVec3 force,torque;
			GetJoint()->getConstraint()->getForce(force, torque);
			PhysicsJointForceEventPtr message(new PhysicsJointForceEvent(PxConvert::ToGASS(force),PxConvert::ToGASS(torque)));
			GetSceneObject()->PostMessage(message);
			std::cout << "Force: " << PxConvert::ToGASS(force) << " Torque:" << PxConvert::ToGASS(torque) << std::endl;
		}
	}*/


	void PhysXBaseJointComponent::SetBody1(SceneObjectRef value) 
	{
		m_Body1 = value;
		if(m_Body1.IsValid())
		{
			PhysXBodyPtr b = m_Body1->GetFirstComponentByClass<IPhysXRigidDynamic>();
			if(!b)
			{
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"The object you try to attach is not derived from IPhysXBody interface", "PhysXBaseJointComponent::SetBody1");
			}
			if(b->GetPxRigidDynamic())
				m_Body1Loaded = true;
			else
				m_Body1Loaded = false;
			if(m_Body1Loaded && m_Body2Loaded)
				CreateJoint();
		}
		else
			m_Body1Loaded = false;
	}

	void PhysXBaseJointComponent::SetBody2(SceneObjectRef value) 
	{
		m_Body2 = value;
		if(m_Body2.IsValid())
		{
			PhysXBodyPtr b = m_Body1->GetFirstComponentByClass<IPhysXRigidDynamic>();
			if(!b)
			{
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"The object you try to attach is not derived from IPhysXBody interface", "PhysXBaseJointComponent::SetBody2");
			}
			if(b->GetPxRigidDynamic())
				m_Body2Loaded = true;
			else
				m_Body2Loaded = false;
			if(m_Body1Loaded && m_Body2Loaded)
				CreateJoint();
		}
		else
			m_Body2Loaded = false;
	}

	void PhysXBaseJointComponent::OnBody1Loaded(PhysicsBodyLoadedEventPtr /*message*/)
	{
		m_Body1Loaded = true;
		if(m_Body2Loaded)
			CreateJoint();
	}

	void PhysXBaseJointComponent::OnBody2Loaded(PhysicsBodyLoadedEventPtr /*message*/)
	{
		m_Body2Loaded = true;
		if(m_Body1Loaded)
			CreateJoint();
	}
}
