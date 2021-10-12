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

#ifndef GASS_COMPONENT_FACTORY_H
#define GASS_COMPONENT_FACTORY_H

#include "Sim/GASSCommon.h"
#include "Sim/GASSComponent.h"
#include "Core/Utils/GASSGenericFactory.h"
#include "Core/Utils/GASSLogger.h"

namespace GASS
{

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Component
	*  @{

	*/
	/**
		The one and only factory holding components.
		This is a singleton implementation of the Factory template.
		By using the singleton pattern each new component type can
		register it	self in a easy way.
		A component have to reigister it self to this
		factory	if it want any type of serialization behavior.
		See Factory class for more information on how to
		do the actual registration.
	*/
	class GASSExport ComponentFactory
	{
	public:
		template<class T>
		void Register(const std::string& key)
		{
			m_Impl.Register<T>(key);
			const std::string class_name = StringUtils::Demangle(typeid(T).name());
			m_ClassNameToKey[class_name] = key;
			m_KeyToClassName[key] = class_name;
		}

		template<class T>
		void Register()
		{
			const std::string class_name = StringUtils::Demangle(typeid(T).name());
			m_Impl.Register<T>(class_name);
			m_ClassNameToKey[class_name] = class_name;
			m_KeyToClassName[class_name] = class_name;
		}

		ComponentPtr Create(const std::string &name)
		{
			ComponentPtr component;
			if(m_Impl.IsCreatable(name))
				component = m_Impl.Create(name);
			else
				GASS_LOG() << "Failed to create component: " << name;
			return component;
		}

		std::vector<std::string> GetFactoryNames()
		{
			return m_Impl.GetAllKeys();
		}

		std::string GetKeyFromClassName(const std::string &class_name)
		{
			auto iter = m_ClassNameToKey.find(class_name);
			if (iter != m_ClassNameToKey.end())
				return iter->second;
			return "";
		}

		std::string GetClassNameFromKey(const std::string &factory_key)
		{
			auto iter = m_KeyToClassName.find(factory_key);
			if (iter != m_KeyToClassName.end())
				return iter->second;
			return "";
		}

		static ComponentFactory* GetPtr();
		static ComponentFactory& Get();
	protected:
		static ComponentFactory* m_Instance;
		GenericFactory<std::string, ComponentPtr> m_Impl;
		std::map<std::string, std::string> m_ClassNameToKey;
		std::map<std::string, std::string> m_KeyToClassName;
	};
}

#endif // #ifndef COMPONENTFACTORY_HH
