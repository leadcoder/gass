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

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Property
//
// Fully defined property class. Specifies property's name, type and the getter/setters are bound as
// members of a specific class type.
//
// Template parameter OwnerType is the class that the getter and setter are a member of and template
// parameter T is the property type.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace GASS
{

    template <class type>
    bool GetValueFromString(type &res,const std::string &s)
    {
        std::stringstream str;
        str << s;
        str >> res;
        return true;
    }

    //Use specialized template to catch std::string
    template <>
    bool GASSCoreExport GetValueFromString<std::string>(std::string &res,const std::string &s);


    template <class type>
    bool GetStringFromValue(const type &val,std::string &res)
    {
        std::stringstream sstream;
        sstream.unsetf(std::ios::skipws);
        sstream << val;
        res = sstream.str();
        return true;
    }

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

        virtual T GetValue(BaseReflectionObject* object) const
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

        std::string GetValueAsString(BaseReflectionObject* object)
        {
            T val = GetValue(object);
            std::string res;
            GetStringFromValue<T>(val,res);
            return res;
        }

        void SetValue(BaseReflectionObject* dest, BaseReflectionObject* src)
        {
            SetValue(dest,GetValue(src));
        }
        void SetValue(BaseReflectionObject* object, boost::any &value)
        {
            T res = boost::any_cast<T>(value);
            SetValue(object,res);
        }

        void GetValue(BaseReflectionObject* object, boost::any &value)
        {
            T res = GetValue(object);
            value = res;
        }
    protected:
        GetterType		m_Getter;
        SetterType		m_Setter;
        SetterTypeConst	m_SetterConst;
    };

    /*template <class OwnerType, class T>
    inline Property<OwnerType, T>::Property( const char* szName, GetterType Getter, SetterType Setter ) :
    TypedProperty<T>	( szName		),
    	m_Getter			( Getter		),
    	m_Setter			( Setter		),
    	m_SetterConst			( NULL)
    {
    }

    template <class OwnerType, class T>
    inline Property<OwnerType, T>::Property( const char* szName, GetterType Getter, SetterTypeConst Setter ) :
    TypedProperty<T>	( szName		),
    	m_Getter			( Getter		),
    	m_SetterConst			( Setter		),
    	m_Setter			( NULL)
    {
    }

    template <class OwnerType, class T>
    T Property<OwnerType, T>::GetValue( BaseReflectionObject* pOwner ) const
    {
    	return (((OwnerType*)pOwner)->*m_Getter)();
    }

    template <class OwnerType, class T>
    void Property<OwnerType, T>::SetValue(BaseReflectionObject* pOwner, const T &Value )
    {
    	if(m_SetterConst)
    	{
    		(((OwnerType*)pOwner)->*m_SetterConst)( Value );
    	}
    	else if(m_Setter)
    	{
    		(((OwnerType*)pOwner)->*m_Setter)( Value );
    	}
    }

    template <class OwnerType, class T>
    void Property<OwnerType, T>::SetValue(BaseReflectionObject* dest, BaseReflectionObject* src)
    {
    	SetValue(dest,GetValue(src));
    }

    template <class OwnerType, class T>
    void Property<OwnerType, T>::SetValue(BaseReflectionObject* pOwner, boost::any &attribute)
    {
    	T res = boost::any_cast<T>(attribute);
    	SetValue(pOwner,res);
    }*/



    //Do nothing by default
    /*	template <class type>
    	bool GetValueFromString(type &res,const std::string &s)
    	{
    		std::stringstream str;
    		str << s;
    		str >> res;
    		//SetValue(pOwner,res);
    		return true;
    	}*/

    //Use specialized template to catch std::string
    /*template <>
    bool GASSCoreExport GetValueFromString<std::string>(std::string &res,const std::string &s)
    {
    	res = s;
    	return true;
    }*/


    /*template <class OwnerType, class T>
    void Property<OwnerType, T>::SetValueByString(BaseReflectionObject* pOwner,const std::string &s)
    {
    	T res;
    	GetValueFromString<T>(res,s);
    	SetValue(pOwner,res);
    }*/

    /*template <class type>
    bool GetStringFromValue(const type &val,std::string &res)
    {
    	std::stringstream sstream;
    	sstream.unsetf(std::ios::skipws);
    	sstream << val;
    	res = sstream.str();
    	return true;
    }

    template <>
    bool GASSCoreExport GetStringFromValue<std::vector<std::string> >(const std::vector<std::string> &val,std::string &s);
    template <>
    bool GASSCoreExport GetStringFromValue<std::vector<int> >(const std::vector<int> &val,std::string &s);
    template <>
    bool GASSCoreExport GetStringFromValue<std::vector<float> >(const std::vector<float> &val,std::string &s);
    template <>
    bool GASSCoreExport GetStringFromValue<std::vector<double> >(const std::vector<double> &val,std::string &s);

    template <class OwnerType, class T>
    std::string Property<OwnerType, T>::GetValueAsString(BaseReflectionObject* pOwner)
    {
    	T val = GetValue(pOwner);
    	std::string res;
    	GetStringFromValue<T>(val,res);
    	return res;
    }
    */
    /*	template <class OwnerType, class T>
    	void Property<OwnerType, T>::GetValue(BaseReflectionObject* pOwner, boost::any &attribute)
    	{
    		T res = GetValue(pOwner);
    		attribute = res;
    	}*/

    /*template <class OwnerType, class T>
    void Property<OwnerType, T>::Serialize(BaseReflectionObject* pOwner,ISerializer* serializer)
    {

    	if(serializer->Loading())
    	{
    		T val;
    		SerialLoader* loader = (SerialLoader*) serializer;
    		loader->IO<T>(val);
    		SetValue(pOwner,val);
    	}
    	else
    	{
    		T val = GetValue(pOwner);
    		SerialSaver* saver = (SerialSaver*) serializer;
    		saver->IO<T>(val);

    	}
    }


    template <class type>
    bool GetVectorFromString(std::vector<type> &res,const std::string &s)
    {
    	res.clear();
    	std::stringstream str(s);
    	type value;
    	while(str >> value)
    	{
    		res.push_back(value);
    	}
    	return true;
    }

    template <class type>
    bool GetStringFromVector(const std::vector<type> &val,std::string &s)
    {
    	std::string str_val;
    	for(int i = 0 ; i < val.size(); i++)
    	{
    		if(i > 0 )
    			s += " ";
    		GetStringFromValue(val[i],str_val);
    		s += str_val;
    	}
    	return true;
    }*/
}
#endif
