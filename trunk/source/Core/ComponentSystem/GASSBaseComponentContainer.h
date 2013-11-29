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

#ifndef GASS_BASE_COMPONENT_CONTAINER_H
#define GASS_BASE_COMPONENT_CONTAINER_H

#include "Core/Reflection/GASSReflection.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/ComponentSystem/GASSIComponentContainer.h"
#include "Core/ComponentSystem/GASSIComponentContainerTemplate.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/Serialize/GASSISerialize.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{
	FDECL(BaseComponentContainer);
	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Component
	*  @{
	*/

	/**
			This is the class that a game or simulation object should be derive from.
			The BaseComponentContainer is a convinience class that implements the
			IComponentContainer it also inherite from the reflection template class
			which enables attribute reflection in a easy way.
			The inheritance from SHARE_CLASS is used to
			get hold of ourself (this) as a shared pointer with the
			shared_from_this() function.
			To get more information what you get by inherit from this
			class see the documentation for each interface

	*/
	class GASSCoreExport BaseComponentContainer : public Reflection<BaseComponentContainer, BaseReflectionObject> ,public SHARE_CLASS<BaseComponentContainer>, public IComponentContainer, public IXMLSerialize, public ISerialize,  public IMessageListener
	{
	public:
		BaseComponentContainer();
		virtual ~BaseComponentContainer();
		static	void RegisterReflection();

		//ComponentContainer interface
		virtual std::string GetName() const {return m_Name;}
		virtual void SetName(const std::string &name) {m_Name = name;}
		virtual void AddChild(ComponentContainerPtr child);
		virtual void RemoveChild(ComponentContainerPtr child);
		virtual ComponentContainerIterator GetChildren();
		virtual ConstComponentContainerIterator GetChildren() const;
		virtual ComponentContainerPtr GetParent() const {return ComponentContainerPtr(m_Parent,NO_THROW);}//allow null pointer}
		virtual void SetParent(ComponentContainerWeakPtr parent){m_Parent = parent;}
		virtual void AddComponent(ComponentPtr comp);
		virtual ComponentPtr GetComponent(const std::string &name) const;
		virtual ConstComponentIterator GetComponents() const;
		virtual ComponentIterator GetComponents();
		virtual size_t GetNumChildren() const;
		virtual ComponentContainerPtr GetChild(size_t index) const;

		//xml serialize interface
		virtual void LoadXML(TiXmlElement *obj_elem);
		virtual void SaveXML(TiXmlElement *obj_elem);

		//serialize interface
		virtual bool Serialize(ISerializer* serializer);

		//print object
		void DebugPrint(int tc = 0);

		//Get/Set section

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
		//Help functions for template creation
		std::string CreateUniqueName();
		void InheritComponentData(ComponentContainerPtr cc);
		//Allow custom parsing for object creation
		virtual ComponentContainerPtr CreateComponentContainer(TiXmlElement *cc_elem) const;
		void Copy(BaseComponentContainerPtr object_to_copy);
	
		ComponentPtr LoadComponent(TiXmlElement *comp_template);
		ComponentVector m_ComponentVector;
		ComponentContainerVector m_ComponentContainerVector;
		std::string m_Name;
		std::string m_TemplateName;
		ComponentContainerWeakPtr m_Parent;
		//Activate/deactivate serialization
		bool m_Serialize;
	};
	typedef SPTR<BaseComponentContainer> BaseComponentContainerPtr;

}
#endif // #ifndef BASECOMPONENTCONTAINER_HH
