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

#ifndef GASS_FILE_RESOURCE_H
#define GASS_FILE_RESOURCE_H

#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSFilePath.h"
#include "Core/Reflection/GASSIProperty.h"
#include "Sim/GASSResourceLocation.h"
#include "Sim/GASSResourceManager.h"
#include "Sim/GASSResourceGroup.h"
#include "Sim/GASSResourceLocation.h"



namespace GASS
{
	class GASSExport FileResource
	{
	public:
		FileResource(const FilePath &path,ResourceLocationPtr location, const std::string &type) : m_ResourcePath(path),
			m_Location(location),
			m_ResourceType(type)
		{

		}
		
		std::string Name() const { return m_ResourcePath.GetFilename();}
		ResourceLocationPtr Location() const { return m_Location;}
		std::string Type() const { return m_ResourceType;}
		FilePath Path() const { return m_ResourcePath;}
	private:
		FilePath m_ResourcePath;
		ResourceLocationPtr m_Location;
		std::string m_ResourceType;
	};
	GASS_PTR_DECL(FileResource)

	template <class TYPE>
	class FileResourceCallback : public IPropertyOptionsCallback<TYPE>
	{
	public:
		FileResourceCallback(std::string res_group, std::string res_type): m_ResourceGroup(res_group),
			m_ResourceType(res_type)
		{

		}
		std::vector<TYPE> GetEnumeration() const override
		{
			std::vector<TYPE> content;
			GASS::ResourceManagerPtr rm = GASS::SimEngine::Get().GetResourceManager();
			GASS::ResourceGroupVector groups = rm->GetResourceGroups();
			std::vector<std::string> values;
			for (size_t i = 0; i < groups.size(); i++)
			{
				GASS::ResourceGroupPtr group = groups[i];
				if (m_ResourceGroup == "" || group->GetName() == m_ResourceGroup)
				{
					GASS::ResourceVector res_vec;
					group->GetResourcesByType(res_vec, m_ResourceType);
					for (size_t j = 0; j < res_vec.size(); j++)
					{
						content.push_back(res_vec[j]->Name());
					}
				}
			}
			return content;
		}
	private:
		std::string m_ResourceGroup;
		std::string m_ResourceType;
	};
	//typedef GASS_SHARED_PTR<FileResourceCallback<TYPE> > FileResourceCallbackPtr;
}
#endif 
