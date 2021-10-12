/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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

#include "Core/Utils/GASSLogger.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSFileUtils.h"
#include "Core/Serialize/tinyxml2.h"

#include "Sim/GASSSceneObjectTemplateManager.h"
#include "Sim/GASSSceneObjectTemplate.h"
#include "Sim/GASSSceneObject.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"


#include <iostream>

namespace GASS
{

	SceneObjectTemplateManager::SceneObjectTemplateManager()
	{

	}

	void SceneObjectTemplateManager::AddTemplate(SceneObjectTemplatePtr obj)
	{
		m_TemplateMap[obj->GetName()] = obj;
		
		SceneObjectTemplate::SceneObjectTemplateIterator children = obj->GetChildren();

		while(children.hasMoreElements())
		{
			SceneObjectTemplatePtr child = children.getNext();
			AddTemplate(child);
		}
	}

	SceneObjectPtr SceneObjectTemplateManager::CreateFromTemplate(const std::string &name) const
	{
		SceneObjectPtr new_cc;
		SceneObjectTemplatePtr cc_tempate =  GetTemplate(name);
		if(cc_tempate)
		{
			new_cc = _CreateSceneObject(cc_tempate);
		}
		else
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to create SceneObject:" + name, "SceneObjectTemplateManager::CreateFromTemplate");
		}
		return new_cc;
	}

	SceneObjectPtr SceneObjectTemplateManager::_CreateSceneObject(SceneObjectTemplatePtr cc_temp) const
	{
		SceneObjectPtr new_object;
		if(cc_temp->GetInheritance() != "")
		{
			SceneObjectTemplatePtr inheritance = GetTemplate(cc_temp->GetInheritance());
			if(inheritance)
			{
				new_object =  _CreateSceneObject(inheritance);
				BaseReflectionObjectPtr ref_obj = GASS_DYNAMIC_PTR_CAST<BaseReflectionObject>(new_object);
				//copy container attributes to new object
				if(ref_obj)
					cc_temp->CopyPropertiesTo(ref_obj);
		
				new_object->SetName(cc_temp->GetName());
				//set template name
				new_object->SetTemplateName(cc_temp->GetName());
				cc_temp->_InheritComponentData(new_object);
			}
			else
			{
				// inheritance template not found!
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Could not find inheritance template:" + cc_temp->GetInheritance(), "SceneObjectTemplateManager::_CreateSceneObject");
			}
		}
		else
		{
			new_object = cc_temp->CreateSceneObject();
			if(new_object)
			{
				new_object->SetName(cc_temp->GetName());
				new_object->SetTemplateName(cc_temp->GetName());
			}
			else
			{
				//failed to create comp container
			}
		}
		//recursive add children
		SceneObjectTemplate::SceneObjectTemplateIterator iter = cc_temp->GetChildren();
		while(iter.hasMoreElements())
		{
			SceneObjectTemplatePtr child = iter.getNext();
			if(child)
			{
				SceneObjectPtr new_cc_child (_CreateSceneObject(child));
				if(new_cc_child)
				{
					//Add new child
					new_object->AddChild(new_cc_child);
				}
			}
		}
		return new_object;
	}

	SceneObjectTemplatePtr SceneObjectTemplateManager::GetTemplate(const std::string &name) const
	{
		TemplateMap::const_iterator pos;
		//find the template
		pos = m_TemplateMap.find(name);

		if (pos != m_TemplateMap.end()) // in map.
		{
			return pos->second;
		}
		SceneObjectTemplatePtr temp;
		return temp;
	}


	bool SceneObjectTemplateManager::HasTemplate(const std::string &name) const
	{
		const auto pos = m_TemplateMap.find(name);
		return (pos != m_TemplateMap.end()) ;
	}

	std::vector<std::string> SceneObjectTemplateManager::GetTemplateNames() const
	{
		std::vector<std::string> templates;
		auto iter = m_TemplateMap.begin();
		while(iter != m_TemplateMap.end())
		{
			templates.push_back(iter->first);
			++iter;
		}
		return templates;
	}


	void SceneObjectTemplateManager::Clear()
	{
		m_TemplateMap.clear();
	}


	void SceneObjectTemplateManager::Load(const std::string &filename)
	{
		if(filename =="")
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS,"No File name provided", "SceneObjectTemplateManager::Load");

		auto *xmlDoc = new tinyxml2::XMLDocument();
		if (xmlDoc->LoadFile(filename.c_str()) != tinyxml2::XML_NO_ERROR)
		{
			delete xmlDoc;
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE, "Failed to load:" + filename,"SceneObjectTemplateManager::Load()");
		}
		tinyxml2::XMLElement *templates = xmlDoc->FirstChildElement("Templates");

		if(templates)
		{
			templates = templates->FirstChildElement();
			// Loop through each template
			while(templates)
			{
				std::string type = templates->Value();
				SceneObjectTemplatePtr container = std::make_shared<SceneObjectTemplate>();
				if(container)
				{
					XMLSerializePtr s_cont = GASS_DYNAMIC_PTR_CAST<IXMLSerialize>(container);
					if(s_cont)
						s_cont->LoadXML(templates);
					AddTemplate(container);
				}
				else
				{
					GASS_LOG(LWARNING) << "Failed to create SceneObjectTemplate:" << type;
				}
				templates  = templates->NextSiblingElement();
			}
		}
		xmlDoc->Clear();
		// Delete our allocated document
		delete xmlDoc;
	}

	void SceneObjectTemplateManager::LoadFromPath(const std::string &path, bool recursive)
	{
		std::vector<std::string> files;
		FileUtils::GetFilesFromPath(files, path, recursive, true);
		for(const auto & file : files)
		{
			if(FileUtils::GetExtension(file) == "template")
				Load(file);
		}
	}
	
}
