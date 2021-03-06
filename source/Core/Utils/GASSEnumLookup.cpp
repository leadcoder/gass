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

#include "GASSEnumLookup.h"
#include "GASSStringUtils.h"

namespace GASS
{
	void EnumLookup::Add(std::string name, int value)
	{
		m_ConstantMap[StringUtils::ToLower(name)] = value;
		m_NumConstants++;
	}

	void EnumLookup::Add(std::string name)
	{
		m_ConstantMap[StringUtils::ToLower(name)] = m_NumConstants;
		m_NumConstants++;
	}

	int EnumLookup::Get(const std::string &name)
	{
		const std::string lower_name = StringUtils::ToLower(name);
		const auto pos = m_ConstantMap.find(lower_name);

		if (pos != m_ConstantMap.end()) //in map.
		{
			return m_ConstantMap[lower_name];
		}
		else
		{
			return -1;
		}
	}


}
