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




#include <memory>

#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObjectVisitors.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSComponent.h"
#include "Sim/Interface/GASSISceneManager.h"
#include "Sim/Interface/GASSIGraphicsSceneManager.h"
#include "Sim/Interface/GASSIPhysicsBodyComponent.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSResourceManager.h"
#include "Sim/GASSResourceGroup.h"
#include "Sim/GASSSimEngine.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSSceneObjectTemplateManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/Utils/GASSFilePath.h"
#include "Core/Utils/GASSFileUtils.h"
#include "Core/Utils/GASSFilesystem.h"
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{
	Scene::Scene(const std::string &name) : m_Name(name) ,
		m_StartPos(0,0,0),
		m_StartRot(0,0,0),
		m_SceneMessageManager(new MessageManager())
		
	{
		
	}

	void Scene::RegisterReflection()
	{
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("The scene object", OF_VISIBLE));
		//RegisterGetSet("Name", &Scene::GetName, &Scene::SetName);
		RegisterGetSet("StartPosition", &Scene::GetStartPos, &Scene::SetStartPos);
		RegisterGetSet("StartRotation", &Scene::GetStartRot, &Scene::SetStartRot);
		RegisterGetSet("Geocentric", &Scene::GetGeocentric, &Scene::SetGeocentric);
		RegisterGetSet("OSGEarth", &Scene::GetOSGEarth, &Scene::SetOSGEarth);
	}

	void Scene::OnCreate()
	{
		if (m_Initlized)
			return;
		//Create empty root node
		m_Root = std::make_shared<SceneObject>();
		m_Root->SetName("Root");
		m_Root->OnInitialize(shared_from_this());

		SystemMessagePtr enter_load_msg(new PreSceneCreateEvent(shared_from_this()));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(enter_load_msg);

		m_SceneMessageManager->RegisterForMessage(typeid(RemoveSceneObjectRequest), TYPED_MESSAGE_FUNC(Scene::OnRemoveSceneObject,RemoveSceneObjectRequest),0);
		m_SceneMessageManager->RegisterForMessage(typeid(SpawnObjectFromTemplateRequest),TYPED_MESSAGE_FUNC(Scene::OnSpawnSceneObjectFromTemplate,SpawnObjectFromTemplateRequest),0);

		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();

		ResourceGroupPtr scene_group(new ResourceGroup(GetResourceGroupName()));
		m_ResourceGroup  = scene_group;
		rm->AddResourceGroup(scene_group);

		//Add all registered scene managers to the scene
		std::vector<std::string> managers = SceneManagerFactory::GetPtr()->GetFactoryNames();
		for(size_t i = 0; i < managers.size();i++)
		{
			SceneManagerPtr sm = SceneManagerFactory::GetPtr()->Create(managers[i], shared_from_this());
			m_SceneManagers.push_back(sm);
		}

		for(size_t i = 0; i < m_SceneManagers.size() ; i++)
		{
			m_SceneManagers[i]->OnSceneCreated();
		}

		SceneObjectPtr  scenery = std::make_shared<SceneObject>();
		scenery->SetName("Scenery");
		scenery->SetID("SCENARY_ROOT");
		m_TerrainObjects = scenery;
		m_Root->AddChildSceneObject(scenery,true);

		//send load message
		SystemMessagePtr system_msg(new PostSceneCreateEvent(shared_from_this()));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(system_msg);
		m_Initlized = true;
	}

	void Scene::OnUnload()
	{
		if (!m_Initlized)
			return;
		m_Root->OnDelete();
		m_Root.reset();
		for(size_t i = 0; i < m_SceneManagers.size() ; i++)
		{
			m_SceneManagers[i]->OnSceneShutdown();
		}

		SystemMessagePtr unload_msg(new SceneUnloadedEvent(shared_from_this()));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(unload_msg);
		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
		rm->RemoveResourceGroup(ResourceGroupPtr(m_ResourceGroup));
		m_SceneManagers.clear();
		m_SceneMessageManager->Clear();
		m_Initlized = false;
	}

	std::string Scene::GetResourceGroupName() const
	{
		return "GASSSceneResGroup" + m_Name;
	}

	void Scene::Clear()
	{
		m_NameGenerator.Clear();
		m_Root->RemoveAllChildrenNotify();
		SceneObjectPtr  scenery = std::make_shared<SceneObject>();
		scenery->SetName("Scenery");
		scenery->SetID("SCENARY_ROOT");
		m_TerrainObjects = scenery;
		m_Root->AddChildSceneObject(scenery, true);
	}

	void Scene::Load(const FilePath& filename)
	{
		Clear();
		if (!filename.Exist())
		{
			GASS_EXCEPT(Exception::ERR_INVALID_STATE,
				"You must provide valid scene file",
				"Scene::Load");
		}
		m_FolderName = filename.GetPathNoFile();
		m_SceneFile = filename;
		m_Name = FileUtils::RemoveExtension(filename.GetFilename());

		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
		rm->RemoveResourceGroup(ResourceGroupPtr(m_ResourceGroup));
		ResourceGroupPtr res_group(new ResourceGroup(GetResourceGroupName()));
		m_ResourceGroup = res_group;
		FilePath scene_path(filename.GetPathNoFile());
		m_ResourceLocation = res_group->AddResourceLocation(scene_path, RLT_FILESYSTEM, true);
		rm->AddResourceGroup(res_group);

		const std::string template_file_name = scene_path.GetFullPath() + "/templates.xml";
		if (FileUtils::FileExist(template_file_name))
			SimEngine::Get().GetSceneObjectTemplateManager()->Load(scene_path.GetFullPath() + "/templates.xml");

		auto* xml_doc = new tinyxml2::XMLDocument();
		if (xml_doc->LoadFile(filename.GetFullPath().c_str()) != tinyxml2::XML_NO_ERROR)
		{
			delete xml_doc;
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE, "Couldn't load: " + filename.GetFullPath(), "Scene::Load");
		}

		tinyxml2::XMLElement* scene_elem = xml_doc->FirstChildElement("Scene");
		if (scene_elem == nullptr)
		{
			delete xml_doc;
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to get Scene tag", "Scene::Load");
		}
		LoadXML(scene_elem);
		xml_doc->Clear();
		//Delete our allocated document
		delete xml_doc;		

		//scene loaded!
		SystemMessagePtr system_msg(new PostSceneLoadEvent(shared_from_this()));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(system_msg);
	}

