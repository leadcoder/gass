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

#include "Sim/GASSResourceManager.h"
#include "Sim/GASSResourceGroup.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Core/Utils/GASSStringUtils.h"
#include "Core/Utils/GASSXMLUtils.h"
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{
	ResourceManager::ResourceManager(void)
	{
	
	}

	void ResourceManager::Load(const ResourceManagerConfig &config)
	{
		for (auto res : config.ResourceLocations)
		{
			AddLocationToGroup(res.Group, FilePath(res.Path), res.Type, res.Recursive);
		}
	}

	bool ResourceManager::HasResourceGroup(const std::string &name)
	{
		ResourceGroupVector::iterator iter = m_ResourceGroups.begin();
		while(iter != m_ResourceGroups.end())
		{
			if(name == (*iter)->GetName())
			{
				return true;
			}
			++iter;
		}
		return false;
	}

	void ResourceManager::ReloadAll()
	{
		const ResourceGroupVector::iterator iter = m_ResourceGroups.begin();
		while (iter != m_ResourceGroups.end())
		{
			(*iter)->Reload();
		}
	}

	ResourceGroupPtr ResourceManager::GetFirstResourceGroupByName(const std::string &name)
	{
		ResourceGroupVector::iterator iter = m_ResourceGroups.begin();
		while(iter != m_ResourceGroups.end())
		{
			if(name == (*iter)->GetName())
			{
				return (*iter);
			}
			++iter;
		}
		GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"resource group not found:" + name, "ResourceManager::GetFirstResourceGroupByName");
	}

	void ResourceManager::AddResourceGroup(ResourceGroupPtr group)
	{
		m_ResourceGroups.push_back(group);
		SimEngine::Get().GetSimSystemManager()->SendImmediate(ResourceGroupCreatedEventPtr(new ResourceGroupCreatedEvent(group)));
	}

	void ResourceManager::RemoveResourceGroup(ResourceGroupPtr group)
	{
		ResourceGroupVector::iterator iter = m_ResourceGroups.begin();
		while(iter != m_ResourceGroups.end())
		{
			if(group == (*iter))
			{
				SimEngine::Get().GetSimSystemManager()->SendImmediate(ResourceGroupRemovedEventPtr(new ResourceGroupRemovedEvent(group)));
				iter = m_ResourceGroups.erase(iter);
				
			}
			else
				++iter;
		}
	}

	
	std::string ResourceManager::GetResourceTypeByExtension(const std::string &extension) const
	{
		std::string ext = StringUtils::ToLower(extension);
	
		for(size_t i = 0; i < m_ResourceTypes.size();i++)
		{
			for(size_t j = 0; j < m_ResourceTypes[i].Extensions.size(); j++)
			{
				if(ext == StringUtils::ToLower(m_ResourceTypes[i].Extensions[j]))
				{
					return m_ResourceTypes[i].Name;
				}
			}
		}
		return "";
	}

	FileResourcePtr ResourceManager::GetFirstResourceByName(const std::string &resource_name) const
	{
		ResourceGroupVector::const_iterator iter = m_ResourceGroups.begin();
		ResourceVector resources;
		while(iter != m_ResourceGroups.end())
		{
			(*iter)->GetResourcesByName(resources,resource_name);
			if(resources.size() > 0)
				return 	resources[0];
			++iter;
		}
		GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"file resource not found:" + resource_name, "ResourceManager::GetFirstResourceByName");		
	}

	ResourceVector ResourceManager::GetResourcesByName(const std::string &resource_name) const
	{
		ResourceGroupVector::const_iterator iter = m_ResourceGroups.begin();
		ResourceVector resources;
		while(iter != m_ResourceGroups.end())
		{
			
			(*iter)->GetResourcesByName(resources,resource_name);
			++iter;
		}
		return resources;		
	}

	bool ResourceManager::HasResource(const std::string &resource_name) const
	{
		ResourceGroupVector::const_iterator iter = m_ResourceGroups.begin();
		ResourceVector resources;
		while(iter != m_ResourceGroups.end())
		{
			if((*iter)->HasResource(resource_name))
				return true;
			++iter;
		}
		return false;		
	}

	void ResourceManager::RegisterResourceType(const ResourceType &res_type) 
	{
		m_ResourceTypes.push_back(res_type);
	}


	GASS::ResourceGroupPtr ResourceManager::GetOrCreateResourceGroup(const std::string &group_name)
	{
		GASS::ResourceGroupPtr gfx_group;
		if (HasResourceGroup(group_name))
		{
			gfx_group = GetFirstResourceGroupByName(group_name);
		}
		else
		{
			gfx_group = GASS_MAKE_SHARED<ResourceGroup>(group_name);
			AddResourceGroup(gfx_group);
		}
		return gfx_group;
	}
	
	void ResourceManager::AddLocationToGroup(const std::string &group_name, const FilePath &path, ResourceLocationType type, bool recursive)
	{
		GASS::ResourceGroupPtr gfx_group = GetOrCreateResourceGroup(group_name);
		gfx_group->AddResourceLocation(path, type, recursive);
	}


}




