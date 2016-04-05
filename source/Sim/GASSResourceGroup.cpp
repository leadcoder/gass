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

#include "GASSResourceGroup.h"
#include "GASSResourceLocation.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSResourceManager.h"
#include "Core/Utils/GASSFilesystem.h"


namespace GASS
{

	ResourceGroup::ResourceGroup(const std::string &name) : m_Name(name)
	{

	}

	ResourceGroup::~ResourceGroup()
	{

	}

	ResourceLocationPtr ResourceGroup::AddResourceLocation(const FilePath &path,ResourceLocationType type, bool recursive)
	{
		ResourceLocationPtr rl(new ResourceLocation(shared_from_this(),path,type));
		if(recursive)
			AddResourceLocationRecursive(rl);
		else
		{
			rl->ParseLocation();
			m_ResourceLocations.push_back(rl);
			SimEngine::Get().GetSimSystemManager()->SendImmediate(ResourceLocationAddedEventPtr(new ResourceLocationAddedEvent(rl)));
		}
		return rl;
	}

	void ResourceGroup::AddResourceLocationRecursive(ResourceLocationPtr rl)
	{
		GASS_FILESYSTEM::path boost_path(rl->GetPath().GetFullPath());
		if( GASS_FILESYSTEM::exists(boost_path))
		{
			rl->ParseLocation();
			m_ResourceLocations.push_back(rl);
			SimEngine::Get().GetSimSystemManager()->SendImmediate(ResourceLocationAddedEventPtr(new ResourceLocationAddedEvent(rl)));
			GASS_FILESYSTEM::directory_iterator end ;
			for(GASS_FILESYSTEM::directory_iterator iter(boost_path) ; iter != end ; ++iter )
			{
				if (GASS_IS_DIRECTORY( *iter ) && iter->path().string().find(".svn") == std::string::npos) //ignore svn folders!
				{
					ResourceLocationPtr rec_rl(new ResourceLocation(shared_from_this(),iter->path().string(),rl->GetType()));

					AddResourceLocationRecursive(rec_rl);
				}
			}
		}
	}

	void ResourceGroup::RemoveResourceLocation(ResourceLocationPtr location)
	{
		std::vector<ResourceLocationPtr>::iterator iter = m_ResourceLocations.begin();
		while(iter != m_ResourceLocations.end())
		{
			if(location == *iter)
			{
				SimEngine::Get().GetSimSystemManager()->SendImmediate(ResourceLocationRemovedEventPtr(new ResourceLocationRemovedEvent(location)));
				iter = m_ResourceLocations.erase(iter);

			}
			else
				++iter;
		}
	}

	void ResourceGroup::Reload() const
	{
		for (std::vector<ResourceLocationPtr>::const_iterator iter = m_ResourceLocations.begin(); iter != m_ResourceLocations.end(); ++iter)
		{
			(*iter)->ParseLocation();
		}
	}

	void ResourceGroup::GetResourcesByType(ResourceVector &resources, const std::string &resource_type) const
	{
		std::vector<ResourceLocationPtr>::const_iterator iter = m_ResourceLocations.begin();
		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
		while(iter != m_ResourceLocations.end())
		{

			ResourceLocation::ResourceMap::const_iterator c_iter = (*iter)->GetResources().begin();
			while(c_iter != (*iter)->GetResources().end())
			{
				if(c_iter->second->Type() == resource_type)
				{
					resources.push_back(c_iter->second);
				}
				c_iter++;
			}
			++iter;
		}
	}

	bool ResourceGroup::HasResource(const std::string &resource_name) const
	{
		std::vector<ResourceLocationPtr>::const_iterator iter = m_ResourceLocations.begin();
		while(iter != m_ResourceLocations.end())
		{
			if((*iter)->HasResource(resource_name))
				return true;
			++iter;
		}
		return false;
	}

	void ResourceGroup::GetResourcesByName(ResourceVector &resources, const std::string &resource_name) const
	{
		std::vector<ResourceLocationPtr>::const_iterator iter = m_ResourceLocations.begin();
		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
		const std::string lower_name = StringUtils::ToLower(resource_name);
		while(iter != m_ResourceLocations.end())
		{
			ResourceLocation::ResourceMap::const_iterator c_iter = (*iter)->GetResources().find(lower_name);
			if(c_iter != (*iter)->GetResources().end())
			{
				resources.push_back(c_iter->second);
			}
			++iter;
		}
	}
}



