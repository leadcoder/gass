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

#include "Sim/GASSResourceLocation.h"
#include "Sim/GASSResourceManager.h"
#include "Sim/GASSSimEngine.h"
#include "Core/Utils/GASSException.h"

namespace GASS
{

	ResourceLocation::ResourceLocation(ResourceGroupPtr owner, const FilePath &path,ResourceLocationType type) : m_Owner(owner),
		m_Path(path),
		m_Type(type)
	{

	}

	ResourceLocation::~ResourceLocation()
	{

	}

	void ResourceLocation::ParseLocation()
	{
		m_Resources.clear();
		std::vector<FilePath> files;
		FilePath::GetFilesFromPath(files,m_Path);
		for(size_t i = 0; i< files.size(); i++)
		{
			ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
			const std::string res_type = rm->GetResourceTypeByExtension(files[i].GetExtension());
			const std::string name = files[i].GetFilename();
			FileResourcePtr res(new FileResource(files[i],shared_from_this(),res_type));
			m_Resources[name] = res;
		}
	}

	FileResourcePtr ResourceLocation::GetResourceByName(const std::string &name) const
	{
		FileResourcePtr res;
		ResourceMap::const_iterator iter = m_Resources.find(name);
		if(iter != m_Resources.end())
		{
			res = iter->second;
		}
		GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to get file resource:" + name,"ResourceLocation::GetResourceByName");
	}

	bool ResourceLocation::HasResource(const std::string &name) const
	{
		ResourceMap::const_iterator iter = m_Resources.find(name);
		if(iter != m_Resources.end())
		{
			return true;
		}
		return false;
	}
}




