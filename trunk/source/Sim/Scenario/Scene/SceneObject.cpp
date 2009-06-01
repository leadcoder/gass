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
		GetMessageManager()->Update(delta_time);
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
		BaseObject::ComponentVector cv =  GetComponents();
		BaseObject::ComponentVector::iterator comp_iter = cv.begin();
		for(;comp_iter != cv.end();comp_iter++)
		{
			BaseSceneComponentPtr comp = boost::shared_static_cast<BaseSceneComponent>(*comp_iter);
			if(comp->GetRTTI()->IsDerivedFrom(class_name))
			{				
				components.push_back(comp);
			}
		}

		BaseObject::ComponentContainerVector children = GetChildren();
		BaseObject::ComponentContainerVector::iterator iter = children.begin();
		for(;iter != children.end();iter++)
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(*iter);
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
			if(GetName().find(name) >= 0)
			{
				SceneObjectPtr obj = boost::shared_static_cast<SceneObject>(shared_from_this());
				objects.push_back(obj);
			}
		}

		BaseObject::ComponentContainerVector children = GetChildren();
		BaseObject::ComponentContainerVector::iterator iter = children.begin();
		for(;iter != children.end();iter++)
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(*iter);
			child->GetObjectsByName(objects,name,exact_math);
		}
	}



	
		

}




