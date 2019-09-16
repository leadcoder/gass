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
#include <typeinfo>
#include "Core/Reflection/GASSRTTI.h"
#include "Core/Reflection/GASSProperty.h"
#include "Core/Utils/GASSStringUtils.h"

namespace tinyxml2
{
	class XMLElement;
}


namespace GASS
{

    //forward declaration
	class BaseReflectionObject;

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Reflection
	*  @{

	*/
    
	/** Reflection is the RTTI "sandwich class" being used to augment a class with RTTI support. Classes
        supporting RTTI need to derived from this class, with their ancestor specified as the TInClass
        template parameter.
	*/

	template <class DerivedClass, class AncestorClass>
	class Reflection : public AncestorClass
	{
	public :
		typedef GASS_SHARED_PTR<DerivedClass> TPtr;
		
		template<typename... Args>
		Reflection(Args... args) :
			AncestorClass(args...) {}


		// Default reflection registration function. Does nothing by default.
		static void	RegisterReflection()
		{

        }

		/** Registers a property. Takes in the property name, its getter and setter functions, and the property
		    type as a template parameter. Should be called from within a user-defined RegisterReflection function.
		*/
		template <typename PropertyType,
			typename GetterReturnType, 
			typename SetterArgumentType, 
			typename SetterReturnType>
		static void RegisterProperty(const std::string &name, 
			GetterReturnType(DerivedClass::*getter)() const,
			SetterReturnType(DerivedClass::*setter)(SetterArgumentType),
			PropertyMetaDataPtr meta_data =  PropertyMetaDataPtr())
		{
			auto* property = new GetSetProperty<DerivedClass, PropertyType, GetterReturnType, SetterArgumentType, SetterReturnType>( name, getter, setter, PF_RESET, "", meta_data);
			DerivedClass::GetClassRTTI()->GetProperties()->push_back(property);
		}

		template <typename GetterReturnType,
			typename MemberType = RemoveConstRef<GetterReturnType>::Type,
			typename SetterArgumentType,
			typename SetterReturnType>
			static GetSetProperty<DerivedClass, MemberType, GetterReturnType, SetterArgumentType, SetterReturnType>* RegisterGetSet(const std::string &name,
				GetterReturnType(DerivedClass::*getter)() const,
				SetterReturnType(DerivedClass::*setter)(SetterArgumentType),
				PropertyFlags flags = PF_RESET,
				const std::string& description = "",
				PropertyMetaDataPtr meta_data = PropertyMetaDataPtr())
		{
			auto* property = MakeGetSetProperty(name, getter, setter, flags, description, meta_data);
			DerivedClass::GetClassRTTI()->GetProperties()->push_back(property);
			return property;
		}
		
		template <typename MemberType>
			static MemberProperty<DerivedClass, MemberType>* RegisterMember(const std::string &name,
				MemberType DerivedClass::* member_ptr,
				PropertyFlags flags = PF_RESET,
				const std::string& description = "",
				PropertyMetaDataPtr meta_data = PropertyMetaDataPtr())
		{
			MemberProperty<DerivedClass, MemberType>* property = MakeMemberProperty<DerivedClass, MemberType>(name, member_ptr , flags , description, meta_data);
			DerivedClass::GetClassRTTI()->GetProperties()->push_back(property);
			return property;
		}

		static inline RTTI* GetClassRTTI()
		{
			return &m_RTTI;
		}

		virtual RTTI* GetRTTI() const
		{
			return &m_RTTI;
		}
	protected :
		static RTTI	m_RTTI;
	};

	template <class DerivedClass, class AncestorClass> RTTI Reflection<DerivedClass, AncestorClass>::m_RTTI
		(GASS::StringUtils::Demangle(std::string(typeid(DerivedClass).name())), AncestorClass::GetClassRTTI(),
		DerivedClass::RegisterReflection );
}