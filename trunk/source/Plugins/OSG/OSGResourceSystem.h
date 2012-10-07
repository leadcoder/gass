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

#include "Sim/GASS.h"
#include <string>

namespace GASS
{
	struct ResourceLocation
	{
		FilePath m_Path;
		std::string m_Type;
		std::string m_Group;
		bool m_Recursive;
	};

	class OSGResourceSystem : public Reflection<OSGResourceSystem, SimSystem>,  public IResourceSystem
	{
	public:
		OSGResourceSystem();
		virtual ~OSGResourceSystem();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void LoadXML(TiXmlElement *elem);
		SystemType GetSystemType() const {return "ResourceSystem";}
		bool GetFullPath(const std::string &file_name,std::string &file_path);
		void AddResourceLocation(const FilePath &path,const std::string &resource_group,const std::string &type,bool recursive = false);
		void LoadResourceGroup(const std::string &resource_group);
	protected:
		void RemoveResourceLocation(const FilePath &path,const std::string &resource_group);
		void RemoveResourceGroup(const std::string &resource_group);
		void AddResourceLocationRecursive(const ResourceLocation &rl);
		void OnInit(InitSystemMessagePtr message);		
		void OnUpdate(MessagePtr message);
		std::string m_RenderSystem;
		std::vector<ResourceLocation> m_ResourceLocations;

	};
}
