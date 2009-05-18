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

#include "Core/Utils/EnumLookup.h"
#include "Core/Utils/Misc.h"

namespace GASS
{
	
	EnumLookup::EnumLookup()
	{
		m_NumConstants = 0;
	}

	EnumLookup::~EnumLookup()
	{

	}

	void EnumLookup::Add(std::string name, int value)
	{
		m_ConstantMap[Misc::ToLower(name)] = value;
		m_NumConstants++;
	}

	void EnumLookup::Add(std::string name)
	{
		m_ConstantMap[Misc::ToLower(name)] = m_NumConstants;
		m_NumConstants++;
	}

	int EnumLookup::Get(std::string name)
	{
		ConstantMap::iterator pos;
		name = Misc::ToLower(name);
		pos = m_ConstantMap.find(name);

		if (pos != m_ConstantMap.end()) //in map.
		{
			return m_ConstantMap[name];
		}
		else
		{
			return -1;
		}
	}

	
}
