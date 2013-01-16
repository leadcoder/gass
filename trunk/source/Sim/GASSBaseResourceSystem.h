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

#include "Sim/GASS.h"
#include <string>

namespace GASS
{

	

	struct ResourceLocation
	{
		FilePath m_Path;
		std::string m_Type;
		std::string m_Group;
		bool m_Recursive;
	};

	class GASSExport BaseResourceSystem : public Reflection<BaseResourceSystem, SimSystem>,  public IResourceSystem
	{
	public:
		BaseResourceSystem();
		virtual ~BaseResourceSystem();
		static void RegisterReflection();
		virtual void Init() {};
		virtual void LoadXML(TiXmlElement *elem);
		std::string GetSystemName() const {return "BaseResourceSystem";}
		void AddResourceLocation(const FilePath &path,const std::string &resource_group,const std::string &type,bool recursive = false);
		void LoadResourceGroup(const std::string &resource_group);
		Resource GetResourceByName(const std::string &resource_name) const;
		std::vector<Resource> GetResources(const std::string &resource_type = "", const std::string &resource_group = "") const;
		const std::string GetType(const std::string extension) const;
		void RegisterResourceType(const ResourceType &res_type);
	protected:
		void RemoveResourceLocation(const FilePath &path,const std::string &resource_group);
		void RemoveResourceGroup(const std::string &resource_group);
		void AddResourceLocationRecursive(const ResourceLocation &rl);
		void AddResourceGroup(const std::string &resource_group){};
		std::vector<ResourceLocation> m_ResourceLocations;
		std::vector<ResourceType> m_ResourceTypes;
	};
}
