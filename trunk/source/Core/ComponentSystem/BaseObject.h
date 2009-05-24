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
#include <boost/enable_shared_from_this.hpp>
#include "Core/Reflection/Reflection.h"
#include "Core/Reflection/BaseReflectionObject.h"
#include "Core/ComponentSystem/IComponentContainer.h"
#include "Core/ComponentSystem/IComponentContainerTemplate.h"
#include "Core/Serialize/IXMLSerialize.h"
#include "Core/Serialize/ISerialize.h"
//#include "Core/Reflection/IReflection.h"

namespace GASS
{

	class MessageManager;

	/**
			This is the class that a game or simulation object should be derive from.
			The BaseObject is a convinience class that implements a couple of 
			usefull interfaces such as that the	IComponentContainer and 
			IComponentContainerTemplate it also inherite from the reflection template class 
			which enables attribute reflection in a easy way. The inheritance from 
			boost::enable_shared_from_this is used to get hold of ourself (this) as 
			a shared pointer with the shared_from_this() function. 
			To get more information what you get by inherit from this 
			class see the documentation for each interface

	*/
	class GASSCoreExport BaseObject : public Reflection<BaseObject, BaseReflectionObject> ,public boost::enable_shared_from_this<BaseObject>, public IComponentContainer, public IXMLSerialize, public ISerialize, public IComponentContainerTemplate
	{
	public:
		//typedef std::vector<IComponent*> ComponentVector;
		//typedef std::vector<IComponentContainer*> ComponentContainerVector;
	public:
		BaseObject();
		virtual ~BaseObject();
		
		static	void RegisterReflection();
		
		//ComponentContainer interface
		virtual void OnCreate();
		virtual std::string GetName() const {return m_Name;}
		virtual void SetName(const std::string &name) {m_Name = name;}
		virtual void AddChild(ComponentContainerPtr child);
		virtual void RemoveChild(ComponentContainerPtr child);
		virtual ComponentContainerVector GetChildren(){return m_ComponentContainerVector;}
		virtual ComponentContainerPtr GetParent() const {return ComponentContainerPtr(m_Parent,boost::detail::sp_nothrow_tag());}//allow null pointer}
		virtual void SetParent(ComponentContainerWeakPtr parent){m_Parent = parent;}
		virtual void AddComponent(ComponentPtr comp);
		virtual ComponentPtr GetComponent(const std::string &name);
		virtual ComponentVector GetComponents();
	
		//xml serialize interface
		virtual void LoadXML(TiXmlElement *obj_elem);
		virtual void SaveXML(TiXmlElement *obj_elem);
		
		//serialize interface
		virtual bool Serialize(ISerializer* serializer);

		//IComponentContainerTemplate interface
		virtual ComponentContainerPtr CreateComponentContainer(int &part_id, ComponentContainerTemplateManagerPtr manager);
		virtual ComponentContainerPtr CreateCopy();
	
		//print object
		void DebugPrint(int tc = 0);

	protected:

		//Get/Set section
		void SetInheritance(const std::string &inheritance) {m_Inheritance = inheritance;}
		std::string GetInheritance()  const {return m_Inheritance;}

		//Help functions for template creation
		std::string CreateUniqueName();
		void InheritComponentData(ComponentContainerPtr cc);

		ComponentPtr LoadComponent(TiXmlElement *comp_template);
		//ComponentContainerPtr GetChild(const std::string &name);
		
		ComponentVector m_ComponentVector;
		ComponentContainerVector m_ComponentContainerVector;
		std::string m_Name;
		std::string m_Inheritance;
		ComponentContainerWeakPtr m_Parent;
	};
	typedef boost::shared_ptr<BaseObject> BaseObjectPtr;

}
