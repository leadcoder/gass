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
#include "GUISchemaLoader.h"
#include "Core/ComponentSystem/GASSComponentContainerFactory.h"

#include "Core/ComponentSystem/GASSBaseComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "tinyxml.h"
#include "Core/Reflection/GASSPropertyTypes.h"
#include "Core/Utils/GASSException.h"

namespace GASS
{
	GUISchemaLoader::GUISchemaLoader() 
	{

	}

	GUISchemaLoader::~GUISchemaLoader(void)
	{

	}

	void GUISchemaLoader::LoadAllFromPath(const std::string filepath)
	{
		std::vector<std::string> files;
		Misc::GetFilesFromPath(files, filepath, true, true);
		for(size_t i = 0;  i < files.size(); i++)
		{
			if(Misc::GetExtension(files[i]) == "xsd")
				Load(files[i]);
		}
	}

	void GUISchemaLoader::Load(const std::string filename)
	{
		TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());
		if(!xmlDoc->LoadFile())
		{
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load: " + filename, "GUISchemaLoader::Load");
		}

		TiXmlElement *schema = xmlDoc->FirstChildElement("xs:schema");
		if(schema == NULL)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get xs:schema tag", "GUISchemaLoader::Load");


		TiXmlElement *obj_elem = schema->FirstChildElement("xs:element");
		if(obj_elem == NULL)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get xs:element tag", "GUISchemaLoader::Load");

		if(!obj_elem->Attribute("name"))
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get name attribute", "GUISchemaLoader::Load");

		const std::string name = obj_elem->Attribute("name");

		ObjectSettings os;
		os.Name = name;
		os.DisplayName = name;

		TiXmlElement *anno_elem =  obj_elem->FirstChildElement("xs:annotation");
		if(anno_elem)
		{
			TiXmlElement *gui_elem =  anno_elem->FirstChildElement("xs:guiSettings");
			if(gui_elem)
			{
				os.Visible  = LoadBoolAttribute(gui_elem,"visible");
			}

			TiXmlElement *doc_elem =  anno_elem->FirstChildElement("xs:documentation");
			if(doc_elem)
			{
				os.Documentation = doc_elem->GetText();
			}
		}

		TiXmlElement *elem =  obj_elem->FirstChildElement("xs:complexType");
		if(elem)
		{
			elem =  elem->FirstChildElement("xs:sequence");
			if(elem)
			{
				elem =  elem->FirstChildElement("xs:element");
				while(elem)
				{
					PropertySettings ps = LoadProperty(elem);
					os.AddProperty(ps);
					elem = elem->NextSiblingElement("xs:element");
				}
			}
		}
		m_Settings[os.Name] = os;
	}

	PropertySettings GUISchemaLoader::LoadProperty(TiXmlElement* elem)
	{
		PropertySettings ps;
		if(elem->Attribute("name"))
		{
			ps.Name = elem->Attribute("name");
			ps.DisplayName = ps.Name;
		}

		TiXmlElement *anno_elem =  elem->FirstChildElement("xs:annotation");
		if(anno_elem)
		{
			TiXmlElement *gui_elem =  anno_elem->FirstChildElement("xs:guiSettings");
			if(gui_elem)
			{
				ps.Visible = LoadBoolAttribute(gui_elem,"visible");
				ps.Editable = LoadBoolAttribute(gui_elem,"editable");
			}
			TiXmlElement *doc_elem =  anno_elem->FirstChildElement("xs:documentation");
			if(doc_elem)
			{
				ps.Documentation = doc_elem->GetText();
			}
		}
		return ps;
	}

	bool GUISchemaLoader::LoadBoolAttribute(TiXmlElement* elem, const std::string &name)
	{
		bool ret  = false;
		if(elem && elem->Attribute(name.c_str()))
		{
				const std::string value = GASS::Misc::ToLower(elem->Attribute(name.c_str()));
				if(value == "true")
					ret  = true;
				else
					ret  = false;
		}
		return ret;
	}

	const ObjectSettings*  GUISchemaLoader::GetObjectSettings(const std::string &name) const
	{
		std::map<std::string,ObjectSettings> ::const_iterator iter = m_Settings.find(name);
		if(iter != m_Settings.end())
			return &iter->second;
		return NULL;
	}


	const PropertySettings* ObjectSettings::GetProperty(const std::string &name) const
	{
		std::map<std::string,PropertySettings> ::const_iterator iter = m_PropertySettingsMap.find(name);
		if(iter != m_PropertySettingsMap.end())
			return &iter->second;
		return NULL;
	}
}




