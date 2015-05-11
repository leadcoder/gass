/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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


#ifndef GASS_RTTI_H
#define GASS_RTTI_H

#include "Core/Common.h"
#include "Core/Reflection/GASSIProperty.h"
#include "Core/Reflection/GASSClassMetaData.h"
#include "Core/Utils/GASSException.h"

namespace GASS
{
	class BaseReflectionObject;
	class RTTI;

	typedef std::string ClassID;
	typedef BaseReflectionObject* (*ClassFactoryFunc)( ClassID );
	typedef bool (*RegisterReflectionFunc)();


	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Reflection
	*  @{
	*/

	/**
	RTTI class used to store properties for classes
	*/
	class GASSCoreExport RTTI
	{

	public:

		/** Constructor
		@param class_name	undecorated class name
		@param base_class_rtti	Pointer to parent class type RTTI implementation
		@param factory	A factory function for creating an instances of RTTI class type
		@param reflection_func	optional function pointer to register class properties, this function is called in this function
		*/
		RTTI(const std::string  &class_name, RTTI* base_class_rtti,/* ClassFactoryFunc factory,*/ RegisterReflectionFunc reflection_func ) : m_ClassName(class_name),
			//m_ObjectFactory(factory),
			m_BaseRTTI(base_class_rtti)
		{
			if ( reflection_func)
				reflection_func();

		}

		/**
		Check if same RTTI, return true if same
		*/
		bool IsTypeOf( RTTI *rtti)
		{
			return this == rtti;
		}

		/**
		Check class name of this RTTI class, return true if same
		*/
		bool IsTypeOf( const std::string &class_name)
		{
			return class_name == GetClassName();
		}

		bool IsDerivedFrom( RTTI *rtti)
		{
			if ( rtti == this)
				return true;
			else if ( m_BaseRTTI )
				return m_BaseRTTI->IsDerivedFrom( rtti);

			return false;
		}

		bool IsDerivedFrom( const std::string &class_name)
		{
			if ( class_name == GetClassName())
				return true;
			else if ( m_BaseRTTI )
				return m_BaseRTTI->IsDerivedFrom( class_name);
			return false;
		}

		/**
		Gets base RTTI class.
		*/
		RTTI* GetAncestorRTTI()
		{
			return m_BaseRTTI;
		}

		/**
		Gets class name for this RTTI instance.
		*/
		std::string GetClassName()
		{
			return m_ClassName;
		}

	
		/**
		Gets access to property iterator.
		*/
		std::list<IProperty*>::iterator	GetFirstProperty() 
		{
			return m_Properties.begin();
		}

		/**
		Gets access to property iterator.
		*/
		std::list<IProperty*>::iterator	GetLastProperty()
		{
			return m_Properties.end();
		}

		/**
		Gets access to property iterator.
		*/
		std::list<IProperty*>*			GetProperties()
		{
			return &m_Properties;
		}

		bool HasMetaData() const
		{
			return (m_MetaData != NULL);
			
		}


		/**
		Gets meta data for this property, meta data can hold 
		information about the property like documentation, max min values etc.
		*/
		
		ClassMetaDataPtr GetMetaData() const
		{
			if(m_MetaData)
			{
				return m_MetaData;
			}
			else
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No meta data present", "RTTI::GetMetaData");
		}
		/**
			Set meta data for this property
		*/
		void SetMetaData(ClassMetaDataPtr data)
		{
			m_MetaData = data;
		}

	private:
		std::string m_ClassName;		// Class name
		RTTI* m_BaseRTTI;			// Base class RTTI structure
		std::list<IProperty*> m_Properties;	 // Property list
		ClassMetaDataPtr m_MetaData;
	};
}
#endif
