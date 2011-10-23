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
#include "Sim/Scenario/Scene/SceneObjectLink.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Core/Common.h"
/*#include "Core/Serialize/Serialize.h"
#include <iostream>
#include <iomanip>
#include <tinyxml.h>
*/
namespace GASS
{
	SceneObjectLink::SceneObjectLink() : m_Initialized(false)
	{

	}

	SceneObjectLink::~SceneObjectLink(void)
	{
		
	}

	void SceneObjectLink::Initlize(SceneObjectPtr owner)
	{
		m_Owner = owner;
		if(m_LinkObjectID != "")
		{
			SceneObjectPtr obj = owner->GetObjectUnderRoot()->GetChildByID(m_LinkObjectID);
			if(obj)
				m_Link = obj;
			else 
				Log::Error("No object found with id %s",m_LinkObjectID.c_str());
		}
		else 
			Log::Error("No id specified for SceneObjectLink");
		m_Initialized = true;
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




