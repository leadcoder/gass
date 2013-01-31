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

#ifndef GASS_ENUM_PROPERTY_H
#define GASS_ENUM_PROPERTY_H

#include "Core/Common.h"
#include "Core/Reflection/GASSProperty.h"

namespace GASS
{
	class BaseReflectionObject;
	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Reflection
	*  @{
	*/

	class IEnumProperty
	{
	public:
		virtual ~IEnumProperty(){}
		virtual std::vector<std::string> GetEnumList(const BaseReflectionObject* object, const std::string &options) const = 0;
		virtual bool IsMultiValue() const = 0;
	};

	/**
	Template class used to define a enum properties.
	@param OwnerType class that has the getter and setter functions
	@param T Enum Poperty Class
	*/
	template <class OwnerType, class T>
	class EnumProperty : public Property<OwnerType,T>, public IEnumProperty
	{
	public:
		EnumProperty(const std::string &name, GetterType getter, SetterType setter ): Property<OwnerType,T>(name,getter,setter)
		{
		}
		EnumProperty( const std::string &name, GetterType getter, SetterTypeConst setter ): Property<OwnerType,T>(name,getter,setter)
		{

		}

		virtual std::vector<std::string> GetEnumList(const BaseReflectionObject* object, const std::string &options) const
		{
			return T::GetAllOptions(object,options);
		}

		virtual bool IsMultiValue() const
		{
			return T::IsMultiValue();
		}
	};
}

#endif
