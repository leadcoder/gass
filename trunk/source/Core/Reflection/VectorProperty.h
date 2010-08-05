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


#ifndef VECTOR_PROPERTY_HH
#define VECTOR_PROPERTY_HH

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

	class IVectorProperty
	{
	public:
		virtual ~IVectorProperty(){}
		virtual void SetValueByStringVector(BaseReflectionObject* pOwner, const std::vector<std::string> &s) = 0;
		virtual std::vector<std::string> GetValueAsStringVector(BaseReflectionObject* pOwner) = 0;
	};
	

	template <class OwnerType, class T>
	class VectorProperty : public TypedProperty<std::vector<T> > , IVectorProperty
	{

	public:

		//----------------------------------------------------------------------------------------------
		typedef std::vector<T>		(OwnerType::*GetterType)() const;				// Getter function
		typedef void	(OwnerType::*SetterType)( std::vector<T> Value);	// Setter function
		typedef void	(OwnerType::*SetterTypeConst)( const std::vector<T> &Value );	// Setter function

		//----------------------------------------------------------------------------------------------
		// Constructor. Takes in property's name, getter and setter functions.
		inline VectorProperty( const char* szName, GetterType Getter, SetterType Setter );
		inline VectorProperty( const char* szName, GetterType Getter, SetterTypeConst Setter );

		//----------------------------------------------------------------------------------------------
		// Determines the value of this property.
		virtual std::vector<T>		GetValue( BaseReflectionObject* pObject ) const;
		virtual void	SetValue( BaseReflectionObject* pObject, const std::vector<T> &Value );
		void Serialize(BaseReflectionObject* pObject,ISerializer* serializer);

		void SetValueByString(BaseReflectionObject* pOwner, const std::string &s);
		std::string GetValueAsString(BaseReflectionObject* pOwner);
		void SetValue(BaseReflectionObject* dest, BaseReflectionObject* src);
		void SetValue(BaseReflectionObject* pOwner, boost::any &attribute);
		void GetValue(BaseReflectionObject* pOwner, boost::any &attribute);


		void SetValueByStringVector(BaseReflectionObject* pOwner, const std::vector<std::string> &s);
		std::vector<std::string> GetValueAsStringVector(BaseReflectionObject* pOwner);
	protected:
		//std::vector<std::string> Tokenize(const std::string & str, const std::string & delim);


		GetterType		m_Getter;
		SetterType		m_Setter;
		SetterTypeConst	m_SetterConst;

	};

	template <class OwnerType, class T>
	inline VectorProperty<OwnerType, T>::VectorProperty( const char* szName, GetterType Getter, SetterType Setter ) :
	TypedProperty<std::vector<T> >	( szName		),
		m_Getter			( Getter		),
		m_Setter			( Setter		),
		m_SetterConst			( NULL)
	{
	}

	template <class OwnerType, class T>
	inline VectorProperty<OwnerType, T>::VectorProperty( const char* szName, GetterType Getter, SetterTypeConst Setter ) :
	TypedProperty<std::vector<T> >	( szName		),
		m_Getter			( Getter		),
		m_SetterConst			( Setter		),
		m_Setter			( NULL)
	{
	}

	template <class OwnerType, class T>
	std::vector<T> VectorProperty<OwnerType, T>::GetValue( BaseReflectionObject* pOwner ) const
	{
		return (((OwnerType*)pOwner)->*m_Getter)();
	}

	template <class OwnerType, class T>
	void VectorProperty<OwnerType, T>::SetValue(BaseReflectionObject* pOwner, const std::vector<T> &Value )
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
	void VectorProperty<OwnerType, T>::SetValue(BaseReflectionObject* dest, BaseReflectionObject* src)
	{
		SetValue(dest,GetValue(src));
	}

	template <class OwnerType, class T>
	void VectorProperty<OwnerType, T>::SetValue(BaseReflectionObject* pOwner, boost::any &attribute)
	{
		std::vector<T> res = boost::any_cast<std::vector<T> >(attribute);
		SetValue(pOwner,res);
	}


	/*template <class type>
	bool GetValueFromString(type &res,const std::string &s)
	{
		std::stringstream str;
		str << s;
		str >> res;
		//SetValue(pOwner,res);
		return true;
	}*/

	template <class OwnerType, class T>
	void VectorProperty<OwnerType, T>::SetValueByString(BaseReflectionObject* pOwner,const std::string &s)
	{
		std::vector<T> res;

		std::stringstream str(s);
		T value;
		while(str >> value)
		{
			res.push_back(value);
		}

		//GetValueFromString<T>(res,s);
		SetValue(pOwner,res);
	}

	template <class OwnerType, class T>
	std::string VectorProperty<OwnerType, T>::GetValueAsString(BaseReflectionObject* pOwner)
	{
		std::vector<T> val = GetValue(pOwner);
		std::string res;

		for(int i = 0 ; i < val.size(); i++)
		{
			std::string str_val;
			if(i > 0 )
				res += " ";

			std::stringstream sstream;
			sstream.unsetf(std::ios::skipws);
			sstream << val[i];
			str_val = sstream.str();


			//GetStringFromValue(val[i],str_val);
			res += str_val;
		}
		return res;
	}


	template <class OwnerType, class T>
	void VectorProperty<OwnerType, T>::SetValueByStringVector(BaseReflectionObject* pOwner, const std::vector<std::string> &s)
	{
		std::vector<T> res;
		for(int i = 0 ; i < s.size(); i++)
		{
			T value;
			std::stringstream ss(s[i]);
			ss >> value;
			res.push_back(value);
		}
		SetValue(pOwner,res);
	}

	template <class OwnerType, class T>
	std::vector<std::string> VectorProperty<OwnerType, T>::GetValueAsStringVector(BaseReflectionObject* pOwner)
	{
		std::vector<T> val = GetValue(pOwner);
		std::vector<std::string> res;

		for(int i = 0 ; i < val.size(); i++)
		{
			std::string str_val;
			std::stringstream sstream;
			sstream.unsetf(std::ios::skipws);
			sstream << val[i];
			str_val = sstream.str();
			res.push_back(str_val);
		}
		return res;
	}

	template <class OwnerType, class T>
	void VectorProperty<OwnerType, T>::GetValue(BaseReflectionObject* pOwner, boost::any &attribute)
	{
		std::vector<T> res = GetValue(pOwner);
		attribute = res;
	}

	template <class OwnerType, class T>
	void VectorProperty<OwnerType, T>::Serialize(BaseReflectionObject* pOwner,ISerializer* serializer)
	{

		if(serializer->Loading())
		{
			std::vector<T> val;

			int num_val;

			SerialLoader* loader = (SerialLoader*) serializer;
			loader->IO<int>(num_val);
			for(int i  = 0 ; i < num_val; i++)
			{
				T value;

				loader->IO<T>(value);
				val.push_back(value);
			}
			SetValue(pOwner,val);
		}
		else
		{
			SerialSaver* saver = (SerialSaver*) serializer;

			std::vector<T> val = GetValue(pOwner);
			int num_val = val.size();
			saver->IO<int>(num_val);

			for(int i = 0 ; i < val.size(); i++)
			{
				saver->IO<T>(val[i]);
			}
		}
	}
}
#endif // #ifndef PROPERTY_HH
