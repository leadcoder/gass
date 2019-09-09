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
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed cast owner of property:" + m_Name, "ObjectTypedProperty::GetValue");
			return Get(o);
		}

		void SetValue(IPropertyOwner* object, const MemberType &value) override
		{
			OwnerType* o = dynamic_cast<OwnerType*>(object);
			if (o == nullptr)
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed cast owner of property:" + m_Name, "ObjectTypedProperty::SetValue");
			Set(o, value);
		}
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
		typename MemberType = RemoveConstRef<GetterReturnType>::Type,
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


#define REG_PROPERTY(TYPE,NAME,CLASS) RegisterGetSet(#NAME, & CLASS::Get##NAME, & CLASS::Set##NAME);
#define REG_PROPERTY2(TYPE,NAME,CLASS,META_DATA) RegisterGetSet(#NAME, & CLASS::Get##NAME, & CLASS::Set##NAME, META_DATA);
	//#define REG_VECTOR_PROPERTY(TYPE,NAME,CLASS) RegisterVectorProperty< TYPE >(#NAME, & CLASS::Get##NAME , & CLASS::Set##NAME);
	//#define REG_VECTOR_PROPERTY2(TYPE,NAME,CLASS,META_DATA) RegisterVectorProperty< TYPE >(#NAME, & CLASS::Get##NAME , & CLASS::Set##NAME, META_DATA);
#define ADD_PROPERTY(TYPE,NAME) TYPE m_ ## NAME ; \
TYPE Get ## NAME () const {return m_ ## NAME ;} \
	void Set ## NAME ( const TYPE &value) {m_ ## NAME = value;}

#define GASS_GETSET_MEMBER(TYPE,NAME) private : \
TYPE m_ ## NAME ; \
public: \
TYPE Get ## NAME () const {return m_ ## NAME ;} \
	void Set ## NAME ( const TYPE &value) {m_ ## NAME = value;}


#define ADD_DEPENDENCY(COMPONENT_NAME) m_Dependencies[GetClassRTTI()].push_back(COMPONENT_NAME);
}

