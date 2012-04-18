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

#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/Systems/SimSystem.h"
#include "Sim/Systems/Messages/CoreSystemMessages.h"
#include "Sim/Systems/Messages/GraphicsSystemMessages.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"
#include "Core/Utils/FilePath.h"

#include "Core/MessageSystem/MessageType.h"
#include <string>
namespace Ogre
{
	class Root;
	class RenderWindow;
	class SceneManager;
}

namespace GASS
{
	struct ResourceLocation
	{
		FilePath m_Path;
		std::string m_Type;
		std::string m_Group;
		bool m_Recursive;
	};

	class OgreResourceSystem : public Reflection<OgreResourceSystem, SimSystem>, public IResourceSystem
	{
	public:
		OgreResourceSystem();
		virtual ~OgreResourceSystem();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void Update();
		virtual void LoadXML(TiXmlElement *elem);
		SystemType GetSystemType() const {return "ResourceSystem";}
		bool GetFullPath(const std::string &file_name,std::string &file_path);
		void AddResourceLocation(const std::string &path,const std::string &resource_group,const std::string &type,bool recursive = false);
		void RemoveResourceLocation(const std::string &path,const std::string &resource_group);
		void RemoveResourceGroup(const std::string &resource_group);
		void LoadResourceGroup(const std::string &resource_group);

	protected:
		void OnInit(MainWindowCreatedNotifyMessagePtr message);		
		std::string m_RenderSystem;
		Ogre::Root* m_Root;
		Ogre::RenderWindow* m_Window;
		Ogre::SceneManager* m_SceneMgr;
		std::vector<ResourceLocation> m_ResourceLocations;


	};
}