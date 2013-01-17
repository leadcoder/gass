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

#include "GASSResourceGroup.h"
#include "GASSResourceLocation.h"
#include "Sim/Interface/GASSIResourceSystem.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"

namespace GASS
{

	ResourceGroup::ResourceGroup(ResourceSystemPtr owner, const std::string &name) : m_Owner(owner),	
		m_Name(name)
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
			m_ResourceLocations.push_back(rl);
			SimEngine::Get().GetSimSystemManager()->SendImmediate(ResourceLocationCreatedEventPtr(new ResourceLocationCreatedEvent(rl)));
		}
		return rl;
	}

	void ResourceGroup::AddResourceLocationRecursive(ResourceLocationPtr rl)
	{
		boost::filesystem::path boost_path(rl->GetPath().GetFullPath()); 
		if( boost::filesystem::exists(boost_path))  
		{
			m_ResourceLocations.push_back(rl);
			boost::filesystem::directory_iterator end ;    
			for(boost::filesystem::directory_iterator iter(boost_path) ; iter != end ; ++iter )      
			{
				if (boost::filesystem::is_directory( *iter ) )      
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

	ResourceVector ResourceGroup::GetResourcesByType(const std::string &resource_type) const
	{
		std::vector<ResourceLocationPtr>::const_iterator iter = m_ResourceLocations.begin();
		ResourceVector resources;
		while(iter != m_ResourceLocations.end())
		{
			std::vector<FilePath> files;
			FilePath::GetFilesFromPath(files,(*iter)->GetPath());
			for(size_t i = 0; i< files.size(); i++)
			{
				const std::string res_type = m_Owner->GetResourceTypeByExtension(files[i].GetExtension());
				if(res_type  == resource_type)
					resources.push_back(Resource(files[i],GetName(),res_type));
			}
			++iter;
		}
		return resources;
	}

	ResourceVector ResourceGroup::GetResourcesByName(const std::string &resource_name) const
	{
		ResourceVector resources;
		std::vector<ResourceLocationPtr>::const_iterator iter = m_ResourceLocations.begin();
		while(iter != m_ResourceLocations.end())
		{
			std::vector<FilePath> files;
			FilePath::GetFilesFromPath(files,(*iter)->GetPath());
			for(size_t i = 0; i< files.size(); i++)
			{
				if(resource_name == files[i].GetFilename())
				{
					const std::string res_type = m_Owner->GetResourceTypeByExtension(files[i].GetExtension());
					resources.push_back(Resource(files[i],GetName(),res_type));
				}
			}
			++iter;
		}
		return resources;
	}
}




