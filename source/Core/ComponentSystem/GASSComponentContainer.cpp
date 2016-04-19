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
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/ComponentSystem/GASSComponentContainer.h"
#include "Core/Serialize/GASSSerialize.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentContainerFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"


#include <iostream>
#include <iomanip>
#include <tinyxml2.h>

namespace GASS
{
	ComponentContainer::ComponentContainer() : m_Serialize(true)
	{

	}


	ComponentContainer::~ComponentContainer(void)
	{

	}

	void ComponentContainer::AddChild(ComponentContainerPtr child)
	{
		ComponentContainerWeakPtr parent = ComponentContainerWeakPtr(shared_from_this());
		child->SetParent(parent);
		m_ComponentContainerVector.push_back(child);
	}


	void ComponentContainer::RegisterReflection()
	{
		RegisterProperty<std::string>("Name", &GASS::ComponentContainer::GetName, &GASS::ComponentContainer::SetName);
		RegisterProperty<std::string>("TemplateName", &GASS::ComponentContainer::GetTemplateName, &GASS::ComponentContainer::SetTemplateName);
		RegisterProperty<bool>("Serialize", &GASS::ComponentContainer::GetSerialize, &GASS::ComponentContainer::SetSerialize);
	}

	ComponentContainer::ComponentIterator ComponentContainer::GetComponents()
	{
		return ComponentIterator(m_ComponentVector.begin(), m_ComponentVector.end());
	}

	ComponentContainer::ConstComponentIterator ComponentContainer::GetComponents() const
	{
		return ConstComponentIterator(m_ComponentVector.begin(), m_ComponentVector.end());
	}

	ComponentPtr ComponentContainer::GetComponent(const std::string &name) const
	{
		ComponentPtr comp;
		for(size_t i = 0 ; i < m_ComponentVector.size(); i++)
		{
			if(m_ComponentVector[i]->GetName() == name)
				return m_ComponentVector[i];
		}
		return comp;
	}

	void ComponentContainer::AddComponent(ComponentPtr comp)
	{
		comp->SetOwner(shared_from_this());
		m_ComponentVector.push_back(comp);
	}

