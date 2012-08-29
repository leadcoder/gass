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
#include "Core/Utils/GASSLogManager.h"
#include "Core/ComponentSystem/GASSBaseComponentContainer.h"
#include "Core/Serialize/GASSSerialize.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/ComponentSystem/GASSIComponentTemplate.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentContainerFactory.h"
#include "Core/ComponentSystem/GASSIComponentContainerTemplateManager.h"

#include <iostream>
#include <iomanip>
#include <tinyxml.h>

namespace GASS
{
	BaseComponentContainer::BaseComponentContainer() : m_Serialize(true)
	{
		
	}

	BaseComponentContainer::~BaseComponentContainer(void)
	{

	}

	void BaseComponentContainer::AddChild(ComponentContainerPtr child)
	{
		ComponentContainerWeakPtr parent = ComponentContainerWeakPtr(shared_from_this());
		child->SetParent(parent);
		m_ComponentContainerVector.push_back(child);
	}


	void BaseComponentContainer::RegisterReflection()
	{
		RegisterProperty<std::string>("Name", &GASS::BaseComponentContainer::GetName, &GASS::BaseComponentContainer::SetName);
		RegisterProperty<std::string>("TemplateName", &GASS::BaseComponentContainer::GetTemplateName, &GASS::BaseComponentContainer::SetTemplateName);
		RegisterProperty<bool>("Serialize", &GASS::BaseComponentContainer::GetSerialize, &GASS::BaseComponentContainer::SetSerialize);
	}

	IComponentContainer::ComponentIterator BaseComponentContainer::GetComponents()
	{
		return ComponentIterator(m_ComponentVector.begin(), m_ComponentVector.end());
	}

	IComponentContainer::ConstComponentIterator BaseComponentContainer::GetComponents() const
	{
		return ConstComponentIterator(m_ComponentVector.begin(), m_ComponentVector.end());
	}

	ComponentPtr BaseComponentContainer::GetComponent(const std::string &name) const
	{
		ComponentPtr comp;
		for(size_t i = 0 ; i < m_ComponentVector.size(); i++)
		{
			if(m_ComponentVector[i]->GetName() == name)
				return m_ComponentVector[i];
		}
		return comp;
	}


	void BaseComponentContainer::AddComponent(ComponentPtr comp)
	{
		comp->SetOwner(shared_from_this());
		m_ComponentVector.push_back(comp);
	}

