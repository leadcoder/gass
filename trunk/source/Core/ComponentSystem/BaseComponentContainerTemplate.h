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

#ifndef BASECOMPONENTCONTAINERTEMPLATE_HH
#define BASECOMPONENTCONTAINERTEMPLATE_HH

#include <boost/enable_shared_from_this.hpp>
#include "Core/Reflection/Reflection.h"
#include "Core/Reflection/BaseReflectionObject.h"
#include "Core/ComponentSystem/IComponentContainer.h"
#include "Core/ComponentSystem/IComponentContainerTemplate.h"
#include "Core/Serialize/IXMLSerialize.h"
#include "Core/Serialize/ISerialize.h"

namespace GASS
{
	class MessageManager;

	/**
			The BaseComponentContainerTemplate is a convinience class that implements
			the	IComponentContainerTemplate it also inherite from the reflection template
			class which enables attribute reflection in a easy way. The inheritance from
			boost::enable_shared_from_this is used to get hold of ourself (this) as
			a shared pointer with the shared_from_this() function.
			To get more information what you get by inherit from this
			class see the documentation for each interface

	*/
	class GASSCoreExport BaseComponentContainerTemplate : public Reflection<BaseComponentContainerTemplate, BaseReflectionObject> ,public boost::enable_shared_from_this<BaseComponentContainerTemplate>, public IComponentContainerTemplate , public IXMLSerialize, public ISerialize
	{
	public:
		//typedef std::vector<IComponent*> ComponentVector;
		//typedef std::vector<IComponentContainer*> ComponentContainerVector;
	public:
		BaseComponentContainerTemplate();
		virtual ~BaseComponentContainerTemplate();

		static	void RegisterReflection();

		//IComponentContainerTemplate interface
		virtual std::string GetName() const {return m_Name;}
		virtual void SetName(const std::string &name) {m_Name = name;}
		virtual void AddChild(ComponentContainerTemplatePtr child);
		virtual void RemoveChild(ComponentContainerTemplatePtr child);
		virtual ComponentContainerTemplateIterator GetChildren();
		virtual ComponentContainerTemplatePtr GetParent() const {return ComponentContainerTemplatePtr(m_Parent,boost::detail::sp_nothrow_tag());}//allow null pointer}
		virtual void SetParent(ComponentContainerTemplateWeakPtr parent){m_Parent = parent;}
		virtual void AddComponent(ComponentPtr comp);
		virtual ComponentPtr GetComponent(const std::string &name);
		virtual ComponentIterator GetComponents();
		virtual ComponentContainerPtr CreateComponentContainer(int &part_id, ComponentContainerTemplateManagerPtr manager);
		void CreateFromComponentContainer(ComponentContainerPtr cc);


		//xml serialize interface
		virtual void LoadXML(TiXmlElement *obj_elem);
		virtual void SaveXML(TiXmlElement *obj_elem);

		//serialize interface
		virtual bool Serialize(ISerializer* serializer);

		//Get/Set section
		void SetInheritance(const std::string &inheritance) {m_Inheritance = inheritance;}
		std::string GetInheritance()  const {return m_Inheritance;}


		//print object
		void DebugPrint(int tc = 0);

	protected:

		//Its possible to override this function if custom creation proccess is needed.
		//By default the container factory name used is the same as the template
		//with the Template part removed, however if thats not the case you have to
		//override this function and supply your own instance. 
		//Another case could be that some attributes have to be transfered 
		//from template to instance in a custom way.
		
		virtual ComponentContainerPtr CreateComponentContainer();

		

		//Help functions for template creation
		std::string CreateUniqueName();
		void InheritComponentData(ComponentContainerPtr cc);

		ComponentPtr LoadComponent(TiXmlElement *comp_template);

		ComponentVector m_ComponentVector;
		ComponentContainerTemplateVector m_ComponentContainerVector;
		std::string m_Name;
		std::string m_Inheritance;
		ComponentContainerTemplateWeakPtr m_Parent;
	};
	typedef boost::shared_ptr<BaseComponentContainerTemplate> BaseComponentContainerTemplatePtr;

}
#endif // #ifndef BASECOMPONENTCONTAINERTEMPLATE_HH
