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

#ifndef GASS_TYPED_PROPERTY_H
#define GASS_TYPED_PROPERTY_H

#include "Core/Reflection/GASSIProperty.h"
#include "Core/Utils/GASSStringUtils.h"
namespace GASS
{
	class BaseReflectionObject;

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Reflection
	*  @{
	*/

	/** TypedProperty This intermediate class defines a property that is typed,
	but not bound as a member of a particular class.
	*/
	template <class T>
	class TypedProperty: public IProperty
	{

	public:

		/**
		 Constructor.
		 @param name Property name.
		**/
		TypedProperty(const std::string  &name ) : IProperty(name)	{}

		/**
		 Returns the type of this property.
		 */
		virtual const std::type_info* GetTypeID() const
		{
			return &typeid(T);
		}

		/**
		 Returns the name of this property.
		 */
		virtual std::string GetTypeName() const
		{
			return StringUtils::Demangle(std::string(typeid(T).name()));
		}

		/**
		Get the value of this property.
		@param object BaseReflectionObject that is owner of this property
		*/
		virtual T GetValue(const IPropertyOwner* object)const = 0;

		/**
		Set the value of this property.
		@param object BaseReflectionObject that is owner of this property
		*/
		virtual void SetValue(IPropertyOwner* object, const T &value) = 0;
	};

	/*template <class T>
	inline TypedProperty<T>::TypedProperty( const std::string &name ) :
	IProperty(name)
	{
	}


	template <class T>
	const std::type_info* TypedProperty<T>::GetTypeID() const
	{
		return &typeid(T);
	}

	/*template <class T>
	std::string TypedProperty<T>::GetTypeName() const
	{
		return StringUtils::Demangle(std::string(typeid(T).name()));
	}*/


}
#endif
