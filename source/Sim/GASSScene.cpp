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




#include "Sim/GASSScene.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Interface/GASSISceneManager.h"
#include "Sim/Interface/GASSIGraphicsSceneManager.h"
#include "Sim/Interface/GASSIPhysicsBodyComponent.h"
#include "Sim/Interface/GASSILocationComponent.h"
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
#include "Core/ComponentSystem/GASSComponentContainerTemplateManager.h"
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
		m_SceneMessageManager(new MessageManager()),
		m_Initlized(false),
		m_Geocentric(false)
	{

	}

	void Scene::RegisterReflection()
	{
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("The scene object", OF_VISIBLE)));

		RegisterProperty<Vec3>("StartPosition", &Scene::GetStartPos, &Scene::SetStartPos);
		RegisterProperty<EulerRotation>("StartRotation", &Scene::GetStartRot, &Scene::SetStartRot);
		RegisterProperty<bool>("Geocentric", &Scene::GetGeocentric, &Scene::SetGeocentric);
	}

	void Scene::OnCreate()
	{
		//Create empty root node
		m_Root = SceneObjectPtr(new SceneObject());
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

		SceneObjectPtr  scenery = SceneObjectPtr(new SceneObject());
		scenery->SetName("Scenery");
		scenery->SetID("SCENARY_ROOT");
		m_TerrainObjects = scenery;
		m_Root->AddChildSceneObject(scenery,true);

		//send load message
		//m_SceneMessageManager->SendImmediate(MessagePtr(new LoadSceneManagersRequest(shared_from_this())));
		SystemMessagePtr system_msg(new PostSceneCreateEvent(shared_from_this()));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(system_msg);
		m_Initlized = true;
	}

	void Scene::OnUnload()
	{
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
		m_Root->RemoveAllChildrenNotify();
		SceneObjectPtr  scenery = SceneObjectPtr(new SceneObject());
		scenery->SetName("Scenery");
		scenery->SetID("SCENARY_ROOT");
		m_TerrainObjects = scenery;
		m_Root->AddChildSceneObject(scenery, true);
	}

	void Scene::Load(const std::string &name)
	{
		m_FolderName = name;
		if(name == "")
		{
			GASS_EXCEPT(Exception::ERR_INVALID_STATE,
				"You must provide a scene name",
				"Scene::Load");
		}

		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
		rm->RemoveResourceGroup(ResourceGroupPtr(m_ResourceGroup));
		ResourceGroupPtr res_group(new ResourceGroup(GetResourceGroupName()));
		m_ResourceGroup = res_group;
		FilePath scene_path(SimEngine::Get().GetScenePath().GetFullPath() + "/"  + name);
		m_ResourceLocation = res_group->AddResourceLocation(scene_path,RLT_FILESYSTEM,true);
		rm->AddResourceGroup(res_group);
		//rs->LoadResourceGroup(GetResourceGroupName());

		const FilePath filename = FilePath(scene_path.GetFullPath() + "/scene.xml");

		const std::string template_file_name = scene_path.GetFullPath() + "/templates.xml";
		if(FileUtils::FileExist(template_file_name))
			SimEngine::Get().GetSceneObjectTemplateManager()->Load(scene_path.GetFullPath() + "/templates.xml");

		tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
		if(xmlDoc->LoadFile(filename.GetFullPath().c_str()) != tinyxml2::XML_NO_ERROR)
		{
			delete xmlDoc;
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load: " + filename.GetFullPath(), "Scene::Load");
		}

		tinyxml2::XMLElement *scene_elem = xmlDoc->FirstChildElement("Scene");
		if(scene_elem == NULL)
		{
			delete xmlDoc;
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get Scene tag", "Scene::Load");
		}
		LoadXML(scene_elem);
		xmlDoc->Clear();
		//Delete our allocated document
		delete xmlDoc;

		//Create new scenery node
		m_Root->RemoveChildSceneObject(SceneObjectPtr(m_TerrainObjects));
		SceneObjectPtr  scenery =  SceneObject::LoadFromXML(GetSceneFolder().GetFullPath() + "/instances.xml");
		scenery->SetName("Scenery");
		scenery->SetID("SCENARY_ROOT");
		m_TerrainObjects = scenery;

		m_Root->AddChildSceneObject(scenery,true);

		//scene loaded!
		SystemMessagePtr system_msg(new PostSceneLoadEvent(shared_from_this()));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(system_msg);
	}

	FilePath Scene::GetSceneFolder() const
	{
		if(m_FolderName != "")
			return FilePath(SimEngine::Get().GetScenePath().GetFullPath() + "/" + m_FolderName);
		return FilePath("");
	}

	void Scene::Save(const std::string &name)
	{
		const FilePath scene_path = FilePath(SimEngine::Get().GetScenePath().GetFullPath() + "/"  +  name);
		
		if (!scene_path.Exist())
		{
			FileUtils::CreateDir(scene_path.GetFullPath());
		}

		else if (!scene_path.IsDir())
		{
			return;
		}
	
		if(m_ResourceLocation.lock())//remove previous location?
		{
			ResourceGroupPtr(m_ResourceGroup)->RemoveResourceLocation(ResourceLocationPtr(m_ResourceLocation));
		}
		m_FolderName = name;
		m_ResourceLocation = ResourceGroupPtr(m_ResourceGroup)->AddResourceLocation(scene_path,RLT_FILESYSTEM,true);

		tinyxml2::XMLDocument doc;
		tinyxml2::XMLDeclaration* decl = doc.NewDeclaration();
		doc.LinkEndChild( decl );

		tinyxml2::XMLElement * scene_elem = doc.NewElement("Scene");
		doc.LinkEndChild( scene_elem);
		SaveXML(scene_elem);

		const FilePath filename = FilePath(scene_path.GetFullPath() + "/Scene.xml");
		doc.SaveFile(filename.GetFullPath().c_str());

		if(SceneObjectPtr(m_TerrainObjects))
			SceneObjectPtr(m_TerrainObjects)->SaveToFile(scene_path.GetFullPath() + "/instances.xml");

		tinyxml2::XMLDocument template_doc;
		tinyxml2::XMLDeclaration* template_decl = template_doc.NewDeclaration();
		template_doc.LinkEndChild( template_decl);
		tinyxml2::XMLElement * template_elem = template_doc.NewElement("Templates");
		template_doc.LinkEndChild( template_elem);
		const FilePath template_filename = FilePath(scene_path.GetFullPath() + "/Templates.xml");
		template_doc.SaveFile(template_filename.GetFullPath().c_str());
	}

	void Scene::SyncMessages(double delta_time) const
	{
		if(m_Initlized)
		{
			m_SceneMessageManager->Update(delta_time);
			m_Root->SyncMessages(delta_time);
		}
	}

	void Scene::LoadXML(tinyxml2::XMLElement *scene)
	{
		tinyxml2::XMLElement *prop = scene->FirstChildElement("Properties");
		if(prop)
			BaseReflectionObject::_LoadProperties(prop);
		else // fallback for old scenes
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get Properties tag", "Scene::LoadXML");
		tinyxml2::XMLElement *scene_manager = scene->FirstChildElement("SceneManagerSettings");
		if(scene_manager)
		{
			scene_manager = scene_manager->FirstChildElement();
			//Load scene manager settings
			while(scene_manager)
			{
				SceneManagerPtr sm = LoadSceneManager(scene_manager);
				scene_manager = scene_manager->NextSiblingElement();
			}
		}
		else
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get SceneManagerSettings tag", "Scene::LoadXML");
	}

	void Scene::SaveXML(tinyxml2::XMLElement *parent)
	{
		tinyxml2::XMLDocument *rootXMLDoc = parent->GetDocument();
		//Create
		tinyxml2::XMLElement *prop = rootXMLDoc->NewElement("Properties");
		parent->LinkEndChild(prop);

		BaseReflectionObject::_SaveProperties(prop);

		tinyxml2::XMLElement *sms_elem = rootXMLDoc->NewElement("SceneManagerSettings");
		parent->LinkEndChild(sms_elem);

		for(size_t i  = 0 ; i < m_SceneManagers.size();i++)
		{
			SceneManagerPtr sm = m_SceneManagers[i];
			if(sm->GetSerialize()) //should we save this scene manager settings to scene?
			{
				XMLSerializePtr serialize = GASS_DYNAMIC_PTR_CAST<IXMLSerialize>(sm);
				if(serialize)
				{
					serialize->SaveXML(sms_elem);
				}
			}
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
