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
#include "Core/Common.h"
#include "Core/ComponentSystem/GASSSchemaManager.h"
#include "Core/ComponentSystem/GASSComponentContainerFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSFileUtils.h"
#include "Core/Serialize/tinyxml2.h"
namespace GASS
{

	SchemaObject::SchemaObject() : m_Document(nullptr), m_Object(nullptr)
	{

	}

	SchemaObject::~SchemaObject()
	{
		delete m_Document;
	}


	tinyxml2::XMLElement* SchemaObject::GetPropertyAnnotation(const std::string &prop_name, const std::string &annotation_tag_name) const
	{
		tinyxml2::XMLElement *elem = m_Object->FirstChildElement("xs:complexType");
		if (!elem)
			return nullptr;
		elem = elem->FirstChildElement("xs:sequence");
		if (!elem)
			return nullptr;

		while (elem)
		{
			if (elem->Attribute("name"))
			{
				const std::string pname = elem->Attribute("name");
				if (pname == prop_name)
				{
					tinyxml2::XMLElement *anno_elem = elem->FirstChildElement("xs:annotation");
					if (!anno_elem)
						return nullptr;
					tinyxml2::XMLElement *user_elem = anno_elem->FirstChildElement(annotation_tag_name.c_str());
					return user_elem;
				}
			}
			elem = elem->NextSiblingElement();
		}
		return nullptr;

	}

	tinyxml2::XMLElement* SchemaObject::GetObjectAnnotation(const std::string &annotation_tag_name) const
	{
		tinyxml2::XMLElement *anno_elem = m_Object->FirstChildElement("xs:annotation");
		if (!anno_elem)
			return nullptr;

		tinyxml2::XMLElement *user_elem = anno_elem->FirstChildElement(annotation_tag_name.c_str());
		return user_elem;


	}

	SchemaManager::SchemaManager()
	{

	}

	SchemaManager::~SchemaManager()
	{

	}

	void SchemaManager::Generate(const std::string& outpath) const
	{
		std::vector<std::string> names = ComponentContainerFactory::GetPtr()->GetFactoryNames();
		for (const auto & name : names)
		{
			ComponentContainerPtr container(ComponentContainerFactory::Get().Create(name));
			BaseReflectionObjectPtr bro = GASS_DYNAMIC_PTR_CAST<BaseReflectionObject>(container);
			_Save(outpath, name, bro);
		}

		names = ComponentFactory::GetPtr()->GetFactoryNames();
		for (const auto & name : names)
		{
			ComponentPtr comp(ComponentFactory::Get().Create(name));
			BaseReflectionObjectPtr bro = GASS_DYNAMIC_PTR_CAST<BaseReflectionObject>(comp);
			std::string class_name = bro->GetRTTI()->GetClassName();
			if (bro)
				_Save(outpath, class_name, bro);
		}
	}

	void SchemaManager::_Save(const std::string& outpath, const std::string &classname, BaseReflectionObjectPtr object) const
	{
		//Create xml file
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLDeclaration* decl = doc.NewDeclaration();
		doc.LinkEndChild(decl);

		tinyxml2::XMLElement * xsd_elem = doc.NewElement("xs:schema");
		doc.LinkEndChild(xsd_elem);

		xsd_elem->SetAttribute("xmlns:xs", "http://www.w3.org/2001/XMLSchema");
		xsd_elem->SetAttribute("xmlns:wmh", "http://www.wmhelp.com/2003/eGenerator");
		xsd_elem->SetAttribute("elementFormDefault", "qualified");
		xsd_elem->SetAttribute("targetNamespace", "GASS");
		xsd_elem->SetAttribute("xmlns", "GASS");
		xsd_elem->SetAttribute("xmlns:GASS", "GASS");

		tinyxml2::XMLElement * obj_elem = doc.NewElement("xs:element");
		xsd_elem->LinkEndChild(obj_elem);
		obj_elem->SetAttribute("name", classname.c_str());

		tinyxml2::XMLElement * ano_elem = doc.NewElement("xs:annotation");
		obj_elem->LinkEndChild(ano_elem);

		tinyxml2::XMLElement * doc_elem = doc.NewElement("xs:documentation");
		ano_elem->LinkEndChild(doc_elem);

		doc_elem->SetAttribute("xml:lang", "en");
		tinyxml2::XMLText * text = doc.NewText("Documentation goes here!");
		doc_elem->LinkEndChild(text);


		tinyxml2::XMLElement * complex_elem = doc.NewElement("xs:complexType");
		obj_elem->LinkEndChild(complex_elem);

		tinyxml2::XMLElement * sequence_elem = doc.NewElement("xs:sequence");
		complex_elem->LinkEndChild(sequence_elem);

		GASS::PropertyVector props = object->GetProperties();
		for (auto & property : props)
		{
			const std::string prop_name = property->GetName();
			_SaveProp(sequence_elem, property);
		}
		std::string filename = outpath + classname;
		filename += ".xsd";
		doc.SaveFile(filename.c_str());
	}

