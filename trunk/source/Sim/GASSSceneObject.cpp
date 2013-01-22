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
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSSimSystemManager.h"
#include "Core/Common.h"
#include "Core/Serialize/GASSSerialize.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSIComponentContainer.h"
#include "Core/ComponentSystem/GASSComponentContainerFactory.h"
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSException.h"
#include <iostream>
#include <iomanip>
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include <tinyxml.h>

namespace GASS
{
	SceneObject::SceneObject() : m_MessageManager (new MessageManager())
	{

	}

	SceneObject::~SceneObject(void)
	{
		
	}

	void SceneObject::RegisterReflection()
	{
		ComponentContainerFactory::GetPtr()->Register("SceneObject",new Creator<SceneObject, IComponentContainer>);
		RegisterProperty<SceneObjectID>("ID", &GASS::SceneObject::GetID, &GASS::SceneObject::SetID);
	}

	void SceneObject::AddChildSceneObject(SceneObjectPtr child , bool load)
	{
		child->InitializePointers();

		BaseComponentContainer::AddChild(child);
		if(load && GetScene()) //if we have scene Initialize?
			child->Initialize(GetScene());
	}


	void SceneObject::InitializePointers()
	{
		ComponentVector::iterator iter = m_ComponentVector.begin();
		while (iter != m_ComponentVector.end())
		{
			BaseSceneComponentPtr bsc = DYNAMIC_CAST<BaseSceneComponent>(*iter);
			bsc->InitializePointers();
			++iter;
		}
		IComponentContainer::ComponentContainerIterator children = GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child = STATIC_CAST<SceneObject>(children.getNext());
			child->InitializePointers();
		}
	}

	//Override
	void SceneObject::RemoveChildSceneObject(SceneObjectPtr child)
	{
		//notify that this objects and its children will be removed
		child->OnDelete();
		BaseComponentContainer::RemoveChild(child);
	}

	void SceneObject::OnDelete()
	{
		BaseComponentContainer::ComponentContainerIterator children = GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child = STATIC_CAST<SceneObject>(children.getNext());
			child->OnDelete();
		}

		ComponentVector::iterator iter = m_ComponentVector.begin();
		while (iter != m_ComponentVector.end())
		{
			BaseSceneComponentPtr bsc = DYNAMIC_CAST<BaseSceneComponent>(*iter);
			
			bsc->OnDelete();
			++iter;
		}

		//MessagePtr msg(new UnloadComponentsMessage());
		//SendImmediate(msg);

		SceneObjectPtr this_obj = STATIC_CAST<SceneObject>(shared_from_this());
		MessagePtr unload_msg(new SceneObjectRemovedEvent(this_obj));
		GetScene()->m_SceneMessageManager->SendImmediate(unload_msg);
	}

	void SceneObject::Initialize(ScenePtr scene)
	{
		m_Scene = scene;

		SceneObjectPtr this_obj = STATIC_CAST<SceneObject>(shared_from_this());
		MessagePtr pre_load_msg(new PreSceneObjectInitializedEvent(this_obj));
		GetScene()->m_SceneMessageManager->SendImmediate(pre_load_msg);
	
		

		RegisterForMessage(REG_TMESS(SceneObject::OnChangeName,SceneObjectNameMessage,0));
		//only initilize components, let each child be initilize manually
		ComponentVector::iterator iter = m_ComponentVector.begin();
		while (iter != m_ComponentVector.end())
		{
			BaseSceneComponentPtr bsc = DYNAMIC_CAST<BaseSceneComponent>(*iter);
			bsc->OnInitialize();
			++iter;
		}
		MessagePtr load_msg(new PostComponentsInitializedEvent(this_obj));
		GetScene()->m_SceneMessageManager->SendImmediate(load_msg);

		//SendImmediate(MessagePtr(new LoadComponentsMessage()));
		//Pump initial messages
		SyncMessages(0,false);

		IComponentContainer::ComponentContainerIterator children = GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child = STATIC_CAST<SceneObject>(children.getNext());
			child->Initialize(scene);
		}

		MessagePtr post_load_msg(new PostSceneObjectInitializedEvent(this_obj));
		GetScene()->m_SceneMessageManager->SendImmediate(post_load_msg);
	}

	SceneObjectPtr SceneObject::GetObjectUnderRoot()
	{
		 ComponentContainerPtr container = shared_from_this();

		SceneObjectPtr root = GetScene()->GetRootSceneObject();

		while(container->GetParent() && ComponentContainerPtr(container->GetParent()) != root)
		{
			container = ComponentContainerPtr(container->GetParent());
		}
		return  STATIC_CAST<SceneObject>(container);
	}

	struct MessageSyncExecutor
	{
		MessageSyncExecutor(const BaseComponentContainer::ComponentContainerVector& cc_vector, double delta_time)
			:m_CCVector(cc_vector),m_DeltaTime(delta_time)
		{}
		MessageSyncExecutor(MessageSyncExecutor& e,tbb::split)
			:m_CCVector(e.m_CCVector)
		{}

		void operator()(const tbb::blocked_range<size_t>& r) const {
			for (size_t i=r.begin();i!=r.end();++i)
			{
				SceneObjectPtr obj = STATIC_CAST<SceneObject>(m_CCVector[i]);
				obj->SyncMessages(m_DeltaTime);
			}
		}
		const BaseComponentContainer::ComponentContainerVector& m_CCVector;
		double m_DeltaTime;
	};

	void SceneObject::SyncMessages(double delta_time, bool recursive) const
	{
		m_MessageManager->Update(delta_time);
		if(recursive)
		{
			//Create copy before update
			/*IComponentContainer::ComponentContainerVector cc_vec_copy = m_ComponentContainerVector;
			IComponentContainer::ComponentContainerVector::const_iterator go_iter;
			for(go_iter = cc_vec_copy.begin(); go_iter != cc_vec_copy.end(); ++go_iter)
			{
				SceneObjectPtr child = STATIC_CAST<SceneObject>( *go_iter);
				child->SyncMessages(delta_time);
			}*/
			
			
			IComponentContainer::ConstComponentContainerIterator cc_iter = GetChildren();
			while(cc_iter.hasMoreElements())
			{
				SceneObjectPtr child = STATIC_CAST<SceneObject>(cc_iter.getNext());
				child->SyncMessages(delta_time);
			}

			//parallel update, problem with set world position that have to use parent transforms
			//MessageSyncExecutor exec(m_ComponentContainerVector,delta_time);
			//tbb::parallel_for(tbb::blocked_range<size_t>(0,m_ComponentContainerVector.size()),exec);
		}
	}

	size_t  SceneObject::GetQueuedMessages() const
	{
		size_t num = m_MessageManager->GetQueuedMessages();
		
		IComponentContainer::ConstComponentContainerIterator cc_iter = GetChildren();
		while(cc_iter.hasMoreElements())
		{
			SceneObjectPtr child = STATIC_CAST<SceneObject>(cc_iter.getNext());
			num += child->GetQueuedMessages();
		}
		return num;
	}

	void SceneObject::GetComponentsByClass(ComponentVector &components, const std::string &class_name, bool recursive)
	{
		//Check all components
		IComponentContainer::ComponentIterator comp_iter = GetComponents();
		while(comp_iter.hasMoreElements())
		{
			BaseSceneComponentPtr comp = STATIC_CAST<BaseSceneComponent>(comp_iter.getNext());
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
				SceneObjectPtr child = STATIC_CAST<SceneObject>(cc_iter.getNext());
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
			BaseSceneComponentPtr comp = STATIC_CAST<BaseSceneComponent>(comp_iter.getNext());
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
				SceneObjectPtr child = STATIC_CAST<SceneObject>(cc_iter.getNext());
				ComponentPtr res = child->GetFirstComponentByClass(class_name,recursive);
				if(res)
					return res;
			}
		}
		return ComponentPtr();
	}

	void SceneObject::GetChildrenByName(SceneObjectVector &objects, const std::string &name, bool exact_math, bool recursive) const
	{
		if(recursive)
		{
			ComponentContainerVector::const_iterator iter =  m_ComponentContainerVector.begin();
			while(iter != m_ComponentContainerVector.end())
			{
				SceneObjectPtr child = STATIC_CAST<SceneObject>(*iter);

				if(exact_math)
				{
					if(child->GetName()== name)
					{
						objects.push_back(child);
					}
				}
				else
				{
					std::string::size_type pos = child->GetName().find(name);
					if(pos != std::string::npos)
					{
						objects.push_back(child);
					}
				}
				if(recursive)
					child->GetChildrenByName(objects,name,exact_math,recursive);
				iter++;
			}
		}
	}

	SceneObjectPtr SceneObject::GetFirstChildByName(const std::string &name, bool exact_math, bool recursive) const
	{
		ComponentContainerVector::const_iterator iter =  m_ComponentContainerVector.begin();
		while(iter != m_ComponentContainerVector.end())
		{
			SceneObjectPtr child = STATIC_CAST<SceneObject>(*iter);
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
				SceneObjectPtr child = STATIC_CAST<SceneObject>(*iter);
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
			SceneObjectPtr child = STATIC_CAST<SceneObject>(*iter);
			iter++;
			if(child->GetID() == id)
			{
				return child;
			}
		}
 		
		iter =  m_ComponentContainerVector.begin();
		while(iter != m_ComponentContainerVector.end())
		{
			SceneObjectPtr child = STATIC_CAST<SceneObject>(*iter);
			iter++;
			SceneObjectPtr ret = child->GetChildByID(id);
			if(ret)
				return ret;
		}
		return SceneObjectPtr();
	}


	void SceneObject::GetChildrenByID(SceneObjectVector &objects, const SceneObjectID &id, bool exact_math, bool recursive) const
	{
		if(recursive)
		{
			ComponentContainerVector::const_iterator iter =  m_ComponentContainerVector.begin();
			while(iter != m_ComponentContainerVector.end())
			{
				SceneObjectPtr child = STATIC_CAST<SceneObject>(*iter);

				if(exact_math)
				{
					if(child->GetID()== id)
					{
						objects.push_back(child);
					}
				}
				else
				{
					std::string::size_type pos = child->GetID().find(id);
					if(pos != std::string::npos)
					{
						objects.push_back(child);
					}
				}
				if(recursive)
					child->GetChildrenByID(objects,id,exact_math,recursive);
				iter++;
			}
		}
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

	void SceneObject::LoadFromFile(const std::string &filename)
	{
		if(filename =="") 
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS,"No filename provided", "SceneObject::LoadFromFile");
		
		TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());
		if(!xmlDoc->LoadFile())
		{
			//Fatal error, cannot load
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load: " +  filename, "SceneObject::LoadXML");
		}
		TiXmlElement *so_elem = xmlDoc->FirstChildElement("SceneObject");
		
		if(!so_elem)
		{
			//Fatal error, cannot load
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"cant find SceneObject tag in: " + filename , "SceneObject::LoadXML");
		}

		LoadXML(so_elem);
		xmlDoc->Clear();
		//Delete our allocated document and return success ;)
		delete xmlDoc;
	}



	void SceneObject::SaveToFile(const std::string &filename)
	{
		if(filename =="") 
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS,"No filename provided", "SceneObject::SaveToFile");

		TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());
		TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
		xmlDoc->LinkEndChild( decl ); 

		//TiXmlElement * so_elem = new TiXmlElement("SceneObject");  
		//doc.LinkEndChild( so_elem); 
		SaveXML((TiXmlElement*) (xmlDoc));
		xmlDoc->SaveFile(filename.c_str());
		delete xmlDoc;
	}



	ComponentContainerPtr SceneObject::CreateComponentContainer(TiXmlElement *cc_elem) const
	{
		
		ComponentContainerPtr cc;
		if(cc_elem->Attribute("from_template"))
		{
			std::string template_name = cc_elem->Attribute("from_template");
			cc = STATIC_CAST<IComponentContainer>(SimEngine::Get().GetSceneObjectTemplateManager()->CreateFromTemplate(template_name));
		}
		else
		{
			const std::string cc_name = cc_elem->Value();
			cc = STATIC_CAST<IComponentContainer>(ComponentContainerFactory::Get().Create(cc_name));
		}
		return cc;
	}
}




