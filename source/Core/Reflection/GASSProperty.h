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

#ifndef GASS_PROPERTY_H
#define GASS_PROPERTY_H

#include "Core/Common.h"
#include "Core/Serialize/GASSSerialize.h"
#include "Core/Serialize/GASSSTDSerialize.h" //move content to this file
#include "Core/Reflection/GASSTypedProperty.h"
#include <cassert>
#include <sstream>
#include <iomanip>

namespace GASS
{
	//-------------------------Get value from string---------------------
	template <class TYPE>
	bool GetValueFromString(TYPE &res, const std::string &s)
	{
		std::stringstream ss(s);
		//str.setf(0,std::ios::floatfield);
		//str.precision(10);
		//str.setf(std::ios::boolalpha);
		
		ss >> res;
		return !ss.fail();
	}

	//Use specialized template to catch std::string
	template <>
	bool GASSCoreExport GetValueFromString<std::string>(std::string &res,const std::string &s);

	//Use specialized template to catch bool
	template <>
	bool GASSCoreExport GetValueFromString<bool>(bool &res, const std::string &s);

	//-------------------------Get string from value---------------------
	template <class TYPE>
	bool GetStringFromValue(const TYPE &val, std::string &res)
	{
		std::stringstream sstream;
		//we want to keep white space to be able to feed Vec3, 
		//std::vector, ColorRGB etc. that use white space as parameter delimiter
		//sstream.unsetf(std::ios::skipws);

		//we prefer bool values as "true"/"false" in string
		sstream.setf(std::ios::boolalpha);

		//unnecessary? should not be set by default?
		//sstream.unsetf(std::ios::floatfield);

		sstream << val;
		res = sstream.str();
		return true;
	}

	//Use specialized template to catch float
	template <>
	bool GASSCoreExport GetStringFromValue<float>(const float &val, std::string &res);

	//Use specialized template to catch double
	template <>
	bool GASSCoreExport GetStringFromValue<double>(const double &val, std::string &res);

	//Use specialized template to catch float vector
	template <>
	bool GASSCoreExport GetStringFromValue< std::vector<float> >(const std::vector<float> &val, std::string &res);

	//Use specialized template to catch double vector
	template <>
	bool GASSCoreExport GetStringFromValue< std::vector<double> >(const std::vector<double> &val, std::string &res);


	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Reflection
	*  @{
	*/

	/**
	Template class used to define a property of a specific type.
	@param OwnerType class that has the getter and setter functions
	@param T Property type
	*/
	template <class OwnerType,
			class T,
			typename GetterReturnType,
			typename SetterArgumentType,
			typename SetterReturnTypeUnused>
	class Property : public TypedProperty<T>
	{
	public:
		typedef GetterReturnType(OwnerType::*GetterType)() const;
		typedef SetterReturnTypeUnused(OwnerType::*SetterType)(SetterArgumentType);
		Property( const std::string &name,
			GetterType getter,
			SetterType setter,
			PropertyMetaDataPtr meta_data):
			m_Getter(getter),
			m_Setter(setter),
			m_MetaData(meta_data),
			m_Name(name)
		{

		}

		GetterReturnType GetValue(const IPropertyOwner* object) const override
		{
			return (((OwnerType*)object)->*m_Getter)();
		}

		bool HasMetaData() const override
		{
			return (m_MetaData != nullptr);
		}

		PropertyMetaDataPtr GetMetaData() const override
		{
			if(m_MetaData)
			{
				return m_MetaData;
			}
			else
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No meta data present", "Property::GetPropertyMetaData");
		}

		void SetValue(IPropertyOwner* object, const T &value ) override
		{
			(((OwnerType*)object)->*m_Setter)( value );
		}

		void Serialize(IPropertyOwner* object,ISerializer* serializer) override
		{
			if (serializer->Loading())
			{
				T val;
				auto* loader = dynamic_cast<SerialLoader*>(serializer);
				if (loader)
				{
					loader->IO<T>(val);
					SetValue(object, val);
				}
			}
			else
			{
				T val = GetValue(object);
				auto* saver = dynamic_cast<SerialSaver*>(serializer);
				if(saver)
					saver->IO<T>(val);
			}
		}

		void SetValueByString(IPropertyOwner* object, const std::string &value) override
		{
			try
			{
				T res;
				GetValueFromString<T>(res,value);
				SetValue(object,res);
			}
			catch(...)
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed to set property:" + m_Name + " With value:" + value,"Property::SetValueByString");
			}
		}

		std::string GetValueAsString(const IPropertyOwner* object) const override
		{
			T val = GetValue(object);
			std::string res;
			GetStringFromValue<T>(val,res);
			return res;
		}

		void Copy(IPropertyOwner* dest, const IPropertyOwner* src) override
		{
			SetValue(dest,GetValue(src));
		}

		void SetValueByAny(IPropertyOwner* object, GASS_ANY &value) override
		{

			T res;
			try
			{
				res = GASS_ANY_CAST<T>(value);
			}
			catch(...)
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed any_cast property:" + m_Name + " Property type may differ from provided any value","Property::SetValue");
			}

			SetValue(object,res);
		}

		void GetValueAsAny(const IPropertyOwner* object, GASS_ANY &value) const override
		{
			T res = GetValue(object);
			value = res;
		}
		std::string GetName() const override { return m_Name; }
	protected:
		GetterType		m_Getter;
		SetterType		m_Setter;
		PropertyMetaDataPtr m_MetaData;
		std::string m_Name;
	};

	template <typename T>
	struct RemoveConstRef {
		typedef typename std::remove_const<typename std::remove_reference<T>::type>::type Type;
	};
	template <typename T,
		typename GetterReturnType,
		typename SetterArgumentType,
		typename SetterReturnType>
		static IProperty* CreateProperty(const std::string &name,
			GetterReturnType(T::*getter)() const,
			SetterReturnType(T::*setter)(SetterArgumentType),
			PropertyMetaDataPtr meta_data = PropertyMetaDataPtr())
	{
		typedef typename RemoveConstRef<GetterReturnType>::Type RawType;
		auto* property = new Property<T, RawType, GetterReturnType, SetterArgumentType, SetterReturnType>(name, getter, setter, meta_data);
		return property;
	}


#define REG_PROPERTY(TYPE,NAME,CLASS) RegisterProperty< TYPE >(#NAME, & CLASS::Get##NAME, & CLASS::Set##NAME);
#define REG_PROPERTY2(TYPE,NAME,CLASS,META_DATA) RegisterProperty< TYPE >(#NAME, & CLASS::Get##NAME, & CLASS::Set##NAME, META_DATA);
//#define REG_VECTOR_PROPERTY(TYPE,NAME,CLASS) RegisterVectorProperty< TYPE >(#NAME, & CLASS::Get##NAME , & CLASS::Set##NAME);
//#define REG_VECTOR_PROPERTY2(TYPE,NAME,CLASS,META_DATA) RegisterVectorProperty< TYPE >(#NAME, & CLASS::Get##NAME , & CLASS::Set##NAME, META_DATA);
#define ADD_PROPERTY(TYPE,NAME) TYPE m_ ## NAME ; \
TYPE Get ## NAME () const {return m_ ## NAME ;} \
	void Set ## NAME ( const TYPE &value) {m_ ## NAME = value;}

#define ADD_DEPENDENCY(COMPONENT_NAME) m_Dependencies[GetClassRTTI()].push_back(COMPONENT_NAME);
}
#endif
