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

#ifndef ENUMLOOKUP_HH
#define ENUMLOOKUP_HH

#include "Core/Common.h"
#include <string>
#include <map>

namespace GASS
{

	class GASSCoreExport EnumLookup
	{

	public:

		EnumLookup();
		virtual ~EnumLookup();
		void Add(std::string name, int value);
		void Add(std::string name);
		int Get(std::string name);
	private:
		typedef std::map<std::string,int> ConstantMap;
		int m_NumConstants;
		ConstantMap m_ConstantMap;
	};
}

#endif // #ifndef ENUMLOOKUP_HH
