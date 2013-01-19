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
#include "Core/Reflection/GASSReflection.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/ComponentSystem/GASSIComponentContainer.h"
#include "Core/ComponentSystem/GASSIComponentContainerTemplate.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/Serialize/GASSISerialize.h"

namespace GASS
{
	class MessageManager;

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Component
	*  @{
	*/

	/**
			The BaseComponentContainerTemplate is a convinience class that implements
			the	IComponentContainerTemplate it also inherite from the reflection template
			class which enables attribute reflection in a easy way. The inheritance from
			SHARE_CLASS is used to get hold of ourself (this) as
			a shared pointer with the shared_from_this() function.
			To get more information what you get by inherit from this
			class see the documentation for each interface

	*/
	class GASSCoreExport BaseComponentContainerTemplate : public Reflection<BaseComponentContainerTemplate, BaseReflectionObject> ,public SHARE_CLASS<BaseComponentContainerTemplate>, public IComponentContainerTemplate , public IXMLSerialize, public ISerialize
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
		virtual ComponentContainerTemplatePtr GetParent() const {return ComponentContainerTemplatePtr(m_Parent,NO_THROW);}//allow null pointer}
		virtual void SetParent(ComponentContainerTemplateWeakPtr parent){m_Parent = parent;}
		virtual void AddComponent(ComponentPtr comp);
		virtual ComponentPtr GetComponent(const std::string &name) const;
		virtual ComponentIterator GetComponents();
		virtual ComponentContainerPtr CreateComponentContainer(int &part_id, ComponentContainerTemplateManagerConstPtr manager) const;
		virtual void CreateFromComponentContainer(ComponentContainerPtr cc,ComponentContainerTemplateManagerConstPtr manager, bool keep_inheritance);


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


		/**Set whether this container should be serialized or not, by defaulit
		all containers are serialized*/
		void SetSerialize(bool value);

		/**Get whether this container should be serialized or not, by defaulit
		all containers are serialized*/
		bool GetSerialize()  const;

	protected:

		//Its possible to override this function if custom creation proccess is needed.
		//By default the container factory name used is the same as the template
		//with the Template part removed, however if thats not the case you have to
		//override this function and supply your own instance. 
		//Another case could be that some attributes have to be transfered 
		//from template to instance in a custom way.
		
		virtual ComponentContainerPtr CreateComponentContainer() const;

		

		//Help functions for template creation
		std::string CreateUniqueName(ComponentContainerTemplateManagerConstPtr manager) const;
		void InheritComponentData(ComponentContainerPtr cc) const;

		ComponentPtr LoadComponent(TiXmlElement *comp_template);

		ComponentVector m_ComponentVector;
		ComponentContainerTemplateVector m_ComponentContainerVector;
		std::string m_Name;
		std::string m_Inheritance;
		bool m_Serialize;
		ComponentContainerTemplateWeakPtr m_Parent;
	};
	typedef SPTR<BaseComponentContainerTemplate> BaseComponentContainerTemplatePtr;

}
#endif // #ifndef BASECOMPONENTCONTAINERTEMPLATE_HH
