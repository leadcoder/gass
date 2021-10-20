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
#include "GASSCommon.h"
#include "Core/Utils/GASSFilePath.h"

namespace GASS
{

	GASS_FORWARD_DECL(ResourceGroup)
	GASS_FORWARD_DECL(FileResource)
	
	
	enum ResourceLocationType
	{
		RLT_FILESYSTEM,
		RLT_ZIP,
		RLT_USER
	};

	class GASSExport ResourceLocation : public  GASS_ENABLE_SHARED_FROM_THIS<ResourceLocation>
	{
	public:
		using ResourceMap = std::map<std::string, FileResourcePtr>;

		ResourceLocation(ResourceGroupPtr owner, const FilePath &path,ResourceLocationType type);
		FilePath GetPath() const {return m_Path;}
		ResourceLocationType  GetType() const {return m_Type;}
		ResourceGroupPtr GetGroup() const {return m_Owner;}
		void ParseLocation();
		FileResourcePtr GetResourceByName(const std::string &name) const;
		bool HasResource(const std::string &name) const;
		const ResourceMap& GetResources() const {return m_Resources;}
	private:
		FilePath m_Path;
		ResourceLocationType m_Type;
		ResourceGroupPtr m_Owner;
		ResourceMap m_Resources;
	};
	using ResourceLocationPtr = std::shared_ptr<ResourceLocation>;
	using ResourceLocationWeakPtr = std::weak_ptr<ResourceLocation>;
}
