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

#pragma once
#include "GASSCommon.h"
#include "Core/Utils/GASSFilePath.h"
namespace GASS
{

	class ResourceGroup;
	typedef boost::shared_ptr<ResourceGroup> ResourceGroupPtr;

	enum ResourceLocationType
	{
		RLT_FILESYSTEM,
		RLT_ZIP,
		RLT_USER
	};

	class GASSExport ResourceLocation
	{
	public:
		ResourceLocation(ResourceGroupPtr owner, const FilePath &path,ResourceLocationType type);
		~ResourceLocation();
		FilePath GetPath() const {return m_Path;}
		ResourceLocationType  GetType() const {return m_Type;}
		ResourceGroupPtr GetGroup() const {return m_Owner;}
	private:
		FilePath m_Path;
		ResourceLocationType m_Type;
		ResourceGroupPtr m_Owner;

	};
	typedef boost::shared_ptr<ResourceLocation> ResourceLocationPtr;
	typedef boost::weak_ptr<ResourceLocation> ResourceLocationWeakPtr;
}
