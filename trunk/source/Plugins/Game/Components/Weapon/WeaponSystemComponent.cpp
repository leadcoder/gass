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

#include "WeaponSystemComponent.h"
#include "GameMessages.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
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
	WeaponSystemComponent::WeaponSystemComponent()  :m_ProjectileStartOffset(0),
		m_ProjectileStartVelocity (50),
		m_NumberOfMagazines (3),
		m_MagazineSize(30),
		m_RecoilForce(0,0,100),
		m_ReloadTime(0.3),
		m_FireDelay(0),
		m_ReadyToFire(true),
		m_AutoReload(true)
	{
		
		m_InputToFire = "Fire";
		m_InputToReload = "Reload";
		m_CurrentMagSize = 0;
		
		m_RoundOfFire = 1;
		m_Automatic = true;
		
		m_Reloading = false;
		m_FireRequest = false;
	}

	WeaponSystemComponent::~WeaponSystemComponent()
	{

	}

	void WeaponSystemComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("WeaponSystemComponent",new Creator<WeaponSystemComponent, IComponent>);
		RegisterProperty<std::string>("ProjectileTemplate", &WeaponSystemComponent::GetProjectileTemplate, &WeaponSystemComponent::SetProjectileTemplate);
		RegisterProperty<float>("ProjectileStartOffset", &WeaponSystemComponent::GetProjectileStartOffset, &WeaponSystemComponent::SetProjectileStartOffset);
		RegisterProperty<float>("ProjectileStartVelocity", &WeaponSystemComponent::GetProjectileStartVelocity, &WeaponSystemComponent::SetProjectileStartVelocity);
		RegisterProperty<int>("NumberOfMagazines", &WeaponSystemComponent::GetNumberOfMagazines, &WeaponSystemComponent::SetNumberOfMagazines);
		RegisterProperty<int>("MagazineSize", &WeaponSystemComponent::GetMagazineSize, &WeaponSystemComponent::SetMagazineSize);
		RegisterProperty<Vec3>("RecoilForce", &WeaponSystemComponent::GetRecoilForce, &WeaponSystemComponent::SetRecoilForce);
		RegisterProperty<float>("ReloadTime", &WeaponSystemComponent::GetReloadTime, &WeaponSystemComponent::SetReloadTime);
		
		//RegisterProperty<float>("FireDelay", &WeaponSystemComponent::GetFireDelay, &WeaponSystemComponent::SetFireDelay);
	}

	void WeaponSystemComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(WeaponSystemComponent::OnLoad,LoadSimComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WeaponSystemComponent::OnExecuteFire,FireMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WeaponSystemComponent::OnReload,ReloadMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WeaponSystemComponent::OnReadyToFire,ReadyToFireMessage,0));

		//SceneObjectPtr parent = boost::dynamic_pointer_cast<SceneObject>(GetSceneObject()->GetParent());
		GetSceneObject()->RegisterForMessage(REG_TMESS(WeaponSystemComponent::OnInput,PlayerInputMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WeaponSystemComponent::OnPhysicsMessage,VelocityNotifyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WeaponSystemComponent::OnTransformationChanged,TransformationNotifyMessage,0));
	}

	void WeaponSystemComponent::OnLoad(LoadSimComponentsMessagePtr message)
	{
		SceneObjectVector objs = GetSceneObject()->GetObjectsByName("FireSound",false);
		if(objs.size() >0)
			m_FireSound = objs.front();

		m_CurrentMagSize = m_MagazineSize;
	}

	void WeaponSystemComponent::OnPhysicsMessage(VelocityNotifyMessagePtr message)
	{
		m_CurrentVelocity = message->GetLinearVelocity();
	}

	void WeaponSystemComponent::OnReload(ReloadMessagePtr message)
	{
		m_NumberOfMagazines--;
		m_CurrentMagSize = m_MagazineSize;
		m_Reloading = false;
	}

	void WeaponSystemComponent::OnExecuteFire(FireMessagePtr message)
	{
		if(m_ReadyToFire && !m_Reloading)
		{
			//Send projectile if we have ammo
			m_FireRequest = false;
			if(m_CurrentMagSize > 0)
			{
				SpawnProjectile(m_ProjectileStartPos,m_ProjectileStartRot);
				m_CurrentMagSize--;
				
				float time_until_fire = 1.0f/m_RoundOfFire;
				MessagePtr ready_msg(new ReadyToFireMessage());
				ready_msg->SetDeliverDelay(time_until_fire);
				GetSceneObject()->PostMessage(ready_msg);
				m_ReadyToFire = false;
			}
			else if(m_AutoReload)
			{
				MessagePtr reload_msg(new ReloadMessage());
				reload_msg->SetDeliverDelay(m_ReloadTime);
				GetSceneObject()->PostMessage(reload_msg);
				m_Reloading = true;

				MessagePtr ready_msg(new ReadyToFireMessage());
				ready_msg->SetDeliverDelay(m_ReloadTime);
				GetSceneObject()->PostMessage(ready_msg);
			}

			
			
			/*if(m_Automatic)
			{
				float time_until_fire = 1.0f/m_RoundOfFire;
				MessagePtr execute_fire_msg(new Message(OBJECT_RM_FIRE));
				execute_fire_msg->SetDeliverDelay(time_until_fire);
				GetSceneObject()->PostMessage(execute_fire_msg);
			}*/
		}
	}

	

	void WeaponSystemComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		m_ProjectileStartPos = message->GetPosition();
		m_ProjectileStartRot = message->GetRotation();
	}

	void WeaponSystemComponent::SpawnProjectile(const Vec3 &projectile_start_pos,const Quaternion &projectile_rot)
	{
		//Vec3 fire_arm_dir = projectile_dir;
		//recoil
		/*if(m_TopPCO) 
		{
			m_TopPCO->ApplyForce(-fire_arm_dir*m_RecoilSize*10);
		}*/

		

		
		//Play fire sound
		if(m_FireSound)
		{
			MessagePtr sound_msg(new SoundParameterMessage(SoundParameterMessage::PLAY,0));
			m_FireSound->PostMessage(sound_msg);
		}



		Mat4 rotmat;
		projectile_rot.ToRotationMatrix(rotmat);
		Vec3 projectile_dir = -rotmat.GetViewDirVector();
		Vec3 vel = projectile_dir*m_ProjectileStartVelocity;
		Vec3 final_pos = projectile_start_pos +  projectile_dir*m_ProjectileStartOffset;

		MessagePtr spawn_msg(new SpawnObjectFromTemplateMessage(m_ProjectileTemplateName,final_pos,projectile_rot,vel));
		GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->PostMessage(spawn_msg);

		//recoil
		MessagePtr force_msg(new PhysicsBodyMessage(PhysicsBodyMessage::FORCE,m_RecoilForce));
		GetSceneObject()->PostMessage(force_msg);

	/*	SceneObjectPtr projectile = GetSceneObject()->GetSceneObjectManager()->LoadFromTemplate(m_ProjectileTemplateName);
		if(projectile)
		{
			int id = (int) this;
			MessagePtr pos_msg(new Message(SceneObject::OBJECT_RM_POSITION,id));
			pos_msg->SetData("Position",projectile_start_pos);
			MessagePtr rot_msg(new Message(SceneObject::OBJECT_RM_ROTATION,id));
			rot_msg->SetData("Rotation",Quaternion(projectile_rot));
			projectile->SendImmediate(pos_msg);
			projectile->SendImmediate(rot_msg);


			Mat4 rotmat;
			projectile_rot.ToRotationMatrix(rotmat);
			Vec3 projectile_dir = -rotmat.GetViewDirVector();
			
			//projectile->SetWeaponSystem(this);
			Vec3 vel = projectile_dir*m_ProjectileVelocity;

			MessagePtr vel_msg(new Message(SceneObject::OBJECT_RM_PHYSICS_BODY_PARAMETER));
			vel_msg->SetData("Parameter",SceneObject::VELOCITY);
			vel_msg->SetData("Value",vel);
			projectile->SendImmediate(vel_msg);
		

			//add platform velocity
			//vel += projectile_dir*m_CurrentVelocity.x
		
			//Send velocity message
		}*/
	}

	void WeaponSystemComponent::OnInput(PlayerInputMessagePtr message)
	{
		std::string name = message->GetController();
		float value = message->GetValue();
		if (name == m_InputToFire)
		{
			//send fire request
			if(value > 0)
			{
				m_FireRequest = true;
				MessagePtr fire_request_msg(new FireMessage());
				fire_request_msg->SetDeliverDelay(m_FireDelay);
				GetSceneObject()->PostMessage(fire_request_msg);
			}
			else 
			{
				m_FireRequest = false;
			}
		}
		else if(name == m_InputToReload)
		{
			if(!m_Reloading)
			{
				m_Reloading = true;
				MessagePtr reload_msg(new ReloadMessage());
				reload_msg->SetDeliverDelay(m_ReloadTime);
				GetSceneObject()->PostMessage(reload_msg);
			}
		}
	}

	void WeaponSystemComponent::OnReadyToFire(ReadyToFireMessagePtr message)
	{
		m_ReadyToFire = true;

		if(m_FireSound)
		{
			MessagePtr sound_msg(new SoundParameterMessage(SoundParameterMessage::STOP,0));
			m_FireSound->PostMessage(sound_msg);
		}
	}

	std::string WeaponSystemComponent::GetProjectileTemplate() const
	{
		return m_ProjectileTemplateName;
	}
	void WeaponSystemComponent::SetProjectileTemplate(const std::string &value)
	{
		m_ProjectileTemplateName = value;
	}

	float WeaponSystemComponent::GetProjectileStartOffset() const
	{
		return m_ProjectileStartOffset;
	}

	void WeaponSystemComponent::SetProjectileStartOffset(float offset) 
	{
		m_ProjectileStartOffset = offset;
	}

	int WeaponSystemComponent::GetNumberOfMagazines() const
	{
		return m_NumberOfMagazines;
	}

	void WeaponSystemComponent::SetNumberOfMagazines(int value) 
	{
		m_NumberOfMagazines = value;
	}

	int WeaponSystemComponent::GetMagazineSize() const
	{
		return m_MagazineSize;
	}

	void WeaponSystemComponent::SetMagazineSize(int value) 
	{
		m_MagazineSize = value;
	}

	void WeaponSystemComponent::SetProjectileStartVelocity(float value) 
	{
		m_ProjectileStartVelocity = value;
	}

	float WeaponSystemComponent::GetProjectileStartVelocity() const
	{
		return m_ProjectileStartVelocity;
	}

	void WeaponSystemComponent::SetRecoilForce(const Vec3 &value)
	{
		m_RecoilForce = value;
	}

	Vec3 WeaponSystemComponent::GetRecoilForce() const
	{
		return m_RecoilForce;
	}

	void WeaponSystemComponent::SetReloadTime(float value) 
	{
		m_ReloadTime = value;
	}

	float WeaponSystemComponent::GetReloadTime() const
	{
		return m_ReloadTime;
	}




	

}




