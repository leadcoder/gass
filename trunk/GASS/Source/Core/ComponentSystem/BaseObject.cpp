/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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
#include "Core/ComponentSystem/BaseObject.h"
#include "Core/Serialize/Serialize.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/ComponentSystem/IComponentTemplate.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/ComponentContainerFactory.h"
#include "Core/ComponentSystem/IComponentContainerTemplateManager.h"

#include "Core/MessageSystem/MessageManager.h"
#include <iostream>
#include <iomanip>
#include <tinyxml.h>

namespace GASS
{
	BaseObject::BaseObject()
	{

	}

	BaseObject::~BaseObject(void)
	{

	}

	void BaseObject::AddChild(ComponentContainerPtr child)
	{
		ComponentContainerWeakPtr parent = ComponentContainerWeakPtr(shared_from_this());
		child->SetParent(parent);
		m_ComponentContainerVector.push_back(child);
	}

/*	ComponentContainerPtr BaseObject::GetChild(const std::string &name)
	{
		
		for(int i = 0 ; i < m_ComponentContainerVector.size(); i++)
		{
			
			if(m_ComponentContainerVector[i]->GetName() == name) 
			{
				return m_ComponentContainerVector[i];
			}
		}
		ComponentContainerPtr cc;
		return cc;
	}*/

	void BaseObject::RegisterReflection()
	{
		//IComponentContainerFactory::GetPtr()->Register("IComponentContainer",new Creator<IComponentContainer, IComponentContainer>);
		RegisterProperty<std::string>("Name", &GASS::BaseObject::GetName, &GASS::BaseObject::SetName);
		RegisterProperty<std::string>("Inheritance", &GASS::BaseObject::GetInheritance, &GASS::BaseObject::SetInheritance);
	}


	IComponentContainer::ComponentVector BaseObject::GetComponents()
	{
		return m_ComponentVector;
	}

	ComponentPtr BaseObject::GetComponent(const std::string &name)
	{
		ComponentPtr comp;
		for(int i = 0 ; i < m_ComponentVector.size(); i++)
		{
			if(m_ComponentVector[i]->GetName() == name)
				return m_ComponentVector[i];
		}
		return comp;
	}


	void BaseObject::AddComponent(ComponentPtr comp)
	{
		comp->SetOwner(shared_from_this());
		m_ComponentVector.push_back(comp);
	}

	bool BaseObject::Serialize(ISerializer* serializer)
	{
		if(!BaseReflectionObject::SerializeProperties(serializer))
			return false;

		if(serializer->Loading())
		{
			int num_comp;
			serializer->IO(num_comp);
			for(int i  = 0 ; i < num_comp; i++)
			{
				std::string comp_type;
				serializer->IO(comp_type);
				ComponentPtr comp (ComponentFactory::Get().Create(comp_type));
				SerializePtr s_comp = boost::shared_dynamic_cast<ISerialize>(comp);
				if(s_comp)
				{
					if(!s_comp->Serialize(serializer))
						return false;
				}
				AddComponent(comp);
			}

			int num_children;
			serializer->IO(num_children);
			for(int i  = 0 ; i < num_children; i++)
			{
				ComponentContainerPtr child  = boost::shared_dynamic_cast<IComponentContainer> (CreateInstance());
				if(child)
				{
					SerializePtr s_child = boost::shared_dynamic_cast<ISerialize>(child);
					if(s_child)
					{
						if(!s_child->Serialize(serializer))
							return false;
					}
					AddChild(child);
				}
			}
		}
		else
		{
			int num_comp = m_ComponentVector.size();
			serializer->IO(num_comp);
			ComponentVector::iterator iter = m_ComponentVector.begin();
			while (iter != m_ComponentVector.end())
			{
				ComponentPtr comp = (*iter);
				SerializePtr s_comp = boost::shared_dynamic_cast<ISerialize>(comp);
				if(s_comp)
				{
					if(!s_comp->Serialize(serializer))
						return false;
					
				}
				++iter;
			}

			int num_children = m_ComponentContainerVector.size();
			serializer->IO(num_children);
			BaseObject::ComponentContainerVector::iterator go_iter;
			for(go_iter = m_ComponentContainerVector.begin(); go_iter != m_ComponentContainerVector.end(); go_iter++)
			{
				ComponentContainerPtr child = *go_iter;
				SerializePtr s_child = boost::shared_dynamic_cast<ISerialize>(child);
				if(s_child)
				{
					if(!s_child->Serialize(serializer))
						return false;
				}
			}
		}
		return true;
	}

