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

#pragma once

#include "Sim/GASSCommon.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Reflection/GASSReflection.h"
#include "Sim/GASSComponent.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/Serialize/GASSISerialize.h"
#include "Core/Utils/GASSIterators.h"

namespace GASS
{
	class MessageManager;
	GASS_FORWARD_DECL(SceneObjectTemplate);
	GASS_FORWARD_DECL(Component);
	GASS_FORWARD_DECL(SceneObject);
	GASS_FORWARD_DECL(SceneObjectTemplateManager);
	typedef GASS_SHARED_PTR<SceneObjectTemplateManager const> SceneObjectTemplateManagerConstPtr;
	typedef std::string SceneObjectID;
	class BaseSceneComponent;
	typedef GASS_SHARED_PTR<BaseSceneComponent> BaseSceneComponentPtr;
	typedef std::vector<SceneObjectTemplatePtr> SceneObjectTemplateVector;

	class GASSExport SceneObjectTemplate : public Reflection<SceneObjectTemplate, BaseReflectionObject>, public GASS_ENABLE_SHARED_FROM_THIS<SceneObjectTemplate>, public IXMLSerialize, public ISerialize
	{
		friend class SceneObjectTemplateManager;
	public:

		typedef std::vector<ComponentPtr> ComponentVector;
		typedef VectorIterator<ComponentVector>  ComponentIterator;
		typedef std::vector<SceneObjectTemplatePtr> SceneObjectTemplateVector;
		typedef VectorIterator<SceneObjectTemplateVector> SceneObjectTemplateIterator;
		typedef ConstVectorIterator<SceneObjectTemplateVector> ConstSceneObjectTemplateIterator;

		SceneObjectTemplate();
		static	void RegisterReflection();
		void SetID(const SceneObjectID& id) { m_ID = id; }
		SceneObjectID GetID() const { return m_ID; }
		void SetInstantiable(bool value) { m_Instantiable = value; }
		bool GetInstantiable() const { return m_Instantiable; }
		/**
			Convenience function for BaseSceneComponent's that call AddComponent on SceneObjectTemplate
		*/
		BaseSceneComponentPtr AddBaseSceneComponent(const std::string& comp_name);


		/**
			Convenience function for BaseSceneComponent's that call GetComponent on SceneObjectTemplate
		*/
		BaseSceneComponentPtr GetBaseSceneComponent(const std::string& comp_name) const;

		/**
			Get SceneObject template name
		*/
		virtual std::string GetName() const { return m_Name; }

		/**
			Set SceneObject template name
		*/
		virtual void SetName(const std::string& name) { m_Name = name; }

		/**
			Add a child SceneObject template.
		*/
		virtual void AddChild(SceneObjectTemplatePtr child);

		/**
			Remove child SceneObject template.
		*/
		virtual void RemoveChild(SceneObjectTemplatePtr child);
		/**
			Get child SceneObjects templates,
			this will only return the ones owned by this object i.e.
			no grandchildren will be returned
		*/
		virtual SceneObjectTemplateIterator GetChildren();

		/**
			Get possible parent SceneObject template
		*/
		virtual SceneObjectTemplatePtr GetParent() const { return m_Parent.lock(); }//allow null pointer}

		/**
			Set parent SceneObject template
		*/
		virtual void SetParent(SceneObjectTemplateWeakPtr parent) { m_Parent = parent; }

		/**
			Add component
		*/
		virtual void AddComponent(ComponentPtr comp);

		/**
			Get component by name,
			only search this SceneObject and first one is returned
		*/
		virtual ComponentPtr GetComponent(const std::string& name) const;

		/**
			Get all components
		*/
		virtual ComponentIterator GetComponents();

		/**
			Rebuild this template SceneObject from existing SceneObject. Still experimental
		*/
		virtual void CreateFromSceneObject(SceneObjectPtr cc, SceneObjectTemplateManagerConstPtr manager, bool keep_inheritance);

		//xml serialize interface
		void LoadXML(tinyxml2::XMLElement* obj_elem) override;
		void SaveXML(tinyxml2::XMLElement* obj_elem) override;

		//serialize interface
		bool Serialize(ISerializer* serializer) override;

		/**
			Set the template name that this template should inherit from.
			Empty string means no inheritance.
			By using inheritance you will be able to make use of other template configurations,
			which include:
			-	All properties of the template class itself
			-	All components (including properties)
			-	All child SceneObjectTemplate's (recursively)
			You can add new components and override components from the inheritance. If you have more than
			one component of the same type (in one SceneObjectTemplate) the override
			feature will use the name attribute of the component to do the matching.
			When overriding a component all attributes need to be specified once more, in other words,
			you can not partially override component properties.
		*/
		void SetInheritance(const std::string& inheritance) { m_Inheritance = inheritance; }

		/**
			Get name of template inheritance, see SetInheritance for more info.
			It no inheritance is present an empty string will be returned.
		*/
		std::string GetInheritance()  const { return m_Inheritance; }

		/**
			Set whether this object should be serialized or not, by default
			all SceneObjects are serialized
		*/
		void SetSerialize(bool value);

		/**
			Get whether this object should be serialized or not, by default
			all SceneObjects are serialized
		*/
		bool GetSerialize()  const;

		/**
			Create and add new component.
			This function will create a new component of the supplied type
			by using the component factory. Then it will be added to the component
			vector for this SceneObject.
			@param comp_type Component type to add
			@return The new component
		*/
		ComponentPtr AddComponent(const std::string& comp_type);

		//Debug functions to print object recursivly to std::cout
		void DebugPrint(int tc = 0);

	protected:

		//It's possible to override this function if custom creation process is needed.
		//By default the factory name used is the same as the template
		//with the Template part removed, however if thats not the case you have to
		//override this function and supply your own instance. 
		//Another case could be that some attributes have to be transfered 
		//from template to instance in a custom way.
		virtual SceneObjectPtr CreateSceneObject() const;

		//Help functions during template creation
		void _InheritComponentData(SceneObjectPtr cc) const;
		ComponentPtr _LoadComponentXML(tinyxml2::XMLElement* comp_template) const;

		ComponentVector m_ComponentVector;
		SceneObjectTemplateVector m_SceneObjectVector;
		std::string m_Name;
		std::string m_Inheritance;
		bool m_Serialize{ true };
		SceneObjectTemplateWeakPtr m_Parent;
	protected:
		SceneObjectID m_ID;
		bool m_Instantiable;
	};
}
