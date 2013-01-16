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

#ifndef GASS_RESOURCE_H
#define GASS_RESOURCE_H

#include "Core/Common.h"
#include "Core/Utils/GASSFilePath.h"
#include "Sim/GASSCommon.h"

namespace GASS
{
	class GASSExport Resource
	{
	public:
		Resource(const FilePath &path,const std::string &group,const std::string &type) :m_ResourcePath(path),
			m_ResourceGroup(group),
			m_ResourceType(type)
		{
		};
		virtual ~Resource(){};
		std::string Name() const { return m_ResourcePath.GetFilename();}
		std::string Group() const { return m_ResourceGroup;}
		std::string Type() const { return m_ResourceType;}
		FilePath Path() { return m_ResourcePath;}
	private:
		FilePath m_ResourcePath;
		std::string m_ResourceGroup;
		std::string m_ResourceType;
	};
}
#endif 
