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
#include "Core/Serialize/GASSSerialize.h"
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
	class TypedProperty : public IProperty
	{

	public:
		TypedProperty(const std::string &name,
				PropertyFlags flags,
				const std::string& description,
				PropertyMetaDataPtr meta_data) :
			m_Name(name),
			m_Flags(flags),
			m_Description(description),
			m_MetaData(meta_data)
		{
			m_Options = _GetEnumeration();
		}

		static std::vector<std::string> _GetEnumeration() 
		{
			return _GetEnumerationImpl(static_cast<T*>(nullptr));
		}
		template <typename EnumClass>
		static auto _GetEnumerationImpl(EnumClass*) -> decltype(EnumClass::GetStringEnumeration(), std::vector<std::string>()) {
			return EnumClass::GetStringEnumeration();
		}

		static std::vector<std::string> _GetEnumerationImpl(...) { std::vector<std::string> ret;  return ret; }

		/**
		 Returns the type of this property.
		 */
		const std::type_info* GetTypeID() const override
		{
			return &typeid(T);
		}


		/**
		 Returns the name of this property.
		 */
		std::string GetTypeName() const override
		{
			return StringUtils::Demangle(std::string(typeid(T).name()));
		}

		/**
		Get the value of this property.
		@param object BaseReflectionObject that is owner of this property
		*/
		virtual T GetValue(const IPropertyOwner* object) const = 0;

		/**
		Set the value of this property.
		@param object BaseReflectionObject that is owner of this property
		*/
		virtual void SetValue(IPropertyOwner* object, const T &value) = 0;

		bool HasMetaData() const override
		{
			return (m_MetaData != nullptr);
		}

		PropertyMetaDataPtr GetMetaData() const override
		{
			if (m_MetaData)
			{
				return m_MetaData;
			}
			else
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "No meta data present", "TypedProperty::GetMetaData");
		}

		void Serialize(IPropertyOwner* object, ISerializer* serializer) override
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
				if (saver)
					saver->IO<T>(val);
			}
		}

		void SetValueByString(IPropertyOwner* object, const std::string &value) override
		{
			try
			{
				T res;
				StringUtils::GetValueFromString<T>(res, value);
				SetValue(object, res);
			}
			catch (...)
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed to set property:" + m_Name + " With value:" + value, "TypedProperty::SetValueByString");
			}
		}

		std::string GetValueAsString(const IPropertyOwner* object) const override
		{
			T val = GetValue(object);
			std::string res;
			StringUtils::GetStringFromValue<T>(val, res);
			return res;
		}

		void Copy(IPropertyOwner* dest, const IPropertyOwner* src) override
		{
			SetValue(dest, GetValue(src));
		}

		void SetValueByAny(IPropertyOwner* object, GASS_ANY &value) override
		{

			T res;
			try
			{
				res = GASS_ANY_CAST<T>(value);
			}
			catch (...)
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed any_cast property:" + m_Name + " Property type may differ from provided any value", "Property::SetValueByAny");
			}

			SetValue(object, res);
		}

		void GetValueAsAny(const IPropertyOwner* object, GASS_ANY &value) const override
		{
			T res = GetValue(object);
			value = res;
		}
		std::string GetName() const override { return m_Name; }
		PropertyFlags GetFlags() const override { return m_Flags;}
		void SetFlags(PropertyFlags flags) override { m_Flags = flags; }
		std::string GetDescription() const override { return m_Description; }
		void SetDescription(const std::string &desciption) override { m_Description = desciption; }
		std::vector<std::string> GetOptions() const override 
		{ 
			std::vector<std::string> options = m_Options;
			if (m_OptionsCallback)
			{
				std::vector<std::string> cb_options = m_OptionsCallback->GetEnumeration();
				options.insert(options.end(), cb_options.begin(), cb_options.end());
			}
			if (m_OptionsFunction)
			{
				std::vector<std::string> func_options = m_OptionsFunction();
				options.insert(options.end(), func_options.begin(), func_options.end());
			}
			return options;
		}

		void AddOption(const std::string& option) override { m_Options.push_back(option);}
		typedef std::function<std::vector<std::string>()> OptionsFunction;
		void SetOptionsFunction(OptionsFunction func)
		{
			m_OptionsFunction = func;
		}

		void SetOptionsCallback(PropertyOptionsCallbackPtr callback) { m_OptionsCallback = callback; }
		void SetMetaData(PropertyMetaDataPtr meta_data) { m_MetaData = meta_data; }
protected:
		PropertyMetaDataPtr m_MetaData;
		std::string m_Name;
		std::string m_Description;
		PropertyFlags m_Flags;
		std::vector<std::string> m_Options;
		OptionsFunction m_OptionsFunction;
		PropertyOptionsCallbackPtr m_OptionsCallback;
	};
}
#endif
