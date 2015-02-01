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

#include "Core/System/GASSBaseSystemManager.h"
#include "Core/System/GASSISystem.h"
#include "Core/System/GASSSystemFactory.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "tinyxml2.h"


namespace GASS
{
	BaseSystemManager::BaseSystemManager()
	{
	}

	BaseSystemManager::~BaseSystemManager()
	{

	}

	void BaseSystemManager::Load(tinyxml2::XMLElement *systems_elem)
	{
		if(systems_elem)
		{
			tinyxml2::XMLElement *sys_elm = systems_elem->FirstChildElement();
			//Loop through each template
			while(sys_elm)
			{
				SystemPtr system = LoadSystem(sys_elm);
				if(system)
				{
					system->OnCreate(shared_from_this());
					LogManager::getSingleton().stream() << system->GetSystemName() << " created";
					m_Systems.push_back(system);
				}
				sys_elm  = sys_elm->NextSiblingElement();
			}
		}
	}


	SystemPtr BaseSystemManager::LoadSystem(tinyxml2::XMLElement *system_elem)
	{
		const std::string system_type = system_elem->Value();
		SystemPtr system = SystemFactory::Get().Create(system_type);
		if(system)
		{
			XMLSerializePtr  serialize = DYNAMIC_PTR_CAST<IXMLSerialize> (system);
			if(serialize)
				serialize->LoadXML(system_elem);
		}
		return system;
	}

	void BaseSystemManager::Init()
	{
	
	}	
}
