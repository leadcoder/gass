/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"
using namespace physx;
namespace GASS
{
	PhysXCharacterComponent::PhysXCharacterComponent() : m_Actor(NULL),
		m_ThrottleInput(0),
		m_SteerInput(0),
		m_StandingSize(1.8),
		m_Radius(0.4),
		m_Yaw(0),
		m_YawMaxVelocity(2),
		m_Acceleration(5.2),
		m_MaxSpeed(4),
		m_CurrentVel(0),
		m_Controller(NULL)
	{

	}

	PhysXCharacterComponent::~PhysXCharacterComponent()
	{

	}

	void PhysXCharacterComponent::OnMassMessage(PhysicsBodyMassRequestPtr message)
	{
		SetMass(message->GetMass());
	}

	void PhysXCharacterComponent::OnDelete()
	{
		if(m_Controller)
		{
			m_Controller->release();
			m_Controller= NULL;
		}
	}


	void PhysXCharacterComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysXCharacterComponent",new Creator<PhysXCharacterComponent, Component>);
		RegisterProperty<float>("Mass", &PhysXCharacterComponent::GetMass, &PhysXCharacterComponent::SetMass);
		RegisterProperty<Float>("Radius", &PhysXCharacterComponent::GetRadius, &PhysXCharacterComponent::SetRadius);
		RegisterProperty<Float>("StandingSize", &PhysXCharacterComponent::GetRadius, &PhysXCharacterComponent::SetRadius);
		RegisterProperty<Float>("YawMaxVelocity", &PhysXCharacterComponent::GetYawMaxVelocity, &PhysXCharacterComponent::SetYawMaxVelocity);
		RegisterProperty<Float>("Acceleration", &PhysXCharacterComponent::GetAcceleration, &PhysXCharacterComponent::SetAcceleration);
		RegisterProperty<Float>("MaxSpeed", &PhysXCharacterComponent::GetMaxSpeed, &PhysXCharacterComponent::SetMaxSpeed);
		
	}

	void PhysXCharacterComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnLocationLoaded,LocationLoadedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnPositionChanged,PositionRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnWorldPositionChanged,WorldPositionRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnRotationChanged,RotationRequest,0 ));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnWorldRotationChanged,WorldRotationRequest,0 ));
		
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnMassMessage,PhysicsBodyMassRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnInput,InputRelayEvent,0));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(PhysXCharacterComponent::OnPostUpdate,PostPhysicsSceneUpdateEvent,0));
		PhysXPhysicsSceneManagerPtr scene_manager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		scene_manager->Register(shared_from_this());
	}

	void PhysXCharacterComponent::Reset()
	{
		m_Yaw = 0;

		Mat4 rot_mat;
		rot_mat.Identity();
		GetRotation().ToRotationMatrix(rot_mat);
		
		m_Yaw = rot_mat.GetEulerHeading();

	}

	void PhysXCharacterComponent::OnPositionChanged(PositionRequestPtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}

	void PhysXCharacterComponent::OnWorldPositionChanged(WorldPositionRequestPtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Vec3 pos = message->GetPosition();
			SetPosition(pos);
		}
	}

	void PhysXCharacterComponent::OnRotationChanged(RotationRequestPtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Quaternion rot = message->GetRotation();
			SetRotation(rot);
		}
	}

	void PhysXCharacterComponent::OnWorldRotationChanged(WorldRotationRequestPtr message)
	{
		int this_id = (int)this; //we used address as id
		if(message->GetSenderID() != this_id) //Check if this message was from this class
		{
			Quaternion rot = message->GetRotation();
			SetRotation(rot);
		}
	}

	void PhysXCharacterComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		PhysXPhysicsSceneManagerPtr sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<PhysXPhysicsSceneManager>();
		m_SceneManager = sm;
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();

		LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		Vec3 pos = location->GetPosition();
		Quaternion rot = location->GetRotation();

		PxExtendedVec3 px_vec(pos.x,pos.y+20,pos.z);

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
		m_Actor = m_Controller->getActor();
		if(m_Actor)
		{
			if(m_Actor->getNbShapes())
			{
				PxShape* ctrlShape;
				m_Actor->getShapes(&ctrlShape,1);
				ctrlShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE,false);
			}
			//else
			//	fatalError("character actor has no shape");
		}
		m_Initialized = true;
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
	}

	void PhysXCharacterComponent::OnPostUpdate(PostPhysicsSceneUpdateEventPtr message)
	{
		Float delta = message->GetDeltaTime();
		int from_id = (int)this; //use address as id

		const Vec3 current_pos  = GetPosition();
		
		GetSceneObject()->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(current_pos ,from_id)));

		m_Yaw += m_SteerInput * m_YawMaxVelocity* delta;
		
		Quaternion new_rot(Vec3(m_Yaw,0,0));
		GetSceneObject()->PostRequest(WorldRotationRequestPtr(new WorldRotationRequest(new_rot,from_id)));
		
		Mat4 rot_mat;
		rot_mat.Identity();
		new_rot.ToRotationMatrix(rot_mat);
		Vec3 forward = rot_mat.GetZAxis();

		Vec3 target_displacement(0,0,0);
		forward.y = 0;
		forward.Normalize();
		Vec3 up(0,1,0);
		Vec3 right = Math::Cross(forward,up);
		m_CurrentVel += m_Acceleration*m_ThrottleInput*delta;
		if(m_CurrentVel > m_MaxSpeed)
			m_CurrentVel = m_MaxSpeed;
		if(m_CurrentVel < -m_MaxSpeed)
			m_CurrentVel = -m_MaxSpeed;

		/*if(m_ThrottleInput > 0)
		{
			target_displacement += forward*m_CurrentVel;
		}
		else if(m_ThrottleInput < 0)	
			target_displacement -= forward;
*/
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

		//if(m_SteerInput > 0)	
		//	target_displacement += right;
		//if(m_SteerInput < 0)	
		//	target_displacement -= right;
		
		//m_Pitch		+= mGamepadPitchInc * dtime;
		// Clamp pitch
		//camera.setRot(PxVec3(0,-m_TargetYaw,0));
		target_displacement += Vec3(0,-9.81,0);
		target_displacement *= delta;
		PxU32 flags = m_Controller->move(PxConvert::ToPx(target_displacement), 0.001f, delta, PxControllerFilters(0));

		//LogManager::getSingleton().stream() << m_CurrentVel << "\n";
		GetSceneObject()->PostEvent(PhysicsVelocityEventPtr(new PhysicsVelocityEvent(Vec3(0,0,m_CurrentVel),Vec3(0,0,0),from_id)));

	}

	void PhysXCharacterComponent::SetMass(float mass)
	{
		m_Mass = mass;
	}

	void PhysXCharacterComponent::SetPosition(const Vec3 &value)
	{
		if(m_Controller)
		{
			Reset();
			Vec3 final_pos = value + PhysXPhysicsSceneManagerPtr(m_SceneManager)->GetOffset();
			m_Controller->setFootPosition(PxExtendedVec3(final_pos.x, final_pos.y, final_pos.z));
		}
	}

	Vec3  PhysXCharacterComponent::GetPosition() const
	{
		Vec3 pos(0,0,0);
		if(m_Controller)
		{
			PxExtendedVec3 foot_pos = m_Controller->getFootPosition();
			pos.Set(foot_pos.x,foot_pos.y,foot_pos.z);
			pos = pos - PhysXPhysicsSceneManagerPtr(m_SceneManager)->GetOffset();
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