	void SchemaManager::_SaveProp(tinyxml2::XMLElement* parent, IProperty* prop) const
	{
		tinyxml2::XMLDocument *rootXMLDoc = parent->GetDocument();
		tinyxml2::XMLElement * prop_elem = rootXMLDoc->NewElement("xs:element");
		parent->LinkEndChild(prop_elem);
		prop_elem->SetAttribute("name", prop->GetName().c_str());

		tinyxml2::XMLElement * ano_elem = rootXMLDoc->NewElement("xs:annotation");
		prop_elem->LinkEndChild(ano_elem);
		tinyxml2::XMLElement * doc_elem = rootXMLDoc->NewElement("xs:documentation");
		ano_elem->LinkEndChild(doc_elem);
		doc_elem->SetAttribute("xml:lang", "en");

		tinyxml2::XMLText * text = rootXMLDoc->NewText("Documentation goes here!");
		doc_elem->LinkEndChild(text);

		tinyxml2::XMLElement * complex_elem = rootXMLDoc->NewElement("xs:complexType");
		prop_elem->LinkEndChild(complex_elem);

		tinyxml2::XMLElement * attrib_elem = rootXMLDoc->NewElement("xs:attribute");
		complex_elem->LinkEndChild(attrib_elem);

		attrib_elem->SetAttribute("name", "value");
		std::string p_type = _GetPropType(prop);
		attrib_elem->SetAttribute("type", p_type.c_str());
		attrib_elem->SetAttribute("use", "required");
	}

	std::string SchemaManager::_GetPropType(IProperty* prop) const
	{
		std::string type_id;

		if (*prop->GetTypeID() == typeid(std::string))
		{
			type_id = "xs:string";
		}
		if (type_id == "")
			type_id = "xs:" + prop->GetTypeName();
		return type_id;
	}

	void SchemaManager::LoadAllFromPath(const std::string filepath)
	{
		std::vector<std::string> files;
		FileUtils::GetFilesFromPath(files, filepath, true, true);
		for (const auto & file : files)
		{
			if (FileUtils::GetExtension(file) == "xsd")
				Load(file);
		}
	}

	void SchemaManager::Load(const std::string filename)
	{
		auto *xmlDoc = new tinyxml2::XMLDocument();
		if (xmlDoc->LoadFile(filename.c_str()) != tinyxml2::XML_NO_ERROR)
		{
			delete xmlDoc;
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE, "Couldn't load: " + filename, "SchemaManager::Load");
		}

		tinyxml2::XMLElement *schema = xmlDoc->FirstChildElement("xs:schema");
		if (schema == nullptr)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to get xs:schema tag", "SchemaManager::Load");


		tinyxml2::XMLElement *obj_elem = schema->FirstChildElement("xs:element");
		if (obj_elem == nullptr)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to get xs:element tag", "SchemaManager::Load");

		if (!obj_elem->Attribute("name"))
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to get name attribute", "SchemaManager::Load");

		const std::string name = obj_elem->Attribute("name");
		SchemaObject object;
		object.m_Name = name;
		object.m_Document = xmlDoc;
		object.m_Object = obj_elem;
		m_Objects[name] = object;
	}


	const SchemaObject*  SchemaManager::GetSchemaObject(const std::string &name) const
	{
		auto iter = m_Objects.find(name);
		if (iter != m_Objects.end())
			return &iter->second;
		return nullptr;
	}
}