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
#include "tinyxml.h"

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
		RegisterProperty<std::string>( "Name", &GASS::BaseSystem::GetName, &GASS::BaseSystem::SetName);
	}

	void BaseSystem::LoadXML(TiXmlElement *xml_elem)
	{
		LoadProperties(xml_elem);
	}

	void BaseSystem::SaveXML(TiXmlElement *xml_elem)
	{
		TiXmlElement * this_elem;
		this_elem = new TiXmlElement( GetRTTI()->GetClassName().c_str());  
		xml_elem->LinkEndChild( this_elem );  
		SaveProperties(this_elem);
	}

	int BaseSystem::GetUpdateBucket() const
	{
		return 0;
	}

	void BaseSystem::SetUpdateBucket(int priority)
	{

	}

}