	void BaseObject::OnCreate()
	{
		ComponentVector::iterator iter = m_ComponentVector.begin();
		while (iter != m_ComponentVector.end())
		{
			ComponentPtr comp = (*iter);
			comp->OnCreate();
			++iter;
		}
		BaseObject::ComponentContainerVector::iterator go_iter;
		for(go_iter = m_ComponentContainerVector.begin(); go_iter != m_ComponentContainerVector.end(); go_iter++)
		{
			ComponentContainerPtr child = *go_iter;
			child->OnCreate();
		}
	}

/*	void BaseObject::SetAttributes(BaseReflectionObject* obj)
	{
		BaseReflectionObject::SetProperties(obj);
	}

	bool BaseObject::SetAttribute(const std::string &attrib_name,const std::string &attrib_val)
	{
		return BaseReflectionObject::SetProperty(attrib_name, attrib_val);
	}*/

	void BaseObject::SaveXML(TiXmlElement *obj_elem)
	{
	
	}

	void BaseObject::LoadXML(TiXmlElement *obj_elem)
	{
		m_Name = obj_elem->Value();
		TiXmlElement *class_attribute = obj_elem->FirstChildElement();
		while(class_attribute)
		{
			std::string data_name = class_attribute->Value();
			if(data_name == "Components")
			{
				TiXmlElement *comp_elem = class_attribute->FirstChildElement();
				while(comp_elem)
				{
					ComponentPtr comp = LoadComponent(comp_elem);
					if(comp)
						AddComponent(comp);
					comp_elem = comp_elem->NextSiblingElement();
				}
			}
			else if(data_name == "ComponentContainers")
			{
				TiXmlElement *cc_elem = class_attribute->FirstChildElement();
				while(cc_elem )
				{
					std::string type = cc_elem->Attribute("type");
					ComponentContainerPtr container (ComponentContainerFactory::Get().Create(type));
					AddChild(container);
					XMLSerializePtr s_container = boost::shared_dynamic_cast<IXMLSerialize> (container);
					if(s_container)
						s_container->LoadXML(cc_elem);
					cc_elem  = cc_elem->NextSiblingElement();
				}
			}
			else //base object attribute
			{
				//std::string attrib_name = class_attribute->FirstAttribute()->Name();
				std::string attrib_val = class_attribute->FirstAttribute()->Value();//class_attribute->Attribute(attrib_name);
				SetPropertyByString(data_name,attrib_val);
			}
			class_attribute  = class_attribute->NextSiblingElement();
		}
	}

	ComponentPtr BaseObject::LoadComponent(TiXmlElement *comp_template)
	{
		std::string comp_name = comp_template->Value();
		std::string comp_type = comp_template->Attribute("type");
		ComponentPtr comp (ComponentFactory::Get().Create(comp_type));
		if(comp)
		{
			comp->SetName(comp_name);
			XMLSerializePtr s_comp = boost::shared_dynamic_cast<IXMLSerialize> (comp);
			if(s_comp)
				s_comp->LoadXML(comp_template);
		}
		return comp;
	}


	std::string BaseObject::CreateUniqueName()
	{
		static int object_counter = 0;
		std::stringstream ss;
		ss << GetName() << "_" << object_counter;
		std::string u_name;
		ss >> u_name;
		object_counter++;
		return u_name ;
	}

	void BaseObject::InheritComponentData(ComponentContainerPtr cc)
	{
		ComponentVector::iterator iter; 
		for(iter = m_ComponentVector.begin(); iter != m_ComponentVector.end(); iter++)
		{
			ComponentPtr comp = (*iter);
			ComponentTemplatePtr template_comp = boost::shared_dynamic_cast<IComponentTemplate>(comp);
			if(template_comp)
			{
				std::string name = comp->GetName();
				ComponentPtr target_comp (cc->GetComponent(name));
				if(target_comp)
				{
					template_comp->Assign(target_comp);
				}
				else
				{
					ComponentPtr new_comp = template_comp->CreateCopy();
					if(new_comp)
					{
						cc->AddComponent(new_comp);
					}
				}
			}
		}
	}

