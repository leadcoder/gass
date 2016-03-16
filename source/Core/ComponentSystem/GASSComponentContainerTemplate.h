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

#ifndef ComponentContainerTemplate_HH
#define ComponentContainerTemplate_HH

#include "Core/Reflection/GASSReflection.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/Serialize/GASSISerialize.h"
#include "Core/Utils/GASSIterators.h"

namespace GASS
{
	class MessageManager;
	GASS_FORWARD_DECL(ComponentContainerTemplate);
	GASS_FORWARD_DECL(Component);
	GASS_FORWARD_DECL(ComponentContainer);
	GASS_FORWARD_DECL(ComponentContainerTemplateManager);
	typedef GASS_SHARED_PTR<ComponentContainerTemplateManager const> ComponentContainerTemplateManagerConstPtr;

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Component
	*  @{
	*/

	/**
			The class has the ability to instance 
			new component containers of the exact same configuration. 
			Templates/archetypes of objects can be created trough configuration files 
			(xml files with extension .template) or in code.
			
			The ComponentContainerTemplate inherit from the reflection template
			class which enables attribute reflection. The inheritance from
			GASS_ENABLE_SHARED_FROM_THIS is used to get hold of ourself (this) as
			a shared pointer with the shared_from_this() function.
	*/
	
	class GASSCoreExport ComponentContainerTemplate : public Reflection<ComponentContainerTemplate, BaseReflectionObject> ,public GASS_ENABLE_SHARED_FROM_THIS<ComponentContainerTemplate>, public IXMLSerialize, public ISerialize
	{
		friend class ComponentContainerTemplateManager;
	public:
		typedef std::vector<ComponentPtr> ComponentVector;
		typedef VectorIterator<ComponentVector>  ComponentIterator;
		typedef std::vector<ComponentContainerTemplatePtr> ComponentContainerTemplateVector;
		typedef VectorIterator<ComponentContainerTemplateVector> ComponentContainerTemplateIterator;
		typedef ConstVectorIterator<ComponentContainerTemplateVector> ConstComponentContainerTemplateIterator;

		ComponentContainerTemplate();
		virtual ~ComponentContainerTemplate();
		/**
		Static reflection function called on start up
		*/
		static	void RegisterReflection();
		
		/**
		Get component container template name
		*/
		virtual std::string GetName() const {return m_Name;}
		
		/**
		Set component container template name
		*/
		virtual void SetName(const std::string &name) {m_Name = name;}
		
		/**
			Add a child component container template.
		*/
		virtual void AddChild(ComponentContainerTemplatePtr child);
		
		/**
			Remove child component container template.
		*/
		virtual void RemoveChild(ComponentContainerTemplatePtr child);
		/**
			Get child component containers templates,
			this will only return the ones owned by this container i.e.
			no grandchildren will be returned
		*/
		virtual ComponentContainerTemplateIterator GetChildren();

		/**
			Get possible parent component container template
		*/
		virtual ComponentContainerTemplatePtr GetParent() const {return m_Parent.lock();}//allow null pointer}
		
		/**
			Set parent component container template
		*/
		virtual void SetParent(ComponentContainerTemplateWeakPtr parent){m_Parent = parent;}
		
		/**
			Add new component to this container
		*/
		virtual void AddComponent(ComponentPtr comp);

		/**
			Get component by name,
			only search this containers components and first one is returned
		*/
		virtual ComponentPtr GetComponent(const std::string &name) const;

		/**
			Get all components owned by this container
		*/
		virtual ComponentIterator GetComponents();

		/**
			Rebuild this template component container from existing ComponentContainer. Still experimental
		*/
		virtual void CreateFromComponentContainer(ComponentContainerPtr cc,ComponentContainerTemplateManagerConstPtr manager, bool keep_inheritance);

		//xml serialize interface
		virtual void LoadXML(tinyxml2::XMLElement *obj_elem);
		virtual void SaveXML(tinyxml2::XMLElement *obj_elem);

		//serialize interface
		virtual bool Serialize(ISerializer* serializer);

		/**
			Set the template name that this template should inherit from. 
			Empty string means no inheritance.
			By using inheritance you will be able to make use of other template configurations,
			which include: 
			-	All properties of the template class itself
			-	All components (including properties)
			-	All child ComponentContainerTemplate's (recursively)
			You can add new components and override components from the inheritance. If you have more than
			one component of the same type (in one ComponentContainerTemplate) the override
			feature will use the name attribute of the component to do the matching.
			When overriding a component all attributes need to be specified once more, in other words, 
			you can not partially override component properties.
		*/
		void SetInheritance(const std::string &inheritance) {m_Inheritance = inheritance;}
		
		/** 
			Get name of template inheritance, see SetInheritance for more info.
			It no inheritance is present an empty string will be returned.
		*/
		std::string GetInheritance()  const {return m_Inheritance;}

		/**Set whether this container should be serialized or not, by default
		all containers are serialized*/
		void SetSerialize(bool value);

		/**Get whether this container should be serialized or not, by default
		all containers are serialized*/
		bool GetSerialize()  const;

		/**
			Create and add new component. 
			This function will create a new component of the supplied type 
			by using the component factory. Then it will be added to the component 
			vector for this component container. 
			@param comp_type Component type to add
			@return The new component 
		*/
		ComponentPtr AddComponent(const std::string &comp_type);

		//Debug functions to print object recursivly to std::cout
		void DebugPrint(int tc = 0);

	protected:

		//It's possible to override this function if custom creation process is needed.
		//By default the container factory name used is the same as the template
		//with the Template part removed, however if thats not the case you have to
		//override this function and supply your own instance. 
		//Another case could be that some attributes have to be transfered 
		//from template to instance in a custom way.
		virtual ComponentContainerPtr CreateComponentContainer() const;

		//Help functions during template creation
		//std::string CreateUniqueName(ComponentContainerTemplateManagerConstPtr manager) const;
		void _InheritComponentData(ComponentContainerPtr cc) const;
		ComponentPtr _LoadComponentXML(tinyxml2::XMLElement *comp_template);

		ComponentVector m_ComponentVector;
		ComponentContainerTemplateVector m_ComponentContainerVector;
		std::string m_Name;
		std::string m_Inheritance;
		bool m_Serialize;
		ComponentContainerTemplateWeakPtr m_Parent;
	};
	

}
#endif // #ifndef ComponentContainerTemplate_HH