	bool BaseComponentContainer::Serialize(ISerializer* serializer)
	{
		if(!m_Serialize)
			return true;
		if(!BaseReflectionObject::SerializeProperties(serializer))
			return false;

		if(serializer->Loading())
		{
			int num_comp;
			SerialLoader* loader = (SerialLoader*) serializer;
			
			loader->IO<int>(num_comp);
			for(int i  = 0 ; i < num_comp; i++)
			{
				std::string comp_type;
				loader->IO<std::string>(comp_type);
				ComponentPtr comp (ComponentFactory::Get().Create(comp_type));
				if(comp)
				{
					SerializePtr s_comp = boost::shared_dynamic_cast<ISerialize>(comp);
					if(s_comp)
					{
						if(!s_comp->Serialize(serializer))
							return false;
					}
					AddComponent(comp);
				}
				else
				{
					LogManager::getSingleton().stream() << "WARNING:Failed to create component " << comp_type;
				}
			}

			int num_children;
			loader->IO<int>(num_children);
			for(int i  = 0 ; i < num_children; i++)
			{

				//TODO: need to change this, should save componentcontainer type instead and create from factory, (same way as components are created)
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
			int num_comp = static_cast<int>(m_ComponentVector.size());
			SerialSaver* saver = (SerialSaver*) serializer;
			
			saver->IO<int>(num_comp);
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

			int num_children = static_cast<int>(m_ComponentContainerVector.size());
			saver->IO<int>(num_children);
			BaseComponentContainer::ComponentContainerVector::iterator go_iter;
			for(go_iter = m_ComponentContainerVector.begin(); go_iter != m_ComponentContainerVector.end(); ++go_iter)
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

	void BaseComponentContainer::RemoveChild(ComponentContainerPtr child)
	{
		BaseComponentContainer::ComponentContainerVector::iterator bo_iter;
		for(bo_iter = m_ComponentContainerVector.begin(); bo_iter!= m_ComponentContainerVector.end(); ++bo_iter)
		{
			if(child == *bo_iter)
			{
				m_ComponentContainerVector.erase(bo_iter);
				return;
			}
		}
	}

	void BaseComponentContainer::SaveXML(TiXmlElement *obj_elem)
	{
		if(!m_Serialize)
			return;
		std::string factory_name = ComponentContainerFactory::Get().GetFactoryName(GetRTTI()->GetClassName());

		TiXmlElement* this_elem = new TiXmlElement( factory_name.c_str() );  
		obj_elem->LinkEndChild( this_elem );  
		//this_elem->SetAttribute("type", GetRTTI()->GetClassName().c_str());
		SaveProperties(this_elem);

		TiXmlElement* comp_elem = new TiXmlElement("Components");
		this_elem->LinkEndChild(comp_elem);

		ComponentVector::iterator iter; 
		for(iter = m_ComponentVector.begin(); iter != m_ComponentVector.end(); ++iter)
		{
			ComponentPtr comp = (*iter);
			XMLSerializePtr s_comp = boost::shared_dynamic_cast<IXMLSerialize> (comp);
			if(s_comp)
				s_comp->SaveXML(comp_elem);
		}

		TiXmlElement* cc_elem = new TiXmlElement("ComponentContainers");
		this_elem->LinkEndChild(cc_elem);
	
		BaseComponentContainer::ComponentContainerVector::iterator cc_iter;
		for(cc_iter = m_ComponentContainerVector.begin(); cc_iter != m_ComponentContainerVector.end(); ++cc_iter)
		{
			XMLSerializePtr child = boost::shared_dynamic_cast<IXMLSerialize>(*cc_iter);
			if(child)
			{
				child->SaveXML(cc_elem);
			}
		}
	}

	void BaseComponentContainer::LoadXML(TiXmlElement *obj_elem)
	{
		if(!m_Serialize)
			return;
		TiXmlElement *class_attribute = obj_elem->FirstChildElement();
		while(class_attribute)
		{
			const std::string data_name = class_attribute->Value();
			if(data_name == "Components")
			{
				TiXmlElement *comp_elem = class_attribute->FirstChildElement();
				while(comp_elem)
				{
					ComponentPtr target_comp;

					//Try Get overload component by name first, if not found assume only one component of same type
					TiXmlElement *name_elem =comp_elem->FirstChildElement("Name");
					if(name_elem)
					{
						const std::string comp_name = name_elem->Attribute("value");
						target_comp  =  GetComponent(comp_name);
					}
					else
					{
						const std::string comp_name = comp_elem->Value();
						target_comp = GetComponent(comp_name);
					}					

					if(target_comp) //over loading component
					{
						ComponentPtr comp = LoadComponent(comp_elem);
						ComponentTemplatePtr template_comp = boost::shared_dynamic_cast<IComponentTemplate>(comp);
						if(template_comp)
						{
							template_comp->AssignFrom(target_comp);
						}
					}

					else
					{
						ComponentPtr comp = LoadComponent(comp_elem);
						if(comp)
							AddComponent(comp);
					}
					comp_elem = comp_elem->NextSiblingElement();
				}
			}
			else if(data_name == "ComponentContainers")
			{
				TiXmlElement *cc_elem = class_attribute->FirstChildElement();
				while(cc_elem )
				{
					//allow over loading
					ComponentContainerPtr container = CreateComponentContainer(cc_elem);
					AddChild(container);
					XMLSerializePtr s_container = boost::shared_dynamic_cast<IXMLSerialize> (container);
					if(s_container)
						s_container->LoadXML(cc_elem);
					cc_elem  = cc_elem->NextSiblingElement();
				}
			}
			else //base object attribute
			{
				const std::string attrib_val = class_attribute->FirstAttribute()->Value();
				if (!SetPropertyByString(data_name,attrib_val))
					LogManager::getSingleton().stream() << "WARNING:BaseComponentContainer::LoadXML() - Filename: " << obj_elem->GetDocument()->Value() << "\tproperty not found: " << data_name;
			}
			class_attribute  = class_attribute->NextSiblingElement();
		}
	}

	ComponentContainerPtr BaseComponentContainer::CreateComponentContainer(TiXmlElement *cc_elem) const
	{
		const std::string type = cc_elem->Value();
		ComponentContainerPtr container (ComponentContainerFactory::Get().Create(type));
		return container;
	}

	ComponentPtr BaseComponentContainer::LoadComponent(TiXmlElement *comp_template)
	{
		const std::string comp_type = comp_template->Value();
		//std::string comp_type = comp_template->Attribute("type");
		ComponentPtr comp (ComponentFactory::Get().Create(comp_type));
		if(comp)
		{
			//Give all components default name
			comp->SetName(comp_type);
			XMLSerializePtr s_comp = boost::shared_dynamic_cast<IXMLSerialize> (comp);
			if(s_comp)
				s_comp->LoadXML(comp_template);
		}
		else
		{
			LogManager::getSingleton().stream() << "WARNING:Failed to create component " << comp_type;
		}
		return comp;
	}

	IComponentContainer::ComponentContainerIterator BaseComponentContainer::GetChildren()
	{
		return IComponentContainer::ComponentContainerIterator(m_ComponentContainerVector.begin(),m_ComponentContainerVector.end());
	}

	IComponentContainer::ConstComponentContainerIterator BaseComponentContainer::GetChildren() const
	{
		return IComponentContainer::ConstComponentContainerIterator(m_ComponentContainerVector.begin(),m_ComponentContainerVector.end());
	}


	
	void BaseComponentContainer::SetTemplateName(const std::string &name) 
	{
		m_TemplateName = name;
	}


	std::string BaseComponentContainer::GetTemplateName()  const 
	{
		return m_TemplateName;
	}

	void BaseComponentContainer::SetSerialize(bool value) 
	{
		m_Serialize = value;
	}

	bool BaseComponentContainer::GetSerialize()  const 
	{
		return m_Serialize;
	}


	#define TAB(val) std::cout << std::setfill(' ') << std::setw(val*3) << std::right << " "; std::cout
	void BaseComponentContainer::DebugPrint(int tc)
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
		BaseComponentContainer::ComponentContainerVector::iterator iter;
		if(m_ComponentContainerVector.size() > 0)
		{
			TAB(tc) << "Children" << std::endl;
		}
		for(iter = m_ComponentContainerVector.begin(); iter != m_ComponentContainerVector.end(); ++iter)
		{
			BaseComponentContainerPtr child = boost::shared_static_cast<BaseComponentContainer>( *iter);
			child->DebugPrint(tc+1);
		}
		//TAB(tc) << "Children - " << std::endl;
		tc--;
		//TAB(tc) << "IComponentContainer - " << std::endl;
	}




}