	ComponentContainerPtr BaseObject::CreateComponentContainer(int &part_id, ComponentContainerTemplateManagerPtr manager)
	{
		ComponentContainerPtr new_object;
		if(m_Inheritance != "")
		{
			ComponentContainerTemplatePtr inheritance = manager->GetTemplate(m_Inheritance);
			if(inheritance)
			{
				new_object =  inheritance->CreateComponentContainer(part_id,manager);

				if(manager->GetForceUniqueName())
					new_object->SetName(CreateUniqueName());
				else 
					new_object->SetName(GetName());
				//if(m_ContainerData)
				//	m_ContainerData->Assign(new_object);
				//check if components already exist,
				//if so replace components 
				InheritComponentData(new_object);
			}
		}
		else
		{
			new_object = CreateCopy();//ComponentContainerFactory::Get().Create(m_Type);
			if(new_object)
			{
				//AddComponentData(new_object);
				
				/*if(m_NameCheck)
				{
				BaseObject* obj = SimEngine::GetPtr()->GetLevel()->GetDynamicObjectContainer()->Get(temp);
				while(obj)
				{
				sprintf(temp,"%s_%d",base_name.c_str(),object_counter);
				object_counter++;
				obj = SimEngine::GetPtr()->GetLevel()->GetDynamicObjectContainer()->Get(temp);
				}
				}*/
				if(manager->GetForceUniqueName())
					new_object->SetName(CreateUniqueName());
				else 
					new_object->SetName(GetName());
				//new_object->SetPartId(part_id);
				part_id++;
				/*{
					//IReflection* ref = dynamic_cast<IReflection*> (this);
					BaseReflectionObject* ref_new_obj = dynamic_cast<BaseReflectionObject*> (new_object);
					if(ref && ref_new_obj)
						SetAttributes(ref_new_obj);
				}*/
			}
			else
			{
				//failed to create comp container
			}
		}
		//recursive add children
		BaseObject::ComponentContainerVector::iterator iter;
		for(iter = m_ComponentContainerVector.begin(); iter != m_ComponentContainerVector.end(); iter++)
		{
			ComponentContainerTemplatePtr child = boost::shared_dynamic_cast<IComponentContainerTemplate>(*iter);
			if(child)
			{
				ComponentContainerPtr new_child (child->CreateComponentContainer(part_id,manager));
				if(new_child)
				{
					//Add new child
					new_object->AddChild(new_child);
				}
			}
		}
		return new_object;
	}


	ComponentContainerPtr BaseObject::CreateCopy()
	{
		BaseObjectPtr new_obj = boost::shared_static_cast<BaseObject>(CreateInstance());
		BaseReflectionObject::SetProperties(new_obj);
		
		ComponentVector::iterator iter; 
		for(iter = m_ComponentVector.begin(); iter != m_ComponentVector.end(); iter++)
		{
			ComponentPtr comp = (*iter);
			ComponentTemplatePtr temp_comp = boost::shared_dynamic_cast<IComponentTemplate>(comp);
			if(temp_comp)
			{
				ComponentPtr new_comp = temp_comp->CreateCopy();
				new_obj->AddComponent(new_comp);
			}
		}
		return new_obj;
	}

	#define TAB(val) std::cout << std::setfill(' ') << std::setw(val*3) << std::right << " "; std::cout
	void BaseObject::DebugPrint(int tc)
	{

		TAB(tc) << GetRTTI()->GetClassName() <<" - " << GetName() << std::endl;
		tc++;
		if(m_ComponentVector.size() > 0)
		{
			TAB(tc) << "Components" << std::endl;
		}
		ComponentVector::iterator comp_iter = m_ComponentVector.begin();
		tc++;
		while (comp_iter != m_ComponentVector.end())
		{
			ComponentPtr comp = (*comp_iter);
			//std::string comp_type = comp->GetRTTI()->GetClassName();
			//TAB(tc) << comp_type << " - " << comp->GetName() << std::endl;
			TAB(tc) << comp->GetName() << std::endl;
			++comp_iter;
		}
		tc--;
		//TAB(tc) << "Components - " << std::endl;
		BaseObject::ComponentContainerVector::iterator iter;
		if(m_ComponentContainerVector.size() > 0)
		{
			TAB(tc) << "Children" << std::endl;
		}
		for(iter = m_ComponentContainerVector.begin(); iter != m_ComponentContainerVector.end(); iter++)
		{
			BaseObjectPtr child = boost::shared_static_cast<BaseObject>( *iter);
			child->DebugPrint(tc+1);
		}
		//TAB(tc) << "Children - " << std::endl;
		tc--;
		//TAB(tc) << "IComponentContainer - " << std::endl;
	}

}




