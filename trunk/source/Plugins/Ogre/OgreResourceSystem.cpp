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
#include "Sim/Systems/SimSystemManager.h"
#include "Plugins/Ogre/OgreResourceSystem.h"
#include "Core/System/SystemFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include <boost/bind.hpp>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include "tinyxml.h"

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
	

	void OgreResourceSystem::OnCreate()
	{
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreResourceSystem::OnInit,MainWindowCreatedNotifyMessage,0));
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

	void OgreResourceSystem::OnInit(MainWindowCreatedNotifyMessagePtr message)
	{
		for(int i = 0; i < m_ResourceLocations.size(); i++)
		{
			ResourceLocation rl = m_ResourceLocations[i];
			AddResourceLocation(rl.m_Path.GetPath(),rl.m_Group,rl.m_Type, rl.m_Recursive);
		}
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}

	/*std::string OgreResourceSystem::ExpandEnvVariables(const std::string &inStr)
	{
		std::string curStr = inStr;
		std::string::size_type occurIndex1 = curStr.find("%");
		std::string varName = "";
		std::string replaceStr = "";
		char * replaceVal = "";
		if ( occurIndex1 != std::string::npos )
		{
			std::string::size_type endVarIndex = curStr.find("%", occurIndex1+1);
			if (endVarIndex == std::string::npos)
			{
				Log::Warning("OgreResourceSystem::ExpandEnvVariables - Erroneous use of environment variable: %s\nOnly one percent sign in string", curStr.c_str());
				assert(true);
			}
			else
			{
				varName = curStr.substr(occurIndex1+1, endVarIndex-occurIndex1-1);
				replaceStr = curStr.substr(occurIndex1, endVarIndex-occurIndex1+1);
				if (varName.length() > 0)
				{
					replaceVal = getenv(varName.c_str());
					if (replaceVal)
					{
						curStr.replace( occurIndex1, replaceStr.length(), replaceVal );
					}
				}
			}
			
		}
		std::string::size_type occurIndex2 = curStr.find("$");
		if (occurIndex2 != std::string::npos )
		{
			std::string::size_type startVarIndex = curStr.find("(");
			std::string::size_type endVarIndex = curStr.find(")");
			if (startVarIndex == std::string::npos || endVarIndex == std::string::npos)
			{
				Log::Warning("OgreResourceSystem::ExpandEnvVariables - Erroneous use of environment variable: %s\nMissing start or end parenthesis", curStr.c_str());
				assert(true);
			}
			else
			{
				varName = curStr.substr(startVarIndex+1, endVarIndex-startVarIndex-1);
				replaceStr = curStr.substr(occurIndex2, endVarIndex-occurIndex2+1);
				if (varName.length() > 0)
				{
					replaceVal = getenv(varName.c_str());
					if (replaceVal)
					{
						curStr.replace( occurIndex2, replaceStr.length(), replaceVal );
					}
				}
			}
		}
		
		return curStr;
	}*/


	void OgreResourceSystem::AddResourceLocation(const std::string &path,const std::string &resource_group,const std::string &type, bool recursive)
	{
		Ogre::ResourceGroupManager *rsm = Ogre::ResourceGroupManager::getSingletonPtr();
		Ogre::StringVector groups = rsm->getResourceGroups();        
		if (std::find(groups.begin(), groups.end(), resource_group) == groups.end())
		{
			rsm->createResourceGroup(resource_group);
		}
		rsm->addResourceLocation(path,type, resource_group,recursive);
	}

	void OgreResourceSystem::LoadResourceGroup(const std::string &resource_group)
	{
		Ogre::ResourceGroupManager *rsm = Ogre::ResourceGroupManager::getSingletonPtr();
		rsm->initialiseResourceGroup(resource_group);
	}


	bool OgreResourceSystem::GetFullPath(const std::string &file_name,std::string &file_path)
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
			Ogre::ResourceGroupManager *rsm = Ogre::ResourceGroupManager::getSingletonPtr();
			//std::string gname = rsm->findGroupContainingResource(file_name);
			//bool found = rsm->resourceExists(gname,file_name);

			Ogre::StringVector groups = rsm->getResourceGroups();
			Ogre::StringVector::iterator iter;

			for(iter = groups.begin(); iter != groups.end();iter++)
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
				
				std::string temp_file_path = m_ResourceLocations[i].m_Path.GetPath() + "/" +  file_name;
				if(fp = fopen(temp_file_path.c_str(),"rb"))
				{
					fclose(fp);
					file_path = temp_file_path;
					return true;
				}
			}
		}
		Log::Warning("Failed to find resource: %s",file_name.c_str());
		return false;
	}
}




