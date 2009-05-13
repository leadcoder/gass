/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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
#include <string>
#include "Core/Common.h"
#include "Core/Math/Vector.h"

namespace GASS
{
	enum ePropertyType
	{
		eptBOOL,
		//eptDWORD,
		eptINT,
		eptFLOAT,
		eptSTRING,
		eptPTR,
		eptVEC3,
		eptMAX_PROPERTY_TYPES
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// PropertyType
	//
	// This templatized class will associate compile-time types with unique enum members.
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////

	template <class T>
	class PropertyType
	{

	public :

		// Returns type ID associated with the templatized type.
		static	ePropertyType	GetTypeID();

	private:

		static			ePropertyType    m_TypeID;

	};

	template<class T>
	ePropertyType PropertyType<T>::GetTypeID()
	{
		return m_TypeID;
	}

}
