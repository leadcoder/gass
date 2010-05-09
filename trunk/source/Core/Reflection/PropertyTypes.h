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

/*
This class is based on the Game Programming Gems 5 article
"Using Templates for Reflection in C++" by Dominic Filion.
*/


#ifndef PROPERTYTYPES_HH
#define PROPERTYTYPES_HH

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
		ePropertyType	GetTypeID() const;

	//private:

		//static ePropertyType    m_TypeID;

	};


/*	template<class T>
	ePropertyType GetPropID(T v)
	{
		return -1;
	}

	template<>
	ePropertyType GASSCoreExport GetPropID<bool>(bool v)
	{
		return eptBOOL;
	}

	template<>
	ePropertyType GASSCoreExport GetPropID<int>(int v)
	{
		return eptINT;
	}

	template<>
	ePropertyType  GASSCoreExport GetPropID<float>(float v)
	{
		return eptFLOAT;
	}

	template<>
	ePropertyType GASSCoreExport GetPropID<std::string>(std::string v)
	{
		return eptSTRING;
	}

	template<>
	ePropertyType GASSCoreExport GetPropID<Vec3>(Vec3 v)
	{
		return eptVEC3;
	}*/



	template<class T>
    ePropertyType PropertyType<T>::GetTypeID()const
	{
	   // T v;
		//return GetPropID<T>(v);
		return eptVEC3;
	}



}
#endif //PROPERTYTYPES_HH
