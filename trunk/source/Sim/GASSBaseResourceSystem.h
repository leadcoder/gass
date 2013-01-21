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
#include "Sim/GASSResourceLocation.h"
#include <string>

namespace GASS
{
	class GASSExport BaseResourceSystem : public Reflection<BaseResourceSystem, SimSystem>,  public IResourceSystem
	{
		
	public:
		
		BaseResourceSystem();
		virtual ~BaseResourceSystem();
		static void RegisterReflection();
		virtual void Init() {};
		virtual void LoadXML(TiXmlElement *elem);
		std::string GetSystemName() const {return "BaseResourceSystem";}
		//IResourceSystem
		//void RemoveResourceGroup(const std::string &name);
		void RemoveResourceGroup(ResourceGroupPtr group);
		void AddResourceGroup(ResourceGroupPtr group);
		ResourceGroupVector GetResourceGroups() const {return m_ResourceGroups;}
		bool HasResourceGroup(const std::string &name);
		std::string GetResourceTypeByExtension(const std::string &extension) const;
		Resource GetFirstResourceByName(const std::string &resource_name) const;
		void RegisterResourceType(const ResourceType &res_type);
		//std::vector<Resource> GetResources(const std::string &resource_type = "", const std::string &resource_group = "") const;
		
		//void RegisterResourceType(const ResourceType &res_type);
	protected:
		
//		void AddResourceGroup(const std::string &resource_group){};
	private:
		ResourceGroupVector m_ResourceGroups;
		std::vector<ResourceType> m_ResourceTypes;
	};
}
