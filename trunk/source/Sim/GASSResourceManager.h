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


#include "Sim/GASSCommon.h"
#include "Sim/GASSResourceLocation.h"
#include "Sim/GASSResource.h"
class TiXmlElement;

namespace GASS
{

	struct ResourceType
	{
		std::string Name;
		std::vector<std::string> Extensions;

	};
	typedef std::vector<ResourceGroupPtr> ResourceGroupVector;

	/**
		ResourceManager
	*/
	class GASSExport ResourceManager
	{
	public:
		ResourceManager();
		virtual ~ResourceManager();

		/**
			Load resource groups from xml
			@resource_group The resources group
		*/

		void LoadXML(TiXmlElement *elem);
		/**
			Add resource group
			@resource_group The resources group
		*/
		void AddResourceGroup(ResourceGroupPtr group);
		

		/**
			Remove resource group
			@resource_group The resources group
		*/

		void RemoveResourceGroup(ResourceGroupPtr group);


		/**
			Get all resource groups
		*/
		ResourceGroupVector GetResourceGroups() const {return m_ResourceGroups;}

		/**
			Check if resource group is present
			@name Name of the resource group
		*/
		bool HasResourceGroup(const std::string &name);

		/**
			Check if extension is associated with resource type
			@extension File extension to check
		*/
		std::string GetResourceTypeByExtension(const std::string &extension) const;

		/**
			Get first resource by name. All resrouces gorups are checked.
			@resource_name Name of resource
		*/
		Resource GetFirstResourceByName(const std::string &resource_name) const;

		/**
			Register new resource type
			@resource_name Name of resource
		*/
		void RegisterResourceType(const ResourceType &res_type);
	protected:
	private:
		ResourceGroupVector m_ResourceGroups;
		std::vector<ResourceType> m_ResourceTypes;
	};
}
