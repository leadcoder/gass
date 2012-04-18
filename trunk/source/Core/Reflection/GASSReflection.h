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
This code is based on the Game Programming Gems 5 article
"Using Templates for Reflection in C++" by Dominic Filion.
*/

#ifndef GASS_REFLECTION_H
#define GASS_REFLECTION_H

#include <typeinfo>
#include "Core/Reflection/GASSRTTI.h"
#include "Core/Reflection/GASSProperty.h"
#include "Core/Reflection/GASSVectorProperty.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSMisc.h"
class TiXmlElement;

namespace GASS
{
	static std::string UnDecorateClassName(const std::string &name)
	{
		//substr(6) is used for removing "class_" from string returned by typeid,
		//we dont want to use any prefix when accessing classes by name,

		std::string ret = name.substr(6);
		//remove namespace
		size_t pos = ret.find("::");
		if(pos != -1)
		{
			ret =  ret.substr(pos+2);
		}
		return ret;
	}

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
    template parameter.*/

	template <class T, class TInClass>
	class Reflection : public TInClass
	{

	public :
		typedef boost::shared_ptr<T> TPtr;
		Reflection()
		{
		}

		/**
            Creates an instance of T. Called by the system to dynamically create
		    class instances from class IDs.
        */
		/*static boost::shared_ptr<T> Create()
		{
            return boost::shared_ptr<T> (new T());
        }*/

		virtual boost::shared_ptr<BaseReflectionObject> CreateInstance()
		{
		boost::shared_ptr<T>  instance (new T());
		return instance;
        }

		// Default reflection registration function. Does nothing by default.
		static void	RegisterReflection()
		{
        }

		//----------------------------------------------------------------------------------------------
		// Registers a property. Takes in the property name, its getter and setter functions, and the property
		// type as a template parameter. Should be called from within a user-defined RegisterReflection function.
		template <class PropertyType>
		static void RegisterProperty(const std::string &name, typename Property<T, PropertyType>::GetterType getter,
			typename Property<T, PropertyType>::SetterType setter )
		{
			Property<T, PropertyType>* property = new Property<T, PropertyType>(name, getter, setter);
			T::GetClassRTTI()->GetProperties()->push_back(property);
		}

		template <class PropertyType>
		static void RegisterProperty(const std::string &name, typename Property<T, PropertyType>::GetterType getter,
			typename Property<T, PropertyType>::SetterTypeConst setter)
		{
			Property<T, PropertyType>* property = new Property<T, PropertyType>( name, getter, setter );
			T::GetClassRTTI()->GetProperties()->push_back(property);
		}

		template <class PropertyType>
		static void RegisterVectorProperty(const std::string &name, typename VectorProperty<T, PropertyType>::GetterType getter,
			typename VectorProperty<T, PropertyType>::SetterType setter)
		{
			VectorProperty<T, PropertyType>* property = new VectorProperty<T, PropertyType>(name, getter, setter);
			T::GetClassRTTI()->GetProperties()->push_back(property);
		}

		template <class PropertyType>
		static void RegisterVectorProperty(const std::string &name, typename VectorProperty<T, PropertyType>::GetterType getter,
			typename VectorProperty<T, PropertyType>::SetterTypeConst setter)
		{
			VectorProperty<T, PropertyType>* property = new VectorProperty<T, PropertyType>( name, getter, setter);
			T::GetClassRTTI()->GetProperties()->push_back(property);
		}

		//----------------------------------------------------------------------------------------------
		// Returns RTTI info associated with this class type.
		static inline RTTI* GetClassRTTI()
		{
			return &m_RTTI;
		}

		//----------------------------------------------------------------------------------------------
		// Returns RTTI info associated with this class instance.
		virtual RTTI* GetRTTI() const
		{
			return &m_RTTI;
		}



	protected :
		static RTTI	m_RTTI;

	};

	template <class T, class TInClass> RTTI Reflection<T, TInClass>::m_RTTI
		(Misc::Demangle(std::string(typeid(T).name())), TInClass::GetClassRTTI(),/* (ClassFactoryFunc)T::Create,*/
		(RegisterReflectionFunc)T::RegisterReflection );
}
#endif