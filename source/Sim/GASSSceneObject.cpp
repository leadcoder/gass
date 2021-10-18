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
#include "Sim/GASSComponent.h"
#include "Sim/GASSComponentFactory.h"
#include "GASSSceneObjectTemplateManager.h"
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
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("Container for all components", OF_VISIBLE)));
		RegisterGetSet("Name", &GASS::SceneObject::GetName, &GASS::SceneObject::SetName, PF_VISIBLE | PF_EDITABLE, "Object Name");
		RegisterGetSet("TemplateName", &GASS::SceneObject::GetTemplateName, &GASS::SceneObject::SetTemplateName);
		RegisterGetSet("Serialize", &GASS::SceneObject::GetSerialize, &GASS::SceneObject::SetSerialize);
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
		ConstComponentIterator comp_iter = GetComponents();
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
		auto children = GetChildren();
		while(children.hasMoreElements())
		{
			auto child = children.getNext();
			auto new_child = child->_CreateCopyRec(copy_children_recursively);
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
			auto children = GetChildren();
			while(children.hasMoreElements())
			{
				auto child = children.getNext();
				child->_GenerateNewGUIDRec(ref_map,recursively);
			}
		}
	}

	void SceneObject::_RemapRefRec(std::map<SceneObjectGUID,SceneObjectGUID> &ref_map)
	{
		//remap references
		ComponentIterator comp_iter = GetComponents();
		while(comp_iter.hasMoreElements())
		{
			BaseSceneComponentPtr comp = GASS_STATIC_PTR_CAST<BaseSceneComponent>(comp_iter.getNext());
			if(comp)
			{
				comp->RemapReferences(ref_map);
			}
		}
		auto children = GetChildren();
		while(children.hasMoreElements())
		{
			auto child = children.getNext();
			child->_RemapRefRec(ref_map);
		}
	}

	void SceneObject::AddChildSceneObject(SceneObjectPtr child , bool load)
	{
		child->_InitializePointers(); //initialize SceneObjectLink:s

		AddChild(child);

		if(load && GetScene()) //if we have scene Initialize?
			child->OnInitialize(GetScene());
	}

	void SceneObject::InsertChildSceneObject(SceneObjectPtr child, size_t index, bool load)
	{
		child->_InitializePointers(); //initialize SceneObjectLink:s
		InsertChild(child,index);
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
		auto children = GetChildren();
		while(children.hasMoreElements())
		{
			auto child = children.getNext();
			child->ResolveTemplateReferences(template_root);
		}
	}

	void SceneObject::GenerateGUID(bool recursive)
	{
		if(m_GUID.is_nil())
			m_GUID = GASS_GUID_GENERATE;
		if(recursive)
		{
			auto children = GetChildren();
			while(children.hasMoreElements())
			{
				auto child = children.getNext();
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
		auto children = GetChildren();
		while(children.hasMoreElements())
		{
			auto child = children.getNext();
			child->_InitializePointers();
		}
	}

	//Override
	void SceneObject::RemoveChildSceneObject(SceneObjectPtr child)
	{
		//notify that this objects and its children will be removed
		child->OnDelete();
		RemoveChild(child);
	}

	void SceneObject::RemoveAllChildrenNotify()
	{
		SceneObjectIterator children = GetChildren();
		while (children.hasMoreElements())
		{
			auto child = children.getNext();
			child->OnDelete();
		}
		RemoveAllChildren();
	}
	
	void SceneObject::SendRemoveRequest(float delay)
	{
		auto obj = shared_from_this();
		SceneMessagePtr remove_msg(new RemoveSceneObjectRequest(obj));
		remove_msg->SetDeliverDelay(delay);
		GetScene()->PostMessage(remove_msg);		
	}

	void SceneObject::OnDelete()
	{
		SceneObjectIterator children = GetChildren();
		while(children.hasMoreElements())
		{
			auto child = children.getNext();
			child->OnDelete();
		}

		auto iter = m_ComponentVector.begin();
		while (iter != m_ComponentVector.end())
		{
			BaseSceneComponentPtr bsc = GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(*iter);
			bsc->OnDelete();
			++iter;
		}
		
		auto this_obj = shared_from_this();
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

		auto this_obj = shared_from_this();
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

		auto children = GetChildren();
		while(children.hasMoreElements())
		{
			auto child = children.getNext();
			child->OnInitialize(scene);
		}

		MessagePtr post_load_msg(new PostSceneObjectInitializedEvent(this_obj));
		GetScene()->m_SceneMessageManager->SendImmediate(post_load_msg);
		m_MessageManager->PostMessage(PostInitializedEventPtr(new PostInitializedEvent()));
		m_Initialized = true;
	}

	SceneObjectPtr SceneObject::GetObjectUnderRoot() 
	{
		SceneObjectPtr so = shared_from_this();

		SceneObjectPtr root = GetScene()->GetRootSceneObject();

		while(so->GetParent() && so->GetParent() != root)
		{
			so = so->GetParent();
		}
		return  so;
	}

	void SceneObject::SyncMessages(double delta_time, bool recursive) const
	{
		m_MessageManager->Update(delta_time);
		if(recursive)
		{
			auto iter = GetChildren();
			while(iter.hasMoreElements())
			{
				auto child = iter.getNext();
				child->SyncMessages(delta_time);
			}
		}
	}

	size_t  SceneObject::GetQueuedMessages() const
	{
		size_t num = m_MessageManager->GetQueuedMessages();

		auto iter = GetChildren();
		while(iter.hasMoreElements())
		{
			SceneObjectPtr child = iter.getNext();
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
		ConstComponentIterator comp_iter = GetComponents();
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
			auto iter = GetChildren();
			while(iter.hasMoreElements())
			{
				auto child = iter.getNext();
				child->GetComponentsByClassName(components, class_name);
			}
		}
	}

	ComponentPtr SceneObject::GetFirstComponentByClassName(const std::string &class_name, bool recursive) const 
	{
		//Check all components
		auto comp_iter = GetComponents();
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
			auto iter = GetChildren();
			while(iter.hasMoreElements())
			{
				auto child = iter.getNext();
				ComponentPtr res = child->GetFirstComponentByClassName(class_name,recursive);
				if(res)
					return res;
			}
		}
		return ComponentPtr();
	}


	bool SceneObject::Accept(SceneObjectVisitorPtr visitor)
	{
		if(!visitor->Visit(shared_from_this()))
			return false;
		auto iter = GetChildren();
		while(iter.hasMoreElements())
		{
			auto child = iter.getNext();
			if(!child->Accept(visitor))
				return false;
		}
		return true;
	}

	SceneObjectPtr SceneObject::GetChildByGUID(const SceneObjectGUID &guid) const
	{
		SceneObjectVector::const_iterator iter =  m_Children.begin();
		while(iter != m_Children.end())
		{
			auto child = *iter;
			++iter;
			if(child->GetGUID() == guid)
			{
				return child;
			}
		}
		
		iter =  m_Children.begin();
		while(iter != m_Children.end())
		{
			auto child = *iter;
			auto ret = child->GetChildByGUID(guid);
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
			SceneObjectVector::const_iterator iter =  m_Children.begin();
			while(iter != m_Children.end())
			{
				auto child = *iter;
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
		SceneObjectVector::const_iterator iter =  m_Children.begin();
		while(iter != m_Children.end())
		{
			auto child = *iter;
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
			iter =  m_Children.begin();

			while(iter != m_Children.end())
			{
				auto child = *iter;
				auto ret = child->GetFirstChildByName(name,exact_math,recursive);
				if(ret)
					return ret;
				++iter;
			}
		}
		return SceneObjectPtr();
	}

	SceneObjectPtr SceneObject::GetChildByID(const SceneObjectID &id) const
	{
		SceneObjectVector::const_iterator iter =  m_Children.begin();
		while(iter != m_Children.end())
		{
			auto child = *iter;
			++iter;
			if(child->GetID() == id)
			{
				return child;
			}
		}

		iter =  m_Children.begin();
		while(iter != m_Children.end())
		{
			auto child = *iter;
			++iter;
			auto ret = child->GetChildByID(id);
			if(ret)
				return ret;
		}
		return SceneObjectPtr();
	}

	void SceneObject::GetChildrenByID(SceneObjectVector &objects, const SceneObjectID &id, bool exact_math, bool recursive) const
	{
		if(recursive)
		{
			SceneObjectVector::const_iterator iter =  m_Children.begin();
			while(iter != m_Children.end())
			{
				auto child = *iter;

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
			//keep track of names?
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
			so = SimEngine::Get().GetSceneObjectTemplateManager()->CreateFromTemplate(template_name);
			so->RemoveAllChildren();
		}
		else
		{
			const std::string cc_name = so_elem->Value();
			so = std::make_shared<SceneObject>();
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

	SceneObjectPtr SceneObject::_CreateSceneObjectXML(tinyxml2::XMLElement *cc_elem) const
	{
		SceneObjectPtr cc;
		if(cc_elem->Attribute("from_template"))
		{
			std::string template_name = cc_elem->Attribute("from_template");
			cc = SimEngine::Get().GetSceneObjectTemplateManager()->CreateFromTemplate(template_name);
			//remove all children, they should be created by template, or?
			cc->RemoveAllChildren();
		}
		else
		{
			cc = std::make_shared<SceneObject>();
		}
		return cc;
	}

	BaseSceneComponentPtr SceneObject::AddBaseSceneComponent(const std::string& comp_name)
	{
		return GASS_DYNAMIC_PTR_CAST<GASS::BaseSceneComponent>(AddComponent(comp_name));
	}

	void SceneObject::AddChild(SceneObjectPtr child)
	{
		child->SetParent(shared_from_this());
		m_Children.push_back(child);
	}

	void SceneObject::InsertChild(SceneObjectPtr child, size_t index)
	{
		child->SetParent(shared_from_this());
		m_Children.insert(m_Children.begin() + index, child);
	}

	SceneObject::ComponentIterator SceneObject::GetComponents()
	{
		return ComponentIterator(m_ComponentVector.begin(), m_ComponentVector.end());
	}

	SceneObject::ConstComponentIterator SceneObject::GetComponents() const
	{
		return ConstComponentIterator(m_ComponentVector.begin(), m_ComponentVector.end());
	}

	ComponentPtr SceneObject::GetComponent(const std::string& name) const
	{
		for (const auto& comp : m_ComponentVector)
		{
			if (comp->GetName() == name)
				return comp;
		}
		return ComponentPtr();
	}

	void SceneObject::AddComponent(ComponentPtr comp)
	{
		comp->SetOwner(shared_from_this());
		m_ComponentVector.push_back(comp);
	}

	bool SceneObject::Serialize(ISerializer* serializer)
	{
		if (!m_Serialize)
			return true;
		if (!BaseReflectionObject::SerializeProperties(serializer))
			return false;

		if (serializer->Loading())
		{
			int num_comp = 0;
			auto* loader = dynamic_cast<SerialLoader*>(serializer);
			if (loader)
			{
				loader->IO<int>(num_comp);
				for (int i = 0; i < num_comp; i++)
				{
					std::string comp_type;
					loader->IO<std::string>(comp_type);
					ComponentPtr comp(ComponentFactory::Get().Create(comp_type));
					if (comp)
					{
						SerializePtr s_comp = GASS_DYNAMIC_PTR_CAST<ISerialize>(comp);
						if (s_comp)
						{
							if (!s_comp->Serialize(serializer))
								return false;
						}
						AddComponent(comp);
					}
					else
					{
						GASS_LOG(LWARNING) << "Failed to create component " << comp_type;
					}
				}


				int num_children = 0;
				loader->IO<int>(num_children);
				for (int i = 0; i < num_children; i++)
				{
					auto child = std::make_shared<SceneObject>();
					if (child)
					{
						SerializePtr s_child = GASS_DYNAMIC_PTR_CAST<ISerialize>(child);
						if (s_child)
						{
							if (!s_child->Serialize(serializer))
								return false;
						}
						AddChild(child);
					}
				}
			}
		}
		else
		{
			const int num_comp = static_cast<int>(m_ComponentVector.size());
			auto* saver = dynamic_cast<SerialSaver*>(serializer);
			if (saver)
			{
				saver->IO<int>(num_comp);
				auto iter = m_ComponentVector.begin();
				while (iter != m_ComponentVector.end())
				{
					ComponentPtr comp = (*iter);
					SerializePtr s_comp = GASS_DYNAMIC_PTR_CAST<ISerialize>(comp);
					if (s_comp)
					{
						if (!s_comp->Serialize(serializer))
							return false;

					}
					++iter;
				}

				const int num_children = static_cast<int>(m_Children.size());
				saver->IO<int>(num_children);
				SceneObjectVector::iterator go_iter;
				for (go_iter = m_Children.begin(); go_iter != m_Children.end(); ++go_iter)
				{
					SceneObjectPtr child = *go_iter;
					SerializePtr s_child = GASS_DYNAMIC_PTR_CAST<ISerialize>(child);
					if (s_child)
					{
						if (!s_child->Serialize(serializer))
							return false;
					}
				}
			}
		}
		return true;
	}

	void SceneObject::RemoveChild(SceneObjectPtr child)
	{
		SceneObjectVector::iterator bo_iter;
		for (bo_iter = m_Children.begin(); bo_iter != m_Children.end(); ++bo_iter)
		{
			if (child == *bo_iter)
			{
				m_Children.erase(bo_iter);
				return;
			}
		}
	}

	void SceneObject::RemoveAllChildren()
	{
		m_Children.clear();
	}

	void SceneObject::SaveXML(tinyxml2::XMLElement* obj_elem)
	{
		if (!m_Serialize)
			return;
		tinyxml2::XMLDocument* rootXMLDoc = obj_elem->GetDocument();
		tinyxml2::XMLElement* this_elem = nullptr;
		if (obj_elem->Parent() == rootXMLDoc) //top element!
		{
			this_elem = obj_elem;
		}
		else
		{
			const std::string tag_name = "SceneObject";
			this_elem = rootXMLDoc->NewElement(tag_name.c_str());
			obj_elem->LinkEndChild(this_elem);
		}

		SaveProperties(this_elem);

		tinyxml2::XMLElement* comp_elem = rootXMLDoc->NewElement("Components");
		this_elem->LinkEndChild(comp_elem);

		ComponentVector::iterator iter;
		for (iter = m_ComponentVector.begin(); iter != m_ComponentVector.end(); ++iter)
		{
			ComponentPtr comp = (*iter);
			XMLSerializePtr s_comp = GASS_DYNAMIC_PTR_CAST<IXMLSerialize>(comp);
			if (s_comp)
				s_comp->SaveXML(comp_elem);
		}

		tinyxml2::XMLElement* cc_elem = rootXMLDoc->NewElement("Children");
		this_elem->LinkEndChild(cc_elem);

		SceneObjectVector::iterator cc_iter;
		for (cc_iter = m_Children.begin(); cc_iter != m_Children.end(); ++cc_iter)
		{
			XMLSerializePtr child = GASS_DYNAMIC_PTR_CAST<IXMLSerialize>(*cc_iter);
			if (child)
			{
				child->SaveXML(cc_elem);
			}
		}
	}

	void SceneObject::LoadXML(tinyxml2::XMLElement* obj_elem)
	{
		if (!m_Serialize)
			return;
		tinyxml2::XMLElement* class_attribute = obj_elem->FirstChildElement();
		while (class_attribute)
		{
			const std::string data_name = class_attribute->Value();
			if (data_name == "Components")
			{
				tinyxml2::XMLElement* comp_elem = class_attribute->FirstChildElement();
				while (comp_elem)
				{
					ComponentPtr target_comp;

					//Try to get component by name first, if not found assume only one component of same type
					const tinyxml2::XMLElement* name_elem = comp_elem->FirstChildElement("Name");
					if (name_elem)
					{
						const std::string comp_name = name_elem->Attribute("value");
						target_comp = GetComponent(comp_name);
					}
					else
					{
						const std::string comp_name = comp_elem->Value();
						target_comp = GetComponent(comp_name);
					}

					if (target_comp) //component already exist, replace attributes component
					{
						ComponentPtr comp = _LoadComponentXML(comp_elem);
						//ComponentTemplatePtr template_comp = GASS_DYNAMIC_PTR_CAST<IComponentTemplate>(comp);
						if (comp)
						{
							comp->CopyPropertiesTo(target_comp);
						}
					}
					else
					{
						ComponentPtr comp = _LoadComponentXML(comp_elem);
						if (comp)
							AddComponent(comp);
					}
					comp_elem = comp_elem->NextSiblingElement();
				}
			}
			else if (data_name == "Children" || data_name == "ComponentContainers")
			{
				tinyxml2::XMLElement* cc_elem = class_attribute->FirstChildElement();
				while (cc_elem)
				{
					//allow over loading
					SceneObjectPtr so = _CreateSceneObjectXML(cc_elem);
					AddChild(so);
					XMLSerializePtr xml_obj = GASS_DYNAMIC_PTR_CAST<IXMLSerialize>(so);
					if (xml_obj)
						xml_obj->LoadXML(cc_elem);
					cc_elem = cc_elem->NextSiblingElement();
				}
			}
			else //base object attribute
			{
				const std::string attrib_val = class_attribute->FirstAttribute()->Value();
				try
				{
					SetPropertyByString(data_name, attrib_val);
				}
				catch (...)
				{
					GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed parsing:" + data_name + " With attribute:" + attrib_val + " in:" + std::string(obj_elem->GetDocument()->GetFileName()), "SceneObject::LoadXML");
				}
			}
			class_attribute = class_attribute->NextSiblingElement();
		}
	}

	ComponentPtr SceneObject::_LoadComponentXML(tinyxml2::XMLElement* comp_template) const
	{
		const std::string comp_type = comp_template->Value();
		//std::string comp_type = comp_template->Attribute("type");
		ComponentPtr comp(ComponentFactory::Get().Create(comp_type));
		if (comp)
		{
			//Give all components default name
			comp->SetName(comp_type);
			XMLSerializePtr s_comp = GASS_DYNAMIC_PTR_CAST<IXMLSerialize>(comp);
			if (s_comp)
				s_comp->LoadXML(comp_template);
		}
		else
		{
			GASS_LOG(LWARNING) << "Failed to create component " << comp_type;
		}
		return comp;
	}

	SceneObject::SceneObjectIterator SceneObject::GetChildren()
	{
		return SceneObjectIterator(m_Children.begin(), m_Children.end());
	}

	SceneObject::ConstSceneObjectIterator SceneObject::GetChildren() const
	{
		return SceneObject::ConstSceneObjectIterator(m_Children.begin(), m_Children.end());
	}

	void SceneObject::SetTemplateName(const std::string& name)
	{
		m_TemplateName = name;
	}

	std::string SceneObject::GetTemplateName()  const
	{
		return m_TemplateName;
	}

	void SceneObject::SetSerialize(bool value)
	{
		m_Serialize = value;
	}

	bool SceneObject::GetSerialize()  const
	{
		return m_Serialize;
	}


	void SceneObject::_CheckComponentDependencies() const
	{
		//get all names
		std::set<std::string> names;
		auto comp_iter = m_ComponentVector.begin();
		while (comp_iter != m_ComponentVector.end())
		{
			ComponentPtr comp = (*comp_iter);
			names.insert(comp->GetRTTI()->GetClassName());
			++comp_iter;
		}

		comp_iter = m_ComponentVector.begin();

		while (comp_iter != m_ComponentVector.end())
		{
			ComponentPtr comp = (*comp_iter);
			const std::vector<std::string> deps = comp->GetDependencies();
			auto dep_iter = deps.begin();
			while (dep_iter != deps.end())
			{
				const std::string comp_name = *dep_iter;
				if (names.find(comp_name) == names.end())
					GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find dependent component:" + comp_name + " in component:" + GetName(), "SceneObject::CheckComponentDependencies");
				++dep_iter;
			}
			++comp_iter;
		}
	}

	size_t SceneObject::GetNumChildren() const
	{
		return m_Children.size();
	}

	SceneObjectPtr SceneObject::GetChild(size_t index) const
	{
		return m_Children[index];
	}

	ComponentPtr SceneObject::AddComponent(const std::string& comp_type)
	{
		ComponentPtr comp = ComponentFactory::Get().Create(comp_type);
		if (!comp)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to create Component:" + comp_type, "SceneObject::AddComponent");
		comp->SetName(comp_type);
		AddComponent(comp);
		return comp;
	}


#define GASS_INDENT(val) std::cout << std::setfill(' ') << std::setw(val*3) << std::right << " "; std::cout
	void SceneObject::DebugPrint(int tc)
	{
		GASS_INDENT(tc) << GetRTTI()->GetClassName() << " - " << GetName() << std::endl;
		tc++;
		if (m_ComponentVector.size() > 0)
		{
			GASS_INDENT(tc) << "Components" << std::endl;
		}
		auto comp_iter = m_ComponentVector.begin();
		tc++;
		while (comp_iter != m_ComponentVector.end())
		{
			ComponentPtr comp = (*comp_iter);
			GASS_INDENT(tc) << comp->GetName() << std::endl;
			++comp_iter;
		}
		SceneObjectVector::iterator iter;
		if (m_Children.size() > 0)
		{
			GASS_INDENT(tc) << "Children" << std::endl;
		}
		for (iter = m_Children.begin(); iter != m_Children.end(); ++iter)
		{
			SceneObjectPtr child = *iter;
			child->DebugPrint(tc + 1);
		}
	}
}