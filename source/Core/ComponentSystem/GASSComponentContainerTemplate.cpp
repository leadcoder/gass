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
#include "Core/ComponentSystem/GASSComponentContainerTemplate.h"
#include "Core/Serialize/GASSSerialize.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentContainerFactory.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplateFactory.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplateManager.h"

#include "Core/Utils/GASSLogger.h"
#include "Core/Utils/GASSException.h"
#include <iostream>
#include <iomanip>
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{
	ComponentContainerTemplate::ComponentContainerTemplate() : m_Serialize(true)
	{

	}

	ComponentContainerTemplate::~ComponentContainerTemplate()
	{

	}

	void ComponentContainerTemplate::AddChild(ComponentContainerTemplatePtr child)
	{
		ComponentContainerTemplateWeakPtr parent = ComponentContainerTemplateWeakPtr(shared_from_this());
		child->SetParent(parent);
		m_ComponentContainerVector.push_back(child);
	}


	void ComponentContainerTemplate::RegisterReflection()
	{
		RegisterProperty<std::string>("Name", &GASS::ComponentContainerTemplate::GetName, &GASS::ComponentContainerTemplate::SetName);
		RegisterProperty<std::string>("Inheritance", &GASS::ComponentContainerTemplate::GetInheritance, &GASS::ComponentContainerTemplate::SetInheritance);
		RegisterProperty<bool>("Serialize", &GASS::ComponentContainerTemplate::GetSerialize, &GASS::ComponentContainerTemplate::SetSerialize);
	}

	bool ComponentContainerTemplate::GetSerialize() const
	{
		return m_Serialize;
	}

	void ComponentContainerTemplate::SetSerialize(bool value)
	{
		m_Serialize = value;
	}

	ComponentPtr ComponentContainerTemplate::GetComponent(const std::string &name) const
	{
		ComponentPtr comp;
		for(size_t i = 0 ; i < m_ComponentVector.size(); i++)
		{
			if(m_ComponentVector[i]->GetName() == name)
				return m_ComponentVector[i];
		}
		return comp;
	}


	void ComponentContainerTemplate::AddComponent(ComponentPtr comp)
	{
		m_ComponentVector.push_back(comp);
	}

	ComponentPtr ComponentContainerTemplate::AddComponent(const std::string &comp_type)
	{
		ComponentPtr comp = ComponentFactory::Get().Create(comp_type);
		if(!comp)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to create Component:" + comp_type,"ComponentContainerTemplate::AddComponent");
		comp->SetName(comp_type);
		AddComponent(comp);
		return comp;
	}

	bool ComponentContainerTemplate::Serialize(ISerializer* serializer)
	{
		if(!BaseReflectionObject::_SerializeProperties(serializer))
			return false;

		if(serializer->Loading())
		{
			int num_comp = 0;
			SerialLoader* loader = dynamic_cast<SerialLoader*>( serializer);
			if (loader)
			{
				loader->IO<int>(num_comp);

				for (int i = 0; i < num_comp; i++)
				{
					std::string comp_type;
					loader->IO<std::string>(comp_type);
					ComponentPtr comp(ComponentFactory::Get().Create(comp_type));
					if (comp)
					{
						SerializePtr s_comp = GASS_DYNAMIC_PTR_CAST<ISerialize>(comp);
						if (s_comp)
						{
							if (!s_comp->Serialize(serializer))
								return false;
						}
						AddComponent(comp);
					}
					else
					{
						GASS_LOG(LWARNING) << "Failed to create component " << comp_type;
					}
				}

				int num_children = 0;
				loader->IO<int>(num_children);
				for (int i = 0; i < num_children; i++)
				{
					const std::string class_name = GetRTTI()->GetClassName();
					ComponentContainerTemplatePtr child = ComponentContainerTemplateFactory::Get().Create(class_name);
					if (!child)
					{
						GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to create ComponentContainerTemplate instance " + class_name, "ComponentContainerTemplate::Serialize");
					}
					//ComponentContainerTemplatePtr child  = GASS_DYNAMIC_PTR_CAST<IComponentContainerTemplate> (CreateInstance());
					if (child)
					{
						SerializePtr s_child = GASS_DYNAMIC_PTR_CAST<ISerialize>(child);
						if (s_child)
						{
							if (!s_child->Serialize(serializer))
								return false;
						}
						AddChild(child);
					}
				}
			}
		}
		else
		{
			int num_comp = static_cast<int>(m_ComponentVector.size());
			SerialSaver* saver = dynamic_cast<SerialSaver*>(serializer);
			if (saver)
			{
				saver->IO<int>(num_comp);

				ComponentVector::iterator iter = m_ComponentVector.begin();
				while (iter != m_ComponentVector.end())
				{
					ComponentPtr comp = (*iter);
					SerializePtr s_comp = GASS_DYNAMIC_PTR_CAST<ISerialize>(comp);
					if (s_comp)
					{
						if (!s_comp->Serialize(serializer))
							return false;

					}
					++iter;
				}

				int num_children = static_cast<int>(m_ComponentContainerVector.size());
				saver->IO<int>(num_children);

				ComponentContainerTemplate::ComponentContainerTemplateVector::iterator go_iter;
				for (go_iter = m_ComponentContainerVector.begin(); go_iter != m_ComponentContainerVector.end(); ++go_iter)
				{
					ComponentContainerTemplatePtr child = *go_iter;
					SerializePtr s_child = GASS_DYNAMIC_PTR_CAST<ISerialize>(child);
					if (s_child)
					{
						if (!s_child->Serialize(serializer))
							return false;
					}
				}
			}
		}
		return true;
	}

	void ComponentContainerTemplate::RemoveChild(ComponentContainerTemplatePtr child)
	{
		ComponentContainerTemplate::ComponentContainerTemplateVector::iterator bo_iter;
		for(bo_iter = m_ComponentContainerVector.begin(); bo_iter!= m_ComponentContainerVector.end(); ++bo_iter)
		{
			if(child == *bo_iter)
			{
				m_ComponentContainerVector.erase(bo_iter);
				return;
			}
		}
	}

	ComponentContainer::ComponentIterator ComponentContainerTemplate::GetComponents()
	{
		return ComponentIterator(m_ComponentVector.begin(), m_ComponentVector.end());
	}


	ComponentContainerTemplate::ComponentContainerTemplateIterator ComponentContainerTemplate::GetChildren()
	{
		return ComponentContainerTemplate::ComponentContainerTemplateIterator(m_ComponentContainerVector.begin(),m_ComponentContainerVector.end());
	}

	void ComponentContainerTemplate::SaveXML(tinyxml2::XMLElement *obj_elem)
	{
		const std::string factory_name = ComponentContainerTemplateFactory::Get().GetFactoryName(GetRTTI()->GetClassName());

		tinyxml2::XMLDocument *rootXMLDoc = obj_elem->GetDocument();
		tinyxml2::XMLElement* this_elem = rootXMLDoc->NewElement(factory_name.c_str() );
		obj_elem->LinkEndChild( this_elem );
		//this_elem->SetAttribute("type", GetRTTI()->GetClassName().c_str());
		_SaveProperties(this_elem);

		tinyxml2::XMLElement* comp_elem = rootXMLDoc->NewElement("Components");
		this_elem->LinkEndChild(comp_elem);

		ComponentVector::iterator iter;
		for(iter = m_ComponentVector.begin(); iter != m_ComponentVector.end(); ++iter)
		{
			ComponentPtr comp = (*iter);
			XMLSerializePtr s_comp = GASS_DYNAMIC_PTR_CAST<IXMLSerialize> (comp);
			if(s_comp)
				s_comp->SaveXML(comp_elem);
		}


		tinyxml2::XMLElement* cc_elem = rootXMLDoc->NewElement("ComponentContainers");
		this_elem->LinkEndChild(cc_elem);

		ComponentContainerTemplate::ComponentContainerTemplateVector::iterator cc_iter;
		for(cc_iter = m_ComponentContainerVector.begin(); cc_iter != m_ComponentContainerVector.end(); ++cc_iter)
		{
			XMLSerializePtr child = GASS_DYNAMIC_PTR_CAST<IXMLSerialize>(*cc_iter);
			if(child)
			{
				child->SaveXML(cc_elem);
			}
		}
	}

	void ComponentContainerTemplate::LoadXML(tinyxml2::XMLElement *obj_elem)
	{
		//m_Name = obj_elem->Value();
		tinyxml2::XMLElement *class_attribute = obj_elem->FirstChildElement();
		while(class_attribute)
		{
			const std::string data_name = class_attribute->Value();
			if(data_name == "Components")
			{
				tinyxml2::XMLElement *comp_elem = class_attribute->FirstChildElement();
				while(comp_elem)
				{
					const std::string comp_name = comp_elem->Value();
					ComponentPtr target_comp (GetComponent(comp_name));
					if(target_comp) //over loading component
					{
						ComponentPtr comp = _LoadComponentXML(comp_elem);
						//ComponentTemplatePtr template_comp = GASS_DYNAMIC_PTR_CAST<IComponentTemplate>(comp);
						if(comp)
						{
							comp->CopyPropertiesTo(target_comp);
						}
					}
					else
					{
						ComponentPtr comp = _LoadComponentXML(comp_elem);
						if(comp)
							AddComponent(comp);
					}
					comp_elem = comp_elem->NextSiblingElement();
				}
			}
			else if(data_name == "ComponentContainers")
			{
				tinyxml2::XMLElement *cc_elem = class_attribute->FirstChildElement();
				while(cc_elem )
				{
					const std::string type = cc_elem->Value(); //Attribute("type");
					ComponentContainerTemplatePtr container (ComponentContainerTemplateFactory::Get().Create(type));
					if(container)
					{
						AddChild(container);
						XMLSerializePtr s_container = GASS_DYNAMIC_PTR_CAST<IXMLSerialize> (container);
						if(s_container)
							s_container->LoadXML(cc_elem);
					}
					else
					{
						GASS_LOG(LWARNING) << "Failed to create ComponentContainer instance from template: " << type;
					}
					cc_elem  = cc_elem->NextSiblingElement();
				}
			}
			else //base object attribute
			{
				//std::string attrib_name = class_attribute->FirstAttribute()->Name();
				if(class_attribute->FirstAttribute())
				{
					const std::string attrib_val = class_attribute->FirstAttribute()->Value();//class_attribute->Attribute(attrib_name);
					try
					{
						SetPropertyByString(data_name,attrib_val);
					}
					catch(...)
					{
						GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed parsing:" + data_name +" With attribute:"+ attrib_val+  " in:" + std::string(obj_elem->GetDocument()->GetFileName()),"ComponentContainerTemplate::LoadXML");
					}
				}
				else
				{
					GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "No value attribute found for xml tag: " + data_name + " In:" + std::string(obj_elem->GetDocument()->GetFileName()), "ComponentContainerTemplate::LoadXML");
				}
			}
			class_attribute  = class_attribute->NextSiblingElement();
		}
	}

	ComponentPtr ComponentContainerTemplate::_LoadComponentXML(tinyxml2::XMLElement *comp_template) const
	{
		const std::string comp_type = comp_template->Value();
		//std::string comp_type = comp_template->Attribute("type");
		ComponentPtr comp (ComponentFactory::Get().Create(comp_type));
		if(comp)
		{
			comp->SetName(comp_type);
			XMLSerializePtr s_comp = GASS_DYNAMIC_PTR_CAST<IXMLSerialize> (comp);
			if(s_comp)
				s_comp->LoadXML(comp_template);
		}
		else
		{
			GASS_LOG(LWARNING) << "Failed to create component "<< comp_type;
		}
		return comp;
	}

	/*std::string ComponentContainerTemplate::CreateUniqueName(ComponentContainerTemplateManagerConstPtr manager) const
	{
		static int object_counter = 0;
		std::stringstream ss;
		ss << GetName() << manager->GetObjectIDPrefix() << object_counter << manager->GetObjectIDSuffix();
		std::string u_name;
		ss >> u_name;
		object_counter++;
		return u_name ;
	}*/

	void ComponentContainerTemplate::_InheritComponentData(ComponentContainerPtr cc) const
	{
		ComponentVector::const_iterator iter;
		for(iter = m_ComponentVector.begin(); iter != m_ComponentVector.end(); ++iter)
		{
			ComponentPtr comp = (*iter);
			if(comp)
			{
				const std::string name = comp->GetName();
				ComponentPtr target_comp (cc->GetComponent(name));
				if(target_comp)
				{
					comp->CopyPropertiesTo(target_comp);
				}
				else
				{
					ComponentPtr new_comp = comp->CreateCopy();
					if(new_comp)
					{
						cc->AddComponent(new_comp);
					}
				}
			}
		}
	}

	/*ComponentContainerPtr ComponentContainerTemplate::CreateComponentContainer(int &part_id, ComponentContainerTemplateManagerConstPtr manager) const
	{
		ComponentContainerPtr new_object;
		if(m_Inheritance != "")
		{
			ComponentContainerTemplatePtr inheritance = manager->GetTemplate(m_Inheritance);
			if(inheritance)
			{
				new_object =  inheritance->CreateComponentContainer(part_id,manager);

				BaseReflectionObjectPtr ref_obj = GASS_DYNAMIC_PTR_CAST<BaseReflectionObject>(new_object);
				//copy container attributes to new object
				if(ref_obj)
					BaseReflectionObject::CopyPropertiesTo(ref_obj);

				if(manager->GetAddObjectIDToName())
					new_object->SetName(CreateUniqueName(manager));
				else
					new_object->SetName(GetName());
				//set template name
				new_object->SetTemplateName(GetName());
				//if(m_ContainerData)
				//	m_ContainerData->Assign(new_object);
				//check if components already exist,
				//if so replace components
				InheritComponentData(new_object);
			}
		}
		else
		{
			new_object = CreateComponentContainer();
			if(new_object)
			{
				//if(m_NameCheck)
				//{
				//ComponentContainerTemplate* obj = SimEngine::GetPtr()->GetLevel()->GetDynamicObjectContainer()->Get(temp);
				//while(obj)
				//{
				//sprintf(temp,"%s_%d",base_name.c_str(),object_counter);
				//object_counter++;
				//obj = SimEngine::GetPtr()->GetLevel()->GetDynamicObjectContainer()->Get(temp);
				//}
				//}
				if(manager->GetAddObjectIDToName())
					new_object->SetName(CreateUniqueName(manager));
				else
					new_object->SetName(GetName());

				new_object->SetTemplateName(GetName());
				//new_object->SetPartId(part_id);
				part_id++;
			}
			else
			{
				//failed to create comp container
			}
		}
		//recursive add children
		ComponentContainerTemplate::ComponentContainerTemplateVector::const_iterator iter;
		for(iter = m_ComponentContainerVector.begin(); iter != m_ComponentContainerVector.end(); ++iter)
		{
			ComponentContainerTemplatePtr child = (*iter);
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
	*/

	ComponentContainerPtr ComponentContainerTemplate::CreateComponentContainer() const
	{
		std::string type = GetRTTI()->GetClassName();
		type = ComponentContainerTemplateFactory::Get().GetFactoryName(type);

		//remove template from name
		const std::string::size_type pos = type.find("Template");
		type = type.substr(0,pos);
		ComponentContainerPtr container (ComponentContainerFactory::Get().Create(type));

		if(!container)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
				"Failed to create instance " + type,"ComponentContainerTemplate::CreateComponentContainer");
		BaseReflectionObjectPtr ref_obj = GASS_DYNAMIC_PTR_CAST<BaseReflectionObject>(container);
		BaseReflectionObject::CopyPropertiesTo(ref_obj);

		ComponentVector::const_iterator iter;
		for(iter = m_ComponentVector.begin(); iter != m_ComponentVector.end(); ++iter)
		{
			ComponentPtr comp = (*iter);
			if(comp)
			{
				ComponentPtr new_comp = comp->CreateCopy();
				container->AddComponent(new_comp);
			}
		}
		return container;
	}

	void ComponentContainerTemplate::CreateFromComponentContainer(ComponentContainerPtr cc,ComponentContainerTemplateManagerConstPtr manager, bool keep_inheritance)
	{
		ComponentContainerTemplatePtr old_temp;
		if(keep_inheritance)
		{
			const std::string old_template_name = cc->GetTemplateName();
			if(old_template_name != "")
			{
				old_temp = GASS_DYNAMIC_PTR_CAST<ComponentContainerTemplate>(manager->GetTemplate(old_template_name));
				if(old_temp)
					SetInheritance(old_temp->GetInheritance());
			}
		}

		BaseReflectionObjectPtr ref_obj = GASS_DYNAMIC_PTR_CAST<BaseReflectionObject>(cc);
		ref_obj->CopyPropertiesTo(shared_from_this());

		ComponentContainer::ComponentIterator comp_iter = cc->GetComponents();
		while(comp_iter.hasMoreElements())
		{
			ComponentPtr comp = comp_iter.getNext();
			if(comp)
			{
				ComponentPtr template_comp = comp->CreateCopy();
				AddComponent(template_comp);
			}
		}

		ComponentContainer::ComponentContainerIterator children = cc->GetChildren();

		while(children.hasMoreElements())
		{
			ComponentContainerPtr child = children.getNext();
			bool found = false;
			if(old_temp)
			{
				ComponentContainerTemplate::ComponentContainerTemplateIterator temp_children = old_temp->GetChildren();
				while(temp_children.hasMoreElements())
				{
					ComponentContainerTemplatePtr temp_child = temp_children.getNext();
					if(child->GetTemplateName() == temp_child->GetName())
					{
						found = true;
						break;
					}
				}
			}
			const std::string factory_class_name = ComponentContainerTemplateFactory::Get().GetFactoryName(GetRTTI()->GetClassName());
			ComponentContainerTemplatePtr new_child = ComponentContainerTemplateFactory::Get().Create(factory_class_name);
			if(!new_child)
			{
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to create ComponentContainerTemplate instance " + factory_class_name,"ComponentContainerTemplate::CreateFromComponentContainer");
			}
			//ComponentContainerTemplatePtr new_child = GASS_DYNAMIC_PTR_CAST<ComponentContainerTemplate>( CreateInstance());
			if(!found && new_child)
			{
				new_child->CreateFromComponentContainer(child,manager,keep_inheritance);
				AddChild(new_child);
			}
		}
	}

#define TAB(val) std::cout << std::setfill(' ') << std::setw(val*3) << std::right << " "; std::cout
	void ComponentContainerTemplate::DebugPrint(int tc)
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
		ComponentContainerTemplate::ComponentContainerTemplateVector::iterator iter;
		if(m_ComponentContainerVector.size() > 0)
		{
			TAB(tc) << "Children" << std::endl;
		}
		for(iter = m_ComponentContainerVector.begin(); iter != m_ComponentContainerVector.end(); ++iter)
		{
			ComponentContainerTemplatePtr child = GASS_STATIC_PTR_CAST<ComponentContainerTemplate>( *iter);
			child->DebugPrint(tc+1);
		}
		//TAB(tc) << "Children - " << std::endl;
		tc--;
		//TAB(tc) << "IComponentContainer - " << std::endl;
	}

}




