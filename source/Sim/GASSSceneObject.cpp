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

#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneObjectVisitors.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Core/Common.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentContainer.h"
#include "Core/ComponentSystem/GASSComponentContainerFactory.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplateManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSException.h"
#include <iomanip>
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{
	SceneObject::SceneObject() : m_MessageManager(new MessageManager()),
		m_Initialized(false)
	{
		m_GUID = GASS_GUID_NULL;
	}

	void SceneObject::RegisterReflection()
	{
		ComponentContainerFactory::Get().Register<SceneObject>("SceneObject");
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("Container for all components", OF_VISIBLE)));
		RegisterGetSet("ID", &GASS::SceneObject::GetID, &GASS::SceneObject::SetID,PF_VISIBLE,"Local IDentifier string");
		RegisterGetSet("GUID", &GASS::SceneObject::GetGUID, &GASS::SceneObject::SetGUID);
	}

	SceneObjectPtr SceneObject::CreateCopy(bool copy_children_recursively) const
	{
		SceneObjectPtr copy = _CreateCopyRec(copy_children_recursively);
		copy->GenerateNewGUID(true);
		return copy;
	}

	SceneObjectPtr SceneObject::_CreateCopyRec(bool copy_children_recursively) const
	{
		// use object factory instead to support derives from scene object?
		SceneObjectPtr new_obj(new  SceneObject());
		//set object properties
		CopyPropertiesTo(new_obj);
		
		//copy components
		ComponentContainer::ConstComponentIterator comp_iter = GetComponents();
		while(comp_iter.hasMoreElements())
		{
			ComponentPtr comp = comp_iter.getNext();
			if(comp)
			{
				ComponentPtr template_comp = comp->CreateCopy();
				new_obj->AddComponent(template_comp);
			}
		}

		//copy children
		ComponentContainer::ConstComponentContainerIterator children = GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child = GASS_DYNAMIC_PTR_CAST<SceneObject>(children.getNext());
			SceneObjectPtr new_child = child->_CreateCopyRec(copy_children_recursively);
			new_obj->AddChild(new_child);
		}
		return new_obj;
	}

	void SceneObject::GenerateNewGUID(bool recursively)
	{
		std::map<SceneObjectGUID,SceneObjectGUID> ref_map;
		//save old guid:s first
		_GenerateNewGUIDRec(ref_map, recursively);

		//remap GUID refs
		_RemapRefRec(ref_map);
	}

	void SceneObject::_GenerateNewGUIDRec(std::map<SceneObjectGUID,SceneObjectGUID> &ref_map, bool recursively)
	{
		//save old guid:s first
		SceneObjectGUID new_guid = GASS_GUID_GENERATE;
		ref_map[m_GUID] = new_guid;
		m_GUID = new_guid;

		if(recursively)
		{
			ComponentContainer::ComponentContainerIterator children = GetChildren();
			while(children.hasMoreElements())
			{
				SceneObjectPtr child = GASS_DYNAMIC_PTR_CAST<SceneObject>(children.getNext());
				child->_GenerateNewGUIDRec(ref_map,recursively);
			}
		}
	}

	void SceneObject::_RemapRefRec(std::map<SceneObjectGUID,SceneObjectGUID> &ref_map)
	{
		//remap references
		ComponentContainer::ComponentIterator comp_iter = GetComponents();
		while(comp_iter.hasMoreElements())
		{
			BaseSceneComponentPtr comp = GASS_STATIC_PTR_CAST<BaseSceneComponent>(comp_iter.getNext());
			if(comp)
			{
				comp->RemapReferences(ref_map);
			}
		}
		ComponentContainer::ComponentContainerIterator children = GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child = GASS_DYNAMIC_PTR_CAST<SceneObject>(children.getNext());
			child->_RemapRefRec(ref_map);
		}
	}

	void SceneObject::AddChildSceneObject(SceneObjectPtr child , bool load)
	{
		child->_InitializePointers(); //initialize SceneObjectLink:s

		ComponentContainer::AddChild(child);

		if(load && GetScene()) //if we have scene Initialize?
			child->OnInitialize(GetScene());
	}

	void SceneObject::InsertChildSceneObject(SceneObjectPtr child, size_t index, bool load)
	{
		child->_InitializePointers(); //initialize SceneObjectLink:s

		ComponentContainer::InsertChild(child,index);

		if (load && GetScene()) //if we have scene Initialize?
			child->OnInitialize(GetScene());
	}

	void SceneObject::ResolveTemplateReferences(SceneObjectPtr template_root)
	{
		ComponentVector::iterator iter = m_ComponentVector.begin();
		while (iter != m_ComponentVector.end())
		{
			BaseSceneComponentPtr bsc = GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(*iter);
			bsc->ResolveTemplateReferences(template_root);
			++iter;
		}
		ComponentContainer::ComponentContainerIterator children = GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(children.getNext());
			child->ResolveTemplateReferences(template_root);
		}
	}

	void SceneObject::GenerateGUID(bool recursive)
	{
		if(m_GUID.is_nil())
			m_GUID = GASS_GUID_GENERATE;
		if(recursive)
		{
			ComponentContainer::ComponentContainerIterator children = GetChildren();
			while(children.hasMoreElements())
			{
				SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(children.getNext());
				child->GenerateGUID(recursive);
			}
		}
	}

	void SceneObject::_InitializePointers()
	{
		ComponentVector::iterator iter = m_ComponentVector.begin();
		while (iter != m_ComponentVector.end())
		{
			BaseSceneComponentPtr bsc = GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(*iter);
			bsc->InitializePointers();
			++iter;
		}
		ComponentContainer::ComponentContainerIterator children = GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(children.getNext());
			child->_InitializePointers();
		}
	}

	//Override
	void SceneObject::RemoveChildSceneObject(SceneObjectPtr child)
	{
		//notify that this objects and its children will be removed
		child->OnDelete();
		ComponentContainer::RemoveChild(child);
	}

	void SceneObject::RemoveAllChildrenNotify()
	{
		ComponentContainer::ComponentContainerIterator children = GetChildren();
		while (children.hasMoreElements())
		{
			SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(children.getNext());
			child->OnDelete();
		}
		ComponentContainer::RemoveAllChildren();
	}
	
	void SceneObject::SendRemoveRequest(float delay)
	{
		SceneObjectPtr obj = GASS_STATIC_PTR_CAST<SceneObject>(shared_from_this());
		SceneMessagePtr remove_msg(new RemoveSceneObjectRequest(obj));
		remove_msg->SetDeliverDelay(delay);
		GetScene()->PostMessage(remove_msg);		
	}

	void SceneObject::OnDelete()
	{
		ComponentContainer::ComponentContainerIterator children = GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(children.getNext());
			child->OnDelete();
		}

		ComponentVector::iterator iter = m_ComponentVector.begin();
		while (iter != m_ComponentVector.end())
		{
			BaseSceneComponentPtr bsc = GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(*iter);

			bsc->OnDelete();
			++iter;
		}
		
		SceneObjectPtr this_obj = GASS_STATIC_PTR_CAST<SceneObject>(shared_from_this());
		MessagePtr unload_msg(new SceneObjectRemovedEvent(this_obj));
		GetScene()->m_SceneMessageManager->SendImmediate(unload_msg);
	}

	void SceneObject::OnInitialize(ScenePtr scene)
	{
		//check dependencies
		_CheckComponentDependencies();

		if(m_GUID.is_nil())
			m_GUID = GASS_GUID_GENERATE;
		m_Scene = scene;

		m_Name = GetScene()->GetNameGenerator().CreateUniqueName(m_Name);

		SceneObjectPtr this_obj = GASS_STATIC_PTR_CAST<SceneObject>(shared_from_this());
		MessagePtr pre_load_msg(new PreSceneObjectInitializedEvent(this_obj));
		GetScene()->m_SceneMessageManager->SendImmediate(pre_load_msg);

		ComponentVector::iterator iter = m_ComponentVector.begin();
		while (iter != m_ComponentVector.end())
		{
			BaseSceneComponentPtr bsc = GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(*iter);
			bsc->OnInitialize();
			++iter;
		}
		MessagePtr load_msg(new PostComponentsInitializedEvent(this_obj));
		GetScene()->m_SceneMessageManager->SendImmediate(load_msg);
		//Pump message system, some components may use PostMessage instead of SendImmediate, 
		//a reason for this could be to make use of the message listener priority flag 
		//to take control of initialization order
		SyncMessages(0,false);

		ComponentContainer::ComponentContainerIterator children = GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(children.getNext());
			child->OnInitialize(scene);
		}

		MessagePtr post_load_msg(new PostSceneObjectInitializedEvent(this_obj));
		GetScene()->m_SceneMessageManager->SendImmediate(post_load_msg);
		m_MessageManager->PostMessage(PostInitializedEventPtr(new PostInitializedEvent()));
		m_Initialized = true;
	}

	SceneObjectPtr SceneObject::GetObjectUnderRoot() 
	{
		ComponentContainerPtr container = shared_from_this();

		SceneObjectPtr root = GetScene()->GetRootSceneObject();

		while(container->GetParent() && ComponentContainerPtr(container->GetParent()) != root)
		{
			container = ComponentContainerPtr(container->GetParent());
		}
		return  GASS_STATIC_PTR_CAST<SceneObject>(container);
	}

	void SceneObject::SyncMessages(double delta_time, bool recursive) const
	{
		m_MessageManager->Update(delta_time);
		if(recursive)
		{
			ComponentContainer::ConstComponentContainerIterator cc_iter = GetChildren();
			while(cc_iter.hasMoreElements())
			{
				SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(cc_iter.getNext());
				child->SyncMessages(delta_time);
			}
		}
	}

	size_t  SceneObject::GetQueuedMessages() const
	{
		size_t num = m_MessageManager->GetQueuedMessages();

		ComponentContainer::ConstComponentContainerIterator cc_iter = GetChildren();
		while(cc_iter.hasMoreElements())
		{
			SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(cc_iter.getNext());
			num += child->GetQueuedMessages();
		}
		return num;
	}

	void SceneObject::ClearMessages() const
	{
		m_MessageManager->Clear();
	}

	void SceneObject::GetComponentsByClassName(ComponentVector &components, const std::string &class_name, bool recursive) const
	{
		//Check all components
		ComponentContainer::ConstComponentIterator comp_iter = GetComponents();
		while(comp_iter.hasMoreElements())
		{
			BaseSceneComponentPtr comp = GASS_STATIC_PTR_CAST<BaseSceneComponent>(comp_iter.getNext());
			if(comp->GetRTTI()->IsDerivedFrom(class_name))
			{
				components.push_back(comp);
			}
		}

		if(recursive)
		{
			ComponentContainer::ConstComponentContainerIterator cc_iter = GetChildren();
			while(cc_iter.hasMoreElements())
			{
				SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(cc_iter.getNext());
				child->GetComponentsByClassName(components, class_name);
			}
		}
	}

	ComponentPtr SceneObject::GetFirstComponentByClassName(const std::string &class_name, bool recursive) const 
	{
		//Check all components
		ComponentContainer::ConstComponentIterator comp_iter = GetComponents();
		while(comp_iter.hasMoreElements())
		{
			BaseSceneComponentPtr comp = GASS_STATIC_PTR_CAST<BaseSceneComponent>(comp_iter.getNext());
			if(comp->GetRTTI()->IsDerivedFrom(class_name))
			{
				return comp;
			}
		}
		if(recursive)
		{
			ComponentContainer::ConstComponentContainerIterator cc_iter = GetChildren();
			while(cc_iter.hasMoreElements())
			{
				SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(cc_iter.getNext());
				ComponentPtr res = child->GetFirstComponentByClassName(class_name,recursive);
				if(res)
					return res;
			}
		}
		return ComponentPtr();
	}


	bool SceneObject::Accept(SceneObjectVisitorPtr visitor)
	{
		if(!visitor->Visit(GASS_DYNAMIC_PTR_CAST<SceneObject>(shared_from_this())))
			return false;
		ComponentContainer::ComponentContainerIterator cc_iter = GetChildren();
		while(cc_iter.hasMoreElements())
		{
			SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(cc_iter.getNext());
			if(!child->Accept(visitor))
				return false;
		}
		return true;
	}

	SceneObjectPtr SceneObject::GetChildByGUID(const SceneObjectGUID &guid) const
	{
		ComponentContainerVector::const_iterator comp_iter =  m_ComponentContainerVector.begin();
		while(comp_iter != m_ComponentContainerVector.end())
		{
			SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(*comp_iter);
			++comp_iter;
			if(child->GetGUID() == guid)
			{
				return child;
			}
		}
		ComponentContainerVector::const_iterator iter =  m_ComponentContainerVector.begin();
		while(iter != m_ComponentContainerVector.end())
		{
			SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(*iter);
			SceneObjectPtr ret = child->GetChildByGUID(guid);
			if(ret)
				return ret;
			++iter;
		}
		return SceneObjectPtr();
	}

	void SceneObject::GetChildrenByName(SceneObjectVector &objects, const std::string &name, bool exact_math, bool recursive) const
	{
		if(recursive)
		{
			ComponentContainerVector::const_iterator iter =  m_ComponentContainerVector.begin();
			while(iter != m_ComponentContainerVector.end())
			{
				SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(*iter);

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
				++iter;
			}
		}
	}

	SceneObjectPtr SceneObject::GetFirstChildByName(const std::string &name, bool exact_math, bool recursive) const
	{
		ComponentContainerVector::const_iterator iter =  m_ComponentContainerVector.begin();
		while(iter != m_ComponentContainerVector.end())
		{
			SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(*iter);
			++iter;
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
			ComponentContainerVector::const_iterator cc_iter =  m_ComponentContainerVector.begin();

			while(cc_iter  != m_ComponentContainerVector.end())
			{
				SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(*cc_iter );
				SceneObjectPtr ret = child->GetFirstChildByName(name,exact_math,recursive);
				if(ret)
					return ret;
				++cc_iter;
			}
		}
		return SceneObjectPtr();
	}

	SceneObjectPtr SceneObject::GetChildByID(const SceneObjectID &id) const
	{
		ComponentContainerVector::const_iterator iter =  m_ComponentContainerVector.begin();
		while(iter != m_ComponentContainerVector.end())
		{
			SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(*iter);
			++iter;
			if(child->GetID() == id)
			{
				return child;
			}
		}

		iter =  m_ComponentContainerVector.begin();
		while(iter != m_ComponentContainerVector.end())
		{
			SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(*iter);
			++iter;
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
				SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(*iter);

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
				++iter;
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

	void SceneObject::PostEvent(SceneObjectEventMessagePtr message)
	{
		m_MessageManager->PostMessage(message);
	}

	void SceneObject::SendImmediateEvent(SceneObjectEventMessagePtr message )
	{
		m_MessageManager->SendImmediate(message);
	}

	void SceneObject::PostRequest(SceneObjectRequestMessagePtr message)
	{
		m_MessageManager->PostMessage(message);
	}

	void SceneObject::SendImmediateRequest(SceneObjectRequestMessagePtr message )
	{
		m_MessageManager->SendImmediate(message);
	}
	
	bool SceneObject::IsInitialized() const
	{
		return m_Initialized;
	}

	void SceneObject::SetName(const std::string &name)
	{ 
		m_Name = name;
		
		if (IsInitialized())
		{
			GetScene()->GetNameGenerator().CreateUniqueName(m_Name);
			SendImmediateEvent(GASS_MAKE_SHARED<SceneObjectNameChangedEvent>(name));
		}
	}

	SceneObjectPtr SceneObject::LoadFromXML(tinyxml2::XMLDocument *xmlDoc)
	{
		tinyxml2::XMLElement *so_elem = xmlDoc->FirstChildElement("SceneObject");

		SceneObjectPtr so;
		if (so_elem->Attribute("from_template"))
		{
			std::string template_name = so_elem->Attribute("from_template");
			so = GASS_STATIC_PTR_CAST<SceneObject>(SimEngine::Get().GetSceneObjectTemplateManager()->CreateFromTemplate(template_name));
			so->RemoveAllChildren();
		}
		else
		{
			const std::string cc_name = so_elem->Value();
			so = GASS_STATIC_PTR_CAST<SceneObject>(ComponentContainerFactory::Get().Create(cc_name));
		}

		if (!so_elem)
		{
			std::string file_name = xmlDoc->GetFileName();
			delete xmlDoc;
			//Fatal error, cannot load

			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE, "Can't find SceneObject tag in: " + file_name, "SceneObject::LoadFromXML");
		}
		so->LoadXML(so_elem);
		return so;
	}

	SceneObjectPtr SceneObject::LoadFromXML(const std::string &filename)
	{
		if(filename =="") 
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS,"No filename provided", "SceneObject::LoadFromFile");

		tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
		if(xmlDoc->LoadFile(filename.c_str()) != tinyxml2::XML_NO_ERROR)
		{
			delete xmlDoc;
			//Fatal error, cannot load
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load: " +  filename, "SceneObject::LoadXML");
		}
		SceneObjectPtr so = LoadFromXML(xmlDoc);
		xmlDoc->Clear();
		//Delete our allocated document and return success ;)
		delete xmlDoc;
		return so;
	}

	void SceneObject::SaveToFile(const std::string &filename)
	{
		if(filename =="") 
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS,"No filename provided", "SceneObject::SaveToFile");

		tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
		tinyxml2::XMLDeclaration* decl = xmlDoc->NewDeclaration();
		xmlDoc->LinkEndChild( decl ); 

		//first save to store filename
		xmlDoc->SaveFile(filename.c_str());
		
		tinyxml2::XMLElement * so_elem = xmlDoc->NewElement("SceneObject");
		xmlDoc->LinkEndChild(so_elem);
		SaveXML(so_elem);
		xmlDoc->SaveFile(filename.c_str());
		delete xmlDoc;
	}

	BaseSceneComponentPtr SceneObject::GetBaseSceneComponent(const std::string &comp_name) const
	{
		return GASS_DYNAMIC_PTR_CAST<GASS::BaseSceneComponent>(GetComponent(comp_name));
	}

	BaseSceneComponent* SceneObject::GetComponentByClassName(const std::string &comp_name) const
	{
		const std::string factory_class_name = ComponentFactory::Get().GetClassNameFromKey(comp_name);
		GASS::BaseSceneComponentPtr bsc = GASS_DYNAMIC_PTR_CAST<GASS::BaseSceneComponent>(GetFirstComponentByClassName(factory_class_name,false));
		return bsc.get();
	}

	SceneObject* SceneObject::GetSceneObjectByName(const std::string &name) const
	{
		SceneObjectPtr so = GetScene()->GetRootSceneObject()->GetFirstChildByName(name,true, true);
		return so.get();
	}

	SceneObject* SceneObject::GetSceneObjectByID(const std::string &id) const
	{
		SceneObjectPtr so = GetScene()->GetRootSceneObject()->GetChildByID(id);
		return so.get();
	}

	ComponentContainerPtr SceneObject::CreateComponentContainerXML(tinyxml2::XMLElement *cc_elem) const
	{
		ComponentContainerPtr cc;
		if(cc_elem->Attribute("from_template"))
		{
			std::string template_name = cc_elem->Attribute("from_template");
			cc = GASS_STATIC_PTR_CAST<ComponentContainer>(SimEngine::Get().GetSceneObjectTemplateManager()->CreateFromTemplate(template_name));
			//remove all children, they should be created by template, or?
			cc->RemoveAllChildren();
		}
		else
		{
			const std::string cc_name = cc_elem->Value();
			cc = ComponentContainerFactory::Get().Create(cc_name);
		}
		return cc;
	}
}