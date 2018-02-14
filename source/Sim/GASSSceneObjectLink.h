/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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

#include "Sim/GASSCommon.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/ComponentSystem/GASSComponentContainer.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSBaseSceneComponent.h"

namespace GASS
{
	static const SceneObjectID UNKNOWN_LINK_ID = "";	

	class GASSExport SceneObjectLink 
	{
		friend class BaseSceneComponent;
	public:
		SceneObjectLink();
		virtual ~SceneObjectLink();
		SceneObjectPtr operator ->()
		{
			return m_Link.lock();
		}
		bool IsValid() const { if (m_Link.lock()) return true; else return false; }
		SceneObjectID GetLinkObjectID() const {return m_LinkObjectID;}
		SceneObjectPtr GetObjectPtr() const {return m_Link.lock();}
	protected:
		SceneObjectPtr GetRoot(SceneObjectPtr obj) const;
		friend std::ostream& operator << (std::ostream& os, const SceneObjectLink& sol)
		{
			os << sol.m_LinkObjectID;
			return os;
		}

		friend std::istream& operator >> (std::istream& os, SceneObjectLink& sol)
		{
			os >> sol.m_LinkObjectID;
			if(!sol.UpdateLink())
				std::cout << "SceneObjectLink - Faild to update link " << sol.m_LinkObjectID << "\n";
			return os;
		}
		bool Initlize(SceneObjectPtr owner);
		bool UpdateLink();
		SceneObjectPtr GetOwnerObject() const;
		
		SceneObjectWeakPtr m_Link;
		SceneObjectWeakPtr m_Owner;
		SceneObjectID m_LinkObjectID;
		bool m_Initialized;
	};
}
