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

#ifndef GASS_COMPONENT_H
#define GASS_COMPONENT_H

#include "Sim/GASSCommon.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/Serialize/GASSISerialize.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSISceneManager.h"
#include "Sim/GASSBaseSceneManager.h"


namespace GASS
{
	GASS_FORWARD_DECL(SceneObject)
		GASS_FORWARD_DECL(Component);

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Component
	*  @{

	*/
	/**
		Class that every new component should be derived from.
		A component is the core building block in the component system,
		here we find actual functionality like vehicle models,
		graphics, network capability etc. All components that should work together is
		owned by a SceneObject. A SceneObject is the owner of
		the components and has it's functionality in it's components.
		The Component class is derived from the reflection template class
		which enables attribute reflection in a easy way. A component always has a
		corresponding C++ class. In order to expose/hide or making properites of components
		write protected or writable, set appropriate PropertyFlags for the properties in
		the class.

		@remarks
		Components are also stored by SceneObject templates.
		The obvious choice would be to have a ComponentTemplate
		class used by the SceneObjectTemplate class,
		however in this component system Components are used in both
		SceneObject's and SceneObjectTemplate.
		This choice was made to make new component
		implementations as convenient as possible.
		To be more precise: the downside of separating
		is that all new components will have two almost
		identical implementations, one for instances
		and one for templates.
	*/

	class GASSExport Component : public Reflection<Component, BaseReflectionObject>, public GASS_ENABLE_SHARED_FROM_THIS<Component>, public IXMLSerialize, public ISerialize, public IMessageListener, public ISceneManagerListener
	{
		friend class SceneObject;
	public:
	
		static void RegisterReflection();
		/**
			Return the name of the component
		*/
		virtual std::string GetName() const;
		/**
			Set the name of the component
		*/
		virtual void SetName(const std::string& name);
		/**
		Get the SceneObject that owns this component
		*/
		virtual SceneObjectPtr GetOwner() const;
		/**
		Set the SceneObject that this component should belong to
		*/
		virtual void SetOwner(SceneObjectPtr owner);

		//binary serialize interface
		bool Serialize(ISerializer* serializer) override;

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
			is called and the tinyxml2::XMLElement argument is the actual
			component xml-tag (MyLocation in the example)

			The user is however free to override this function for a
			specific component implementation and create it's own
			loading procedure, this will of cause also require a custom
			save implementation.
		*/
		void LoadXML(tinyxml2::XMLElement* obj_elem) override;

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
		void SaveXML(tinyxml2::XMLElement* obj_elem) override;

		/**
			This function will allocate a new component
			of the same type and copy all attributes
			to this new component.
		*/
		virtual ComponentPtr CreateCopy();
		/**
			This function will copy all matching
			attributes (attributes with same name) from
			this component to destination component.
			@dest_comp Destination component
		*/
		//virtual void CopyPropertiesTo(ComponentPtr dest_comp) const;

		/**
			Get component dependencies.
			@return names of other components that this component require
		*/
		std::vector<std::string> GetDependencies() const;

		/**
		Get owner scene object.
		Same as get owner in component
	*/
		SceneObjectPtr GetSceneObject() const;
		/**
			Called by SceneObject during initialization.
		*/
		virtual void OnInitialize();

		/**
			Called by SceneObject when ditto deleted.
			default implementation to minimize concrete components implementation
		*/
		virtual void OnDelete() {};


		//ISceneManagerListener
		/**
			Implements the ISceneManagerListener interface
			Called by responsible SceneManager if components is registered as listener.
		*/
		void SceneManagerTick(double delta) override { (void)delta; }

		/*template <typename Message>
		int RegisterForMessage(GASS_FUNCTION message_callback, int priority = 0)
		{
			GASS::MessageFuncPtr mess_func(new GASS::MessageFunc<Message>(GASS_BIND(message_callback, this, GASS_PLACEHOLDERS::_1), GASSMessageHasher(#FUNCTION), shared_from_this()));
			RegisterForMessage(typeid(Message), mess_func);
		}*/

	protected:
		static std::map<RTTI*, std::vector<std::string> > m_Dependencies;
		std::string m_Name;
		SceneObjectWeakPtr m_Owner;

		template <class T>
		void RegisterForPreUpdate()
		{
			GASS_SHARED_PTR<BaseSceneManager> base_sm = GASS_DYNAMIC_PTR_CAST<BaseSceneManager>(GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<T>());
			if (!base_sm)
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find SceneManager", "BaseSceneManager::RegisterForPreUpdate");
			SceneManagerListenerPtr listener = shared_from_this();
			base_sm->RegisterPreUpdate(listener);
		}
		template <class T>
		void RegisterForPostUpdate()
		{

			GASS_SHARED_PTR<BaseSceneManager> base_sm = GASS_DYNAMIC_PTR_CAST<BaseSceneManager>(GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<T>());
			if (!base_sm)
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find SceneManager", "BaseSceneManager::RegisterForPostUpdate");
			SceneManagerListenerPtr listener = shared_from_this();
			base_sm->RegisterPostUpdate(listener);
		}

		void RemapReferences(const std::map<SceneObjectGUID, SceneObjectGUID>& ref_map);
		void InitializePointers();
		void InitializeSceneObjectRef();
		//called by scene object to resolve SceneObjectRef 
		void ResolveTemplateReferences(SceneObjectPtr template_root);
	};
	//Declare shared pointers
	GASS_PTR_DECL(Component)
}
#endif // #ifndef Component_HH
