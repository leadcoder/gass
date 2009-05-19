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
#include "Core/ComponentSystem/BaseComponent.h"
#include "tinyxml.h"

namespace GASS
{
	BaseComponent::BaseComponent(void) : m_Owner(ComponentContainerWeakPtr())
	{
	
	}

	BaseComponent::~BaseComponent(void)
	{
	
	}

	void BaseComponent::RegisterReflection()
	{
		RegisterProperty<std::string>( "Name", &BaseComponent::GetName, &BaseComponent::SetName);
	}

	std::string BaseComponent::GetName() const 
	{
		return m_Name;
	}
	void BaseComponent::SetName(const std::string &name) 
	{
		m_Name = name;
	}
	
	ComponentContainerPtr BaseComponent::GetOwner() const 
	{
		return ComponentContainerPtr(m_Owner);
	}

	void BaseComponent::SetOwner(ComponentContainerPtr owner)
	{
		m_Owner = owner;
	}
	
	void BaseComponent::OnCreate()
	{

	}

	bool BaseComponent::Serialize(ISerializer* serializer)
	{
		if(serializer->Loading())
		{
			if(!BaseReflectionObject::SerializeProperties(serializer))
				return false;
		}
		else
		{
			std::string comp_type = GetRTTI()->GetClassName();
			serializer->IO(comp_type);
			if(!BaseReflectionObject::SerializeProperties(serializer))
				return false;
		}
		return true;
	}


	void BaseComponent::LoadXML(TiXmlElement *obj_elem)
	{
		BaseReflectionObject::LoadProperties(obj_elem);
	}

	void BaseComponent::SaveXML(TiXmlElement *xml_elem)
	{
		TiXmlElement * this_elem;
		this_elem = new TiXmlElement( GetName().c_str() );  
		xml_elem->LinkEndChild( this_elem );  
		this_elem->SetAttribute("type", GetRTTI()->GetClassName().c_str());
		SaveProperties(this_elem);
	}

	ComponentPtr BaseComponent::CreateCopy()
	{
		BaseComponentPtr new_comp = boost::shared_static_cast<BaseComponent>(CreateInstance());
		BaseReflectionObject::SetProperties(new_comp);
		return new_comp;
	}

	void BaseComponent::Assign(ComponentPtr obj)
	{
		BaseComponentPtr  base_comp = boost::shared_static_cast<BaseComponent>(obj);
		BaseReflectionObject::SetProperties(base_comp);
	}	
	
}




