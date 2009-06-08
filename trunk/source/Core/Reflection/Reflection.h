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

#ifndef REFLECTION_HH
#define REFLECTION_HH

#include <typeinfo>
#include "Core/Reflection/RTTI.h"
#include "Core/Reflection/Property.h"
//#include "Core/Reflection/PropertySystem.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Reflection
//
// Reflection is the RTTI "sandwich class" being used to augment a class with RTTI support. Classes
// supporting RTTI need to derive from this class, with their ancestor specified as the TInClass
// template parameter.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
class TiXmlElement;

namespace GASS
{

	static std::string UnDecorateClassName(const std::string &name)
	{
		//substr(6) is used for removing "class_" from string returned by typeid,
		//we dont want to use any prefix when accessing classes by name,
		//To be invesitgated if typeid return same prefix in gcc
		std::string ret = name.substr(6);

		//remove namespace
		size_t pos = ret.find("::");
		if(pos != -1)
		{
			ret =  ret.substr(pos+2);
		}
		return ret;
	}

	class BaseReflectionObject;
	template <class T, class TInClass>
	class Reflection : public TInClass
	{

	public :
		typedef boost::shared_ptr<T> TPtr;
//		typedef boost::shared_ptr<Reflection> ReflectionPtr

		//----------------------------------------------------------------------------------------------
		// Constructor
		Reflection();

		//----------------------------------------------------------------------------------------------
		// Default factory function. Creates an instance of T. Called by the system to dynamically create
		// class instances from class IDs.
		static boost::shared_ptr<T> Create();

		virtual boost::shared_ptr<BaseReflectionObject> CreateInstance();
		//virtual void Assign(void* dst_class);

		//----------------------------------------------------------------------------------------------
		// Default reflection registration function. Does nothing by default.
		static void	RegisterReflection();

		//----------------------------------------------------------------------------------------------
		// Registers a property. Takes in the property name, its getter and setter functions, and the property
		// type as a template parameter. Should be called from within a user-defined RegisterReflection function.
		template <class PropertyType>
		static	void RegisterProperty(	const char* szName, typename Property<T, PropertyType>::GetterType Getter,
			typename Property<T, PropertyType>::SetterType Setter )
		{
			Property<T, PropertyType>* pProperty = new Property<T, PropertyType>( szName, Getter, Setter );
			T::GetClassRTTI()->GetProperties()->push_back( pProperty );
			//PropertySystem::GetProperties()->push_back( pProperty );
		}

		template <class PropertyType>
		static	void RegisterProperty(	const char* szName, typename Property<T, PropertyType>::GetterType Getter,
			typename Property<T, PropertyType>::SetterTypeConst Setter )
		{
			Property<T, PropertyType>* pProperty = new Property<T, PropertyType>( szName, Getter, Setter );
			T::GetClassRTTI()->GetProperties()->push_back( pProperty );
			//PropertySystem::GetProperties()->push_back( pProperty );
		}

		//bool SetProperty(const std::string &attrib_name,const std::string &attrib_val);


		//----------------------------------------------------------------------------------------------
		// Returns RTTI info associated with this class type.
		static inline RTTI* GetClassRTTI()
		{
			return &m_RTTI;
		}

		//----------------------------------------------------------------------------------------------
		// Returns RTTI info associated with this class instance.
		virtual RTTI* GetRTTI()
		{
			return &m_RTTI;
		}



	protected :
		static RTTI	m_RTTI;				// RTTI structure

	};



	/*template <class T, class TInClass>
	void Reflection<T, TInClass>::Load(TiXmlElement *elem)
	{
		TiXmlElement *attrib = elem->FirstChildElement();
		while(attrib)
		{
			std::string attrib_name = attrib->Value();
			std::string attrib_val = attrib->FirstAttribute()->Value();
			SetProperty(attrib_name,attrib_val);
			attrib  = attrib->NextSiblingElement();
		}
	}*/

	template <class T, class TInClass> RTTI Reflection<T, TInClass>::m_RTTI
		(UnDecorateClassName(std::string(typeid(T).name())), TInClass::GetClassRTTI(), (ClassFactoryFunc)T::Create,
		(RegisterReflectionFunc)T::RegisterReflection );

	template <class T, class TInClass>
	Reflection<T, TInClass>::Reflection()
	{
	}

	template <class T, class TInClass>
	boost::shared_ptr<T> Reflection<T, TInClass>::Create()
	{
		return boost::shared_ptr<T> (new T());
	}

	template <class T, class TInClass>
	boost::shared_ptr<BaseReflectionObject> Reflection<T, TInClass>::CreateInstance()
	{
		boost::shared_ptr<T>  instance (new T());
		return instance;
	}

	/*template <class T, class TInClass>
	void Reflection<T, TInClass>::Assign(void* dst_class)
	{
	//Convenient hack to for class assignment,
	//for instance this will consider any memory
	//that has to be reallocated -> invalid pointers etc.
	//This function should probably  be mandatory for every
	//derived class to implement.

	T* dst = (T*) dst_class;
	*dst = *((T*)this);
	}*/

	template <class T, class TInClass>
	void Reflection<T, TInClass>::RegisterReflection()
	{
	}
}
#endif // #ifndef REFLECTION_HH
