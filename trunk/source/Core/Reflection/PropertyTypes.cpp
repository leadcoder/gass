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


#include "Core/Reflection/PropertyTypes.h"

namespace GASS
{
//#ifndef WIN32
	// For some reason these deklarations must be in the cpp file to compile with gcc
	/*//template<class T> ePropertyType GASSCoreExport PropertyType<T>::m_TypeID = eptPTR;
	template<> ePropertyType PropertyType<bool>::m_TypeID		= eptBOOL;
	//template<> ePropertyType PropertyType<DWORD>::m_TypeID	= eptDWORD;
	template<> ePropertyType PropertyType<int>::m_TypeID		= eptINT;
	template<> ePropertyType PropertyType<float>::m_TypeID	= eptFLOAT;
	template<> ePropertyType PropertyType<std::string>::m_TypeID	= eptSTRING;
	template<> ePropertyType PropertyType<Vec3>::m_TypeID	= eptVEC3;*/
//#endif
}
