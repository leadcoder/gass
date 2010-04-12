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

#ifndef WEAPON_SYSTEM_COMPONENT_H
#define WEAPON_SYSTEM_COMPONENT_H

#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Common.h"
#include "Core/Math/Quaternion.h"
#include "Plugins/Game/GameMessages.h"


namespace GASS
{

	class SceneObject;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class WeaponSystemComponent : public Reflection<WeaponSystemComponent,BaseSceneComponent>
	{
	public:
		WeaponSystemComponent();
		virtual ~WeaponSystemComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	private:
		void OnLoad(LoadGameComponentsMessagePtr message);
		void OnInput(ControllerMessagePtr message);
		void OnExecuteFire(FireMessagePtr message);
		void OnReload(ReloadMessagePtr message);
		void OnReadyToFire(ReadyToFireMessagePtr message);
		
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		void OnPhysicsMessage(VelocityNotifyMessagePtr message);
		void SpawnProjectile(const Vec3 &projectile_start_pos,const Quaternion &projectile_rot);

		//get/set section
		float GetProjectileStartOffset() const;
		void SetProjectileStartOffset(float offset);
		std::string GetProjectileTemplate() const;
		void SetProjectileTemplate(const std::string &value);
		int GetNumberOfMagazines() const;
		void SetNumberOfMagazines(int value);
		int GetMagazineSize() const;
		void SetMagazineSize(int value) ;
		void SetProjectileStartVelocity(float value);
		float GetProjectileStartVelocity() const;
		void SetRecoilForce(const Vec3 &value);
		Vec3 GetRecoilForce() const;
		float GetReloadTime() const;
		void SetReloadTime(float offset);
		
		

		


		//Projectile* m_ProjectileTemplate;
		SceneObjectPtr m_FireSound;
		/*SceneObjectPtr m_ReloadPatch; 
		SceneObjectPtr m_ReleasePatch;
		SceneObjectPtr m_ShellBouncePatch; 
		SceneObjectPtr m_MachineGunDistance;
		SceneObjectPtr m_FireLoopPatch; */


		Vec3 m_ProjectilePosition;
		std::string m_ProjectileTemplateName;
		std::string m_InputToFire;
		std::string m_InputToReload;

		//float m_FireingForce;
		//float m_RecoilSpeed;
		//float m_RecoilSize;
		//float m_ReloadTimeRemaining;

		//float m_Velocity;
		//int m_FireOnce;
		//int m_FireInCameraDof;
		//float m_MinDamage;
		//float m_DistToStartLoseDamage;
		//float m_DistToMinDamage;
		Vec3 m_RecoilForce;
		float m_RoundOfFire;
		float m_FireDelay;
		int m_NumberOfMagazines;
		int m_MagazineSize;
		float m_ReloadTime;
		bool m_Automatic;
		//helpers
		bool m_FireRequest;
		bool m_AutoReload;
		bool m_Reloading;
		int m_CurrentMagSize;
		//int m_CurrentNumOfMag;

		//helpers
		bool m_ReadyToFire;

		Vec3 m_CurrentVelocity;
		float m_ProjectileStartVelocity;
		float m_ProjectileStartOffset;

		Quaternion m_ProjectileStartRot;
		Vec3 m_ProjectileStartPos;


	};
}
#endif
