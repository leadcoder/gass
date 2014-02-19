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

#include "Plugins/PhysX3/PhysXCharacterComponent.h"
#include "Plugins/PhysX3/PhysXWheelComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"
#include "Plugins/PhysX3/PhysXVehicleSceneQuery.h"

using namespace physx;
namespace GASS
{
	PhysXCharacterComponent::PhysXCharacterComponent() : m_Actor(NULL),
		m_ThrottleInput(0),
		m_SteerInput(0),
		m_StandingSize(1.8),
		m_Radius(0.4),
		m_Yaw(0)
	{

	}

	PhysXCharacterComponent::~PhysXCharacterComponent()
	{

	}

	void PhysXCharacterComponent::OnMassMessage(PhysicsBodyMassRequestPtr message)
	{
		SetMass(message->GetMass());
	}

	void PhysXCharacterComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysXCharacterComponent",new Creator<PhysXCharacterComponent, IComponent>);
		RegisterProperty<float>("Mass", &PhysXCharacterComponent::GetMass, &PhysXCharacterComponent::SetMass);
	}

	void PhysXCharacterComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnLocationLoaded,LocationLoadedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnPositionChanged,PositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnWorldPositionChanged,WorldPositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnRotationChanged,RotationMessage,0 ));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnMassMessage,PhysicsBodyMassRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnInput,InputControllerMessage,0));


		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnPostSceneObjectInitializedEvent,PostSceneObjectInitializedEvent,0));
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		scene_manager->Register(shared_from_this());
	}

	void PhysXCharacterComponent::Reset()
	{

	}

	void PhysXCharacterComponent::OnPostSceneObjectInitializedEvent(PostSceneObjectInitializedEventPtr message)
	{
		if(message->GetSceneObject() != GetSceneObject())
			return;
		m_Initialized = true;
	}

	void PhysXCharacterComponent::OnPositionChanged(PositionMessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}

	void PhysXCharacterComponent::OnWorldPositionChanged(WorldPositionMessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}

	void PhysXCharacterComponent::OnRotationChanged(RotationMessagePtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Quaternion rot = message->GetRotation();
			SetRotation(rot);
		}
	}

	void PhysXCharacterComponent::OnLocationLoaded(LocationLoadedMessagePtr message)
	{
		PhysXPhysicsSceneManagerPtr sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		m_SceneManager = sm;
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();


		LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		Vec3 pos = location->GetPosition();
		Quaternion rot = location->GetRotation();

		PxExtendedVec3 px_vec(pos.x,pos.y,pos.z);

		PxCapsuleControllerDesc cDesc;
		cDesc.material			= system->GetDefaultMaterial();
		cDesc.position			= px_vec;
		cDesc.height			= m_StandingSize;
		cDesc.radius			= m_Radius;
		cDesc.slopeLimit		= 0.0f;
		cDesc.contactOffset		= 0.1f;
		cDesc.stepOffset		= 0.02f;
		cDesc.callback			= this;
		cDesc.behaviorCallback	= this;


		m_Controller = static_cast<PxCapsuleController*>(system->GetControllerManager()->createController(*system->GetPxSDK(),sm->GetPxScene(),cDesc));
		// remove controller shape from scene query for standup overlap test
		PxRigidDynamic* actor = m_Controller->getActor();
		if(actor)
		{
			if(actor->getNbShapes())
			{
				PxShape* ctrlShape;
				actor->getShapes(&ctrlShape,1);
				ctrlShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE,false);
			}
			//else
			//	fatalError("character actor has no shape");
		}
		//else
		//	fatalError("character could not create actor");
	}

	void PhysXCharacterComponent::onShapeHit(const PxControllerShapeHit& hit)
	{
		PxRigidDynamic* actor = hit.shape->getActor().is<PxRigidDynamic>();
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

	void PhysXCharacterComponent::SceneManagerTick(double delta)
	{
		int from_id = (int)this; //use address as id

		Vec3 current_pos  = GetPosition();

		MessagePtr pos_msg(new WorldPositionMessage(current_pos ,from_id));
		GetSceneObject()->PostMessage(pos_msg);

		MessagePtr rot_msg(new WorldRotationMessage(GetRotation(),from_id));
		GetSceneObject()->PostMessage(rot_msg);

		PxVec3 targetKeyDisplacement(0);
		PxVec3 targetPadDisplacement(0);

		PxVec3 forward;// = m_ViewDir;
		forward.y = 0;
		forward.normalize();
		PxVec3 up = PxVec3(0,1,0);
		PxVec3 right = forward.cross(up);

		if(m_ThrottleInput > 0)	
			targetKeyDisplacement += forward;
		if(m_ThrottleInput < 0)	
			targetKeyDisplacement -= forward;

		//if(m_SteerInput > 0)	
		//	targetKeyDisplacement += right;
		//if(m_SteerInput < 0)	
		//	targetKeyDisplacement -= right;
		m_Yaw += m_SteerInput * delta;
		//m_Pitch		+= mGamepadPitchInc * dtime;
		// Clamp pitch
		//camera.setRot(PxVec3(0,-m_TargetYaw,0));
		targetKeyDisplacement += PxVec3(0,-9.81,0);
		targetKeyDisplacement *= delta;
		PxU32 flags = m_Controller->move(targetKeyDisplacement, 0.001f, delta, PxControllerFilters(0));
	}

	void PhysXCharacterComponent::SetMass(float mass)
	{
		m_Mass = mass;
	}

	void PhysXCharacterComponent::SetPosition(const Vec3 &value)
	{
		if(m_Actor)
		{
			Reset();
			m_Actor->setGlobalPose(physx::PxTransform(PxConvert::ToPx(value), m_Actor->getGlobalPose().q));
		}
	}

	Vec3  PhysXCharacterComponent::GetPosition() const
	{
		Vec3 pos(0,0,0);
		if(m_Actor)
		{
			pos = PxConvert::ToGASS(m_Actor->getGlobalPose().p);
		}
		return pos;
	}

	void PhysXCharacterComponent::SetRotation(const Quaternion &rot)
	{
		if(m_Actor)
		{
			Reset();
			m_Actor->setGlobalPose(physx::PxTransform(m_Actor->getGlobalPose().p,PxConvert::ToPx(rot)));
		}
	}

	Quaternion PhysXCharacterComponent::GetRotation()
	{
		Quaternion q;
		if(m_Actor)
		{
			q = PxConvert::ToGASS(m_Actor->getGlobalPose().q);
		}
		return q;
	}

	void PhysXCharacterComponent::OnInput(InputControllerMessagePtr message)
	{
		std::string name = message->GetController();
		float value = message->GetValue();

		if (name == "Throttle")
		{
			m_ThrottleInput = value;
		}
		else if (name == "Steer")
		{
			m_SteerInput = value;
		}
	}
}
