/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
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


#include "Sim/GASSResource.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSIResourceSystem.h"
#include "Core/Utils/GASSException.h"

namespace GASS
{
	Resource::Resource()
	{

	}

	Resource::Resource(const std::string &name) : m_ResourceName(name)
	{

	}

	Resource::~Resource()
	{

	}

	bool Resource::Valid() const
	{
		return (m_ResourceName != "");
	}

	FilePath Resource::GetFilePath() const
	{
		FilePath path;
		ResourceSystemPtr rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>();
		std::string full_path;
		if(rs->GetFullPath(m_ResourceName,full_path))
		{
			path.SetPath(full_path);
		}
		return path;
	}


	std::vector<std::string> MeshResource::GetAllOptions()
	{
		ResourceSystemPtr rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>();
		if(rs == NULL)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No Resource Manager Found", "MeshResource::GetAllOptions");
		std::vector<std::string> options = rs->GetResourcesFromGroup(RT_MESH,"GASS");
		return options;
	}
		
}
