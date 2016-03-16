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

#ifndef GASS_BASE_REFLECTION_OBJECT_H
#define GASS_BASE_REFLECTION_OBJECT_H

#include "Core/Common.h"
#include "Core/Serialize/GASSSerialize.h"
#include "Core/Reflection/GASSRTTI.h"
#include "Core/Reflection/GASSReflection.h"
#include "Core/Reflection/GASSNullClass.h"

namespace tinyxml2
{
	class XMLElement;
};

namespace GASS
{
	class BaseReflectionObject;
	
	typedef GASS_SHARED_PTR<BaseReflectionObject> BaseReflectionObjectPtr;
	typedef GASS_WEAK_PTR<BaseReflectionObject> BaseReflectionObjectWeakPtr;
	typedef std::vector<IProperty*> PropertyVector;

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Reflection
	*  @{
	*/

    /**
        This class implement basic reflection functionality.
        All classes that want reflection support should be derived from this class
    */
	class GASSCoreExport BaseReflectionObject : public Reflection<BaseReflectionObject, NullClass>
	{
	public:
		BaseReflectionObject();
		virtual ~BaseReflectionObject();

		/**Set property by string
		    @param property_name Name of the property to change
            @param value The value provided as string
            @return true if property found and set
		*/
		void SetPropertyByString(const std::string &property_name,const std::string &value);
		/**Get property by string
		    @param property_name Name of the property to get
            @param value The value provided as string
            @return true if property found
		*/
		bool GetPropertyByString(const std::string &property_name, std::string &value) const;

		/**Set property by value using GASS_ANY
		    @param property_name Name of the property to get
            @param value The value provided as GASS_ANY
            @return true if property found and set
		*/
		void SetPropertyByType(const std::string &property_name, GASS_ANY value);

		/**Check if property exist
		    @param property_name Name of the property to get
            @return true if property found
		*/
		bool HasProperty(const std::string &property_name) const;


		/**Get property by value using GASS_ANY 
		    @param property_name Name of the property to get
            @param value The returned value provided as GASS_ANY argument
            @return true if property found
		*/
		bool GetPropertyByType(const std::string &property_name, GASS_ANY &value);

		/**
		Get all properties from this object
		*/
		PropertyVector GetProperties() const;


		/**
			Check if this property has meta data
		*/
		bool HasMetaData() const;

		/**
			Get meta data for this object, if not present an exception is thrown (use HasMetaData to be sure)
		*/
		ClassMetaDataPtr GetMetaData() const;
		
		void CopyPropertiesTo(BaseReflectionObjectPtr dest) const;

		//internal stuff
	protected:
		void _LoadProperties(tinyxml2::XMLElement *elem);
		void _SaveProperties(tinyxml2::XMLElement *parent);

		bool _SerializeProperties(ISerializer* serializer);
		
	private:
	};

}
#endif // #ifndef BASEREFLECTIONOBJECT_HH
