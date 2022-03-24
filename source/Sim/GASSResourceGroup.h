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

#pragma once

#include "Core/Utils/GASSFilePath.h"
#include "Sim/GASSCommon.h"
#include "Sim/GASSResource.h"
#include "Sim/GASSResourceLocation.h"

namespace GASS
{
	GASS_IFORWARD_DECL(ResourceSystem)
	using ResourceVector = std::vector<FileResourcePtr>; 
	using ResourceLocationVector = std::vector<ResourceLocationPtr>;
	class GASSExport ResourceGroup : public GASS_ENABLE_SHARED_FROM_THIS<ResourceGroup>
	{
	public:
		ResourceGroup(const std::string &name);
		ResourceLocationPtr AddResourceLocation(const FilePath &path, ResourceLocationType type, bool recursive);
		void RemoveResourceLocation(ResourceLocationPtr location);
		
		// Reload all ResourceLocations and parse all folders
		void Reload();
		ResourceLocationVector GetResourceLocations() const {return m_ResourceLocations;}
		std::string GetName() const{return m_Name;}
		bool HasResource(const std::string &resource_name) const;
		/**
		* \brief documentera ...
		* \return
		*/
		void GetResourcesByName(ResourceVector &resources, const std::string &resource_name) const;
		/**
		* \brief documentera ...
		* \return
		*/
		void GetResourcesByType(ResourceVector &resources, const std::string &resource_type) const;
	private:
		void AddResourceLocationRecursive(ResourceLocationPtr rl);
		std::string m_Name;
		ResourceLocationVector m_ResourceLocations;
	};
	GASS_PTR_DECL(ResourceGroup)
}
