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
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Components/BaseSceneComponent.h"

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
		RegisterForMessage(OBJECT_RM_SCENE_OBJECT_NAME,MESSAGE_FUNC(SceneObject::OnChangeName)); 
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

	SceneObjectPtr SceneObject::GetObjectUnderRoot()
	{
		ComponentContainerPtr container = shared_from_this();

		SceneObjectPtr root = GetSceneObjectManager()->GetSceneRoot();
		
		while(ComponentContainerPtr(container->GetParent()) != root)
		{
			container = ComponentContainerPtr(container->GetParent());
		}
		return  boost::shared_static_cast<SceneObject>(container);
	}

	void SceneObject::SyncMessages(double delta_time)
	{
		m_MessageManager->Update(delta_time);
		IComponentContainer::ComponentContainerVector::iterator go_iter;
		for(go_iter = m_ComponentContainerVector.begin(); go_iter != m_ComponentContainerVector.end(); go_iter++)
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject> (*go_iter);
			child->SyncMessages(delta_time);
		}
	}

	void SceneObject::GetComponentsByClass(ComponentVector &components, const std::string &class_name)
	{
		//Check all components
		IComponentContainer::ComponentIterator comp_iter = GetComponents();
		while(comp_iter.hasMoreElements())
		{
			BaseSceneComponentPtr comp = boost::shared_static_cast<BaseSceneComponent>(comp_iter.getNext());
			if(comp->GetRTTI()->IsDerivedFrom(class_name))
			{				
				components.push_back(comp);
			}
		}

		IComponentContainer::ComponentContainerIterator cc_iter = GetChildren();
		while(cc_iter.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(cc_iter.getNext());
			child->GetComponentsByClass(components, class_name);
		}
	}


	SceneObjectVector SceneObject::GetObjectsByName(const std::string &name, bool exact_math)
	{
		SceneObjectVector objects;
		GetObjectsByName(objects, name,exact_math);
		return objects;
	}

	void SceneObject::GetObjectsByName(SceneObjectVector &objects, const std::string &name, bool exact_math)
	{
		SceneObjectPtr ret;

		if(exact_math)
		{
			if(GetName()== name)
			{
				SceneObjectPtr obj = boost::shared_static_cast<SceneObject>(shared_from_this());
				objects.push_back(obj);
			}
		}
		else
		{
			int pos = GetName().find(name);
			if(pos  >= 0)
			{
				SceneObjectPtr obj = boost::shared_static_cast<SceneObject>(shared_from_this());
				objects.push_back(obj);
			}
		}

		IComponentContainer::ComponentContainerIterator children = GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(children.getNext());
			child->GetObjectsByName(objects,name,exact_math);
		}
	}


	int SceneObject::RegisterForMessage( ObjectMessage type, MessageFunc callback, int priority )
	{
		return m_MessageManager->RegisterForMessage((int)type, callback, priority); 
	}

	void SceneObject::UnregisterForMessage(ObjectMessage type, MessageFunc callback)
	{
		m_MessageManager->UnregisterForMessage((int)type, callback);
	}

	void SceneObject::PostMessage( MessagePtr message )
	{
		m_MessageManager->PostMessage(message);
	}

	void SceneObject::SendImmediate( MessagePtr message )
	{
		m_MessageManager->SendImmediate(message);
	}

	void SceneObject::OnChangeName(MessagePtr message)
	{
		std::string name = boost::any_cast<std::string>(message->GetData("Name"));
		SetName(name);
	}
}




