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
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Plugins/Ogre/OgreResourceManager.h"
#include "Core/System/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include <boost/bind.hpp>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreMaterialManager.h>
#include <OgreMeshManager.h>
#include "tinyxml.h"
#include <boost/filesystem.hpp>

using namespace Ogre;

namespace GASS
{
	OgreResourceManager::OgreResourceManager(void)
	{
	}

	OgreResourceManager::~OgreResourceManager(void)
	{

	}

	void OgreResourceManager::Init()
	{
		LogManager::getSingleton().stream() << "OgreResourceManager Initlize Started";
		//get resource system
		ResourceSystemPtr rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<IResourceSystem>();
		//Parse for ogre scripts
		ResourceGroupVector groups = rs->GetResourceGroups();
		for(int i = 0; i < groups.size(); i++)
		{
			ResourceLocationVector locations = groups[i]->GetResourceLocations();
			for(int j = 0; j < locations.size(); j++)
			{
				ResourceLocationPtr rl = locations[j];
				if(rl->GetType() == RLT_FILESYSTEM)
					AddResourceLocation(rl->GetPath().GetFullPath(),rl->GetGroup()->GetName(),"FileSystem");
				else if(rl->GetType() == RLT_ZIP)
					AddResourceLocation(rl->GetPath().GetFullPath(),rl->GetGroup()->GetName(),"Zip");

			}
		}
		LogManager::getSingleton().stream() << "OgreResourceManager Completed";
	}

	void OgreResourceManager::AddResourceLocation(const FilePath &path,const std::string &resource_group,const std::string &type)
	{
		Ogre::ResourceGroupManager *rsm = Ogre::ResourceGroupManager::getSingletonPtr();
		Ogre::StringVector groups = rsm->getResourceGroups();
		if (std::find(groups.begin(), groups.end(), resource_group) == groups.end())
		{
			rsm->createResourceGroup(resource_group);
		}
		rsm->addResourceLocation(path.GetFullPath(),type, resource_group,false);
	}

	void OgreResourceManager::RemoveResourceLocation(const FilePath &path,const std::string &resource_group)
	{
		Ogre::ResourceGroupManager *rsm = Ogre::ResourceGroupManager::getSingletonPtr();
		Ogre::StringVector groups = rsm->getResourceGroups();
		if (std::find(groups.begin(), groups.end(), resource_group) != groups.end())
		{
			rsm->removeResourceLocation(path.GetFullPath(),resource_group);
		}
	}

	void OgreResourceManager::AddResourceGroup(const std::string &resource_group)
	{
		Ogre::ResourceGroupManager *rsm = Ogre::ResourceGroupManager::getSingletonPtr();
		rsm->createResourceGroup(resource_group);
	}

	void OgreResourceManager::RemoveResourceGroup(const std::string &resource_group)
	{
		Ogre::ResourceGroupManager *rsm = Ogre::ResourceGroupManager::getSingletonPtr();
		Ogre::StringVector groups = rsm->getResourceGroups();
		if (std::find(groups.begin(), groups.end(), resource_group) != groups.end())
		{
			rsm->destroyResourceGroup(resource_group);
		}
	}

	void OgreResourceManager::LoadResourceGroup(const std::string &resource_group)
	{
		Ogre::ResourceGroupManager *rsm = Ogre::ResourceGroupManager::getSingletonPtr();
		rsm->initialiseResourceGroup(resource_group);
	}
}




