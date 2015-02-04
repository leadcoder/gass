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

#ifndef GASS_VECTOR_PROPERTY_H
#define GASS_VECTOR_PROPERTY_H

#include "Core/Common.h"
#include "Core/Serialize/GASSSerialize.h"
#include "Core/Reflection/GASSTypedProperty.h"
#include <sstream>
#define VEC_ELEM_SEP ";"

namespace GASS
{


	/**
		Interface for properties based on the std::vector class
	*/
	class IVectorProperty
	{
	public:
		virtual ~IVectorProperty(){}
		/**
		Set values by string
		*/
		virtual void SetValueByStringVector(BaseReflectionObject* object, const std::vector<std::string> &value) = 0;
		/**
		Get values by string
		*/
		virtual std::vector<std::string> GetValueAsStringVector(const BaseReflectionObject* object) const = 0;
	};


	/**
	Template class used to define a vector property of a specific type.
	@param OwnerType class that has the getter and setter functions
	@param T Property type
	*/

	template <class OwnerType, class T>
	class VectorProperty : public TypedProperty<std::vector<T> > , public IVectorProperty
	{

	public:
		typedef std::vector<T> (OwnerType::*GetterType)() const;				// Getter function
		typedef void (OwnerType::*SetterType)( std::vector<T> Value);	// Setter function
		typedef void (OwnerType::*SetterTypeConst)( const std::vector<T> &Value );	// cpnst setter function
		VectorProperty(const std::string &name, GetterType getter, SetterType setter,PropertyMetaDataPtr meta_data):
		TypedProperty<std::vector<T> >(name),
			m_Getter(getter),
			m_Setter(setter),
			m_SetterConst(NULL),
			m_MetaData(meta_data)
		{

		}
		VectorProperty(const std::string &name, GetterType getter, SetterTypeConst setter,PropertyMetaDataPtr meta_data):
		TypedProperty<std::vector<T> >(name),
			m_Getter(getter),
			m_SetterConst(setter),
			m_Setter(NULL),
			m_MetaData(meta_data)
		{
		}

		virtual std::vector<T> GetValue(const BaseReflectionObject* object) const
		{
			return (((OwnerType*)object)->*m_Getter)();
		}

		virtual void SetValue(BaseReflectionObject* object, const std::vector<T> &value)
		{
			if (m_SetterConst)
			{
				(((OwnerType*)object)->*m_SetterConst)(value);
			}
			else if (m_Setter)
			{
				(((OwnerType*)object)->*m_Setter)(value);
			}
		}

		virtual const std::type_info* GetTypeID() const
		{
			return &typeid(T);
		}


		void Serialize(BaseReflectionObject* object,ISerializer* serializer)
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
				SetValue(object,val);
			}
			else
			{
				SerialSaver* saver = (SerialSaver*) serializer;

				std::vector<T> val = GetValue(object);
				int num_val = static_cast<int>( val.size());
				saver->IO<int>(num_val);

				for(size_t i = 0 ; i < val.size(); i++)
				{
					saver->IO<T>(val[i]);
				}
			}
		}

		void SetValueByString(BaseReflectionObject* object, const std::string &value)
		{
			try
			{
				std::vector<T> res;
				T out_value;
				std::string temp = value;
				std::string::size_type pos = temp.find(VEC_ELEM_SEP,0);
				while(pos != std::string::npos)
				{
					const std::string elem_str = temp.substr(0,pos);
					std::stringstream str(elem_str);
					str >> out_value;
					res.push_back(out_value);
					temp = temp.substr(pos+1);
					pos = temp.find(VEC_ELEM_SEP,0);
				}
				if(temp != "") //last element
				{
					std::stringstream str(temp);
					str >> out_value;
					res.push_back(out_value);
				}

				/*while(str >> out_value)
				{
					res.push_back(out_value);
				}*/

				SetValue(object,res);
			}
			catch(...)
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed to set property:" + IProperty::m_Name + " With value:" + value,"VectorProperty::SetValueByString");
			}
		}

		std::string GetValueAsString(const BaseReflectionObject* object) const
		{
			std::vector<T> val = GetValue(object);
			std::string res;

			for(size_t i = 0 ; i < val.size(); i++)
			{
				std::string str_val;
				if(i > 0 )
					res += VEC_ELEM_SEP;

				std::stringstream sstream;
				sstream.unsetf(std::ios::skipws);
				sstream << val[i];
				str_val = sstream.str();
				res += str_val;
			}
			return res;
		}

		void SetValue(BaseReflectionObject* dest, const BaseReflectionObject* src)
		{
			SetValue(dest,GetValue(src));
		}

		void SetValue(BaseReflectionObject* object, boost::any &value)
		{
			std::vector<T> res = boost::any_cast<std::vector<T> >(value);
			SetValue(object,res);
		}

		void GetValue(const BaseReflectionObject* object, boost::any &value) const
		{
			std::vector<T> res = GetValue(object);
			value = res;
		}


		void SetValueByStringVector(BaseReflectionObject* object, const std::vector<std::string> &value)
		{
			std::vector<T> res;
			for(size_t i = 0 ; i < value.size(); i++)
			{
				T out_value;
				std::stringstream ss(value[i]);
				ss >> out_value;
				res.push_back(out_value);
			}
			SetValue(object,res);
		}

		std::vector<std::string> GetValueAsStringVector(const BaseReflectionObject* object) const
		{
			std::vector<T> val = GetValue(object);
			std::vector<std::string> res;

			for(size_t i = 0 ; i < val.size(); i++)
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
		virtual bool HasMetaData() const {return (m_MetaData != NULL);}
		virtual PropertyMetaDataPtr GetMetaData() const {return m_MetaData;}
	protected:
		GetterType		m_Getter;
		SetterType		m_Setter;
		SetterTypeConst	m_SetterConst;
		PropertyMetaDataPtr m_MetaData;
	};
}
#endif
