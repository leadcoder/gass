/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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

	class GASSCoreExport BaseReflectionObject : public Reflection<BaseReflectionObject, NullClass>
	{
	public:
		BaseReflectionObject();
		virtual ~BaseReflectionObject();
		
		//helpers to modify properties
		void LoadProperties(TiXmlElement *elem);
		bool SetPropertyByString(const std::string &attrib_name,const std::string &attrib_val);
		bool GetPropertyByString(const std::string &attrib_name, std::string &value);
		bool SetPropertyByType(const std::string &attrib_name, boost::any attribute);
		bool GetPropertyByType(const std::string &attrib_name, boost::any &attribute);
		bool SerializeProperties(ISerializer* serializer);
		void SetProperties(BaseReflectionObjectPtr dest);
	
	};

	
}
