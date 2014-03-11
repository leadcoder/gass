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
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"

#include "Sim/Messages/GASSSoundSceneObjectMessages.h"

#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"

#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/GASSGraphicsMesh.h"

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
		m_AutoReload(true),
		m_1FP(false)
	{

		m_FireController = "Fire";
		m_ReloadController= "Reload";
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
		RegisterProperty<float>("RoundOfFire", &WeaponSystemComponent::GetRoundOfFire, &WeaponSystemComponent::SetRoundOfFire);
		RegisterProperty<int>("CurrentMagazineSize", &WeaponSystemComponent::GetCurrentMagazineSize, &WeaponSystemComponent::SetCurrentMagazineSize);
		RegisterProperty<SceneObjectRef>("FireSoundObject1P", &WeaponSystemComponent::GetFireSoundObject1P, &WeaponSystemComponent::SetFireSoundObject1P);
		RegisterProperty<SceneObjectRef>("FireSoundObject3P", &WeaponSystemComponent::GetFireSoundObject3P, &WeaponSystemComponent::SetFireSoundObject3P);
		
		RegisterProperty<std::string>("FireEffectTemplate", &WeaponSystemComponent::GetFireEffectTemplate, &WeaponSystemComponent::SetFireEffectTemplate);
		RegisterProperty<std::string>("FireController", &WeaponSystemComponent::GetFireController, &WeaponSystemComponent::SetFireController);
		RegisterProperty<std::string>("ReloadController", &WeaponSystemComponent::GetReloadController, &WeaponSystemComponent::SetReloadController);
	}

	void WeaponSystemComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(WeaponSystemComponent::OnExecuteFire,FireMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WeaponSystemComponent::OnReload,ReloadMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WeaponSystemComponent::OnReadyToFire,ReadyToFireMessage,0));

		//SceneObjectPtr parent = boost::dynamic_pointer_cast<SceneObject>(GetSceneObject()->GetParent());
		GetSceneObject()->RegisterForMessage(REG_TMESS(WeaponSystemComponent::OnInput,InputRelayEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WeaponSystemComponent::OnPhysicsMessage,VelocityNotifyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WeaponSystemComponent::OnTransformationChanged,TransformationNotifyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WeaponSystemComponent::OnLODChange,LODMessage,0));
		BaseSceneComponent::OnInitialize();
	
		/*m_FireSoundObject1F = GetSceneObject()->GetFirstChildByName("FireSound1P",false);
		m_FireSoundObject3F = GetSceneObject()->GetFirstChildByName("FireSound3P",false);
		
		if(!m_FireSound1Fp)
		{
			m_FireSound1Fp = GetSceneObject()->GetFirstChildByName("FireSound",false);
			m_FireSound3Fp = m_FireSound1Fp;
		}*/

		m_CurrentMagSize = m_MagazineSize;

		//cross hair?
		/*ManualMeshDataPtr mesh_data(new ManualMeshData());
		MeshVertex vertex;
		sub_mesh_data->MaterialName = "WhiteTransparentNoLighting";
		mesh_data->ScreenSpace = true;

		vertex.TexCoord.Set(0,0);
		vertex.Color = Vec4(1,1,1,1);
		vertex.Normal = Vec3(0,1,0);
		mesh_data->Type = LINE_LIST;
		std::vector<Vec3> conrners;

		Vec2 size;
		size.x =0.5;
		size.y =0.5;
		conrners.push_back(Vec3( size.x ,size.y, 0));
		conrners.push_back(Vec3(-size.x,size.y, 0));
		conrners.push_back(Vec3(-size.x,size.y,0));
		conrners.push_back(Vec3( size.x,size.y,0));
		
		for(int i = 0; i < 4; i++)
		{
			Vec3 pos =conrners[i];
			sub_mesh_data->PositionVector.push_back(pos);
			pos =conrners[i];
			sub_mesh_data->PositionVector.push_back(pos);
		}
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		GetSceneObject()->PostMessage(mesh_message);*/
	}

	void WeaponSystemComponent::OnLODChange(LODMessagePtr message)
	{
		if(message->GetLevel() == LODMessage::LOD_HIGH)
		{
			m_1FP = true;
			//std::cout << "fP1" << std::endl;
		}
		else
		{
			m_1FP = false; 
		}
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
		if(m_FireRequest && m_ReadyToFire && !m_Reloading)
		{
			//Send projectile if we have ammo
			//m_FireRequest = false;
			if(m_CurrentMagSize > 0)
			{
				SpawnProjectile(m_ProjectileStartPos,m_ProjectileStartRot);
				m_CurrentMagSize--;

				float time_until_fire = 1.0f/m_RoundOfFire;
				ReadyToFireMessagePtr ready_msg(new ReadyToFireMessage());
				ready_msg->SetDeliverDelay(time_until_fire);
				GetSceneObject()->PostEvent(ready_msg);
				m_ReadyToFire = false;
			}
			else if(m_AutoReload)
			{
				ReloadMessagePtr reload_msg(new ReloadMessage());
				reload_msg->SetDeliverDelay(m_ReloadTime);
				GetSceneObject()->PostRequest(reload_msg);
				m_Reloading = true;

				ReadyToFireMessagePtr ready_msg(new ReadyToFireMessage());
				ready_msg->SetDeliverDelay(m_ReloadTime);
				GetSceneObject()->PostEvent(ready_msg);
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
		if(m_FireSoundObject1P.IsValid() && m_1FP)
		{
			m_FireSoundObject1P->PostRequest(SoundParameterMessagePtr(new SoundParameterMessage(SoundParameterMessage::PLAY,0)));
			//std::cout << "fire fP1" << std::endl;
		}
		else if(m_FireSoundObject3P.IsValid())
		{
			SoundParameterMessagePtr sound_msg(new SoundParameterMessage(SoundParameterMessage::PLAY,0));
			m_FireSoundObject3P->PostRequest(sound_msg);
			//std::cout << "fire fP3" << std::endl;
		}

		Mat4 rotmat;
		projectile_rot.ToRotationMatrix(rotmat);
		Vec3 projectile_dir = -rotmat.GetViewDirVector();
		Vec3 vel = projectile_dir*m_ProjectileStartVelocity;
		Vec3 final_pos = projectile_start_pos +  projectile_dir*m_ProjectileStartOffset;

		SceneMessagePtr spawn_msg(new SpawnObjectFromTemplateRequest(m_ProjectileTemplateName,final_pos,projectile_rot,vel));
		GetSceneObject()->GetScene()->PostMessage(spawn_msg);

		//recoil
		
		GetSceneObject()->PostRequest(PhysicsBodyAddForceRequestPtr(new PhysicsBodyAddForceRequest(m_RecoilForce)));

		//effect
		if(m_FireEffectTemplate != "")
		{
			SceneMessagePtr spawn_msg(new SpawnObjectFromTemplateRequest(m_FireEffectTemplate,Vec3(0,0,0),Quaternion::IDENTITY,vel,GetSceneObject()));
			//MessagePtr spawn_msg(new SpawnObjectFromTemplateRequest(m_FireEffectTemplate,projectile_start_pos,projectile_rot,vel));
			GetSceneObject()->GetScene()->PostMessage(spawn_msg);
		}


		ParticleSystemParameterMessagePtr particle_msg(new ParticleSystemParameterMessage(ParticleSystemParameterMessage::EMISSION_RATE,0,m_RoundOfFire));
		GetSceneObject()->PostRequest(particle_msg);
		ParticleSystemParameterMessagePtr particle_msg2(new ParticleSystemParameterMessage(ParticleSystemParameterMessage::EMISSION_RATE,0,0));
		particle_msg2->SetDeliverDelay(0.5);
		GetSceneObject()->PostRequest(particle_msg2);

	/*	SceneObjectPtr projectile = GetSceneObject()->GetScene()->LoadObjectFromTemplate(m_ProjectileTemplateName);
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

	void WeaponSystemComponent::OnInput(InputRelayEventPtr message)
	{
		std::string name = message->GetController();
		float value = message->GetValue();
		//std::cout << "Fire:" << name << "  " << m_FireController << std::endl;
		if (name == m_FireController)
		{
			//send fire request
			if(value > 0)
			{
				m_FireRequest = true;
				FireMessagePtr fire_request_msg(new FireMessage());
				fire_request_msg->SetDeliverDelay(m_FireDelay);
				GetSceneObject()->PostRequest(fire_request_msg);
			}
			else
			{
				m_FireRequest = false;
			}
		}
		else if(name == m_ReloadController)
		{
			if(!m_Reloading)
			{
				m_Reloading = true;
				ReloadMessagePtr reload_msg(new ReloadMessage());
				reload_msg->SetDeliverDelay(m_ReloadTime);
				GetSceneObject()->PostRequest(reload_msg);
			}
		}
	}

	void WeaponSystemComponent::OnReadyToFire(ReadyToFireMessagePtr message)
	{
		m_ReadyToFire = true;

		if(m_FireSoundObject1P.IsValid())
		{
			SoundParameterMessagePtr sound_msg(new SoundParameterMessage(SoundParameterMessage::STOP,0));
			m_FireSoundObject1P->PostRequest(sound_msg);
		}

		if(m_FireSoundObject3P.IsValid())
		{
			SoundParameterMessagePtr sound_msg(new SoundParameterMessage(SoundParameterMessage::STOP,0));
			m_FireSoundObject3P->PostRequest(sound_msg);
		}

		if(m_Automatic && m_FireRequest)
		{
		    //std::cout << "send new fire" << std::endl;
            FireMessagePtr fire_request_msg(new FireMessage());
            fire_request_msg->SetDeliverDelay(m_FireDelay);
            GetSceneObject()->PostRequest(fire_request_msg);
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

	std::string WeaponSystemComponent::GetFireEffectTemplate() const
	{
		return m_FireEffectTemplate;
	}

	void WeaponSystemComponent::SetFireEffectTemplate(const std::string &value)
	{
		m_FireEffectTemplate = value;
	}
	
    void WeaponSystemComponent::SetRoundOfFire(float value)
	{
		m_RoundOfFire = value;
	}

	float WeaponSystemComponent::GetRoundOfFire() const
	{
		return m_RoundOfFire;
	}

	int WeaponSystemComponent::GetCurrentMagazineSize() const
	{
		return m_CurrentMagSize;
	}

	void WeaponSystemComponent::SetCurrentMagazineSize(int value)
	{
		m_CurrentMagSize = value;
	}

}




