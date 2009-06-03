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
#pragma once
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
	template <class OwnerType, class T>
	class Property : public TypedProperty<T>
	{

	public:

		//----------------------------------------------------------------------------------------------
		typedef T		(OwnerType::*GetterType)() const;				// Getter function
		typedef void	(OwnerType::*SetterType)( T Value);	// Setter function
		typedef void	(OwnerType::*SetterTypeConst)( const T &Value );	// Setter function

		//----------------------------------------------------------------------------------------------
		// Constructor. Takes in property's name, getter and setter functions.
		inline Property( const char* szName, GetterType Getter, SetterType Setter );
		inline Property( const char* szName, GetterType Getter, SetterTypeConst Setter );

		//----------------------------------------------------------------------------------------------
		// Determines the value of this property.
		virtual T		GetValue( BaseReflectionObject* pObject ) const;
		virtual void	SetValue( BaseReflectionObject* pObject, const T &Value );
		void Serialize(BaseReflectionObject* pObject,ISerializer* serializer);

		void SetValueByString(BaseReflectionObject* pObject, const std::string &s);
		std::string GetValueAsString(BaseReflectionObject* pObject);
		void SetValue(BaseReflectionObject* dest, BaseReflectionObject* src);
		void SetValue(BaseReflectionObject* pOwner, boost::any &attribute);
		void GetValue(BaseReflectionObject* pOwner, boost::any &attribute);
	protected:
		std::vector<std::string> Tokenize(const std::string & str, const std::string & delim);


		GetterType		m_Getter;
		SetterType		m_Setter;
		SetterTypeConst	m_SetterConst;

	};

	template <class OwnerType, class T>
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
	}

	//Do nothing by default
	template <class type>
	bool GetValueFromString(type &res,const std::string &s)
	{
		std::stringstream str;
		str << s;
		str >> res;
		//SetValue(pOwner,res);
		return true;
	}
	//Use specialized template to catch std::string
	template <>
	bool GASSCoreExport GetValueFromString<std::string>(std::string &res,const std::string &s);

	//Use specialized template to catch vector
	template <>
	bool GASSCoreExport GetValueFromString<std::vector<int> >(std::vector<int> &res,const std::string &s);

	//Use specialized template to catch vector
	template <>
	bool GASSCoreExport GetValueFromString<std::vector<float> >(std::vector<float> &res,const std::string &s);

	//Use specialized template to catch vector
	template <>
	bool GASSCoreExport GetValueFromString<std::vector<double> >(std::vector<double> &res,const std::string &s);

	//Use specialized template to catch vector
	template <>
	bool GASSCoreExport GetValueFromString<std::vector<std::string> >(std::vector<std::string> &res,const std::string &s);


	template <class OwnerType, class T>
	void Property<OwnerType, T>::SetValueByString(BaseReflectionObject* pOwner,const std::string &s)
	{
		T res;
		GetValueFromString<T>(res,s);
		SetValue(pOwner,res);
	}




	template <class type>
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

	template <class OwnerType, class T>
	void Property<OwnerType, T>::GetValue(BaseReflectionObject* pOwner, boost::any &attribute)
	{
		T res = GetValue(pOwner);
		attribute = res;
	}

	template <class OwnerType, class T>
	std::vector<std::string> Property<OwnerType, T>::Tokenize(const std::string & str, const std::string & delim)
	{
		using namespace std;
		vector<string> tokens;
		size_t p0 = 0, p1 = string::npos;
		while(p0 != string::npos)
		{
			p1 = str.find_first_of(delim, p0);
			if(p1 != p0)
			{
				string token = str.substr(p0, p1 - p0);
				tokens.push_back(token);
			}
			p0 = str.find_first_not_of(delim, p1);
		}
		return tokens;
	}

	template <class OwnerType, class T>
	void Property<OwnerType, T>::Serialize(BaseReflectionObject* pOwner,ISerializer* serializer)
	{

		if(serializer->Loading())
		{
			T val;
			serializer->IO(val);
			SetValue(pOwner,val);
		}
		else
		{
			T val = GetValue(pOwner);
			serializer->IO(val);
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
	}
}
