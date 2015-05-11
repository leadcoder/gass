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

#ifndef ARMOR_COMPONENT_H
#define ARMOR_COMPONENT_H

#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Plugins/Game/GameMessages.h"

namespace GASS
{

	class SceneObject;
	typedef SPTR<SceneObject> SceneObjectPtr;
	typedef WPTR<SceneObject> SceneObjectWeakPtr;

	/**
		Simple armor model used to keep track of the objects current armor state
		By listing to hit messages (sent by ProjectileComponent) armor is decreased on hit.
		When no armor left a OutOfArmor messages is sent and optional damage model will displayed
	*/

	class ArmorComponent :  public Reflection<ArmorComponent,BaseSceneComponent>
	{
	public:
		ArmorComponent();
		virtual ~ArmorComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
	private:
		void OnLoad(LocationLoadedEventPtr message);
		void OnHit(HitMessagePtr message);
		void OnOutOfArmor(OutOfArmorMessagePtr message);
		float GetArmor() const; 
		void SetArmor(float value);
		std::string GetDamageMesh() const;
		void SetDamageMesh(const std::string &value);
		std::string GetDamageEffect1() const;
		void SetDamageEffect1(const std::string &name);
		

		ADD_PROPERTY(float,OutOfArmorForce);
		float m_Armor;
		float m_CurrentArmor;
		std::string m_DamageMesh;
		std::string m_DamageEffect1;
	};
}
#endif
