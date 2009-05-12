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

#include "Core/Reflection/AbstractProperty.h"

namespace GASS
{

	class BaseReflectionObject;
	/** TypedProperty This intermediate class defines a property that is typed, but not bound as a member of a particular
	 class.
	*/
	template <class T>
	class TypedProperty: public AbstractProperty
	{

	public:

		/**
		 Constructor. Takes in property name.
		**/
		inline TypedProperty( const char* szName );

		/**
		 Returns the type of this property.
		 */
		virtual ePropertyType	GetType() const;

		/**
		Determines the value of this property.
		*/
		virtual T		GetValue( BaseReflectionObject* pObject )const = 0;
		virtual void	SetValue( BaseReflectionObject* pObject,const  T &Value ) = 0;

	};

	template <class T>
	inline TypedProperty<T>::TypedProperty( const char* szName ) :
	AbstractProperty( szName			)
	{
	}

	template <class T>
	ePropertyType TypedProperty<T>::GetType() const
	{
		return PropertyType<T>::GetTypeID();
	}

	template<class T> ePropertyType PropertyType<T>::m_TypeID = eptPTR;
	template<> ePropertyType PropertyType<bool>::m_TypeID		= eptBOOL;
	//template<> ePropertyType PropertyType<DWORD>::m_TypeID	= eptDWORD;
	template<> ePropertyType PropertyType<int>::m_TypeID		= eptINT;
	template<> ePropertyType PropertyType<float>::m_TypeID	= eptFLOAT;
	template<> ePropertyType PropertyType<std::string>::m_TypeID	= eptSTRING;
	template<> ePropertyType PropertyType<Vec3>::m_TypeID	= eptVEC3;

}
