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
#include "Core/ComponentSystem/GASSSchemaManager.h"
#include "Core/ComponentSystem/GASSComponentContainerFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSFileUtils.h"
#include "tinyxml.h"
namespace GASS
{

	SchemaObject::SchemaObject() : m_Document(NULL),m_Object(NULL)
	{

	}

	SchemaObject::~SchemaObject() 
	{
		delete m_Document;
	}


	TiXmlElement* SchemaObject::GetPropertyAnnotation(const std::string &prop_name,const std::string &annotation_tag_name) const
	{
		TiXmlElement *elem =  m_Object->FirstChildElement("xs:complexType");
		if(!elem)
			return NULL;
		elem =  elem->FirstChildElement("xs:sequence");
		if(!elem)
			return NULL;

		while(elem)
		{
			if(elem->Attribute("name"))
			{
				const std::string pname = elem->Attribute("name");
				if(pname == prop_name)
				{
					TiXmlElement *anno_elem =  elem->FirstChildElement("xs:annotation");
					if(!anno_elem)
						return NULL;
					TiXmlElement *user_elem =  anno_elem->FirstChildElement(annotation_tag_name.c_str());
					return user_elem;
				}
			}
			elem = elem->NextSiblingElement();
		}
		return NULL;

	}

	TiXmlElement* SchemaObject::GetObjectAnnotation(const std::string &annotation_tag_name) const
	{
		TiXmlElement *anno_elem =  m_Object->FirstChildElement("xs:annotation");
		if(!anno_elem)
			return NULL;

		TiXmlElement *user_elem =  anno_elem->FirstChildElement(annotation_tag_name.c_str());
		return user_elem;


	}

	SchemaManager::SchemaManager() 
	{

	}

	SchemaManager::~SchemaManager(void)
	{

	}

	void SchemaManager::Generate(const std::string& outpath)
	{
		std::vector<std::string> names= ComponentContainerFactory::GetPtr()->GetFactoryNames();
		for(size_t i = 0 ; i < names.size(); i++)
		{
			ComponentContainerPtr container (ComponentContainerFactory::Get().Create(names[i]));
			BaseReflectionObjectPtr bro = DYNAMIC_PTR_CAST<BaseReflectionObject>(container);
			Save(outpath,names[i],bro);
		}

		names = ComponentFactory::GetPtr()->GetFactoryNames();
		for(size_t i = 0 ; i < names.size(); i++)
		{
			ComponentPtr comp (ComponentFactory::Get().Create(names[i]));
			BaseReflectionObjectPtr bro = DYNAMIC_PTR_CAST<BaseReflectionObject>(comp);
			std::string class_name = bro->GetRTTI()->GetClassName();
			if(bro)
				Save(outpath,class_name,bro);
		}
	}

	void SchemaManager::Save(const std::string& outpath, const std::string &classname, BaseReflectionObjectPtr object)
	{
		//Create xml file
		TiXmlDocument doc;  
		TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
		doc.LinkEndChild( decl ); 

		TiXmlElement * xsd_elem = new TiXmlElement("xs:schema");  
		doc.LinkEndChild( xsd_elem);

		xsd_elem->SetAttribute("xmlns:xs", "http://www.w3.org/2001/XMLSchema");
		xsd_elem->SetAttribute("xmlns:wmh", "http://www.wmhelp.com/2003/eGenerator");
		xsd_elem->SetAttribute("elementFormDefault","qualified");
		xsd_elem->SetAttribute("targetNamespace","GASS");
		xsd_elem->SetAttribute("xmlns","GASS");
		xsd_elem->SetAttribute("xmlns:GASS","GASS");

		TiXmlElement * obj_elem = new TiXmlElement("xs:element");  
		xsd_elem->LinkEndChild( obj_elem); 
		obj_elem->SetAttribute("name",classname.c_str());

		TiXmlElement * ano_elem = new TiXmlElement("xs:annotation");  
		obj_elem->LinkEndChild( ano_elem);

		TiXmlElement * doc_elem = new TiXmlElement("xs:documentation");  
		ano_elem->LinkEndChild( doc_elem); 

		doc_elem->SetAttribute("xml:lang","en");
		TiXmlText * text = new TiXmlText( "Documentation goes here!" );
		doc_elem->LinkEndChild( text );


		TiXmlElement * complex_elem = new TiXmlElement("xs:complexType");  
		obj_elem->LinkEndChild( complex_elem);

		TiXmlElement * sequence_elem = new TiXmlElement("xs:sequence");  
		complex_elem->LinkEndChild( sequence_elem);

		GASS::PropertyVector props = object->GetProperties();
		for(size_t i = 0;  i < props.size(); i++)
		{
			const std::string prop_name = props[i]->GetName();
			SaveProp(sequence_elem, props[i]);
		}
		std::string filename = outpath + classname;
		filename += ".xsd";
		doc.SaveFile(filename.c_str());
	}

