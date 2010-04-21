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
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"
#include "Core/ComponentSystem/IComponentContainerTemplate.h"
#include "Core/ComponentSystem/ComponentContainerTemplateFactory.h"
#include "Core/Serialize/IXMLSerialize.h"

#include "Core/Utils/Log.h"
#include "tinyxml.h"

#include <iostream>

namespace GASS
{

	BaseComponentContainerTemplateManager::BaseComponentContainerTemplateManager() : m_ForceUniqueName(true)
	{
		//m_NameCheck = false;
	}

	BaseComponentContainerTemplateManager::~BaseComponentContainerTemplateManager()
	{

	}

	void BaseComponentContainerTemplateManager::AddTemplate(ComponentContainerTemplatePtr obj)
	{
		m_TemplateMap[obj->GetName()] = obj;
		
		IComponentContainerTemplate::ComponentContainerTemplateIterator children = obj->GetChildren();

		while(children.hasMoreElements())
		{
			ComponentContainerTemplatePtr child = children.getNext();
			AddTemplate(child);
		}
	}

	ComponentContainerPtr BaseComponentContainerTemplateManager::CreateFromTemplate(const std::string &name)
	{
		ComponentContainerPtr new_cc;
		ComponentContainerTemplatePtr temp =  GetTemplate(name);
		int part_id = 0;
		if(temp)
		{
			BaseComponentContainerTemplateManagerPtr manager = shared_from_this();
			new_cc = temp->CreateComponentContainer(part_id,manager);
		}
		else
			std::cout << "Failed to load template:" << name  << std::endl;
		return new_cc;
	}

	ComponentContainerTemplatePtr BaseComponentContainerTemplateManager::GetTemplate(const std::string &name)
	{
		TemplateMap::iterator pos;
		//find the template
		pos = m_TemplateMap.find(name);

		if (pos != m_TemplateMap.end()) // in map.
		{
			return pos->second;
		}
		ComponentContainerTemplatePtr temp;
		return temp;
	}

	bool BaseComponentContainerTemplateManager::Load(const std::string &filename)
	{
		if(filename =="") return false;
		TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());
		if (!xmlDoc->LoadFile())
		{
			// Fatal error, cannot load
			Log::Warning("BaseComponentContainerTemplateManager::Load() - Couldn't load: %s", filename.c_str());
			return 0;
		}
		TiXmlElement *templates = xmlDoc->FirstChildElement("Templates");

		if(templates)
		{
			templates = templates->FirstChildElement();
			// Loop through each template
			while(templates)
			{
				std::string type = templates->Value();
				ComponentContainerTemplatePtr container = (ComponentContainerTemplateFactory::Get().Create(type));
				if(container)
				{
					XMLSerializePtr s_cont = boost::shared_dynamic_cast<IXMLSerialize>(container);
					if(s_cont)
						s_cont->LoadXML(templates);
					AddTemplate(container);
				}
				else
				{
					Log::Warning("Failed to create ComponentContainerTemplate:%s",type.c_str());
				}
				templates  = templates->NextSiblingElement();
			}
		}
		xmlDoc->Clear();
		// Delete our allocated document and return success ;)
		delete xmlDoc;
		return 1;
	}

	/*void BaseComponentContainerTemplateManager::AddRecursive(IComponentContainer* cc)
	{
		IComponentContainerTemplate* temp = dynamic_cast<IComponentContainerTemplate*>(cc);
		if(temp)
		{
			AddTemplate(temp);
			IComponentContainer::ComponentContainerVector children = cc->GetChildren();
			IComponentContainer::ComponentContainerVector::iterator iter;
			for(iter  = children.begin(); iter != children.end(); iter++)
			{
				AddRecursive(*iter);
			}
		}
	}*/

	
}
