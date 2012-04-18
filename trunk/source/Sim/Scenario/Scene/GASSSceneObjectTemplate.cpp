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
#include "Sim/Scenario/Scene/GASSSceneObjectTemplate.h"
#include "Sim/Scenario/Scene/GASSSceneObjectManager.h"
#include "Sim/Components/GASSBaseSceneComponent.h"

#include "Core/Common.h"
#include "Core/Serialize/GASSSerialize.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplateFactory.h"

namespace GASS
{
	SceneObjectTemplate::SceneObjectTemplate()
	{
		
	}

	SceneObjectTemplate::~SceneObjectTemplate(void)
	{
	}

	void SceneObjectTemplate::RegisterReflection()
	{
		ComponentContainerTemplateFactory::GetPtr()->Register("SceneObjectTemplate",new Creator<SceneObjectTemplate, IComponentContainerTemplate>);
		RegisterProperty<SceneObjectID>("ID", &GASS::SceneObjectTemplate::GetID, &GASS::SceneObjectTemplate::SetID);
	}

}



