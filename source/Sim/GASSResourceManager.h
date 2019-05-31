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


#include "Sim/GASSCommon.h"
#include "Sim/GASSResourceLocation.h"
#include "Sim/GASSResource.h"

namespace tinyxml2
{
	class XMLElement;
}

namespace GASS
{

	struct ResourceLocationConfig
	{
		ResourceLocationConfig(){}
		ResourceLocationConfig(std::string group, std::string path,	bool recursive) :  Group(group),
			Path(path),
			Recursive(recursive)
		{}
		std::string Group;
		std::string Path;
		ResourceLocationType Type = RLT_FILESYSTEM;
		bool Recursive = false;
	};

	struct ResourceManagerConfig
	{
		std::vector<ResourceLocationConfig> ResourceLocations;
	};

	struct ResourceType
	{
		std::string Name;
		std::vector<std::string> Extensions;
	};
	typedef std::vector<ResourceGroupPtr> ResourceGroupVector;
	typedef std::vector<FileResourcePtr> ResourceVector;

	/**
		ResourceManager
	*/
	class GASSExport ResourceManager
	{
	public:
		ResourceManager();
	
		void Load(const ResourceManagerConfig &config);
	
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
			Get first resource group by name
			@name Name of the resource group
		*/
		ResourceGroupPtr GetFirstResourceGroupByName(const std::string &name);
		
		/**
			Check if extension is associated with resource type
			@extension File extension to check
		*/
		std::string GetResourceTypeByExtension(const std::string &extension) const;

		/**
			Get first resource by name. All resrouces gorups are checked.
			Exception will be cast if resource not found
			@resource_name Name of resource
		*/
		FileResourcePtr GetFirstResourceByName(const std::string &resource_name) const;


		/**
			Get first resources by name. All resrouces gorups are checked.
			@resource_name Name of resource
		*/
		ResourceVector GetResourcesByName(const std::string &resource_name) const;

		/**
			Check if resource exits.
			@resource_name Name of resource
		*/
		bool HasResource(const std::string &resource_name) const;

		/**
			Register new resource type
			@resource_name Name of resource
		*/
		void RegisterResourceType(const ResourceType &res_type);

		/**
			Reload all groups
		*/
		void ReloadAll();

		/**
			Get or create resource group
		*/
		GASS::ResourceGroupPtr ResourceManager::GetOrCreateResourceGroup(const std::string &group_name);

		/**
		Add resource location to resource group, the group is created if not already present.
		*/
		void AddLocationToGroup(const std::string &group_name, const FilePath &path, ResourceLocationType type, bool recursive);
	private:
		ResourceGroupVector m_ResourceGroups;
		std::vector<ResourceType> m_ResourceTypes;
	};
	typedef GASS_SHARED_PTR<ResourceManager> ResourceManagerPtr;
}
