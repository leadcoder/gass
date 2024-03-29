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

#ifndef ENUMLOOKUP_HH
#define ENUMLOOKUP_HH

#include "Core/Common.h"
#include <map>

namespace GASS
{

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Utility
	*  @{
	*/



    /**
    Table used to map strings to unique integers
    It's also possible to map custom int values to
    specific enums.
    */

	class GASSCoreExport EnumLookup
	{

	public:
		/**
            Add string to table
            @param name Name of enum
            @param value Value to map enum to
		*/
		void Add(std::string name, int value);

		/**
            Add string to table and give this enum
            a unique int value
            @param name Name of enum
 		*/
		void Add(std::string name);

		/**
            Get int value maped to enum
            @param name Name of enum
			@return signed in if valid enum name provided, otherwise -1 
 		*/
		int Get(const std::string &name);
	private:
		using ConstantMap = std::map<std::string, int>;
		int m_NumConstants = 0;
		ConstantMap m_ConstantMap;
	};
}

#endif // #ifndef ENUMLOOKUP_HH
