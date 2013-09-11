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

#ifndef XMLUtils_HH
#define XMLUtils_HH

#include "Core/Common.h"
#include "Core/Math/GASSVector.h"
class TiXmlElement;
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
		static std::string ReadString(TiXmlElement *xml_elem, const std::string &tag);
		static bool ReadBool(TiXmlElement *xml_elem, const std::string &tag);
		static Float ReadFloat(TiXmlElement *xml_elem, const std::string &tag);
		static int ReadInt(TiXmlElement *xml_elem, const std::string &tag);
		static std::string ReadStringAttribute(TiXmlElement *xml_elem, const std::string &attrib);
	};
}

#endif 

