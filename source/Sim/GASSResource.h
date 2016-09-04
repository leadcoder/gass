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
#include "Core/Reflection/GASSPropertyMetaData.h"
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

		};
		virtual ~FileResource(){};
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


	class GASSExport FileResourcePropertyMetaData : public EnumerationPropertyMetaData
	{
	public:
		FileResourcePropertyMetaData(const std::string &annotation, PropertyFlags flags, std::string res_group, std::string res_type): EnumerationPropertyMetaData(annotation,flags,false),
			m_ResourceGroup(res_group),
			m_ResourceType(res_type)
		{

		}
		virtual std::vector<std::string> GetEnumeration(BaseReflectionObjectPtr object) const;
	private:
		std::string m_ResourceGroup;
		std::string m_ResourceType;
	};
	typedef GASS_SHARED_PTR<FileResourcePropertyMetaData> FileResourcePropertyMetaDataPtr;
}
#endif 
