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

#pragma once

#include "Sim/Common.h"
#include <string>

namespace GASS
{
	class GASSExport IResourceSystem
	{
	public:
		virtual ~IResourceSystem(){}
		virtual bool GetFullPath(const std::string &file_name,std::string &file_path) = 0;
		virtual void AddResourceLocation(const std::string &path,const std::string &resource_group,const std::string &type, bool recursive) = 0;
		virtual void RemoveResourceLocation(const std::string &path,const std::string &resource_group) = 0;
		virtual void RemoveResourceGroup(const std::string &resource_group) = 0;
		virtual void LoadResourceGroup(const std::string &resource_group) = 0;
	protected:
	};

	typedef boost::shared_ptr<IResourceSystem> ResourceSystemPtr;
}