	bool ComponentContainer::Serialize(ISerializer* serializer)
	{
		if(!m_Serialize)
			return true;
		if(!BaseReflectionObject::_SerializeProperties(serializer))
			return false;

		if(serializer->Loading())
		{
			int num_comp = 0;
			SerialLoader* loader = (SerialLoader*) serializer;

			loader->IO<int>(num_comp);
			for(int i  = 0 ; i < num_comp; i++)
			{
				std::string comp_type;
				loader->IO<std::string>(comp_type);
				ComponentPtr comp (ComponentFactory::Get().Create(comp_type));
				if(comp)
				{
					SerializePtr s_comp = GASS_DYNAMIC_PTR_CAST<ISerialize>(comp);
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

			int num_children = 0;
			loader->IO<int>(num_children);
			for(int i  = 0 ; i < num_children; i++)
			{

				//TODO: need to change this, should save componentcontainer type instead and create from factory, (same way as components are created)
				const std::string factory_class_name = ComponentContainerFactory::Get().GetFactoryName(GetRTTI()->GetClassName());
				ComponentContainerPtr child = ComponentContainerFactory::Get().Create(factory_class_name);
				if(!child)
				{
					GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to create instance " + factory_class_name,"ComponentContainer::Serialize");
				}

				//ComponentContainerPtr child  = GASS_DYNAMIC_PTR_CAST<IComponentContainer> (CreateInstance());
				if(child)
				{
					SerializePtr s_child = GASS_DYNAMIC_PTR_CAST<ISerialize>(child);
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
				SerializePtr s_comp = GASS_DYNAMIC_PTR_CAST<ISerialize>(comp);
				if(s_comp)
				{
					if(!s_comp->Serialize(serializer))
						return false;

				}
				++iter;
			}

			int num_children = static_cast<int>(m_ComponentContainerVector.size());
			saver->IO<int>(num_children);
			ComponentContainer::ComponentContainerVector::iterator go_iter;
			for(go_iter = m_ComponentContainerVector.begin(); go_iter != m_ComponentContainerVector.end(); ++go_iter)
			{
				ComponentContainerPtr child = *go_iter;
				SerializePtr s_child = GASS_DYNAMIC_PTR_CAST<ISerialize>(child);
				if(s_child)
				{
					if(!s_child->Serialize(serializer))
						return false;
				}
			}
		}
		return true;
	}

	void ComponentContainer::RemoveChild(ComponentContainerPtr child)
	{
		ComponentContainer::ComponentContainerVector::iterator bo_iter;
		for(bo_iter = m_ComponentContainerVector.begin(); bo_iter!= m_ComponentContainerVector.end(); ++bo_iter)
		{
			if(child == *bo_iter)
			{
				m_ComponentContainerVector.erase(bo_iter);
				return;
			}
		}
	}

	void ComponentContainer::SaveXML(tinyxml2::XMLElement *obj_elem)
	{
		if(!m_Serialize)
			return;
		std::string factory_name = ComponentContainerFactory::Get().GetFactoryName(GetRTTI()->GetClassName());
		tinyxml2::XMLDocument *rootXMLDoc = obj_elem->GetDocument();

		tinyxml2::XMLElement* this_elem = rootXMLDoc->NewElement(factory_name.c_str());
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

		ComponentContainer::ComponentContainerVector::iterator cc_iter;
		for(cc_iter = m_ComponentContainerVector.begin(); cc_iter != m_ComponentContainerVector.end(); ++cc_iter)
		{
			XMLSerializePtr child = GASS_DYNAMIC_PTR_CAST<IXMLSerialize>(*cc_iter);
			if(child)
			{
				child->SaveXML(cc_elem);
			}
		}
	}

	void ComponentContainer::LoadXML(tinyxml2::XMLElement *obj_elem)
	{
		if(!m_Serialize)
			return;
		tinyxml2::XMLElement *class_attribute = obj_elem->FirstChildElement();
		while(class_attribute)
		{
			const std::string data_name = class_attribute->Value();
			if(data_name == "Components")
			{
				tinyxml2::XMLElement *comp_elem = class_attribute->FirstChildElement();
				while(comp_elem)
				{
					ComponentPtr target_comp;

					//Try to get component by name first, if not found assume only one component of same type
					tinyxml2::XMLElement *name_elem =comp_elem->FirstChildElement("Name");
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

					if(target_comp) //component already exist, replace attributes component
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
					//allow over loading
					ComponentContainerPtr container = CreateComponentContainerXML(cc_elem);
					AddChild(container);
					XMLSerializePtr s_container = GASS_DYNAMIC_PTR_CAST<IXMLSerialize> (container);
					if(s_container)
						s_container->LoadXML(cc_elem);
					cc_elem  = cc_elem->NextSiblingElement();
				}
			}
			else //base object attribute
			{
				const std::string attrib_val = class_attribute->FirstAttribute()->Value();
				try
				{
					SetPropertyByString(data_name,attrib_val);
				}
				catch(...)
				{
					GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed parsing:" + data_name +" With attribute:"+ attrib_val+  " in:" + std::string(obj_elem->GetDocument()->GetFileName()),"ComponentContainer::LoadXML");
				}
			}
			class_attribute  = class_attribute->NextSiblingElement();
		}
	}

	ComponentContainerPtr ComponentContainer::CreateComponentContainerXML(tinyxml2::XMLElement *cc_elem) const
	{
		const std::string type = cc_elem->Value();
		ComponentContainerPtr container (ComponentContainerFactory::Get().Create(type));
		return container;
	}

	ComponentPtr ComponentContainer::_LoadComponentXML(tinyxml2::XMLElement *comp_template)
	{
		const std::string comp_type = comp_template->Value();
		//std::string comp_type = comp_template->Attribute("type");
		ComponentPtr comp (ComponentFactory::Get().Create(comp_type));
		if(comp)
		{
			//Give all components default name
			comp->SetName(comp_type);
			XMLSerializePtr s_comp = GASS_DYNAMIC_PTR_CAST<IXMLSerialize> (comp);
			if(s_comp)
				s_comp->LoadXML(comp_template);
		}
		else
		{
			LogManager::getSingleton().stream() << "WARNING:Failed to create component " << comp_type;
		}
		return comp;
	}

	ComponentContainer::ComponentContainerIterator ComponentContainer::GetChildren()
	{
		return ComponentContainerIterator(m_ComponentContainerVector.begin(),m_ComponentContainerVector.end());
	}

	ComponentContainer::ConstComponentContainerIterator ComponentContainer::GetChildren() const
	{
		return ComponentContainer::ConstComponentContainerIterator(m_ComponentContainerVector.begin(),m_ComponentContainerVector.end());
	}

	void ComponentContainer::SetTemplateName(const std::string &name)
	{
		m_TemplateName = name;
	}

	std::string ComponentContainer::GetTemplateName()  const
	{
		return m_TemplateName;
	}

	void ComponentContainer::SetSerialize(bool value)
	{
		m_Serialize = value;
	}

	bool ComponentContainer::GetSerialize()  const
	{
		return m_Serialize;
	}


	void ComponentContainer::_CheckComponentDependencies() const
	{
		//get all names
		std::set<std::string> names;
		ComponentVector::const_iterator comp_iter = m_ComponentVector.begin();
		while (comp_iter != m_ComponentVector.end())
		{
			ComponentPtr comp = (*comp_iter);
			names.insert(comp->GetRTTI()->GetClassName());
			++comp_iter;
		}

		comp_iter = m_ComponentVector.begin();

		while (comp_iter != m_ComponentVector.end())
		{
			ComponentPtr comp = (*comp_iter);
			const std::vector<std::string> deps = comp->GetDependencies();
			std::vector<std::string>::const_iterator dep_iter = deps.begin();
			while(dep_iter != deps.end())
			{
				const std::string comp_name = *dep_iter;
				if(names.find(comp_name) == names.end())
					GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find dependent component:" + comp_name + " in component:" + GetName(),"ComponentContainer::CheckComponentDependencies");
				dep_iter++;
			}
			++comp_iter;
		}
	}

	size_t ComponentContainer::GetNumChildren() const
	{
		return m_ComponentContainerVector.size();
	}

	ComponentContainerPtr ComponentContainer::GetChild(size_t index) const
	{
		return m_ComponentContainerVector[index];
	}


	#define TAB(val) std::cout << std::setfill(' ') << std::setw(val*3) << std::right << " "; std::cout
	void ComponentContainer::DebugPrint(int tc)
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
		ComponentContainer::ComponentContainerVector::iterator iter;
		if(m_ComponentContainerVector.size() > 0)
		{
			TAB(tc) << "Children" << std::endl;
		}
		for(iter = m_ComponentContainerVector.begin(); iter != m_ComponentContainerVector.end(); ++iter)
		{
			ComponentContainerPtr child = GASS_STATIC_PTR_CAST<ComponentContainer>( *iter);
			child->DebugPrint(tc+1);
		}
		//TAB(tc) << "Children - " << std::endl;
		tc--;
		//TAB(tc) << "IComponentContainer - " << std::endl;
	}



}




