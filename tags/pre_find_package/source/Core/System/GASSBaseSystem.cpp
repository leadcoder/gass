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
#include "Core/Common.h"
#include "Core/System/GASSBaseSystem.h"
#include "tinyxml2.h"

namespace GASS
{
	BaseSystem::BaseSystem(void)
	{
	
	}

	BaseSystem::~BaseSystem(void)
	{
	
	}

	void BaseSystem::RegisterReflection()
	{
	}

	void BaseSystem::LoadXML(tinyxml2::XMLElement *xml_elem)
	{
		_LoadProperties(xml_elem);
	}

	void BaseSystem::SaveXML(tinyxml2::XMLElement *xml_elem)
	{
		tinyxml2::XMLElement * this_elem;
		this_elem = xml_elem->GetDocument()->NewElement(GetRTTI()->GetClassName().c_str());  
		xml_elem->LinkEndChild( this_elem );  
		_SaveProperties(this_elem);
	}
}




