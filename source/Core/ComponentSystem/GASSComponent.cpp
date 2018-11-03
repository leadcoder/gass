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
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/Utils/GASSException.h"
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{

	std::map<RTTI* ,std::vector<std::string> >  Component::m_Dependencies;

	Component::Component() : m_Owner(ComponentContainerWeakPtr())
	{
	
	}

	Component::~Component()
	{
	
	}

	void Component::RegisterReflection()
	{
		RegisterProperty<std::string>( "Name", &Component::GetName, &Component::SetName);
	}

	std::string Component::GetName() const 
	{
		return m_Name;
	}
	void Component::SetName(const std::string &name) 
	{
		m_Name = name;
	}
	
	ComponentContainerPtr Component::GetOwner() const 
	{
		return m_Owner.lock();
	}

	void Component::SetOwner(ComponentContainerPtr owner)
	{
		m_Owner = owner;
	}
	
	bool Component::Serialize(ISerializer* serializer)
	{
		if(serializer->Loading())
		{
			if(!BaseReflectionObject::_SerializeProperties(serializer))
				return false;
		}
		else
		{
			auto* saver = static_cast<SerialSaver*>(serializer);
			std::string comp_type = GetRTTI()->GetClassName();
			saver->IO<std::string>(comp_type);
			if(!BaseReflectionObject::_SerializeProperties(serializer))
				return false;
		}
		return true;
	}


	void Component::LoadXML(tinyxml2::XMLElement *obj_elem)
	{
		BaseReflectionObject::_LoadProperties(obj_elem);
	}

	void Component::SaveXML(tinyxml2::XMLElement *xml_elem)
	{
		tinyxml2::XMLElement * this_elem;
		std::string factoryname = ComponentFactory::Get().GetFactoryName(GetRTTI()->GetClassName());
		this_elem = xml_elem->GetDocument()->NewElement(factoryname.c_str() );  
		xml_elem->LinkEndChild( this_elem );  
		_SaveProperties(this_elem);
	}

	ComponentPtr Component::CreateCopy()
	{
		const std::string factory_class_name = ComponentFactory::Get().GetFactoryName(GetRTTI()->GetClassName());
		ComponentPtr new_comp = GASS_STATIC_PTR_CAST<Component>(ComponentFactory::Get().Create(factory_class_name));
		if(!new_comp)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to create component instance " + factory_class_name,"Component::CreateCopy");
		}
		//ComponentPtr new_comp = GASS_STATIC_PTR_CAST<Component>(CreateInstance());
		BaseReflectionObject::CopyPropertiesTo(new_comp);
		return new_comp;
	}

	void Component::CopyPropertiesTo(ComponentPtr dest_comp)
	{
		BaseReflectionObject::CopyPropertiesTo(dest_comp);
	}

	std::vector<std::string> Component::GetDependencies() const
	{
		return m_Dependencies[GetRTTI()];
	}
	
}




