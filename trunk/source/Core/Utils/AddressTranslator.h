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

#pragma warning (disable : 4786)  

#include "Core/Common.h"
#include <map>

namespace GASS
{


	class GASSCoreExport AddressTranslator
	{
	public:
		static void AddAddress(void * oldAddress, void * newAddress);
		static void * TranslateAddress(void * oldAddress);
		static void Reset();

	private:
		//typedef std::hash_map<int, int> AddressTable;
		typedef std::map<int, int> AddressTable;
		static AddressTable m_AddressTable;
	};
}


