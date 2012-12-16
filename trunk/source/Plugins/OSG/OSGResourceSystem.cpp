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
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <tinyxml.h>
#include "Plugins/OSG/OSGResourceSystem.h"


namespace GASS
{
	OSGResourceSystem::OSGResourceSystem(void)
	{
	
	}

	OSGResourceSystem::~OSGResourceSystem(void)
	{
	
	}

	void OSGResourceSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("OSGResourceSystem",new GASS::Creator<OSGResourceSystem, ISystem>);
	}
	

	
	//Create custom load
	void OSGResourceSystem::LoadXML(TiXmlElement *elem)
	{
		TiXmlElement *attrib = elem->FirstChildElement();
		while(attrib)
		{
			std::string attrib_name = attrib->Value();

			if(attrib_name == "AddResourceLocation")
			{
				ResourceLocation rl;

				rl.m_Path = FilePath(attrib->Attribute("Path"));
				rl.m_Type = attrib->Attribute("Type");
				rl.m_Group = attrib->Attribute("Group");
				std::string rec = attrib->Attribute("Recursive");
				rl.m_Recursive = false;
				if(rec == "true")
					rl.m_Recursive = true;

				AddResourceLocationRecursive(rl);
			}
			else
			{
				std::string attrib_val = attrib->FirstAttribute()->Value();
				SetPropertyByString(attrib_name,attrib_val);
			}
			attrib  = attrib->NextSiblingElement();
		}
	}

	void OSGResourceSystem::AddResourceLocationRecursive(const ResourceLocation &rl)
	{
		boost::filesystem::path boost_path(rl.m_Path.GetFullPath()); 
		if( boost::filesystem::exists(boost_path))  
		{
			m_ResourceLocations.push_back(rl);
			if(rl.m_Recursive)
			{
				boost::filesystem::directory_iterator end ;    
				for( boost::filesystem::directory_iterator iter(boost_path) ; iter != end ; ++iter )      
				{
					if (boost::filesystem::is_directory( *iter ) )      
					{   
						ResourceLocation rec_rl = rl;
						rec_rl.m_Path = iter->path().string();
						AddResourceLocationRecursive(rec_rl);
					}     
				}
			}
		}
	}

	void OSGResourceSystem::AddResourceLocation(const FilePath &path,const std::string &resource_group,const std::string &type, bool recursive)
	{
		ResourceLocation rl;
		rl.m_Path = path;
		rl.m_Type= type;
		rl.m_Group = resource_group;
		rl.m_Recursive = recursive;
		AddResourceLocationRecursive(rl);
	}

	void OSGResourceSystem::LoadResourceGroup(const std::string &resource_group)
	{

	}


	void OSGResourceSystem::RemoveResourceLocation(const FilePath &path,const std::string &resource_group)
	{
		std::vector<ResourceLocation>::iterator iter = m_ResourceLocations.begin();
		while(iter != m_ResourceLocations.end())
		{
			std::string temp_file_path = iter->m_Path.GetFullPath();
			if(temp_file_path  == path.GetFullPath() && resource_group == iter->m_Group)
			{
				iter = m_ResourceLocations.erase(iter);
			}
			else
				++iter;
		}
	}

	void OSGResourceSystem::RemoveResourceGroup(const std::string &resource_group)
	{
		std::vector<ResourceLocation>::iterator iter = m_ResourceLocations.begin();
		while(iter != m_ResourceLocations.end())
		{
			if(resource_group == iter->m_Group)
			{
				iter = m_ResourceLocations.erase(iter);
			}
			else
				++iter;
		}
	}

	std::vector<std::string> OSGResourceSystem::GetResourceNames(const std::string &resource_group) const
	{
		std::vector<ResourceLocation>::const_iterator iter = m_ResourceLocations.begin();
		std::vector<std::string> ret;
	
		while(iter != m_ResourceLocations.end())
		{
			if(resource_group == iter->m_Group)
			{
				ret.push_back(iter->m_Path.GetFullPath());
			}
			++iter;
		}
		return ret;
	}

	bool OSGResourceSystem::GetFullPath(const std::string &file_name,std::string &file_path)
	{
		FILE*fp;
		if(file_name == "") 
		{
			return false;
		}

		if(fp = fopen(file_name.c_str(),"rb"))
		{
			fclose(fp);
			file_path = file_name;
			return true;
		}
		else 
		{
			for(int i  = 0; i < m_ResourceLocations.size(); i++)
			{
				
				std::string temp_file_path = m_ResourceLocations[i].m_Path.GetFullPath() + "/" +  file_name;
				if(fp = fopen(temp_file_path.c_str(),"rb"))
				{
					fclose(fp);
					file_path = temp_file_path;
					return true;
				}
			}
		}
		LogManager::getSingleton().stream() << "WARNING:Failed to find resource: " << file_name;
		return false;
	}
}




