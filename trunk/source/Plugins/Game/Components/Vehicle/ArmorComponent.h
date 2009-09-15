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

#ifndef ARMOR_COMPONENT_H
#define ARMOR_COMPONENT_H

#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Common.h"
#include "Core/MessageSystem/Message.h"

namespace GASS
{

	class SceneObject;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class ArmorComponent :  public Reflection<ArmorComponent,BaseSceneComponent>
	{
	public:
		ArmorComponent();
		virtual ~ArmorComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	private:
		void OnHit(MessagePtr message);
		float GetArmor() const; 
		void SetArmor(float value);
		float m_Armor;
		float m_CurrentArmor;
	};
}
#endif
