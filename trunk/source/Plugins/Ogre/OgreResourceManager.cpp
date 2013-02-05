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
#include "Plugins/Ogre/OgreResourceManager.h"
#include "Core/System/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSResourceGroup.h"
#include <boost/bind.hpp>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreMaterialManager.h>
#include <OgreMeshManager.h>
#include <tinyxml.h>
#include <boost/filesystem.hpp>

using namespace Ogre;

namespace GASS
{
	OgreResourceManager::OgreResourceManager(void) : m_Initialized (false)
	{
	}

	OgreResourceManager::~OgreResourceManager(void)
	{

	}

	void OgreResourceManager::Init()
	{
		LogManager::getSingleton().stream() << "OgreResourceManager Initlize Started";
		//get resource system
		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
		//Parse for ogre scripts
		ResourceGroupVector groups = rm->GetResourceGroups();
		for(int i = 0; i < groups.size(); i++)
		{
			AddResourceGroup(groups[i],false);
		}
		LogManager::getSingleton().stream() << "OgreResourceManager Completed";
	}

	void OgreResourceManager::AddResourceGroup(ResourceGroupPtr group, bool load)
	{
		Ogre::ResourceGroupManager *rsm = Ogre::ResourceGroupManager::getSingletonPtr();
		Ogre::StringVector groups = rsm->getResourceGroups();
		if (std::find(groups.begin(), groups.end(), group->GetName()) == groups.end())
		{
			rsm->createResourceGroup(group->GetName());
		}
		ResourceLocationVector locations = group->GetResourceLocations();
		for(int i = 0; i < locations.size(); i++)
		{
			AddResourceLocation(locations[i]);
		}
		if(load)
			rsm->initialiseResourceGroup(group->GetName());
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

	void OgreResourceManager::AddResourceLocation(ResourceLocationPtr location)
	{
		Ogre::ResourceGroupManager *rsm = Ogre::ResourceGroupManager::getSingletonPtr();
		if(location->GetType() == RLT_FILESYSTEM)
			rsm->addResourceLocation(location->GetPath().GetFullPath(),"FileSystem", location->GetGroup()->GetName(),false);
		else if(location->GetType() == RLT_ZIP)
			rsm->addResourceLocation(location->GetPath().GetFullPath(),"Zip", location->GetGroup()->GetName(),false);
	}

	void OgreResourceManager::RemoveResourceLocation(ResourceLocationPtr location)
	{
		Ogre::ResourceGroupManager *rsm = Ogre::ResourceGroupManager::getSingletonPtr();
		rsm->removeResourceLocation(location->GetPath().GetFullPath(),location->GetGroup()->GetName());
	}
}




