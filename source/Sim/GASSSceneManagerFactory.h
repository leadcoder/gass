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
#include "Sim/Interface/GASSISceneManager.h"
#include "Core/Utils/GASSGenericFactory.h"

namespace GASS
{
	

	class GASSExport SceneManagerFactory  
	{
	public:
		SceneManagerFactory();
		template<class T>
		void Register(const std::string& name)
		{
			m_Impl.Register<T>(name);
		}

		SceneManagerPtr Create(const std::string &name, ScenePtr scene)
		{
			SceneManagerPtr sm = m_Impl.Create(name, scene);
			sm->SetName(name);
			sm->OnPostConstruction();
			return sm;
		}

		std::vector<std::string> GetFactoryNames()
		{
			return m_Impl.GetAllKeys();
		}
	
		static SceneManagerFactory* GetPtr();
		static SceneManagerFactory& Get();
	protected:

	protected:
		GenericFactory<std::string, SceneManagerPtr, ScenePtr> m_Impl;
		static SceneManagerFactory* m_Instance;
	};
	//smart pointer typedef
	typedef GASS_SHARED_PTR<SceneManagerFactory> SceneManagerFactoryPtr;
}

