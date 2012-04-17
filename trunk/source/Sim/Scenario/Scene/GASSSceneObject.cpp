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
#include "Sim/Scenario/Scene/GASSSceneObject.h"

#include "Sim/Scenario/Scene/GASSSceneObjectManager.h"
#include "Sim/Components/GASSBaseSceneComponent.h"

#include "Core/Common.h"
#include "Core/Serialize/GASSSerialize.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentContainerFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include <iostream>
#include <iomanip>
#include <tinyxml.h>

namespace GASS
{
	SceneObject::SceneObject() : m_MessageManager (new MessageManager())
	{

	}

	SceneObject::~SceneObject(void)
	{
		//delete m_MessageManager;
	}

	void SceneObject::RegisterReflection()
	{
		ComponentContainerFactory::GetPtr()->Register("SceneObject",new Creator<SceneObject, IComponentContainer>);
		RegisterProperty<SceneObjectID>("ID", &GASS::SceneObject::GetID, &GASS::SceneObject::SetID);
	}

	void SceneObject::OnCreate()
	{
		RegisterForMessage(typeid(SceneObjectNameMessage),TYPED_MESSAGE_FUNC(SceneObject::OnChangeName,SceneObjectNameMessage));
		//only initilize components, let each child be initilize manually
		ComponentVector::iterator iter = m_ComponentVector.begin();
		while (iter != m_ComponentVector.end())
		{
			BaseSceneComponentPtr bsc = boost::shared_dynamic_cast<BaseSceneComponent>(*iter);
			
			bsc->OnCreate();
			++iter;
		}
	}

