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
#include "Sim/Scenario/Scene/GASSSceneObjectLink.h"
#include "Sim/Scenario/Scene/GASSSceneObject.h"
#include "Core/Common.h"

namespace GASS
{
	SceneObjectLink::SceneObjectLink() : m_Initialized(false),
		m_LinkObjectID(UNKOWN_LINK_ID)
	{

	}

	SceneObjectLink::~SceneObjectLink(void)
	{
		
	}

	bool SceneObjectLink::UpdateLink()
	{
		if(m_Initialized)
		{
			SceneObjectPtr owner(m_Owner,boost::detail::sp_nothrow_tag());
			if(owner && m_LinkObjectID != UNKOWN_LINK_ID)
			{
				SceneObjectPtr obj;
				if(owner->GetObjectUnderRoot()->GetID()  == m_LinkObjectID)
					obj = owner->GetObjectUnderRoot();
				else obj = owner->GetObjectUnderRoot()->GetChildByID(m_LinkObjectID);

				if(obj)
					m_Link = obj;
				else 
				{
					return false;
				}
			}
			else 
			{
				return false;
			}
		}
		return true;
	}

	bool SceneObjectLink::Initlize(SceneObjectPtr owner)
	{
		m_Owner = owner;
		m_Initialized = true;
		return UpdateLink();
	}

	/*SceneObjectPtr SceneObjectLink::GetLinkObject() const
	{
		return SceneObjectPtr(m_Link,boost::detail::sp_nothrow_tag());
	}*/

	SceneObjectPtr SceneObjectLink::GetOwnerObject() const
	{
		return SceneObjectPtr(m_Owner,boost::detail::sp_nothrow_tag());
	}
}



