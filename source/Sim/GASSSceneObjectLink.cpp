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
#include "Sim/GASSSceneObjectLink.h"
#include "Sim/GASSSceneObject.h"
#include "Core/Common.h"

namespace GASS
{
	SceneObjectLink::SceneObjectLink() : m_Initialized(false),
		m_LinkObjectID(UNKNOWN_LINK_ID)
	{

	}

	SceneObjectLink::~SceneObjectLink(void)
	{
		
	}

	bool SceneObjectLink::UpdateLink()
	{
		if(m_Initialized)
		{
			SceneObjectPtr owner = m_Owner.lock();
			if(owner && m_LinkObjectID != UNKNOWN_LINK_ID)
			{
				SceneObjectPtr obj;
				if(GetRoot(owner)->GetID()  == m_LinkObjectID)
					obj = GetRoot(owner);
				else obj = GetRoot(owner)->GetChildByID(m_LinkObjectID);

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


	SceneObjectPtr SceneObjectLink::GetRoot(SceneObjectPtr obj) const
	{
		ComponentContainerPtr container = obj;

		while(container->GetParent())
		{
			container = ComponentContainerPtr(container->GetParent());
		}
		return  GASS_STATIC_PTR_CAST<SceneObject>(container);
	}


	bool SceneObjectLink::Initlize(SceneObjectPtr owner)
	{
		m_Owner = owner;
		m_Initialized = true;
		return UpdateLink();
	}

	/*SceneObjectPtr SceneObjectLink::GetLinkObject() const
	{
		return m_Link.lock();
	}*/

	SceneObjectPtr SceneObjectLink::GetOwnerObject() const
	{
		return m_Owner.lock();
	}
}




