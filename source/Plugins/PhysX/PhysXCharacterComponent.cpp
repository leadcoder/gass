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

#include "Plugins/PhysX/PhysXCharacterComponent.h"
#include "Plugins/PhysX/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX/PhysXPhysicsSystem.h"
#include "Core/Math/GASSMath.h"
using namespace physx;
namespace GASS
{
	PhysXCharacterComponent::PhysXCharacterComponent() 
		
	{

	}

	PhysXCharacterComponent::~PhysXCharacterComponent()
	{

	}

	void PhysXCharacterComponent::OnDelete()
	{
		if(m_Controller)
		{
			m_Controller->release();
			m_Controller= nullptr;
		}
	}

	void PhysXCharacterComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<PhysXCharacterComponent>();
		RegisterGetSet("Mass", &PhysXCharacterComponent::GetMass, &PhysXCharacterComponent::SetMass);
		RegisterMember("MaxSpeed", &PhysXCharacterComponent::m_MaxSpeed);
		RegisterMember("Radius", &PhysXCharacterComponent::m_Radius);
		RegisterMember("StandingSize", &PhysXCharacterComponent::m_StandingSize);
		RegisterMember("YawMaxVelocity", &PhysXCharacterComponent::m_YawMaxVelocity);
		RegisterMember("Acceleration", &PhysXCharacterComponent::m_Acceleration);
	}

	void PhysXCharacterComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnLocationLoaded,LocationLoadedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnTransformationChanged, TransformationChangedEvent, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnInput,InputRelayEvent,0));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnPostUpdate,PostPhysicsSceneUpdateEvent,0));
		RegisterForPostUpdate<PhysXPhysicsSceneManager>();
	}

	void PhysXCharacterComponent::OnTransformationChanged(TransformationChangedEventPtr event)
	{
		if (m_TrackTransformation)
		{
			SetPosition(event->GetPosition());
			SetRotation(event->GetRotation());
		}
	}

	void PhysXCharacterComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		m_SceneManager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>().get();
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
		
		LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		Vec3 pos = location->GetPosition();
		PxExtendedVec3 px_initial_pos(pos.x, pos.y, pos.z);

		PxCapsuleControllerDesc cDesc;
		cDesc.material			= system->GetDefaultMaterial();
		cDesc.position			= px_initial_pos;
		cDesc.height			= static_cast<float>(m_StandingSize);
		cDesc.radius			= static_cast<float>(m_Radius);
		cDesc.slopeLimit		= 0.0f;
		cDesc.contactOffset		= 0.1f;
		cDesc.stepOffset		= 0.02f;
		cDesc.upDirection = PxConvert::ToPx(m_SceneManager->GetUpVector());
		cDesc.behaviorCallback	= this;
		m_Controller = static_cast<PxCapsuleController*>(m_SceneManager->GetControllerManager()->createController(cDesc));
		// remove controller shape from scene query for standup overlap test
		m_Actor = m_Controller->getActor();
		if(m_Actor)
		{
			if(m_Actor->getNbShapes())
			{
				PxShape* ctrlShape;
				m_Actor->getShapes(&ctrlShape,1);
				ctrlShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE,false);
			}
		}
		m_Initialized = true;
	}

	void PhysXCharacterComponent::onShapeHit(const PxControllerShapeHit& hit)
	{
		PxRigidDynamic* actor = hit.shape->getActor()->is<PxRigidDynamic>();
		if(actor)
		{
			// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
			// useless stress on the solver. It would be possible to enable/disable vertical pushes on
			// particular objects, if the gameplay requires it.
			if(hit.dir.y==0.0f)
			{
				PxReal coeff = actor->getMass() * hit.length;
				PxRigidBodyExt::addForceAtLocalPos(*actor,hit.dir*coeff, PxVec3(0,0,0), PxForceMode::eIMPULSE);
			}
		}
	}

	void PhysXCharacterComponent::SceneManagerTick(double /*delta*/)
	{
	}

	void PhysXCharacterComponent::OnPostUpdate(PostPhysicsSceneUpdateEventPtr message)
	{
		Float delta = message->GetDeltaTime();
		int from_id = GASS_PTR_TO_INT(this); //use address as id

		const Vec3 current_pos  = GetPosition();

		//update rotation
		const double yaw = m_SteerInput * m_YawMaxVelocity* delta;
		const Quaternion new_rot = GetRotation() * Quaternion::CreateFromEulerYXZ(Vec3(0, yaw, 0));
		SetRotation(new_rot);

		m_TrackTransformation = false;
		GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->SetWorldPosition(current_pos);
		GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->SetWorldRotation(new_rot);
		m_TrackTransformation = true;

		const Vec3 forward = new_rot.GetZAxis();
		Vec3 target_displacement(0,0,0);
		m_CurrentVel += m_Acceleration*m_ThrottleInput*delta;
		if(m_CurrentVel > m_MaxSpeed)
			m_CurrentVel = m_MaxSpeed;
		if(m_CurrentVel < -m_MaxSpeed)
			m_CurrentVel = -m_MaxSpeed;

		//damp
		if(m_ThrottleInput == 0)
		{
			if(m_CurrentVel > 0)
			{
				m_CurrentVel -= m_Acceleration*delta;
				if(m_CurrentVel < 0)
					m_CurrentVel = 0;
			}
			else if(m_CurrentVel < 0)
			{
				m_CurrentVel += m_Acceleration*delta;
				if(m_CurrentVel > 0)
					m_CurrentVel = 0;
			}
		}
		
		target_displacement += forward*m_CurrentVel;
		target_displacement += PxConvert::ToGASS(m_SceneManager->GetPxScene()->getGravity());
		target_displacement *= delta;
		PxControllerCollisionFlags flags = m_Controller->move(PxConvert::ToPx(target_displacement), 0.001f, static_cast<float>(delta), PxControllerFilters(nullptr));
		GetSceneObject()->PostEvent(std::make_shared<PhysicsVelocityEvent>(Vec3(0,0,m_CurrentVel),Vec3(0,0,0),from_id));
	}

	void PhysXCharacterComponent::SetMass(float mass)
	{
		m_Mass = mass;
	}

	void PhysXCharacterComponent::SetPosition(const Vec3 &value)
	{
		if(m_Controller)
		{
			const PxVec3 final_pos = m_SceneManager->WorldToLocal(value);
			m_Controller->setFootPosition(PxExtendedVec3(final_pos.x, final_pos.y, final_pos.z));
		}
	}

	Vec3  PhysXCharacterComponent::GetPosition() const
	{
		Vec3 pos(0,0,0);
		if(m_Controller)
		{
			PxExtendedVec3 foot_pos = m_Controller->getFootPosition();
			pos = m_SceneManager->LocalToWorld(PxVec3(static_cast<PxReal>(foot_pos.x),
										  static_cast<PxReal>(foot_pos.y), 
										  static_cast<PxReal>(foot_pos.z)));
		}
		return pos;
	}

	void PhysXCharacterComponent::SetRotation(const Quaternion &rot)
	{
		m_Rotation = rot;
	}

	Quaternion PhysXCharacterComponent::GetRotation()
	{
		return m_Rotation;
	}

	void PhysXCharacterComponent::OnInput(InputRelayEventPtr message)
	{
		std::string name = message->GetController();
		float value = message->GetValue();

		if (name == "Throttle")
		{
			m_ThrottleInput = -value;
		}
		else if (name == "Steer")
		{
			m_SteerInput = value;
		}
	}
}
