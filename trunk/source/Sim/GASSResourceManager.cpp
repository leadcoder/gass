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

#include "Sim/GASSResourceManager.h"
#include "Sim/GASSResourceGroup.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Core/Utils/GASSMisc.h"
#include <tinyxml.h>

namespace GASS
{
	ResourceManager::ResourceManager(void)
	{
	
	}

	ResourceManager::~ResourceManager(void)
	{
	
	}
	
	void ResourceManager::LoadXML(TiXmlElement *elem)
	{
		TiXmlElement *prop_elem = elem->FirstChildElement();
		while(prop_elem)
		{
			const std::string elem_name = prop_elem->Value();
			if(elem_name == "ResourceGroup")
			{
				TiXmlElement *group_elem = prop_elem->FirstChildElement();
				const std::string group_name = Misc::ReadStringAttribute(prop_elem,"name");
				ResourceGroupPtr group(new ResourceGroup(group_name));
				while(group_elem)
				{
					const std::string group_elem_name = group_elem->Value();
					if(group_elem_name == "ResourceLocation")
					{
						const FilePath path = Misc::ReadStringAttribute(group_elem,"path");
						const std::string type = Misc::ReadStringAttribute(group_elem,"type");
						const std::string rec = Misc::ReadStringAttribute(group_elem,"recursive");
						bool recursive = false;
						if(Misc::ToLower(rec) == "true")
							recursive = true;

						if(Misc::ToLower(type) == "filesystem")
							group->AddResourceLocation(path,RLT_FILESYSTEM,recursive);
						else if(Misc::ToLower(type) == "zip")
							group->AddResourceLocation(path,RLT_ZIP,recursive);
						//CreateResourceLocation();
					}
					group_elem  = group_elem->NextSiblingElement();
				}
				AddResourceGroup(group);
			}
			/*else
			{
				const std::string attrib_val = prop_elem->FirstAttribute()->Value();
				SetPropertyByString(elem_name,attrib_val);
			}*/
			prop_elem  = prop_elem->NextSiblingElement();
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
		std::string ext = Misc::ToLower(extension);
	
		for(size_t i = 0; i < m_ResourceTypes.size();i++)
		{
			for(size_t j = 0; j < m_ResourceTypes[i].Extensions.size(); j++)
			{
				if(ext == Misc::ToLower(m_ResourceTypes[i].Extensions[j]))
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
}