	void SceneObject::SetSceneObjectManager(SceneObjectManagerPtr manager)
	{
		m_Manager = manager;
		IComponentContainer::ComponentContainerVector::iterator go_iter;
		for(go_iter = m_ComponentContainerVector.begin(); go_iter != m_ComponentContainerVector.end(); ++go_iter)
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>( *go_iter);
			child->SetSceneObjectManager(manager);
		}
	}

	SceneObjectPtr SceneObject::GetObjectUnderRoot()
	{
		ComponentContainerPtr container = shared_from_this();

		SceneObjectPtr root = GetSceneObjectManager()->GetSceneRoot();

		while(container->GetParent() && ComponentContainerPtr(container->GetParent()) != root)
		{
			container = ComponentContainerPtr(container->GetParent());
		}
		return  boost::shared_static_cast<SceneObject>(container);
	}

	void SceneObject::SyncMessages(double delta_time, bool recursive)
	{
		m_MessageManager->Update(delta_time);
		if(recursive)
		{
			//Create copy before update
			IComponentContainer::ComponentContainerVector cc_vec_copy = m_ComponentContainerVector;
			IComponentContainer::ComponentContainerVector::iterator go_iter;
			for(go_iter = cc_vec_copy.begin(); go_iter != cc_vec_copy.end(); ++go_iter)
			{
				SceneObjectPtr child = boost::shared_static_cast<SceneObject>( *go_iter);
				child->SyncMessages(delta_time);
			}

			/*IComponentContainer::ComponentContainerIterator cc_iter = GetChildren();
			while(cc_iter.hasMoreElements())
			{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(cc_iter.getNext());
			child->SyncMessages(delta_time);
			}*/
		}
	}

	void SceneObject::GetComponentsByClass(ComponentVector &components, const std::string &class_name, bool recursive)
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

		if(recursive)
		{
			IComponentContainer::ComponentContainerIterator cc_iter = GetChildren();
			while(cc_iter.hasMoreElements())
			{
				SceneObjectPtr child = boost::shared_static_cast<SceneObject>(cc_iter.getNext());
				child->GetComponentsByClass(components, class_name);
			}
		}
	}

	ComponentPtr SceneObject::GetFirstComponentByClass(const std::string &class_name, bool recursive) 
	{
		//Check all components
		IComponentContainer::ComponentIterator comp_iter = GetComponents();
		while(comp_iter.hasMoreElements())
		{
			BaseSceneComponentPtr comp = boost::shared_static_cast<BaseSceneComponent>(comp_iter.getNext());
			if(comp->GetRTTI()->IsDerivedFrom(class_name))
			{
				return comp;
			}
		}
		if(recursive)
		{
			IComponentContainer::ComponentContainerIterator cc_iter = GetChildren();
			while(cc_iter.hasMoreElements())
			{
				SceneObjectPtr child = boost::shared_static_cast<SceneObject>(cc_iter.getNext());
				ComponentPtr res = child->GetFirstComponentByClass(class_name,recursive);
				if(res)
					return res;
			}
		}
		return ComponentPtr();
	}

	/*SceneObjectVector SceneObject::GetObjectsByName(const std::string &name, bool exact_math)
	{
	SceneObjectVector objects;
	GetObjectsByName(objects, name,exact_math);
	return objects;
	}*/

	void SceneObject::GetChildrenByName(SceneObjectVector &objects, const std::string &name, bool exact_math, bool recursive)
	{
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
			std::string::size_type pos = GetName().find(name);
			if(pos != std::string::npos)
			{
				SceneObjectPtr obj = boost::shared_static_cast<SceneObject>(shared_from_this());
				objects.push_back(obj);
			}
		}

		if(recursive)
		{
			ComponentContainerVector::const_iterator iter =  m_ComponentContainerVector.begin();
			while(iter != m_ComponentContainerVector.end())
			{
				SceneObjectPtr child = boost::shared_static_cast<SceneObject>(*iter);
				child->GetChildrenByName(objects,name,exact_math,recursive);
				iter++;
			}
			/*IComponentContainer::ComponentContainerIterator children = GetChildren();
			while(children.hasMoreElements())
			{
				SceneObjectPtr child = boost::shared_static_cast<SceneObject>(children.getNext());
				child->GetChildrenByName(objects,name,exact_math,recursive);
			}*/
		}
	}

	SceneObjectPtr SceneObject::GetFirstChildByName(const std::string &name, bool exact_math, bool recursive) const
	{
		ComponentContainerVector::const_iterator iter =  m_ComponentContainerVector.begin();
		while(iter != m_ComponentContainerVector.end())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(*iter);
			iter++;
			if(exact_math)
			{
				if(child->GetName() == name)
				{
					return child;
				}
			}
			else
			{
				std::string::size_type pos = child->GetName().find(name);
				if(pos != std::string::npos)
				{
					return child;
				}
			}
		}
		if(recursive)
		{
			ComponentContainerVector::const_iterator iter =  m_ComponentContainerVector.begin();
			
			while(iter != m_ComponentContainerVector.end())
			{
				SceneObjectPtr child = boost::shared_static_cast<SceneObject>(*iter);
				SceneObjectPtr ret = child->GetFirstChildByName(name,exact_math,recursive);
				if(ret)
					return ret;
				iter++;
			}
		}
		return SceneObjectPtr();
	}

	SceneObjectPtr SceneObject::GetChildByID(const SceneObjectID &id) const
	{
		ComponentContainerVector::const_iterator iter =  m_ComponentContainerVector.begin();
		while(iter != m_ComponentContainerVector.end())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(*iter);
			iter++;
			if(child->GetID() == id)
			{
				return child;
			}
		}
 		
		iter =  m_ComponentContainerVector.begin();
		while(iter != m_ComponentContainerVector.end())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(*iter);
			iter++;
			SceneObjectPtr ret = child->GetChildByID(id);
			if(ret)
				return ret;
		}
		return SceneObjectPtr();
	}

	int SceneObject::RegisterForMessage( const MessageType &type, MessageFuncPtr callback, int priority )
	{
		return m_MessageManager->RegisterForMessage(type, callback, priority);
	}

	void SceneObject::UnregisterForMessage(const MessageType &type, MessageFuncPtr callback)
	{
		m_MessageManager->UnregisterForMessage(type, callback);
	}

	void SceneObject::PostMessage( MessagePtr message )
	{
		m_MessageManager->PostMessage(message);
	}

	void SceneObject::SendImmediate( MessagePtr message )
	{
		m_MessageManager->SendImmediate(message);
	}

	void SceneObject::OnChangeName(SceneObjectNameMessagePtr message)
	{
		std::string name = message->GetName();
		SetName(name);
	}
}




