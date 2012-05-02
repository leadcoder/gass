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

#ifndef BASECOMPONENT_HH
#define BASECOMPONENT_HH

#include <Core/Common.h>

#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/Serialize/GASSISerialize.h"

#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/ComponentSystem/GASSIComponentTemplate.h"

namespace GASS
{
	class IComponentContainer;

	typedef boost::shared_ptr<IComponentContainer> ComponentContainerPtr;
	typedef boost::weak_ptr<IComponentContainer> ComponentContainerWeakPtr;
	typedef boost::shared_ptr<IComponent> ComponentPtr;
	typedef boost::weak_ptr<IComponent> ComponentWeakPtr;

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Component
	*  @{
	*/

	/**
			The BaseComponent is a convinience class that implements the
			IComponent interface it also inherite from the reflection template class
			which enables attribute reflection in a easy way.
			To get more information what you get by inherit from this
			class see the documentation for each interface.
			If you intened to make new components this is a good 
			class to inherit from or use as starting point for your own
			IComponent implementation
	*/

	class GASSCoreExport BaseComponent : public Reflection<BaseComponent, BaseReflectionObject>, public IComponent , public IXMLSerialize, public ISerialize, public IComponentTemplate
	{
	public:
		BaseComponent();
		virtual ~BaseComponent();
		static void RegisterReflection();
		//component interface
		//virtual void OnCreate();
		virtual std::string GetName() const;
		virtual void SetName(const std::string &name);
		/**
		Get the component container that owns this component
		*/
		virtual ComponentContainerPtr GetOwner() const;
		/**
		Set the component container that this component should belong to
		*/
		virtual void SetOwner(ComponentContainerPtr owner);

		//binary serialize interface
		virtual bool Serialize(ISerializer* serializer);

		//xml-serialize interface
		/**
			Function to load component attributes from xml-tag.
			Each data field is assumed to be on the following 
			format: <AttributName value="data"/> where 
			AttributeName is the name of the registred attribute
			and "data" is the actual value to be loaded into that
			attribute. 
			Example of component xml-tag:
			<MyLocation type="LocationComponent">
				<Position value="0 0 0"/>
				<Rotation value="0 0 0"/>
				<Visible value="1"/>
			</MyLocation>

			Notice that the actual component, a LocationComponent 
			in the example above ,is created before this function 
			is called and the TiXmlElement argument is the actual 
			component xml-tag (MyLocation in the example)

			The user is however free to override this function for a
			specific component implementation and create it's own 
			loading procedure, this will of cause also require a custom 
			save implementation.
		*/
		virtual void LoadXML(TiXmlElement *obj_elem);

		/**
			Function to save component attributes to xml-tag.
			Besides saving component attributes this function
			also save the component type that is required when
			loading the component.

			Example of component xml-tag:
			<MyLocation type="LocationComponent">
				<Position value="0 0 0"/>
				<Rotation value="0 0 0"/>
				<Visible value="1"/>
			</MyLocation>

			In the above example a LocationComponent with 
			three attributes is saved to a xml-tag. 
			The user is however free to override this function for a
			specific component implementation and create it's own 
			saving procedure, this will of cause also require a custom 
			load implementation.

		*/
		virtual void SaveXML(TiXmlElement *obj_elem);

		//template interface
		/**
			This function will allocate a new component
			of the same type and copy all attributes 
			to this new component.
		*/
		virtual ComponentPtr CreateCopy();
		/**
			This function will assign all matching 
			attributes (attributes with same name) from 
			supplied comp argument to this component.
		*/
		virtual void AssignFrom(ComponentPtr comp);

	protected:
		std::string m_Name;
		ComponentContainerWeakPtr m_Owner;
	};
	typedef boost::shared_ptr<BaseComponent> BaseComponentPtr;

}
#endif // #ifndef BASECOMPONENT_HH
