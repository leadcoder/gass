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

#include "Core/System/BaseSystemManager.h"
#include "Core/System/ISystem.h"
#include "Core/System/SystemFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/Utils/Log.h"
#include "Core/Serialize/IXMLSerialize.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "tinyxml.h"


namespace GASS
{
	BaseSystemManager::BaseSystemManager()
	{
	}

	BaseSystemManager::~BaseSystemManager()
	{

	}
	
	bool BaseSystemManager::Load(const std::string &filename)
	{
		if(filename =="") return false;
		TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());
		if (!xmlDoc->LoadFile())
		{
			//Fatal error, cannot load
			Log::Warning("BaseSystemManager::Load() - Couldn't load: %s", filename.c_str());
			return 0;
		}
		TiXmlElement *systems = xmlDoc->FirstChildElement("Systems");

		if(systems)
		{
			systems= systems->FirstChildElement();
			//Loop through each template
			while(systems)
			{
				SystemPtr system = LoadSystem(systems);
				if(system)
				{
					system->SetOwner(this);
					system->OnCreate();
					m_Systems.push_back(system);
					systems  = systems->NextSiblingElement();
				}
			}
		}
		xmlDoc->Clear();
		// Delete our allocated document and return success ;)
		delete xmlDoc;
		return 1;
	}

	SystemPtr BaseSystemManager::LoadSystem(TiXmlElement *system_elem)
	{
		std::string system_name = system_elem->Value();
		std::string system_type = system_elem->Attribute("type");
		SystemPtr system = SystemFactory::Get().Create(system_type);
		if(system)
		{
			system->SetName(system_name);
			XMLSerializePtr  serialize = boost::shared_dynamic_cast<IXMLSerialize> (system);
			if(serialize)
				serialize->LoadXML(system_elem);
		}
		return system;
	}

	void BaseSystemManager::Init()
	{
	
	}	

	void BaseSystemManager::Update(double delta_time)
	{

	}
}
