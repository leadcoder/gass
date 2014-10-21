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
#include "Sim/GASSResourceManager.h"
#include "Sim/GASSResourceGroup.h"
#include "Sim/GASSResourceLocation.h"
#include "Core/Utils/GASSException.h"

namespace GASS
{
	std::vector<std::string> FileResourcePropertyMetaData::GetEnumeration(BaseReflectionObjectPtr object) const
	{
		std::vector<std::string> content;
		GASS::ResourceManagerPtr rm = GASS::SimEngine::Get().GetResourceManager();
		GASS::ResourceGroupVector groups = rm->GetResourceGroups();
		std::vector<std::string> values;
		for(size_t i = 0; i < groups.size();i++)
		{
			GASS::ResourceGroupPtr group = groups[i];
			if(m_ResourceGroup == "" || group->GetName() == m_ResourceGroup)
			{
				GASS::ResourceVector res_vec;
				group->GetResourcesByType(res_vec,m_ResourceType);
				for(size_t j = 0; j < res_vec.size();j++)
				{
					content.push_back(res_vec[j]->Name());
				}
			}
		}
		return content;
	}	
}
