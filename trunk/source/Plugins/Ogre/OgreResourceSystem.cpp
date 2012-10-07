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
#include "Core/Common.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Plugins/Ogre/OgreResourceSystem.h"
#include "Core/System/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include <boost/bind.hpp>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include "tinyxml.h"
#include <boost/filesystem.hpp>

using namespace Ogre;

namespace GASS
{
	OgreResourceSystem::OgreResourceSystem(void)
	{
	}

	OgreResourceSystem::~OgreResourceSystem(void)
	{

	}

	void OgreResourceSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("OgreResourceSystem",new GASS::Creator<OgreResourceSystem, ISystem>);
	}


	void OgreResourceSystem::OnCreate(SystemManagerPtr owner)
	{
		SimSystem::OnCreate(owner);
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreResourceSystem::OnInit,MainWindowCreatedNotifyMessage,0));
	}

	void OgreResourceSystem::Init()
	{

	}

	void OgreResourceSystem::Update()
	{

	}

	//Create custom load
	void OgreResourceSystem::LoadXML(TiXmlElement *elem)
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

				m_ResourceLocations.push_back(rl);
			}
			else
			{
				std::string attrib_val = attrib->FirstAttribute()->Value();
				SetPropertyByString(attrib_name,attrib_val);
			}
			attrib  = attrib->NextSiblingElement();
		}
	}

	/*void OgreResourceSystem::AddResourceLocationRecursive(const ResourceLocation &rl)
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
	}*/

	void OgreResourceSystem::OnInit(MainWindowCreatedNotifyMessagePtr message)
	{
		LogManager::getSingleton().stream() << "OgreResourceSystem Initlize Started";
		for(int i = 0; i < m_ResourceLocations.size(); i++)
		{
			ResourceLocation rl = m_ResourceLocations[i];
			AddResourceLocation(rl.m_Path.GetFullPath(),rl.m_Group,rl.m_Type, rl.m_Recursive);
		}
		LogManager::getSingleton().stream() << "OgreResourceSystem Initlize All Resource Groups";
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		LogManager::getSingleton().stream() << "OgreResourceSystem Completed";
		
	}


	void OgreResourceSystem::AddResourceLocation(const FilePath &path,const std::string &resource_group,const std::string &type, bool recursive)
	{
		boost::filesystem::path boost_path(path.GetFullPath()); 
		if( boost::filesystem::exists(boost_path))  
		{
			Ogre::ResourceGroupManager *rsm = Ogre::ResourceGroupManager::getSingletonPtr();
			Ogre::StringVector groups = rsm->getResourceGroups();
			if (std::find(groups.begin(), groups.end(), resource_group) == groups.end())
			{
				rsm->createResourceGroup(resource_group);
			}
			rsm->addResourceLocation(path.GetFullPath(),type, resource_group,false);

			if(recursive)
			{
				boost::filesystem::directory_iterator end;    
				for( boost::filesystem::directory_iterator iter(boost_path) ; iter != end ; ++iter )      
				{
					if (boost::filesystem::is_directory( *iter ) )      
					{   
						
						const std::string sub_dir_path  = iter->path().string();
						AddResourceLocation(sub_dir_path,resource_group,type,recursive);
					}     
				}
			}
		}			
	}



	void OgreResourceSystem::RemoveResourceLocation(const FilePath &path,const std::string &resource_group)
	{
		Ogre::ResourceGroupManager *rsm = Ogre::ResourceGroupManager::getSingletonPtr();
		Ogre::StringVector groups = rsm->getResourceGroups();
		if (std::find(groups.begin(), groups.end(), resource_group) != groups.end())
		{
			rsm->removeResourceLocation(path.GetFullPath(),resource_group);
		}

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

	void OgreResourceSystem::RemoveResourceGroup(const std::string &resource_group)
	{
		Ogre::ResourceGroupManager *rsm = Ogre::ResourceGroupManager::getSingletonPtr();
		Ogre::StringVector groups = rsm->getResourceGroups();
		if (std::find(groups.begin(), groups.end(), resource_group) != groups.end())
		{
			rsm->destroyResourceGroup(resource_group);
		}

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


	void OgreResourceSystem::LoadResourceGroup(const std::string &resource_group)
	{
		Ogre::ResourceGroupManager *rsm = Ogre::ResourceGroupManager::getSingletonPtr();
		rsm->initialiseResourceGroup(resource_group);
	}

	bool OgreResourceSystem::GetFullPath(const std::string &file_name,std::string &file_path)
	{
		if(file_name == "")
		{
			return false;
		}

		if(boost::filesystem::exists(file_name))
		{
			file_path = file_name;
			return true;
		}
		else
		{
			Ogre::ResourceGroupManager *rsm = Ogre::ResourceGroupManager::getSingletonPtr();
			//std::string gname = rsm->findGroupContainingResource(file_name);
			//bool found = rsm->resourceExists(gname,file_name);

			Ogre::StringVector groups = rsm->getResourceGroups();
			Ogre::StringVector::iterator iter;

			for(iter = groups.begin(); iter != groups.end();++iter)
			{
				std::string gname = *iter;
				bool found = rsm->resourceExists(gname,file_name);

				if(found)
				{
					Ogre::FileInfoListPtr files = rsm->findResourceFileInfo(gname, file_name);
					if(files->size() > 0)
					{
						Ogre::FileInfo info = *files->begin();
						if(info.archive)
							file_path = info.archive->getName();
						file_path += "/";
						file_path += info.filename;
						return true;
					}
				}
			}

			for(int i  = 0; i < m_ResourceLocations.size(); i++)
			{

				std::string temp_file_path = m_ResourceLocations[i].m_Path.GetFullPath() + "/" +  file_name;
				if(boost::filesystem::exists(temp_file_path))
				{
					file_path = temp_file_path;
					return true;
				}
			}
		}
		LogManager::getSingleton().stream() << "WARNING:Failed to find resource: " << file_name;
		return false;
	}
}