	void SchemaManager::SaveProp(TiXmlElement* parent, IProperty* prop) const 
	{
		TiXmlElement * prop_elem = new TiXmlElement("xs:element");  
		parent->LinkEndChild(prop_elem);
		prop_elem->SetAttribute("name",prop->GetName().c_str());

		TiXmlElement * ano_elem = new TiXmlElement("xs:annotation");  
		prop_elem->LinkEndChild( ano_elem);
		TiXmlElement * doc_elem = new TiXmlElement("xs:documentation");  
		ano_elem->LinkEndChild( doc_elem); 
		doc_elem->SetAttribute("xml:lang","en");

		TiXmlText * text = new TiXmlText( "Documentation goes here!" );
		doc_elem->LinkEndChild( text );

		TiXmlElement * complex_elem = new TiXmlElement("xs:complexType");  
		prop_elem->LinkEndChild(complex_elem);

		TiXmlElement * attrib_elem = new TiXmlElement("xs:attribute");  
		complex_elem->LinkEndChild(attrib_elem);

		attrib_elem->SetAttribute("name","value");
		std::string p_type = GetPropType(prop);
		attrib_elem->SetAttribute("type",p_type.c_str());
		attrib_elem->SetAttribute("use","required");
	}

	std::string SchemaManager::GetPropType(IProperty* prop) const
	{
		std::string type_id;

		if(*prop->GetTypeID() == typeid(std::string))
		{
			type_id = "xs:string";
		}
		if(type_id == "")
			type_id = "xs:" + prop->GetTypeName();
		return type_id;
	}

	void SchemaManager::LoadAllFromPath(const std::string filepath)
	{
		std::vector<std::string> files;
		FileUtils::GetFilesFromPath(files, filepath, true, true);
		for(size_t i = 0;  i < files.size(); i++)
		{
			if(FileUtils::GetExtension(files[i]) == "xsd")
				Load(files[i]);
		}
	}

	void SchemaManager::Load(const std::string filename)
	{
		TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());
		if(!xmlDoc->LoadFile())
		{
			delete xmlDoc;
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load: " + filename, "SchemaManager::Load");
		}

		TiXmlElement *schema = xmlDoc->FirstChildElement("xs:schema");
		if(schema == NULL)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get xs:schema tag", "SchemaManager::Load");


		TiXmlElement *obj_elem = schema->FirstChildElement("xs:element");
		if(obj_elem == NULL)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get xs:element tag", "SchemaManager::Load");

		if(!obj_elem->Attribute("name"))
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get name attribute", "SchemaManager::Load");

		const std::string name = obj_elem->Attribute("name");
		SchemaObject object;
		object.m_Name = name;
		object.m_Document = xmlDoc;
		object.m_Object = obj_elem;
		m_Objects[name] = object;
	}


	const SchemaObject*  SchemaManager::GetSchemaObject(const std::string &name) const
	{
		std::map<std::string,SchemaObject> ::const_iterator iter = m_Objects.find(name);
		if(iter != m_Objects.end())
			return &iter->second;
		return NULL;
	}
}




