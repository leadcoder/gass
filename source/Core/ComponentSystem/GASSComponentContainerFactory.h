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

#include "Core/Common.h"
#include "Core/ComponentSystem/GASSComponentContainer.h"
#include "Core/Utils/GASSGenericFactory.h"

namespace GASS
{

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Component
	*  @{
	*/

	/**
		The one and only factory holding component containers.
		This is a singleton implementation of the Factory template.
		By using the singleton pattern each component container
		type can register it self in a easy way.
		A component container have to reigister it self to this
		factory	if it want any type of serialization behavior.
		See Factory class for more information on how to
		do the actual registration.
	*/
	class GASSCoreExport ComponentContainerFactory
	{
	public:
		template<class T>
		void Register(const std::string& name)
		{
			m_Impl.Register<T>(name);
			const std::string class_name = StringUtils::Demangle(typeid(T).name());
			m_ClassNameToKey[class_name] = name;
		}

		ComponentContainerPtr Create(const std::string &name)
		{
			ComponentContainerPtr cc = m_Impl.Create(name);
			return cc;
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

		static ComponentContainerFactory* GetPtr();
		static ComponentContainerFactory& Get();
	protected:
		GenericFactory<std::string, ComponentContainerPtr> m_Impl;
		std::map<std::string, std::string> m_ClassNameToKey;
		static ComponentContainerFactory* m_Instance;
	protected:
	};
}
