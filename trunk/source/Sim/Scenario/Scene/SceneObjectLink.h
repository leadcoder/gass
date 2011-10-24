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

#pragma once

#include "Sim/Common.h"
#include "Core/Reflection/BaseReflectionObject.h"
#include "Core/ComponentSystem/BaseComponentContainer.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scenario/Scene/Messages/CoreScenarioSceneMessages.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Components/BaseSceneComponent.h"
namespace GASS
{
	class GASSExport SceneObjectLink 
	{
		friend class BaseSceneComponent;
	public:
		SceneObjectLink();
		virtual ~SceneObjectLink();
		SceneObjectPtr operator ->()
		{
			return SceneObjectPtr(m_Link,boost::detail::sp_nothrow_tag());
		}
		bool IsValid() const {return SceneObjectPtr(m_Link,boost::detail::sp_nothrow_tag());}
	protected:
		friend std::ostream& operator << (std::ostream& os, const SceneObjectLink& sol)
		{
			os << sol.m_LinkObjectID;
			return os;
		}

		friend std::istream& operator >> (std::istream& os, SceneObjectLink& sol)
		{
			os >> sol.m_LinkObjectID;
			return os;
		}
		void Initlize(SceneObjectPtr owner);
		SceneObjectPtr GetOwnerObject() const;
		void UpdateLink();
		//SceneObjectPtr FindLinkObject(const std::string &name) const;
		
		SceneObjectWeakPtr m_Link;
		SceneObjectWeakPtr m_Owner;
		SceneObjectID m_LinkObjectID;
		bool m_Initialized;
	};
}
