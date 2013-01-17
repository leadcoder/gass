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
#include "GASSBaseResourceSystem.h"


namespace GASS
{
	BaseResourceSystem::BaseResourceSystem(void)
	{
	
	}

	BaseResourceSystem::~BaseResourceSystem(void)
	{
	
	}

	void BaseResourceSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("BaseResourceSystem",new GASS::Creator<BaseResourceSystem, ISystem>);
	}
	
	//Use custom load
	void BaseResourceSystem::LoadXML(TiXmlElement *elem)
	{
		TiXmlElement *attrib = elem->FirstChildElement();
		while(attrib)
		{
			std::string attrib_name = attrib->Value();

			if(attrib_name == "AddResourceLocation")
			{
				
				const FilePath path = FilePath(attrib->Attribute("Path"));
				const std::string type = attrib->Attribute("Type");
				const std::string group = attrib->Attribute("Group");
				const std::string rec = attrib->Attribute("Recursive");
				bool recursive = false;
				if(Misc::ToLower(rec) == "true")
					recursive = true;
				//CreateResourceLocation(path,group,type,recursive);
			}
			else
			{
				std::string attrib_val = attrib->FirstAttribute()->Value();
				SetPropertyByString(attrib_name,attrib_val);
			}
			attrib  = attrib->NextSiblingElement();
		}
	}


	/*void BaseResourceSystem::LoadResourceGroup(const std::string &resource_group)
	{

	}*/

	bool BaseResourceSystem::HasResourceGroup(const std::string &name)
	{
		ResourceGroupVector::iterator iter = m_ResourceGroups.begin();
		while(iter != m_ResourceGroups.end())
		{
			if(name == (*iter)->GetName())
			{
				return true;
			}
			++iter;
		}
		return false;
	}

	ResourceGroupPtr BaseResourceSystem::CreateResourceGroup(const std::string &name)
	{
		ResourceSystemPtr system = DCAST(IResourceSystem,shared_from_this());
		ResourceGroupPtr group(new ResourceGroup(system,name));
		m_ResourceGroups.push_back(group);
		return group;
	}


	void BaseResourceSystem::RemoveResourceGroup(ResourceGroupPtr group)
	{
		ResourceGroupVector::iterator iter = m_ResourceGroups.begin();
		while(iter != m_ResourceGroups.end())
		{
			if(group == (*iter))
			{
				iter = m_ResourceGroups.erase(iter);
			}
			else
				++iter;
		}
	}

	/*std::vector<Resource> BaseResourceSystem::GetResources(const std::string &resource_type, const std::string &resource_group) const
	{
		std::vector<ResourceLocationPtr>::const_iterator iter = m_ResourceLocations.begin();
		std::vector<Resource> resources;
		while(iter != m_ResourceLocations.end())
		{
			if(resource_group == (*iter)->Group)
			{
				std::vector<FilePath> files;
				FilePath::GetFilesFromPath(files,(*iter)->m_Path);
				for(size_t i = 0; i< files.size(); i++)
				{
					const std::string res_type = GetType(files[i].GetExtension());
					if(resource_type == "" || res_type  == resource_type)
						resources.push_back(Resource(files[i],resource_group,res_type));
				}
			}
			++iter;
		}
		return resources;
	}*/

	std::string BaseResourceSystem::GetResourceTypeByExtension(const std::string &extension) const
	{
		for(size_t i = 0; i < m_ResourceTypes.size();i++)
		{
			for(size_t j = 0; j < m_ResourceTypes[i].Extensions.size(); j++)
			{
				if(extension == m_ResourceTypes[i].Extensions[j])
				{
					return m_ResourceTypes[i].Name;
				}
			}
		}
		return "";
	}

	Resource BaseResourceSystem::GetFirstResourceByName(const std::string &resource_name) const
	{
		ResourceGroupVector::const_iterator iter = m_ResourceGroups.begin();
		ResourceVector resources;
		while(iter != m_ResourceGroups.end())
		{
			resources = (*iter)->GetResourcesByName(resource_name);
			if(resources.size() > 0)
				return 	resources[0];
			++iter;
		}
		return Resource(FilePath(""),"","");
	}

	/*bool BaseResourceSystem::GetFullPath(const std::string &file_name,std::string &file_path)
	{
		boost::filesystem::path boost_file(file_name); 
		if( boost::filesystem::exists(boost_file))  
		{
			return true;
		}
		else 
		{
			for(int i  = 0; i < m_ResourceLocations.size(); i++)
			{
				boost::filesystem::path temp_file_path(m_ResourceLocations[i].m_Path.GetFullPath() + "/" +  file_name);
				if(boost::filesystem::exists(temp_file_path))
				{
					file_path = temp_file_path.string();
					return true;
				}
			}
		}
		LogManager::getSingleton().stream() << "WARNING:Failed to find resource: " << file_name;
		return false;
	}*/

	void BaseResourceSystem::RegisterResourceType(const ResourceType &res_type) 
	{
		m_ResourceTypes.push_back(res_type);
	}

	/*std::vector<Resource> BaseResourceSystem::GetResourcesFromGroup(const std::string &resource_group, const std::string &resource_class) const
	{
		std::vector<Resource> content;
		std::vector<std::string> files = GetResourceNames(resource_group);
		if(resource_class != "")
		{
			if(m_ResourceTypes.find(resource_class) != m_ResourceTypes.end())
			{
				for(size_t i = 0; i < files.size(); i++)
				{
					const std::string ext = Misc::GetExtension(files[i]);
					if(HasFileType(resource_class,ext))
					{
						content.push_back(files[i]);	
					}
				}
				return content;
			}
			else
			{

			}
		}
		else
		{

		}
		return content;
	}

	std::vector<std::string> BaseResourceSystem::GetResourcesFromGroup(ResourceType rt, const std::string &resource_group) const
	{
		std::vector<std::string> content;
		switch(rt)
		{
		case RT_MATERIAL:
			{
			}
			break;
		case RT_TEXTURE:
			{
				std::vector<std::string> files = GetResourceNames(resource_group);
				for(size_t i = 0; i < files.size(); i++)
				{
					const std::string ext = Misc::GetExtension(files[i]);
					if(ext == "bmp"||
						ext == "jpg"||
						ext == "gif"||
						ext == "dds"||
						ext == "tga"||
						ext == "png"
						)
						content.push_back(files[i]);
				}
			}
			break;
		case RT_MESH:
			{
				std::vector<std::string> meshes = GetResourceNames(resource_group);
				for(size_t i = 0; i < meshes.size(); i++)
				{
					if(Misc::GetExtension(meshes[i]) == "mesh")
						content.push_back(meshes[i]);
				}
			}
			break;
		case RT_SOUND:
			{
				std::vector<std::string> sounds = GetResourceNames(resource_group);
				for(size_t i = 0; i < sounds.size(); i++)
				{
					if(Misc::GetExtension(sounds[i]) == "wav")
						content.push_back(sounds[i]);
				}
			}
			break;
		}
		return content;
	}

	std::vector<std::string> BaseResourceSystem::GetResourcesFromGroup(ResourceType rt,const std::string &resource_group) const
	{
		std::vector<std::string> content;
		return content;
	}*/
}