#if 0
	class MaxNameIDVisitor : public SceneObjectVisitor
	{
	public:
		
		std::string GetStrippedName(const std::string& name) const
		{
			auto pos = name.find(m_Prefix);
			if (pos != std::string::npos)
				return name.substr(0, pos);
			return name;
		}

		int GetLastNumber(const std::string& name)
		{
			int num = 0;
			std::size_t const n = name.find_last_of("0123456789");
			if (n != std::string::npos)
			{
				std::size_t const m = name.find_last_not_of("0123456789", n);
				const auto num_str = m != std::string::npos ? name.substr(m + 1, n - m) : name.substr(0, n + 1);
				num = std::stoi(num_str);
			}
			return num;
		}

		MaxNameIDVisitor(const std::string& name)  { m_Name = GetStrippedName(name); }
		virtual bool Visit(SceneObjectPtr scene_object)
		{
			const auto object_name = scene_object->GetName();
			const auto stripped_name = GetStrippedName(object_name);
			if (stripped_name == m_Name)
			{
				m_MaxId = std::max<int>(m_MaxId, GetLastNumber(object_name));
			}
			return true;
		}
		int m_MaxId = 0;
		std::string m_Name;
		std::string m_Prefix = " (";
	};

	std::string Scene::CreateUniqueName(const std::string &name)
	{
		auto vistor = std::make_shared<MaxNameIDVisitor>(name);
		m_Root->Accept(vistor);
		return vistor->m_Name + " (" + std::to_string(vistor->m_MaxId +1 )+ ")";
	}
