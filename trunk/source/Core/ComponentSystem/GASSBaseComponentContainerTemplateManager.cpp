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
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"
#include "Core/ComponentSystem/GASSIComponentContainerTemplate.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplateFactory.h"
#include "Core/Serialize/GASSIXMLSerialize.h"

#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSFilePath.h"
#include "Core/Utils/GASSFileUtils.h"
#include "tinyxml.h"

#include <iostream>

namespace GASS
{

	BaseComponentContainerTemplateManager::BaseComponentContainerTemplateManager() : m_AddObjectIDToName(true), m_ObjectIDPrefix("_")
	{
		
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

	ComponentContainerPtr BaseComponentContainerTemplateManager::CreateFromTemplate(const std::string &name) const
	{
		ComponentContainerPtr new_cc;
		ComponentContainerTemplatePtr temp =  GetTemplate(name);
		int part_id = 0;
		if(temp)
		{
			ComponentContainerTemplateManagerConstPtr manager = shared_from_this();
			new_cc = temp->CreateComponentContainer(part_id,manager);
		}
		else
		{
			LogManager::getSingleton().stream() << "WARNING:Failed to create ComponentContainer:" << name;
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to create ComponentContainer:" + name, "BaseComponentContainerTemplateManager::CreateFromTemplate");
		}
		return new_cc;
	}

	ComponentContainerTemplatePtr BaseComponentContainerTemplateManager::GetTemplate(const std::string &name) const
	{
		TemplateMap::const_iterator pos;
		//find the template
		pos = m_TemplateMap.find(name);

		if (pos != m_TemplateMap.end()) // in map.
		{
			return pos->second;
		}
		ComponentContainerTemplatePtr temp;
		return temp;
	}

	std::vector<std::string> BaseComponentContainerTemplateManager::GetTemplateNames() const
	{
		std::vector<std::string> templates;
		TemplateMap::const_iterator iter = m_TemplateMap.begin();
		while(iter != m_TemplateMap.end())
		{
			templates.push_back(iter->first);
			++iter;
		}
		return templates;
	}


	void BaseComponentContainerTemplateManager::Load(const std::string &filename)
	{
		if(filename =="")
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS,"No File name provided", "BaseComponentContainerTemplateManager::Load");

		TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());
		if (!xmlDoc->LoadFile())
		{
			delete xmlDoc;
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE, "Failed to load:" + filename,"BaseComponentContainerTemplateManager::Load()");
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
					XMLSerializePtr s_cont = DYNAMIC_PTR_CAST<IXMLSerialize>(container);
					if(s_cont)
						s_cont->LoadXML(templates);
					AddTemplate(container);
				}
				else
				{
					LogManager::getSingleton().stream() << "WARNING:Failed to create ComponentContainerTemplate:" << type;
				}
				templates  = templates->NextSiblingElement();
			}
		}
		xmlDoc->Clear();
		// Delete our allocated document
		delete xmlDoc;
	}

	void BaseComponentContainerTemplateManager::LoadFromPath(const std::string &path, bool recursive)
	{
		std::vector<std::string> files;
		FileUtils::GetFilesFromPath(files, path, recursive, true);
		for(size_t i = 0; i< files.size(); i++)
		{
			if(FileUtils::GetExtension(files[i]) == "xml")
				Load(files[i]);
		}
	}
}
