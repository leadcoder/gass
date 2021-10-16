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
#include "Sim/GASSSceneObjectTemplate.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Core/Serialize/GASSSerialize.h"
#include "Sim/GASSComponentFactory.h"
#include "Sim/GASSSceneObjectTemplateManager.h"
#include "Core/Utils/GASSLogger.h"
#include "Core/Utils/GASSException.h"
#include <iostream>
#include <iomanip>
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{
	SceneObjectTemplate::SceneObjectTemplate() : m_Instantiable(false)
	{
		
	}
	
	void SceneObjectTemplate::RegisterReflection()
	{
		RegisterGetSet("Name", &GASS::SceneObjectTemplate::GetName, &GASS::SceneObjectTemplate::SetName);
		RegisterGetSet("Inheritance", &GASS::SceneObjectTemplate::GetInheritance, &GASS::SceneObjectTemplate::SetInheritance);
		RegisterGetSet("Serialize", &GASS::SceneObjectTemplate::GetSerialize, &GASS::SceneObjectTemplate::SetSerialize);
		RegisterGetSet("ID", &GASS::SceneObjectTemplate::GetID, &GASS::SceneObjectTemplate::SetID);
		RegisterGetSet("Instantiable", &GASS::SceneObjectTemplate::GetInstantiable, &GASS::SceneObjectTemplate::SetInstantiable);
	}


	BaseSceneComponentPtr SceneObjectTemplate::AddBaseSceneComponent(const std::string &comp_name)
	{
		return GASS_DYNAMIC_PTR_CAST<GASS::BaseSceneComponent>(AddComponent(comp_name));
	}

	BaseSceneComponentPtr SceneObjectTemplate::GetBaseSceneComponent(const std::string &comp_name) const
	{
		return GASS_DYNAMIC_PTR_CAST<GASS::BaseSceneComponent>(GetComponent(comp_name));
	}

	void SceneObjectTemplate::AddChild(SceneObjectTemplatePtr child)
	{
		SceneObjectTemplateWeakPtr parent = SceneObjectTemplateWeakPtr(shared_from_this());
		child->SetParent(parent);
		m_SceneObjectVector.push_back(child);
	}

	bool SceneObjectTemplate::GetSerialize() const
	{
		return m_Serialize;
	}

	void SceneObjectTemplate::SetSerialize(bool value)
	{
		m_Serialize = value;
	}

	ComponentPtr SceneObjectTemplate::GetComponent(const std::string& name) const
	{
		for (const auto& comp : m_ComponentVector)
		{
			if (comp->GetName() == name)
				return comp;
		}
		return ComponentPtr();
	}


	void SceneObjectTemplate::AddComponent(ComponentPtr comp)
	{
		m_ComponentVector.push_back(comp);
	}

	ComponentPtr SceneObjectTemplate::AddComponent(const std::string& comp_type)
	{
		ComponentPtr comp = ComponentFactory::Get().Create(comp_type);
		if (!comp)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to create Component:" + comp_type, "SceneObjectTemplate::AddComponent");
		comp->SetName(comp_type);
		AddComponent(comp);
		return comp;
	}

	bool SceneObjectTemplate::Serialize(ISerializer* serializer)
	{
		if (!BaseReflectionObject::_SerializeProperties(serializer))
			return false;

		if (serializer->Loading())
		{
			int num_comp = 0;
			auto* loader = dynamic_cast<SerialLoader*>(serializer);
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
					//const std::string class_name = GetRTTI()->GetClassName();
					SceneObjectTemplatePtr child = std::make_shared<SceneObjectTemplate>();
					if (!child)
					{
						GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to create SceneObjectTemplate instance", "SceneObjectTemplate::Serialize");
					}
					//SceneObjectTemplatePtr child  = GASS_DYNAMIC_PTR_CAST<ISceneObjectTemplate> (CreateInstance());
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
			const int num_comp = static_cast<int>(m_ComponentVector.size());
			auto* saver = dynamic_cast<SerialSaver*>(serializer);
			if (saver)
			{
				saver->IO<int>(num_comp);

				auto iter = m_ComponentVector.begin();
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

				const int num_children = static_cast<int>(m_SceneObjectVector.size());
				saver->IO<int>(num_children);

				SceneObjectTemplate::SceneObjectTemplateVector::iterator go_iter;
				for (go_iter = m_SceneObjectVector.begin(); go_iter != m_SceneObjectVector.end(); ++go_iter)
				{
					SceneObjectTemplatePtr child = *go_iter;
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

	void SceneObjectTemplate::RemoveChild(SceneObjectTemplatePtr child)
	{
		SceneObjectTemplate::SceneObjectTemplateVector::iterator bo_iter;
		for (bo_iter = m_SceneObjectVector.begin(); bo_iter != m_SceneObjectVector.end(); ++bo_iter)
		{
			if (child == *bo_iter)
			{
				m_SceneObjectVector.erase(bo_iter);
				return;
			}
		}
	}

	SceneObject::ComponentIterator SceneObjectTemplate::GetComponents()
	{
		return ComponentIterator(m_ComponentVector.begin(), m_ComponentVector.end());
	}


	SceneObjectTemplate::SceneObjectTemplateIterator SceneObjectTemplate::GetChildren()
	{
		return SceneObjectTemplate::SceneObjectTemplateIterator(m_SceneObjectVector.begin(), m_SceneObjectVector.end());
	}

	void SceneObjectTemplate::SaveXML(tinyxml2::XMLElement* obj_elem)
	{
		tinyxml2::XMLDocument* rootXMLDoc = obj_elem->GetDocument();
		tinyxml2::XMLElement* this_elem = rootXMLDoc->NewElement("SceneObjectTemplate");
		obj_elem->LinkEndChild(this_elem);
		_SaveProperties(this_elem);

		tinyxml2::XMLElement* comp_elem = rootXMLDoc->NewElement("Components");
		this_elem->LinkEndChild(comp_elem);

		ComponentVector::iterator iter;
		for (iter = m_ComponentVector.begin(); iter != m_ComponentVector.end(); ++iter)
		{
			ComponentPtr comp = (*iter);
			XMLSerializePtr s_comp = GASS_DYNAMIC_PTR_CAST<IXMLSerialize>(comp);
			if (s_comp)
				s_comp->SaveXML(comp_elem);
		}


		tinyxml2::XMLElement* cc_elem = rootXMLDoc->NewElement("Children");
		this_elem->LinkEndChild(cc_elem);

		SceneObjectTemplate::SceneObjectTemplateVector::iterator cc_iter;
		for (cc_iter = m_SceneObjectVector.begin(); cc_iter != m_SceneObjectVector.end(); ++cc_iter)
		{
			XMLSerializePtr child = GASS_DYNAMIC_PTR_CAST<IXMLSerialize>(*cc_iter);
			if (child)
			{
				child->SaveXML(cc_elem);
			}
		}
	}

	void SceneObjectTemplate::LoadXML(tinyxml2::XMLElement* obj_elem)
	{
		//m_Name = obj_elem->Value();
		tinyxml2::XMLElement* class_attribute = obj_elem->FirstChildElement();
		while (class_attribute)
		{
			const std::string data_name = class_attribute->Value();
			if (data_name == "Components")
			{
				tinyxml2::XMLElement* comp_elem = class_attribute->FirstChildElement();
				while (comp_elem)
				{
					const std::string comp_name = comp_elem->Value();
					ComponentPtr target_comp(GetComponent(comp_name));
					if (target_comp) //over loading component
					{
						ComponentPtr comp = _LoadComponentXML(comp_elem);
						//ComponentTemplatePtr template_comp = GASS_DYNAMIC_PTR_CAST<IComponentTemplate>(comp);
						if (comp)
						{
							comp->CopyPropertiesTo(target_comp);
						}
					}
					else
					{
						ComponentPtr comp = _LoadComponentXML(comp_elem);
						if (comp)
							AddComponent(comp);
					}
					comp_elem = comp_elem->NextSiblingElement();
				}
			}
			else if (data_name == "Children" || data_name == "ComponentContainers")
			{
				tinyxml2::XMLElement* cc_elem = class_attribute->FirstChildElement();
				while (cc_elem)
				{
					
					auto so = std::make_shared<SceneObjectTemplate>();
					if (so)
					{
						AddChild(so);
						XMLSerializePtr xml_obj = GASS_DYNAMIC_PTR_CAST<IXMLSerialize>(so);
						if (xml_obj)
							xml_obj->LoadXML(cc_elem);
					}
					else
					{
						GASS_LOG(LWARNING) << "Failed to create SceneObjectTemplate";
					}
					cc_elem = cc_elem->NextSiblingElement();
				}
			}
			else //base object attribute
			{
				//std::string attrib_name = class_attribute->FirstAttribute()->Name();
				if (class_attribute->FirstAttribute())
				{
					const std::string attrib_val = class_attribute->FirstAttribute()->Value();//class_attribute->Attribute(attrib_name);
					try
					{
						SetPropertyByString(data_name, attrib_val);
					}
					catch (...)
					{
						GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed parsing:" + data_name + " With attribute:" + attrib_val + " in:" + std::string(obj_elem->GetDocument()->GetFileName()), "SceneObjectTemplate::LoadXML");
					}
				}
				else
				{
					GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "No value attribute found for xml tag: " + data_name + " In:" + std::string(obj_elem->GetDocument()->GetFileName()), "SceneObjectTemplate::LoadXML");
				}
			}
			class_attribute = class_attribute->NextSiblingElement();
		}
	}

	ComponentPtr SceneObjectTemplate::_LoadComponentXML(tinyxml2::XMLElement* comp_template) const
	{
		const std::string comp_type = comp_template->Value();
		//std::string comp_type = comp_template->Attribute("type");
		ComponentPtr comp(ComponentFactory::Get().Create(comp_type));
		if (comp)
		{
			comp->SetName(comp_type);
			XMLSerializePtr s_comp = GASS_DYNAMIC_PTR_CAST<IXMLSerialize>(comp);
			if (s_comp)
				s_comp->LoadXML(comp_template);
		}
		else
		{
			GASS_LOG(LWARNING) << "Failed to create component " << comp_type;
		}
		return comp;
	}

	/*std::string SceneObjectTemplate::CreateUniqueName(SceneObjectTemplateManagerConstPtr manager) const
	{
		static int object_counter = 0;
		std::stringstream ss;
		ss << GetName() << manager->GetObjectIDPrefix() << object_counter << manager->GetObjectIDSuffix();
		std::string u_name;
		ss >> u_name;
		object_counter++;
		return u_name ;
	}*/

	void SceneObjectTemplate::_InheritComponentData(SceneObjectPtr cc) const
	{
		ComponentVector::const_iterator iter;
		for (iter = m_ComponentVector.begin(); iter != m_ComponentVector.end(); ++iter)
		{
			ComponentPtr comp = (*iter);
			if (comp)
			{
				const std::string name = comp->GetName();
				ComponentPtr target_comp(cc->GetComponent(name));
				if (target_comp)
				{
					comp->CopyPropertiesTo(target_comp);
				}
				else
				{
					ComponentPtr new_comp = comp->CreateCopy();
					if (new_comp)
					{
						cc->AddComponent(new_comp);
					}
				}
			}
		}
	}

	SceneObjectPtr SceneObjectTemplate::CreateSceneObject() const
	{
		auto so = std::make_shared<SceneObject>();
		BaseReflectionObjectPtr ref_obj = GASS_DYNAMIC_PTR_CAST<BaseReflectionObject>(so);
		BaseReflectionObject::CopyPropertiesTo(ref_obj);

		ComponentVector::const_iterator iter;
		for (iter = m_ComponentVector.begin(); iter != m_ComponentVector.end(); ++iter)
		{
			ComponentPtr comp = (*iter);
			if (comp)
			{
				ComponentPtr new_comp = comp->CreateCopy();
				so->AddComponent(new_comp);
			}
		}
		return so;
	}

	void SceneObjectTemplate::CreateFromSceneObject(SceneObjectPtr so, SceneObjectTemplateManagerConstPtr manager, bool keep_inheritance)
	{
		SceneObjectTemplatePtr old_temp;
		if (keep_inheritance)
		{
			const std::string old_template_name = so->GetTemplateName();
			if (old_template_name != "")
			{
				old_temp = manager->GetTemplate(old_template_name);
				if (old_temp)
					SetInheritance(old_temp->GetInheritance());
			}
		}

		BaseReflectionObjectPtr ref_obj = GASS_DYNAMIC_PTR_CAST<BaseReflectionObject>(so);
		ref_obj->CopyPropertiesTo(shared_from_this());

		auto comp_iter = so->GetComponents();
		while (comp_iter.hasMoreElements())
		{
			ComponentPtr comp = comp_iter.getNext();
			if (comp)
			{
				ComponentPtr template_comp = comp->CreateCopy();
				AddComponent(template_comp);
			}
		}

		auto children = so->GetChildren();

		while (children.hasMoreElements())
		{
			SceneObjectPtr child = children.getNext();
			bool found = false;
			if (old_temp)
			{
				auto temp_children = old_temp->GetChildren();
				while (temp_children.hasMoreElements())
				{
					SceneObjectTemplatePtr temp_child = temp_children.getNext();
					if (child->GetTemplateName() == temp_child->GetName())
					{
						found = true;
						break;
					}
				}
			}
			SceneObjectTemplatePtr new_child = std::make_shared<SceneObjectTemplate>();
			if (!found && new_child)
			{
				new_child->CreateFromSceneObject(child, manager, keep_inheritance);
				AddChild(new_child);
			}
		}
	}

#define GASS_INDENT(val) std::cout << std::setfill(' ') << std::setw(val*3) << std::right << " "; std::cout
	void SceneObjectTemplate::DebugPrint(int tc)
	{

		GASS_INDENT(tc) << GetRTTI()->GetClassName() << " - " << GetName() << std::endl;
		tc++;
		if (m_ComponentVector.size() > 0)
		{
			GASS_INDENT(tc) << "Components" << std::endl;
		}
		auto comp_iter = m_ComponentVector.begin();
		tc++;
		while (comp_iter != m_ComponentVector.end())
		{
			ComponentPtr comp = (*comp_iter);
			//std::string comp_type = comp->GetRTTI()->GetClassName();
			//TAB(tc) << comp_type << " - " << comp->GetName() << std::endl;
			GASS_INDENT(tc) << comp->GetName() << std::endl;
			++comp_iter;
		}
		tc--;
		//TAB(tc) << "Components - " << std::endl;
		SceneObjectTemplate::SceneObjectTemplateVector::iterator iter;
		if (m_SceneObjectVector.size() > 0)
		{
			GASS_INDENT(tc) << "Children" << std::endl;
		}
		for (iter = m_SceneObjectVector.begin(); iter != m_SceneObjectVector.end(); ++iter)
		{
			auto child = *iter;
			child->DebugPrint(tc + 1);
		}
	}

}






