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

#ifndef ABSTRACTPROPERTY_HH
#define ABSTRACTPROPERTY_HH

#include <boost/any.hpp>
#include "Core/Common.h"
#include "Core/Reflection/PropertyTypes.h"

namespace GASS
{
	class ISerializer;
	class BaseReflectionObject;
	/**
        Interface class for all properties
	*/
	class GASSCoreExport AbstractProperty
	{
	public:
        /**
            Constructor
            @param name Name of this property
        */
        inline AbstractProperty(const std::string &name);
		inline  std::string GetName() const;
		virtual ePropertyType GetTypeID() const = 0;

		/**
            Set value of this property by string
            @param object The object that is owner of this property
            @param value The value provided as string
        */
		virtual void SetValueByString(BaseReflectionObject* object,const std::string &s) = 0;

		/**
            Get the value of this property
            @param object The object that is owner of this property
        */
    	virtual std::string GetValueAsString(BaseReflectionObject* pObject) = 0;
		virtual void Serialize(BaseReflectionObject* pObject, ISerializer* serializer) = 0;
		virtual void SetValue(BaseReflectionObject* dest, BaseReflectionObject* src) = 0;
		virtual void SetValue(BaseReflectionObject* pObject, boost::any &attribute) = 0;
		virtual void GetValue(BaseReflectionObject* pObject, boost::any &attribute) = 0;
	protected :
		std::string	m_Name;
	};

	inline AbstractProperty::AbstractProperty( const std::string  &name ) :
		m_Name(name)
	{
	}
	inline std::string AbstractProperty::GetName() const
	{
		return m_Name;
	}
}

#endif // #ifndef ABSTRACTPROPERTY_HH