#endif

	FilePath Scene::GetSceneFolder() const
	{
		if(m_FolderName != "")
			return FilePath(SimEngine::Get().GetScenePath().GetFullPath() + "/" + m_FolderName);
		return FilePath("");
	}


	void Scene::New()
	{
		Clear();
		if (m_ResourceLocation.lock())//remove previous location?
		{
			ResourceGroupPtr(m_ResourceGroup)->RemoveResourceLocation(ResourceLocationPtr(m_ResourceLocation));
		}
		m_Name = "MyScene";
		m_SceneFile = FilePath("");
		m_FolderName = "";
	}

	void Scene::Save(const FilePath& filename)
	{
		if (m_ResourceLocation.lock())//remove previous location?
		{
			ResourceGroupPtr(m_ResourceGroup)->RemoveResourceLocation(ResourceLocationPtr(m_ResourceLocation));
		}
		m_Name = FileUtils::RemoveExtension(filename.GetFilename());
		m_SceneFile = filename;
		m_FolderName = filename.GetPathNoFile();
		m_ResourceLocation = ResourceGroupPtr(m_ResourceGroup)->AddResourceLocation(FilePath(m_FolderName), RLT_FILESYSTEM, true);

		//Save camera pos
		if (auto camera = m_Root->GetFirstComponentByClass<ICameraComponent>(true))
		{
			m_StartPos = camera->GetSceneObject()->GetWorldPosition();
			m_StartRot = EulerRotation::FromQuaternion(camera->GetSceneObject()->GetWorldRotation());
		}

		tinyxml2::XMLDocument doc;
		tinyxml2::XMLDeclaration* decl = doc.NewDeclaration();
		doc.LinkEndChild(decl);

		tinyxml2::XMLElement* scene_elem = doc.NewElement("Scene");
		doc.LinkEndChild(scene_elem);
		SaveXML(scene_elem);
		doc.SaveFile(filename.GetFullPath().c_str());
	}

	void Scene::SyncMessages(double delta_time) const
	{
		if(m_Initlized)
		{
			m_SceneMessageManager->Update(delta_time);
			m_Root->SyncMessages(delta_time);
		}
	}

	void Scene::LoadXML(tinyxml2::XMLElement* scene)
	{
		tinyxml2::XMLElement* prop = scene->FirstChildElement("Properties");
		if (prop)
			BaseReflectionObject::LoadProperties(prop);
		else // fallback for old scenes
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to get Properties tag", "Scene::LoadXML");
		tinyxml2::XMLElement* scene_manager = scene->FirstChildElement("SceneManagerSettings");
		if (scene_manager)
		{
			scene_manager = scene_manager->FirstChildElement();
			//Load scene manager settings
			while (scene_manager)
			{
				SceneManagerPtr sm = LoadSceneManager(scene_manager);
				scene_manager = scene_manager->NextSiblingElement();
			}
		}
		else
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to get SceneManagerSettings tag", "Scene::LoadXML");
	
		//Create new scenery node
		m_NameGenerator.Clear("Scenery");
		m_Root->RemoveChildSceneObject(SceneObjectPtr(m_TerrainObjects));
		
		SceneObjectPtr  scenery = SceneObjectPtr(new SceneObject);
		tinyxml2::XMLElement* scenery_elem = scene->FirstChildElement("TerrainObjects");
		if (scenery_elem && scenery_elem->FirstChildElement("SceneObject"))
			scenery->LoadXML(scenery_elem->FirstChildElement("SceneObject"));
		scenery->SetName("Scenery");
		scenery->SetID("SCENARY_ROOT");
		m_TerrainObjects = scenery;
		m_Root->AddChildSceneObject(scenery, true);
	}

	void Scene::SaveXML(tinyxml2::XMLElement* parent)
	{
		tinyxml2::XMLDocument* root_xml_doc = parent->GetDocument();
		//Create
		tinyxml2::XMLElement* prop = root_xml_doc->NewElement("Properties");
		parent->LinkEndChild(prop);

		BaseReflectionObject::SaveProperties(prop);

		tinyxml2::XMLElement* sms_elem = root_xml_doc->NewElement("SceneManagerSettings");
		parent->LinkEndChild(sms_elem);

		for (size_t i = 0; i < m_SceneManagers.size(); i++)
		{
			SceneManagerPtr sm = m_SceneManagers[i];
			if (sm->GetSerialize()) //should we save this scene manager settings to scene?
			{
				XMLSerializePtr serialize = GASS_DYNAMIC_PTR_CAST<IXMLSerialize>(sm);
				if (serialize)
				{
					serialize->SaveXML(sms_elem);
				}
			}
		}

		tinyxml2::XMLElement* terrrain_elem = root_xml_doc->NewElement("TerrainObjects");
		parent->LinkEndChild(terrrain_elem);

		if (SceneObjectPtr(m_TerrainObjects))
		{
			SceneObjectPtr(m_TerrainObjects)->SaveXML(terrrain_elem);
		}
	}

	SceneManagerPtr Scene::LoadSceneManager(tinyxml2::XMLElement *sm_elem)
	{
		SceneManagerPtr sm;
		std::string sm_name = sm_elem->Value();
		for(size_t i = 0; i < m_SceneManagers.size() ; i++)
		{
			if(m_SceneManagers[i]->GetName() == sm_name)
			{
				sm = m_SceneManagers[i];
				XMLSerializePtr serialize = GASS_DYNAMIC_PTR_CAST<IXMLSerialize>(sm);
				if(serialize)
					serialize->LoadXML(sm_elem);
				return sm;
			}
		}
		return sm;
	}

	SceneManagerPtr Scene::GetSceneManagerByName(const std::string &name) const
	{
		for(size_t i = 0; i < m_SceneManagers.size(); i++)
		{
			if(m_SceneManagers[i]->GetName() == name)
				return m_SceneManagers[i];
		}
		SceneManagerPtr empty;
		return empty;
	}

	void Scene::OnSpawnSceneObjectFromTemplate(SpawnObjectFromTemplateRequestPtr message)
	{
		std::string obj_template = message->GetTemplateName();
		SceneObjectPtr so = LoadObjectFromTemplate(obj_template,message->GetParent());
		if(so)
		{
			LocationComponentPtr lc = so->GetFirstComponentByClass<ILocationComponent>();
			if(lc)
			{
				lc->SetWorldPosition(message->GetPosition());
				lc->SetWorldRotation(message->GetRotation());
			}
			 
			//Check if we have body?
			PhysicsBodyComponentPtr body =  so->GetFirstComponentByClass<IPhysicsBodyComponent>();
			if(body)
				body->SetVelocity(message->GetVelocity(),true);
		}
	}

	SceneObjectPtr Scene::LoadObjectFromTemplate(const std::string &template_name, SceneObjectPtr parent) const
	{
		SceneObjectPtr so = SimEngine::Get().CreateObjectFromTemplate(template_name);
		if(so)
		{
			if(parent)
			{
				parent->AddChildSceneObject(so,true);
			}
			else
				m_Root->AddChildSceneObject(so,true);
		}
		return so;
	}

	void Scene::OnRemoveSceneObject(RemoveSceneObjectRequestPtr message)
	{
		SceneObjectPtr so = message->GetSceneObject();
		if(so && so->GetParentSceneObject())
			so->GetParentSceneObject()->RemoveChildSceneObject(so);
	}

	SceneManagerIterator Scene::GetSceneManagers()
	{
		return SceneManagerIterator(m_SceneManagers.begin(),m_SceneManagers.end());
	}

	int Scene::RegisterForMessage(const MessageType &type, MessageFuncPtr callback, int priority )
	{
		return m_SceneMessageManager->RegisterForMessage(type, callback, priority);
	}

	void Scene::UnregisterForMessage(const MessageType &type, MessageFuncPtr callback)
	{
		m_SceneMessageManager->UnregisterForMessage(type, callback);
	}

	void Scene::PostMessage( SceneMessagePtr message )
	{
		m_SceneMessageManager->PostMessage(message);
	}

	void Scene::SendImmediate( SceneMessagePtr message )
	{
		m_SceneMessageManager->SendImmediate(message);
	}

	size_t Scene::GetQueuedMessages() const
	{
		size_t num = m_SceneMessageManager->GetQueuedMessages();
		num += m_Root->GetQueuedMessages();
		return num;
	}

	std::vector<std::string> Scene::GetScenes(const FilePath &path)
	{

		std::vector<std::string> scene_names;
		if (path.Exist())
		{
			std::vector<FilePath> folders;
			FilePath::GetFoldersFromPath(folders, path, false);
			for (size_t i = 0; i < folders.size(); ++i)
			{
				if (FileUtils::FileExist(folders[i].GetFullPath() + "scene.xml"))
				{
					std::cout << folders[i] << "\n";
					std::string scene_name = folders[i].GetLastFolder();
					scene_names.push_back(scene_name);
				}
			}
		}
		return scene_names;
	}

	void Scene::DrawDebugLine(const Vec3 &start_point, const Vec3 &end_point, const ColorRGBA &start_color, const ColorRGBA &end_color) const
	{
			GetFirstSceneManagerByClass<IGraphicsSceneManager>()->DrawLine(start_point, end_point,start_color,end_color);
	}
}
