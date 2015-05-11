/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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

#ifndef GASS_BASE_COMPONENT_CONTAINER_H
#define GASS_BASE_COMPONENT_CONTAINER_H

#include "Core/Reflection/GASSReflection.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/Serialize/GASSISerialize.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSIterators.h"

namespace GASS
{
	FDECL(ComponentContainer);
	FDECL(Component);
	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Component
	*  @{
	*/

	/**
			

	*/

		/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Component
	*  @{
	*/

	/**
		A component container is the owner of components and child
		components containers. This way a tree structure can be created
		with component containers where each node in the tree represented by a
		component container that can have it's own set of components
		giving the node unique functionality.

		Game or simulation objects should be derived from this class and 
		can extend it with application specific attributes and functions.
		The class also enables attribute reflection in a easy way.
		The inheritance from SHARE_CLASS is used to
		get hold of ourself (this) as a shared pointer with the
		shared_from_this() function.
	*/

	class GASSCoreExport ComponentContainer : public Reflection<ComponentContainer, BaseReflectionObject> , public SHARE_CLASS<ComponentContainer>, public IXMLSerialize, public ISerialize,  public IMessageListener
	{
	public:
		typedef std::vector<ComponentPtr> ComponentVector;
		typedef VectorIterator<ComponentVector>  ComponentIterator;
		typedef ConstVectorIterator<ComponentVector>  ConstComponentIterator;
		typedef std::vector<ComponentContainerPtr> ComponentContainerVector;
		typedef VectorIterator<ComponentContainerVector> ComponentContainerIterator;
		typedef ConstVectorIterator<ComponentContainerVector> ConstComponentContainerIterator;

		ComponentContainer();
		virtual ~ComponentContainer();
		
		/**
		Static reflection function called on start up
		*/
		static	void RegisterReflection();
		
		/**
			Get component container name
		*/
		virtual std::string GetName() const {return m_Name;}
		
		/**
			Set component container name
		*/
		virtual void SetName(const std::string &name) {m_Name = name;}

		/**
			Add a child component container.
		*/
		virtual void AddChild(ComponentContainerPtr child);
		
		/**
			Remove a child component container
		*/
		virtual void RemoveChild(ComponentContainerPtr child);
		
		/**
			Get child component containers,
			this will only return the ones owned by this container i.e.
			no grandchildren will be returned.
			
		*/
		virtual ComponentContainerIterator GetChildren();
		
		/**
			Get child component containers,
			this will only return the ones owned by this container i.e.
			no grandchildren will be returned.
		*/
		virtual ConstComponentContainerIterator GetChildren() const;
		
		/**
			Get possible parent component container
		*/
		virtual ComponentContainerPtr GetParent() const {return ComponentContainerPtr(m_Parent,NO_THROW);}//allow null pointer}
		
		/**
			Set parent component container
		*/
		virtual void SetParent(ComponentContainerWeakPtr parent){m_Parent = parent;}
		
		/**
			Add a component to this component container
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
		virtual ConstComponentIterator GetComponents() const;
		
		/**
			Get all components owned by this container
		*/
		virtual ComponentIterator GetComponents();
		

		/**
			Get Number of children
		*/
		virtual size_t GetNumChildren() const;

		/**
			Get component by index
		*/
		virtual ComponentContainerPtr GetChild(size_t index) const;

		//xml serialize interface
		virtual void LoadXML(tinyxml2::XMLElement *obj_elem);
		virtual void SaveXML(tinyxml2::XMLElement *obj_elem);

		//serialize interface
		virtual bool Serialize(ISerializer* serializer);

		//print object
		void DebugPrint(int tc = 0);
		
		/**Set the template this container is derived from*/
		void SetTemplateName(const std::string &name);

		/**Get the  template this container is derived from*/
		std::string GetTemplateName() const;

		/**Set whether this container should be serialized or not, 
		by default all containers are serialized*/
		void SetSerialize(bool value);

		/**Get whether this container should be serialized or not, 
		by default all containers are serialized*/
		bool GetSerialize()  const;
	protected:
		void _CheckComponentDependencies() const;
		void _InheritComponentData(ComponentContainerPtr cc);
		
		//virtual to allow custom parsing for object creation
		virtual ComponentContainerPtr CreateComponentContainerXML(tinyxml2::XMLElement *cc_elem) const;
		ComponentPtr _LoadComponentXML(tinyxml2::XMLElement *comp_template);
		void _Copy(ComponentContainerPtr object_to_copy);
		
		ComponentVector m_ComponentVector;
		ComponentContainerVector m_ComponentContainerVector;
		std::string m_Name;
		std::string m_TemplateName;
		ComponentContainerWeakPtr m_Parent;
		//Activate/deactivate serialization
		bool m_Serialize;
	};
	typedef SPTR<ComponentContainer> ComponentContainerPtr;

}
#endif // #ifndef ComponentContainer_HH
