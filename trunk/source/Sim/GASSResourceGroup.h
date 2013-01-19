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

#include "Core/Utils/GASSFilePath.h"
#include "Sim/GASSCommon.h"
#include "Sim/GASSResource.h"
#include "Sim/GASSResourceLocation.h"

namespace GASS
{
	IFDECL(ResourceSystem)
	typedef std::vector<Resource> ResourceVector; 
	typedef std::vector<ResourceLocationPtr> ResourceLocationVector;
	class GASSExport ResourceGroup : public SHARE_CLASS<ResourceGroup>
	{
	public:
		ResourceGroup(ResourceSystemPtr owner, const std::string &name);
		~ResourceGroup();
		ResourceLocationPtr AddResourceLocation(const FilePath &path, ResourceLocationType type, bool recursive);
		void RemoveResourceLocation(ResourceLocationPtr location);
		ResourceLocationVector GetResourceLocations() const {return m_ResourceLocations;}
		std::string GetName() const{return m_Name;}
		ResourceVector GetResourcesByName(const std::string &resource_name) const;
		ResourceVector GetResourcesByType(const std::string &resource_type = "") const;
		//ResourceVector GetResources(const std::string &resource_type = "") const;
	private:
		void AddResourceLocationRecursive(ResourceLocationPtr rl);
		std::string m_Name;
		ResourceLocationVector m_ResourceLocations;
		ResourceSystemPtr m_Owner;
	};
	PDECL(ResourceGroup)
}
