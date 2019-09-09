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
#include "Sim/GASSSceneObjectTemplate.h"
#include "Sim/GASSBaseSceneComponent.h"

#include "Core/Common.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplateFactory.h"

namespace GASS
{
	SceneObjectTemplate::SceneObjectTemplate() : m_Instantiable(false)
	{
		
	}

	void SceneObjectTemplate::RegisterReflection()
	{
		ComponentContainerTemplateFactory::GetPtr()->Register("SceneObjectTemplate",new Creator<SceneObjectTemplate, ComponentContainerTemplate>);
		RegisterGetSet("ID", &GASS::SceneObjectTemplate::GetID, &GASS::SceneObjectTemplate::SetID);
		RegisterGetSet("Instantiable", &GASS::SceneObjectTemplate::GetInstantiable, &GASS::SceneObjectTemplate::SetInstantiable);
	}


	BaseSceneComponentPtr SceneObjectTemplate::AddBaseSceneComponent(const std::string &comp_name)
	{
		return GASS_DYNAMIC_PTR_CAST<GASS::BaseSceneComponent>(AddComponent(comp_name));
	}

	BaseSceneComponentPtr SceneObjectTemplate::GetBaseSceneComponent(const std::string &comp_name) const
	{
		return GASS_DYNAMIC_PTR_CAST<GASS::BaseSceneComponent>(GetComponent(comp_name));
	}

}




