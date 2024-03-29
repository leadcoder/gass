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


#include "Core/Common.h"
#include "GASSXMLUtils.h"
#include "GASSStringUtils.h"
#include "GASSException.h"
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{
	std::string XMLUtils::ReadString(const tinyxml2::XMLElement *xml_elem, const std::string &tag)
	{
		std::string ret;
		const tinyxml2::XMLElement *xml_value = xml_elem->FirstChildElement(tag.c_str());
		if(xml_value)
		{
			 ret = xml_value->Attribute("value");
		}
		else
		{
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Failed to find tag:" + tag, "XMLUtils::ReadString");
		}
		return ret;
	}

	std::string XMLUtils::ReadStringAttribute(const tinyxml2::XMLElement *xml_elem, const std::string &attrib)
	{
		std::string ret;
		if(xml_elem->Attribute(attrib.c_str()))
		{
			ret = xml_elem->Attribute(attrib.c_str());
		}
		else
		{
			const std::string document_name = xml_elem->GetDocument()->GetFileName();
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Failed to find attribute:" + attrib +" In file:" +document_name, "XMLUtils::ReadStringAttribute");
		}
		return ret;
	}

	bool XMLUtils::ReadBool(const tinyxml2::XMLElement *xml_elem, const std::string &tag)
	{
		bool ret = false;
		const tinyxml2::XMLElement *xml_value = xml_elem->FirstChildElement(tag.c_str());
		if(xml_value && xml_value->Attribute("value"))
		{
			 std::string value = xml_value->Attribute("value");
			 if(StringUtils::ToLower(value) == "true")
				 ret = true;
			 else if(StringUtils::ToLower(value) == "false")
				 ret = false;
			 else
			 {
				 ret = (atoi(value.c_str()) != 0);
			 }
		}
		else
		{
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Failed to read tag:" + tag, "XMLUtils::ReadString");
		}
		return ret;
	}

	int XMLUtils::ReadInt(const tinyxml2::XMLElement *xml_elem, const std::string &tag)
	{
		int ret = 0;
		const tinyxml2::XMLElement *xml_value = xml_elem->FirstChildElement(tag.c_str());
		if(xml_value && xml_value->Attribute("value"))
		{
			 std::string value = xml_value->Attribute("value");
			 ret = atoi(value.c_str());
		}
		else
		{
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Failed to read tag:" + tag, "XMLUtils::ReadString");
		}
		return ret;
	}

	Float XMLUtils::ReadFloat(const tinyxml2::XMLElement *xml_elem, const std::string &tag)
	{
		Float ret = 0;
		const tinyxml2::XMLElement *xml_value = xml_elem->FirstChildElement(tag.c_str());
		if(xml_value)
		{
			 xml_value->QueryDoubleAttribute("value",&ret);
		}
		else
		{
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Failed to read tag:" + tag, "XMLUtils::ReadString");
		}
		return ret;
	}
}
