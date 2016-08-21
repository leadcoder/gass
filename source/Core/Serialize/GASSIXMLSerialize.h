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

#ifndef GASS_I_XMLSERIALIZE_H
#define GASS_I_XMLSERIALIZE_H

#include "Core/Math/GASSVector.h"

namespace tinyxml2
{
	class XMLElement;
}

namespace GASS
{
	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Serialize
	*  @{
	*/

	/**
		Interface for xml serialization
	*/

	class GASSCoreExport IXMLSerialize
	{
	public:
		virtual ~IXMLSerialize(){}
		virtual void LoadXML(tinyxml2::XMLElement *obj_elem) = 0;
		virtual void SaveXML(tinyxml2::XMLElement *obj_elem) = 0;
	};

	typedef GASS_SHARED_PTR<IXMLSerialize> XMLSerializePtr;
}
#endif // #ifndef IXMLSERIALIZE_HH
