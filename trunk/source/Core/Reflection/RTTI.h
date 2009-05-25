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

#include <list>
#include <vector>
#include "Core/Common.h"
#include "Core/Reflection/AbstractProperty.h"


namespace GASS
{
	class BaseReflectionObject;
	class RTTI;

	typedef std::string				ClassID;
	typedef BaseReflectionObject*	(*ClassFactoryFunc)( ClassID );
	typedef bool			(*RegisterReflectionFunc)();


	

	class GASSCoreExport RTTI
	{

	public:

		//----------------------------------------------------------------------------------------------
		// RTTI constructor.
		// The first paramter is a stub. I am not sure why this stub is necessary - removing ths stub will
		// confuse the .NET compiler and produce compile errors with subsequent parameters. If anybody knows 
		// why this is so, feel free to e-mail me at dfilion@hotmail.com
		//
		// The RTTI structure constructor takes in the following parameters:
		//		dwStub			Just a stub
		//		CLID			A unique class ID
		//		szClassName		The name of the class type this RTTI structure represents
		//		pBaseReflectionObjectRTTI	The parent RTTI structure for this RTTI structure
		//		pFactory		A factory function callback for creating an instance of the bound class type
		//		pReflectionFunc	Callback called by the system to register the reflective properties
		RTTI(const std::string  &class_name, RTTI* base_class_RTTI, ClassFactoryFunc factory, RegisterReflectionFunc reflection_func );

		//----------------------------------------------------------------------------------------------
		// Fills a vector with all properties of the represented class type, including all ancestor types.
		void	EnumProperties( std::vector<AbstractProperty*>& o_Result );

		//----------------------------------------------------------------------------------------------
		// Returns true if the RTTI structure is of the type specified by CLID.
		inline bool				IsTypeOf( RTTI *rtti);
		inline bool				IsTypeOf( const std::string &class_name);

		// Returns true if the RTTI structure is derived from the type specified by CLID.
		inline bool				IsDerivedFrom( RTTI *rtti);
		inline bool				IsDerivedFrom( const std::string &class_name);

		//----------------------------------------------------------------------------------------------
		// Gets base class' RTTI structure.
		inline RTTI*			GetAncestorRTTI();

		//----------------------------------------------------------------------------------------------
		//----------------------------------------------------------------------------------------------
		// Gets the class name.
		inline std::string		GetClassName();
		//inline std::string		GetClassNameNoNamespace();

		//----------------------------------------------------------------------------------------------
		// Gets the class factory function.
		inline ClassFactoryFunc	GetClassFactory();

		//----------------------------------------------------------------------------------------------
		// Provides access to the properties bound to this run-time type. Does not include ancestor class
		// properties. Use EnumProperties to include ancestor properties.
		inline	std::list<AbstractProperty*>::iterator	GetFirstProperty();
		inline	std::list<AbstractProperty*>::iterator	GetLastProperty();
		inline	std::list<AbstractProperty*>*			GetProperties();

	private:

		std::string					m_ClassName;		// Class name
		RTTI*						m_BaseRTTI;							// Base class RTTI structure
		ClassFactoryFunc			m_ObjectFactory;						// Factory function
		std::list<AbstractProperty*>	m_Properties;							// Property list

	};

	inline RTTI* RTTI::GetAncestorRTTI()
	{
		return m_BaseRTTI;
	}

	inline ClassFactoryFunc RTTI::GetClassFactory()
	{
		return m_ObjectFactory;
	}

	inline std::string RTTI::GetClassName()
	{
		return m_ClassName;
	}

	

	inline std::list<AbstractProperty*>::iterator RTTI::GetFirstProperty()
	{
		return m_Properties.begin();
	}

	inline std::list<AbstractProperty*>* RTTI::GetProperties()
	{
		return &m_Properties;
	}

	inline bool RTTI::IsDerivedFrom( RTTI* rtti )
	{
		if( rtti == this)
			return true;
		else if( m_BaseRTTI )
			return m_BaseRTTI->IsDerivedFrom( rtti);

		return false;
	}

	inline bool RTTI::IsDerivedFrom( const std::string &class_name)
	{
		if( class_name == GetClassName())
			return true;
		else if( m_BaseRTTI )
			return m_BaseRTTI->IsDerivedFrom( class_name);
		return false;
	}

	inline bool RTTI::IsTypeOf( RTTI* rtti )
	{
		return this == rtti;
	}

	inline bool RTTI::IsTypeOf(const std::string &class_name)
	{
		return class_name == GetClassName();
	}

	inline std::list<AbstractProperty*>::iterator RTTI::GetLastProperty()
	{
		return m_Properties.end();
	}

}
