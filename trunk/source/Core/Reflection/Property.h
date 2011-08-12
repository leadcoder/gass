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


#ifndef PROPERTY_HH
#define PROPERTY_HH

#include <boost/any.hpp>
#include "Core/Common.h"
#include "Core/Serialize/Serialize.h"
#include "Core/Reflection/TypedProperty.h"
#include <assert.h>
#include <sstream>
#include <vector>

namespace GASS
{



    template <class type>
    bool GetValueFromString(type &res,const std::string &s)
    {
        std::stringstream str;
		str.setf(0,std::ios::floatfield);
		str.precision(10);

		//str.setf(std::ios::boolalpha);
        str << s;
        str >> res;
        return true;
    }

    //Use specialized template to catch std::string
    template <>
    bool GASSCoreExport GetValueFromString<std::string>(std::string &res,const std::string &s);


	//Use specialized template to catch float
    template <>
    bool GASSCoreExport GetValueFromString<float>(float &res,const std::string &s);

	//Use specialized template to catch bool
    template <>
    bool GASSCoreExport GetValueFromString<bool>(bool &res,const std::string &s);


    template <class type>
    bool GetStringFromValue(const type &val,std::string &res)
    {
        std::stringstream sstream;
        sstream.unsetf(std::ios::skipws);
		sstream.setf(std::ios::boolalpha);
		sstream.setf(0,std::ios::floatfield);
		sstream.precision(10);
		
        sstream << val;
        res = sstream.str();
        return true;
    }

	//Use specialized template to catch float
    template <>
    bool GASSCoreExport GetStringFromValue<float>(const float &val, std::string &res);

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Reflection
	*  @{
	*/

    /**
    Template class used to define a property of a specific type.
    @param OwnerType class that has the getter and setter functions
    @param T Poperty type
    */
    template <class OwnerType, class T>
    class Property : public TypedProperty<T>
    {
    public:
        typedef T (OwnerType::*GetterType)() const; // Getter function
        typedef void (OwnerType::*SetterType)( T Value); // Setter function
        typedef void (OwnerType::*SetterTypeConst)( const T &Value ); // Const setter function
        Property( const std::string &name, GetterType getter, SetterType setter ):
                TypedProperty<T>(name),
                m_Getter(getter),
                m_Setter(setter),
                m_SetterConst(NULL)
        {

        }
        Property( const std::string &name, GetterType getter, SetterTypeConst setter ):
                TypedProperty<T>(name),
                m_Getter(getter),
                m_SetterConst(setter),
                m_Setter(NULL)
        {
        }

        virtual T GetValue(const BaseReflectionObject* object) const
        {
            return (((OwnerType*)object)->*m_Getter)();
        }

        virtual void SetValue( BaseReflectionObject* object, const T &value )
        {
            if (m_SetterConst)
            {
                (((OwnerType*)object)->*m_SetterConst)( value );
            }
            else if (m_Setter)
            {
                (((OwnerType*)object)->*m_Setter)( value );
            }
        }

        void Serialize(BaseReflectionObject* object,ISerializer* serializer)
        {
            if (serializer->Loading())
            {
                T val;
                SerialLoader* loader = (SerialLoader*) serializer;
                loader->IO<T>(val);
                SetValue(object,val);
            }
            else
            {
                T val = GetValue(object);
                SerialSaver* saver = (SerialSaver*) serializer;
                saver->IO<T>(val);
            }
        }

        void SetValueByString(BaseReflectionObject* object, const std::string &value)
        {
            T res;
            GetValueFromString<T>(res,value);
            SetValue(object,res);
        }

        std::string GetValueAsString(const BaseReflectionObject* object) const 
        {
            T val = GetValue(object);
            std::string res;
            GetStringFromValue<T>(val,res);
            return res;
        }

        void SetValue(BaseReflectionObject* dest, const BaseReflectionObject* src)
        {
            SetValue(dest,GetValue(src));
        }
        void SetValue(BaseReflectionObject* object, boost::any &value)
        {
            T res = boost::any_cast<T>(value);
            SetValue(object,res);
        }

        void GetValue(const BaseReflectionObject* object, boost::any &value) const 
        {
            T res = GetValue(object);
            value = res;
        }
    protected:
        GetterType		m_Getter;
        SetterType		m_Setter;
        SetterTypeConst	m_SetterConst;
    };
}
#endif
