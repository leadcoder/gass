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

#pragma once

#include "Core/Common.h"

#include "Core/Reflection/GASSTypedProperty.h"
#include <cassert>
#include <sstream>
#include <iomanip>

namespace GASS
{
	template<typename T>
	struct GetOptionType 
	{
		typedef typename  std::remove_reference<T>::type Type;
	};

	template<typename T>
	struct GetOptionType<std::vector<T>> 
	{
		typedef typename std::remove_reference<T>::type Type;
	};

	
	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Reflection
	*  @{
	*/

	template <class OwnerType, typename MemberType, typename GetterType = MemberType>
	class ObjectTypedProperty : public TypedProperty<MemberType>
	{
	public:
		ObjectTypedProperty(const std::string &name,
			PropertyFlags flags,
			const std::string& description,
			PropertyMetaDataPtr meta_data) : TypedProperty<MemberType>(name, flags, description, meta_data)
		{
			
		}

		virtual GetterType Get(const OwnerType*) const = 0;
		virtual void Set(OwnerType*, const MemberType &value) const = 0;

		MemberType GetValue(const IPropertyOwner* object) const override
		{
			const OwnerType* o = dynamic_cast<const OwnerType*>(object);
			if (o == nullptr)
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed cast owner of property:" + this->m_Name, "ObjectTypedProperty::GetValue");
			return Get(o);
		}

		void SetValue(IPropertyOwner* object, const MemberType &value) override
		{
			OwnerType* o = dynamic_cast<OwnerType*>(object);
			if (o == nullptr)
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed cast owner of property:" + this->m_Name, "ObjectTypedProperty::SetValue");
			Set(o, value);
		}
		typedef typename GetOptionType<MemberType>::Type OptionType;
		typedef std::function<std::vector<OptionType>(OwnerType*)> ObjectOptionsFunction;
		void SetObjectOptionsFunction(ObjectOptionsFunction func) { m_ObjectOptionsFunction = func;}
		bool HasObjectOptions() const { return m_ObjectOptionsFunction != nullptr;}
		std::vector<OptionType> GetOptionsByObject(IPropertyOwner* object) const
		{
			std::vector<OptionType> options;
			if (m_ObjectOptionsFunction)
			{
				if(OwnerType* o = dynamic_cast<OwnerType*>(object))
					options = m_ObjectOptionsFunction(o);
			}
			return options;
		}

		std::vector<std::string> GetStringOptionsByObject(IPropertyOwner* object) const override
		{
			std::vector<std::string> options;
			std::vector<OptionType> typed_options = GetOptionsByObject(object);
			for (OptionType option : typed_options)
			{
				std::stringstream ss;
				ss << option;
				options.push_back(ss.str());
			}
			return options;
		}

		bool HasOptions() const override
		{
			bool has_opions = !this->m_Options.empty();
			if (HasObjectOptions() || this->m_OptionsCallback || this->m_OptionsFunction)
			{
				has_opions = true;
			}
			return has_opions;
		}

	private:
		ObjectOptionsFunction m_ObjectOptionsFunction;
	};

	/**
	Template class used to define a property of a specific type.
	@param OwnerType class that has the getter and setter functions
	@param T Property type
	*/
	template <typename OwnerType,
		typename MemberType,
		typename GetterReturnType,
		typename SetterArgumentType,
		typename SetterReturnTypeUnused>
		class GetSetProperty : public ObjectTypedProperty<OwnerType, MemberType, GetterReturnType>
	{
	public:
		typedef GetterReturnType(OwnerType::*GetterType)() const;
		typedef SetterReturnTypeUnused(OwnerType::*SetterType)(SetterArgumentType);
		GetSetProperty(const std::string &name,
			GetterType getter,
			SetterType setter,
			PropertyFlags flags,
			const std::string& description, 
			PropertyMetaDataPtr meta_data) : ObjectTypedProperty<OwnerType, MemberType, GetterReturnType>(name, flags, description, meta_data),
			m_Getter(getter),
			m_Setter(setter)
		{

		}

		GetterReturnType Get(const OwnerType* object) const override
		{
			return (object->*m_Getter)();
		}

		void Set(OwnerType* object, const MemberType &value) const override
		{
			(object->*m_Setter)(value);
		}
	protected:
		GetterType		m_Getter;
		SetterType		m_Setter;
	};

	template <typename ConstType>
	struct RemoveConstRef {
		typedef typename std::remove_const<typename std::remove_reference<ConstType>::type>::type Type;
	};

	template <typename OwnerType,
		typename GetterReturnType,
		typename MemberType = typename RemoveConstRef<GetterReturnType>::Type,
		typename SetterArgumentType,
		typename SetterReturnType
		>
		static GetSetProperty<OwnerType, MemberType, GetterReturnType, SetterArgumentType, SetterReturnType>* MakeGetSetProperty(const std::string &name,
			GetterReturnType(OwnerType::*getter)() const,
			SetterReturnType(OwnerType::*setter)(SetterArgumentType),
			PropertyFlags flags = PF_RESET,
			const std::string& description = "",
			PropertyMetaDataPtr meta_data = PropertyMetaDataPtr())
	{
		auto* property = new GetSetProperty<OwnerType, MemberType, GetterReturnType, SetterArgumentType, SetterReturnType>(name, getter, setter,flags, description, meta_data);
		return property;
	}

	template <class OwnerType,
		typename MemberType>
		class MemberProperty : public ObjectTypedProperty<OwnerType, MemberType, MemberType>
	{
	public:
		typedef MemberType OwnerType::* MemberPointer;
		MemberProperty(const std::string &name,
			MemberPointer member,
			PropertyFlags flags,
			const std::string& description,
			PropertyMetaDataPtr meta_data) : ObjectTypedProperty<OwnerType, MemberType, MemberType>(name, flags, description, meta_data),
			m_Member(member)
		{

		}

		MemberType Get(const OwnerType* object) const
		{
			return object->*m_Member;
		}

		void Set(OwnerType* object, const MemberType &value) const
		{
			object->*m_Member = value;
		}
	protected:
		MemberPointer m_Member;
	};

	template <typename OwnerType,
		typename MemberType>
		static MemberProperty<OwnerType, MemberType>* MakeMemberProperty(const std::string &name,
			MemberType OwnerType::* member,
			PropertyFlags flags = PF_RESET,
			const std::string& description = "",
			PropertyMetaDataPtr meta_data = PropertyMetaDataPtr())
	{
		auto* property = new MemberProperty<OwnerType, MemberType>(name, member, flags, description, meta_data);
		return property;
	}

#define ADD_DEPENDENCY(COMPONENT_NAME) m_Dependencies[GetClassRTTI()].push_back(COMPONENT_NAME);
}

