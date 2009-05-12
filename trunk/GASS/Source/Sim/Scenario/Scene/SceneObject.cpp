/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Core/Common.h"
#include "Core/Serialize/Serialize.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/ComponentContainerFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include <iostream>
#include <iomanip>
#include <tinyxml.h>

namespace GASS
{
	SceneObject::SceneObject()
	{
		m_MessageManager = new MessageManager();
	}

	SceneObject::~SceneObject(void)
	{
		delete m_MessageManager;
	}

	void SceneObject::RegisterReflection()
	{
		ComponentContainerFactory::GetPtr()->Register("SceneObject",new Creator<SceneObject, IComponentContainer>);
	}

	void SceneObject::OnCreate()
	{
		//only initilize components, let each child be initilize manually
		ComponentVector::iterator iter = m_ComponentVector.begin();
		while (iter != m_ComponentVector.end())
		{
			(*iter)->OnCreate();
			++iter;
		}
	}

	void SceneObject::SetSceneObjectManager(SceneObjectManager* manager)
	{
		m_Manager = manager;
		IComponentContainer::ComponentContainerVector::iterator go_iter;
		for(go_iter = m_ComponentContainerVector.begin(); go_iter != m_ComponentContainerVector.end(); go_iter++)
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>( *go_iter);
			child->SetSceneObjectManager(manager);
		}
	}

	SceneObjectPtr SceneObject::GetRoot()
	{
		ComponentContainerPtr container = shared_from_this();
		while(ComponentContainerPtr(container->GetParent()))
		{
			container = ComponentContainerPtr(container->GetParent());
		}
		return  boost::shared_static_cast<SceneObject>(container);
	}

	void SceneObject::SyncMessages(double delta_time)
	{
		GetMessageManager()->Update(delta_time);
		IComponentContainer::ComponentContainerVector::iterator go_iter;
		for(go_iter = m_ComponentContainerVector.begin(); go_iter != m_ComponentContainerVector.end(); go_iter++)
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject> (*go_iter);
			child->SyncMessages(delta_time);
		}
	}
}




