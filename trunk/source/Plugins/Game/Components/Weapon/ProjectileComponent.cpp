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
#include "GameMessages.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"



namespace GASS
{
	ProjectileComponent::ProjectileComponent() 
	{
		m_Hit = false;
		m_TotSquaredDist = 0;
		m_ExplodeNearEnemyDistance = -1;
		m_TimeLeft = 0;
		m_TimeToLive = 4;
		m_DamgeRadius = -1;
		m_DieAfterColl = 1;
		m_Velocity.Set(0,-0.1,0);
		m_MatID = -1;
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
	}

	void ProjectileComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_LOAD_SIM_COMPONENTS, MESSAGE_FUNC(ProjectileComponent::OnLoad));
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_UNLOAD_COMPONENTS, MESSAGE_FUNC(ProjectileComponent::OnUnload));
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_POSITION, MESSAGE_FUNC( ProjectileComponent::OnPositionMessage));
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_ROTATION, MESSAGE_FUNC( ProjectileComponent::OnRotationMessage));
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_PHYSICS_BODY_PARAMETER,  MESSAGE_FUNC(ProjectileComponent::OnPhysicsParameterMessage));
	}

	void ProjectileComponent::OnLoad(MessagePtr message)
	{
		if(m_StartEffectTemplateName != "") 
		{
			/*EffectBundle* effect = (EffectBundle*) Root::Get().GetBaseObjectTemplateManager()->CreateFromTemplate(m_StartEffectTemplateName );
			if(effect)
			{
				Log::Print("Effect loaded:%s",m_StartEffectTemplateName.c_str());
				effect->Init();
				//ISceneNode* root = Root::GetPtr()->GetLevel()->GetParticleManager()->GetRoot();
				AddChild(effect);
				effect->Restart();
			}*/
		}
		m_TimeLeft = m_TimeToLive;
		SimEngine::GetPtr()->GetRuntimeController()->Register(this);

		//for fast access
		m_ColSys = SimEngine::GetPtr()->GetSystemManager()->GetFirstSystem<GASS::ICollisionSystem>();

		std::cout << "Loaded:" << GetSceneObject()->GetName() << std::endl;
		//Send delete message?
	}

	
	void ProjectileComponent::OnUnload(MessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
		std::cout << "Unloaded:" << GetSceneObject()->GetName() << std::endl;
	}

	void ProjectileComponent::OnPhysicsParameterMessage(MessagePtr message)
	{
		SceneObject::PhysicsBodyParameterType type = boost::any_cast<SceneObject::PhysicsBodyParameterType>(message->GetData("Parameter"));
		switch(type)
		{
		case SceneObject::FORCE:
			{
				Vec3 value = boost::any_cast<Vec3>(message->GetData("Value"));
				//what to do?
			}
			break;
		case SceneObject::TORQUE:
			{
				Vec3 value = boost::any_cast<Vec3>(message->GetData("Value"));
				//what to do?
			}
		case SceneObject::VELOCITY:
			{
				m_Velocity = boost::any_cast<Vec3>(message->GetData("Value"));
			}
		}
	}


	void ProjectileComponent::OnPositionMessage(MessagePtr message)
	{
		if(message->GetSenderID() != (int) this)
			m_Pos = boost::any_cast<Vec3>(message->GetData("Position"));
	}

	void ProjectileComponent::OnRotationMessage(MessagePtr message)
	{
		if(message->GetSenderID() != (int) this)
			m_Rot = boost::any_cast<Quaternion>(message->GetData("Rotation"));
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
			rot_mat.m_Data2[8] = dir.x;
			rot_mat.m_Data2[9] = dir.y;
			rot_mat.m_Data2[10] = dir.z;

			rot_mat.m_Data2[0] = right.x;
			rot_mat.m_Data2[1] = right.y;
			rot_mat.m_Data2[2] = right.z;

			rot_mat.m_Data2[4] = up.x;
			rot_mat.m_Data2[5] = up.y;
			rot_mat.m_Data2[6] = up.z;

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
			request.Scene = GetSceneObject()->GetSceneObjectManager()->GetScenarioScene();
			request.ReturnFirstCollisionPoint = false;

			m_ColHandle = m_ColSys->Request(request);
			m_HasColHandle = true;
			
			m_Pos = new_pos;
			//std::cout << "pos:" << m_Pos.x << " y"<< m_Pos.y << " z" << m_Pos.z<< std::endl;
			m_Rot.FromRotationMatrix(rot_mat);
		}
	}

	void ProjectileComponent::Update(double time)
	{
		std::cout << "Update proj:" << GetSceneObject()->GetName() << std::endl;
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
				//GetSceneObject()->GetSceneObjectManager()->DeleteObject(GetSceneObject());
				MessagePtr remove_msg(new Message(ScenarioScene::SCENARIO_RM_REMOVE_OBJECT));
				remove_msg->SetData("SceneObject",GetSceneObject());
				GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->PostMessage(remove_msg);

				return;
			}
		}

		if(m_HasColHandle)
		{
			GASS::CollisionResult result;
			if(m_ColSys->Check(m_ColHandle,result))
			{
				m_HasColHandle = false;
				if(result.Coll)
				{
					m_Hit = true;
					if(m_DieAfterColl)	
						impact = true;
					//correct postition
					m_Pos = result.CollPosition;
				}
				//send position and rotaion update
				int id = (int) this;
				MessagePtr pos_msg(new Message(SceneObject::OBJECT_RM_POSITION,id));
				pos_msg->SetData("Position",m_Pos);
				MessagePtr rot_msg(new Message(SceneObject::OBJECT_RM_ROTATION,id));
				rot_msg->SetData("Rotation",m_Rot);
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
			
			}
			//GetSceneObject()->GetSceneObjectManager()->DeleteObject(GetSceneObject());
			MessagePtr remove_msg(new Message(ScenarioScene::SCENARIO_RM_REMOVE_OBJECT));
			remove_msg->SetData("SceneObject",GetSceneObject());
			GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->PostMessage(remove_msg);


			SpawnEffect("strf9040ProjectileEffect");

			return;
		}

		//std::cout << "Step physics:" << GetSceneObject()->GetName() << std::endl;

		StepPhysics(m_PhysicsDeltaTime);
		m_PhysicsDeltaTime = 0;
	}

	TaskGroup ProjectileComponent::GetTaskGroup() const
	{
		return "PROJECTILE_TASK_GROUP";
	}

	void ProjectileComponent::SpawnEffect(const std::string &effect)
	{
		Vec3 vel(0,0,0);
		MessagePtr spawn_msg(new Message(ScenarioScene::SCENARIO_RM_SPAWN_OBJECT_FROM_TEMPLATE));
		spawn_msg->SetData("Template",effect);
		spawn_msg->SetData("Position",m_Pos);
		spawn_msg->SetData("Rotation",m_Rot);
		spawn_msg->SetData("Velocity",vel);
		GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->PostMessage(spawn_msg);
	}
}
