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


#ifndef TYPEPROPERTY_HH
#define TYPEPROPERTY_HH

#include "Core/Reflection/AbstractProperty.h"
#include "Core/Reflection/PropertyTypes.h"
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
		virtual ePropertyType	GetTypeID() const;

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

		template<class T>
	ePropertyType GetPropID(T v)
	{
		return eptBOOL;
	}

/*	template<>
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
	}
*/




	template <class T>
	ePropertyType TypedProperty<T>::GetTypeID() const
	{
	    //PropertyType<T> ptype;
		//return ptype.GetTypeID();
		//T v;
		//GetPropID<T>(v);
		if(typeid(T) == typeid(int))
		{
		   return eptINT;
		}
		else if(typeid(T) == typeid(float))
		{
		   return eptFLOAT;
		}
		else if(typeid(T) == typeid(std::string))
        {
		   return eptSTRING;
		}
		else if(typeid(T) == typeid(Vec2))
        {
		   return eptVEC2;
		}
		else if(typeid(T) == typeid(Vec3))
        {
		   return eptVEC3;
		}
		else if(typeid(T) == typeid(bool))
        {
		   return eptBOOL;
		}
		else
			return eptUNKNOWN;
	}




#ifdef WIN32
/*	template<class T> ePropertyType PropertyType<T>::m_TypeID = eptPTR;
	template<> ePropertyType PropertyType<bool>::m_TypeID		= eptBOOL;
	//template<> ePropertyType PropertyType<DWORD>::m_TypeID	= eptDWORD;
	template<> ePropertyType PropertyType<int>::m_TypeID		= eptINT;
	template<> ePropertyType PropertyType<float>::m_TypeID	= eptFLOAT;
	template<> ePropertyType PropertyType<std::string>::m_TypeID	= eptSTRING;
	template<> ePropertyType PropertyType<Vec3>::m_TypeID	= eptVEC3;*/
#endif
}
#endif // TYPEPROPERTY_HH
