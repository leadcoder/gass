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
		 Constructor.
		 @param name Property name.
		**/
		inline TypedProperty(const std::string  &name );

		/**
		 Returns the type of this property.
		 */
		virtual PropertyType GetTypeID() const;

		/**
		Get the value of this property.
		@param object BaseReflectionObject that is owner of this property
		*/
		virtual T GetValue(BaseReflectionObject* object)const = 0;

		/**
		Set the value of this property.
		@param object BaseReflectionObject that is owner of this property
		*/
		virtual void	SetValue(BaseReflectionObject* object, const T &value) = 0;
	};

	template <class T>
	inline TypedProperty<T>::TypedProperty( const std::string &name ) :
	AbstractProperty(name)
	{
	}

	template <class T>
	PropertyType TypedProperty<T>::GetTypeID() const
	{

		if(typeid(T) == typeid(int))
		{
		   return PROP_INT;
		}
		else if(typeid(T) == typeid(float))
		{
		   return PROP_FLOAT;
		}
		else if(typeid(T) == typeid(std::string))
        {
		   return PROP_STRING;
		}
		else if(typeid(T) == typeid(Vec2))
        {
		   return PROP_VEC2;
		}
		else if(typeid(T) == typeid(Vec3))
        {
		   return PROP_VEC3;
		}
		else if(typeid(T) == typeid(Vec4))
        {
		   return PROP_VEC4;
		}
		else if(typeid(T) == typeid(bool))
        {
		   return PROP_BOOL;
		}
		else
			return PROP_UNKNOWN;
	}
}
#endif
