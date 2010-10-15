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

#ifndef BASEREFLECTIONOBJECT_HH
#define BASEREFLECTIONOBJECT_HH

#include <boost/any.hpp>
#include "Core/Common.h"
#include "Core/Serialize/Serialize.h"
#include "Core/Reflection/RTTI.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Reflection/NullClass.h"

class TiXmlElement;
namespace GASS
{
	class BaseReflectionObject;
	typedef boost::shared_ptr<BaseReflectionObject> BaseReflectionObjectPtr;
	typedef boost::weak_ptr<BaseReflectionObject> BaseReflectionObjectWeakPtr;

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
		bool SetPropertyByString(const std::string &property_name,const std::string &value);
		/**Get property by string
		    @param property_name Name of the property to get
            @param value The value provided as string
            @return true if property found
		*/
		bool GetPropertyByString(const std::string &property_name, std::string &value);

		/**Set property by using boost::any
		    @param property_name Name of the property to get
            @param value The value provided as boost::any
            @return true if property found and set
		*/
		bool SetPropertyByType(const std::string &property_name, boost::any value);

		/**Get property value
		    @param property_name Name of the property to get
            @param value The value provided as boost::any
            @return true if property found
		*/
		bool GetPropertyByType(const std::string &property_name, boost::any &value);

		void LoadProperties(TiXmlElement *elem);
		void SaveProperties(TiXmlElement *parent);

		bool SerializeProperties(ISerializer* serializer);
		void SetProperties(BaseReflectionObjectPtr dest);
	};

}
#endif // #ifndef BASEREFLECTIONOBJECT_HH
