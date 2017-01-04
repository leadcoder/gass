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

#ifndef XMLUtils_HH
#define XMLUtils_HH

#include "Core/Common.h"

namespace tinyxml2
{
	class XMLElement;
}

namespace GASS
{
	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Utility
	*  @{
	*/

	/**
	Class used for XML operations
	*/

	class GASSCoreExport XMLUtils
	{
	public:
		XMLUtils();
		virtual ~XMLUtils();
		static std::string ReadString(tinyxml2::XMLElement *xml_elem, const std::string &tag);
		static bool ReadBool(tinyxml2::XMLElement *xml_elem, const std::string &tag);
		static Float ReadFloat(tinyxml2::XMLElement *xml_elem, const std::string &tag);
		static int ReadInt(tinyxml2::XMLElement *xml_elem, const std::string &tag);
		static std::string ReadStringAttribute(tinyxml2::XMLElement *xml_elem, const std::string &attrib);
	};
}

#endif 

