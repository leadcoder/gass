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

#pragma once

#include <Core/Common.h>

#include "Core/Reflection/BaseReflectionObject.h"
#include "Core/Serialize/IXMLSerialize.h"
#include "Core/Serialize/ISerialize.h"

#include "Core/ComponentSystem/IComponent.h"
#include "Core/ComponentSystem/IComponentTemplate.h"

namespace GASS
{
	class IComponentContainer;

	typedef boost::shared_ptr<IComponentContainer> ComponentContainerPtr;
	typedef boost::weak_ptr<IComponentContainer> ComponentContainerWeakPtr;
	typedef boost::shared_ptr<IComponent> ComponentPtr;
	typedef boost::weak_ptr<IComponent> ComponentWeakPtr;

	class GASSCoreExport BaseComponent : public Reflection<BaseComponent, BaseReflectionObject>, public IComponent , public IXMLSerialize, public ISerialize, public IComponentTemplate
	{
	public:
		BaseComponent();
		virtual ~BaseComponent();
		static void RegisterReflection();
		//component interface
		virtual void OnCreate();
		virtual std::string GetName() const;
		virtual void SetName(const std::string &name);
		virtual ComponentContainerPtr GetOwner() const;
		virtual void SetOwner(ComponentContainerPtr owner);
		

		//serialize interface
		virtual bool Serialize(ISerializer* serializer);
		
		//xml-serialize interface
		virtual void LoadXML(TiXmlElement *obj_elem);
		virtual void SaveXML(TiXmlElement *obj_elem);


		//reflection interface
		//virtual bool SetAttribute(const std::string &attrib_name,const std::string &attrib_val);
		//virtual void SetAttributes(BaseReflectionObject* obj);
		//template interface
		virtual ComponentPtr CreateCopy();
		virtual void Assign(ComponentPtr comp);
		
	protected:
		std::string m_Name;
		ComponentContainerWeakPtr m_Owner;
	};
	typedef boost::shared_ptr<BaseComponent> BaseComponentPtr;
	
}
