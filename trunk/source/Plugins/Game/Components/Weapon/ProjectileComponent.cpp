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

#include "ProjectileComponent.h"
#include "Plugins/Game/GameSceneManager.h"
#include "GameMessages.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"

#include "Sim/Systems/Resource/GASSIResourceSystem.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Scheduling/GASSIRuntimeController.h"



namespace GASS
{
	ProjectileComponent::ProjectileComponent() : m_MaxDamage(10000), m_ImpactForce(0,0,0)
	{
		m_TotSquaredDist = 0;
		m_ExplodeNearEnemyDistance = -1;
		m_TimeLeft = 0;
		m_TimeToLive = 4;
		m_DamgeRadius = -1;
		m_DieAfterColl = 1;
		m_Velocity.Set(0,-0.1,0);
		m_ColHandle = 0;
		m_HasColHandle = false;
		m_PhysicsDeltaTime = 0;
	}

	ProjectileComponent::~ProjectileComponent()
	{

	}

	void ProjectileComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("ProjectileComponent",new Creator<ProjectileComponent, IComponent>);
		RegisterProperty<std::string>("EndEffectTemplate", &ProjectileComponent::GetEndEffectTemplateName, &ProjectileComponent::SetEndEffectTemplateName);
		RegisterProperty<Vec3>("ImpactForce", &ProjectileComponent::GetImpactForce, &ProjectileComponent::SetImpactForce);
		RegisterProperty<float>("MaxDamage", &ProjectileComponent::GetMaxDamage, &ProjectileComponent::SetMaxDamage);
	}

	void ProjectileComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(ProjectileComponent::OnPositionMessage,PositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ProjectileComponent::OnRotationMessage,RotationMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(ProjectileComponent::OnPhysicsParameterMessage,PhysicsBodyMessage,0));

		m_TimeLeft = m_TimeToLive;
		//save for fast access
		m_ColSys = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<GASS::ICollisionSystem>();
		//register fot ticks
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<GameSceneManager>()->Register(shared_from_this());
	}

	void ProjectileComponent::OnDelete()
	{
		
	}

	void ProjectileComponent::OnPhysicsParameterMessage(PhysicsBodyMessagePtr message)
	{
		PhysicsBodyMessage::PhysicsBodyParameterType type = message->GetParameter();
		Vec3 value = message->GetValue();
		switch(type)
		{
		case PhysicsBodyMessage::FORCE:
			{
				//what to do?
			}
			break;
		case PhysicsBodyMessage::TORQUE:
			{
				//what to do?
			}
		case PhysicsBodyMessage::VELOCITY:
			{
				m_Velocity = value;
			}
		}
	}


	void ProjectileComponent::OnPositionMessage(PositionMessagePtr message)
	{
		if(message->GetSenderID() != (int) this)
			m_Pos = message->GetPosition();
	}

	void ProjectileComponent::OnRotationMessage(RotationMessagePtr message)
	{
		if(message->GetSenderID() != (int) this)
			m_Rot = message->GetRotation();
	}

	void ProjectileComponent::StepPhysics(double time)
	{
		Vec3 gravity;
		gravity.Set(0,-9.82,0);

		Vec3 ray_start; 
		Vec3 ray_end; 
		Vec3 ray_dir;
		Vec3 right,up;



		//calc some basic physics
		if(m_Velocity.SquaredLength() > 0.000001)
		{
			Mat4 rot_mat;
			//Create rotation matrix
			Vec3 drag = -m_Velocity;
			drag.Normalize();
			Vec3 dir = -drag;

			//right
			right.x = dir.z;
			right.y = 0;
			right.z = -dir.x;
			if(right.SquaredLength() > 0.00001)
			{
				right.FastNormalize();
			}
			else right.Set(0,0,1);
			//up
			up = Math::Cross(dir,right);
			up.Normalize();


			
			rot_mat.Identity();
			rot_mat.SetViewDirVector(-dir);
			rot_mat.SetRightVector(-right);
			rot_mat.SetUpVector(up);
			
			//air drag ~ v^2*k
			drag = drag* m_Velocity.SquaredLength()*time;

			m_Velocity = m_Velocity + gravity*time;// + drag*time;
			Vec3 new_pos = m_Pos + m_Velocity*time;

			//Send collision request
			ray_start = m_Pos; 
			ray_end = new_pos; 
			ray_dir = ray_end - ray_start;
			float l = ray_dir.FastLength();

			GASS::CollisionRequest request;

			request.LineStart = ray_start;
			request.LineEnd = ray_start + ray_dir;
			request.Type = COL_LINE;
			request.Scene = GetSceneObject()->GetScene();
			request.ReturnFirstCollisionPoint = false;
			request.CollisionBits = 1;
				

			m_ColHandle = m_ColSys->Request(request);
			m_HasColHandle = true;

			m_Pos = new_pos;
			//std::cout << "pos:" << m_Pos.x << " y"<< m_Pos.y << " z" << m_Pos.z<< std::endl;
			m_Rot.FromRotationMatrix(rot_mat);
		}
	}

	void ProjectileComponent::SceneManagerTick(double time)
	{
		//std::cout << "Update proj:" << GetSceneObject()->GetName() << std::endl;
		bool impact = false;
		m_TimeLeft -= time;
		m_PhysicsDeltaTime += time;
		if(m_TimeLeft < time && m_ExplodeNearEnemyDistance < 0) //do not explode on time stop if mine
		{
			//Check for expolsion
			if(m_DamgeRadius > 0)
			{
				impact = true;
			}
			else
			{
				MessagePtr remove_msg(new RemoveSceneObjectMessage(GetSceneObject()));
				GetSceneObject()->GetScene()->PostMessage(remove_msg);

				return;
			}
		}

		GASS::CollisionResult result;
		if(m_HasColHandle)
		{
			if(m_ColSys->Check(m_ColHandle,result))
			{
				m_HasColHandle = false;
				if(result.Coll)
				{
					if(m_DieAfterColl)	
						impact = true;
					//correct postition
					m_Pos = result.CollPosition;
				}
				//send position and rotaion update
				int id = (int) this;
				MessagePtr pos_msg(new PositionMessage(m_Pos,id));
				MessagePtr rot_msg(new RotationMessage(m_Rot,id));
				GetSceneObject()->PostMessage(pos_msg);
				GetSceneObject()->PostMessage(rot_msg);
			}
			else
				//wait for collision results
				return;
		}

		if(impact)
		{
			if(m_DamgeRadius > 0)
			{

			}
			else 
			{
				Vec3 proj_dir = m_Velocity;
				proj_dir.FastNormalize();

				float angle_falloff = fabs(Math::Dot(proj_dir,result.CollNormal));
				float damage_value = angle_falloff*m_MaxDamage;

				MessagePtr hit_msg(new HitMessage(damage_value,m_Pos,proj_dir));
				SceneObjectPtr(result.CollSceneObject)->PostMessage(hit_msg);

				//Send force message to indicate hit

				Vec3 force = proj_dir*m_ImpactForce;
				MessagePtr force_msg(new PhysicsBodyMessage(PhysicsBodyMessage::FORCE,force));
				SceneObjectPtr(result.CollSceneObject)->PostMessage(force_msg);
			}

			//GetSceneObject()->GetScene()->DeleteObject(GetSceneObject());
			MessagePtr remove_msg(new RemoveSceneObjectMessage(GetSceneObject()));
			GetSceneObject()->GetScene()->PostMessage(remove_msg);

			if(m_EndEffectTemplateName != "")
				SpawnEffect(m_EndEffectTemplateName);
			return;
		}

		//std::cout << "Step physics:" << GetSceneObject()->GetName() << std::endl;

		StepPhysics(m_PhysicsDeltaTime);
		m_PhysicsDeltaTime = 0;
	}

	void ProjectileComponent::SpawnEffect(const std::string &effect)
	{
		Vec3 vel(0,0,0);
		MessagePtr spawn_msg(new SpawnObjectFromTemplateMessage(effect,m_Pos,m_Rot,vel));
		GetSceneObject()->GetScene()->PostMessage(spawn_msg);
	}

	void ProjectileComponent::SetEndEffectTemplateName(const std::string &effect)
	{
		m_EndEffectTemplateName = effect;
	}

	std::string ProjectileComponent::GetEndEffectTemplateName() const 
	{
		return m_EndEffectTemplateName;
	}


	Vec3 ProjectileComponent::GetImpactForce() const
	{
		return m_ImpactForce;
	}
	void ProjectileComponent::SetImpactForce(const Vec3 &value)
	{
		m_ImpactForce = value;
	}

	float ProjectileComponent::GetMaxDamage() const
	{
		return m_MaxDamage;
	}

	void ProjectileComponent::SetMaxDamage(float value)
	{
		m_MaxDamage = value;
	}
}
