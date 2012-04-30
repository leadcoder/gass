#include "EditorApplication.h"
#include "Modules/Editor/EditorManager.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"
#include "Sim/GASS.h"
#include "Modules/Editor/ToolSystem/MoveTool.h"
#include "Modules/Editor/ToolSystem/SelectTool.h"
#include "Modules/Editor/ToolSystem/PaintTool.h"
#include "Modules/Editor/ToolSystem/VerticalMoveTool.h"
#include "Modules/Editor/ToolSystem/RotateTool.h"
#include "Modules/Editor/ToolSystem/CreateTool.h"
#include "Modules/Editor/ToolSystem/MeasurementTool.h"
#include "Modules/Editor/ToolSystem/TerrainDeformTool.h"

#include "tinyxml.h"
#include <boost/filesystem.hpp>

namespace GASS
{
	EditorApplication::EditorApplication() : m_Initilized(false), 
		m_NewScene(false), 
		m_LoadScene(false),
		m_UseObjectID(true),
		m_UseTerrainNormalOnDrop(false),
		m_RayPickDist(3000),
		m_NumRTCThreads(-1),
		m_Mode("StandAlone"),
		m_ServerName("GASS_SERVER"),
		m_ClientName("GASS_CLIENT"),
		m_ServerPort(2001),
		m_ClientPort(2002),
		m_UpdateFreq(160)
	{

		m_Scene  = ScenePtr(new Scene());
		new SimEngine();
		new EditorManager();
		LogManager* log_man = new LogManager();
		log_man->createLog("GASS.log", true, true);
	}

	EditorApplication::~EditorApplication(void)
	{
		m_Scene.reset();
		delete EditorManager::GetPtr();
		delete SimEngine::GetPtr();

	}

	void EditorApplication::Init(const FilePath &working_folder, const FilePath &appdata_folder_path, const FilePath &mydocuments_folder_path, void* main_win_handle,void*render_win_handle)
	{	
		const std::string config_path = working_folder.GetPath() + "../configuration/";
		LoadSettings(config_path + "EditorApplication.xml");
		SimEngine *se = SimEngine::GetPtr();
		se->Init(config_path +  "GASSPlugins.xml", config_path +  "GASSSystems.xml", config_path +  "GASSControlSettings.xml",m_NumRTCThreads);

		se->GetSimSystemManager()->RegisterForMessage(REG_TMESS(EditorApplication::OnLoadScene,SceneLoadedNotifyMessage,0));

		se->GetSceneObjectTemplateManager()->SetAddObjectIDToName(m_UseObjectID);
		se->GetSceneObjectTemplateManager()->SetObjectIDPrefix(m_IDPrefix);
		se->GetSceneObjectTemplateManager()->SetObjectIDSuffix(m_IDSuffix);
		//boot gass editor
		
		EditorManager::GetPtr()->Init(working_folder,appdata_folder_path,mydocuments_folder_path);
		EditorManager::Get().GetMouseToolController()->SetUseTerrainNormalOnDrop(m_UseTerrainNormalOnDrop);
		EditorManager::Get().GetMouseToolController()->SetRayPickDistance(m_RayPickDist);

		//Load templates
		std::vector<std::string>::iterator iter = m_Templates.begin();
		while(iter != m_Templates.end())
		{
			std::string file_path = *iter;
			se->GetSceneObjectTemplateManager()->Load(file_path);
			iter++;
		}

		//Start client or server?
		if(Misc::ToLower(m_Mode) == "server")
		{
			MessagePtr server_mess(new StartServerMessage(m_ServerName, m_ServerPort));
			se->GetSimSystemManager()->PostMessage(server_mess);
		}
		else if (Misc::ToLower(m_Mode) == "client")
		{
			se->GetSimSystemManager()->SendImmediate(MessagePtr(new StartClientMessage(m_ClientName, m_ClientPort,m_ServerPort)));

			MessagePtr connect_message(new ConnectToServerMessage(m_ServerName, m_ServerPort,8888, 2));
			se->GetSimSystemManager()->PostMessage(connect_message);
		}

		//main render window
		const std::string render_win_name = "RenderWindow";
		GASS::GraphicsSystemPtr gfx_system = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystem<GASS::IGraphicsSystem>();
		if(gfx_system)
		{
			gfx_system->CreateRenderWindow(render_win_name,800,600,render_win_handle,main_win_handle);
			gfx_system->CreateViewport(render_win_name,render_win_name, 0, 0, 1, 1);
		}

		//add some test tools

		GASS::MouseToolController* tools = EditorManager::Get().GetMouseToolController().get();

		IMouseTool* tool = new MoveTool(tools);
		tools->AddTool(tool);

		tool = new SelectTool(tools);
		tools->AddTool(tool);

		tool = new RotateTool(tools);
		tools->AddTool(tool);

		tool = new CreateTool(tools);
		tools->AddTool(tool);

		tool = new MeasurementTool(tools);
		tools->AddTool(tool);

		//tool = new TerrainDeformTool(tools);
		//tools->AddTool(tool);

		tools->SelectTool("SelectTool");
		tools->SetActive(true);

		m_Initilized  = true;
	}

	void EditorApplication::Update()
	{
		static GASS::Timer  timer;
		static double prev_time = 0;
		double current_time = timer.GetTime();
		double delta_time = current_time - prev_time;
		if(delta_time < 0.00001)
			delta_time = 0.001;
		if(delta_time > 1)
			delta_time = 1;

		double update_time = 1.0/m_UpdateFreq;
		if(delta_time > update_time)
		{
			prev_time = current_time;
			if(m_Initilized)
			{
				SimEngine::Get().Update(delta_time);
				m_Scene->OnUpdate(delta_time);
			}
			EditorManager::Get().Update(delta_time);
		}
	}

	void EditorApplication::OnLoadScene(SceneLoadedNotifyMessagePtr message)
	{
		
		ScenePtr scene = message->GetScene();

		//load top camera
		Vec3 vel(0,0,0);
		Vec3 pos = scene->GetStartPos();
		Quaternion rot(scene->GetStartRot());

		//auto create free camera?
		
		SceneObjectPtr free_obj = scene->LoadObjectFromTemplate("FreeCameraObject",scene->GetRootSceneObject());
		
		if(!free_obj) //If no FreeCameraObject template found, create one
		{
			SceneObjectTemplatePtr fre_cam_template (new SceneObjectTemplate);
			fre_cam_template->SetName("FreeCameraObject");
			ComponentPtr location_comp (ComponentFactory::Get().Create("LocationComponent"));
			location_comp->SetName("LocationComp");

			ComponentPtr camera_comp (ComponentFactory::Get().Create("CameraComponent"));
			camera_comp->SetName("FreeCameraComp");

			ComponentPtr cc_comp (ComponentFactory::Get().Create("FreeCamControlComponent"));
			cc_comp->SetName("FreeCameraCtrlComp");

			fre_cam_template->AddComponent(location_comp);
			fre_cam_template->AddComponent(camera_comp);
			fre_cam_template->AddComponent(cc_comp);

			SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(fre_cam_template);

			free_obj = scene->LoadObjectFromTemplate("FreeCameraObject",scene->GetRootSceneObject());
		}

		MessagePtr pos_msg(new PositionMessage(scene->GetStartPos()));
		if(free_obj)
		{
			free_obj->SendImmediate(pos_msg);
			MessagePtr camera_msg(new ChangeCameraMessage(free_obj,"RenderWindow"));
			scene->PostMessage(camera_msg);
		}

		SceneObjectPtr top_obj = scene->LoadObjectFromTemplate("TopCameraObject",scene->GetRootSceneObject());
		if(top_obj)
			top_obj->SendImmediate(pos_msg);
	}

	bool  EditorApplication::LoadSettings(const std::string &filename)
	{
		TiXmlDocument     *xmlDoc = new TiXmlDocument(filename.c_str());
		if (!xmlDoc->LoadFile())
		{
			// Fatal error, cannot load
			return false;
		}
		TiXmlElement *xSettings = 0;
		xSettings = xmlDoc->FirstChildElement("EditorApplication");
		TiXmlElement *xscenes = xSettings->FirstChildElement("SceneList");
		TiXmlElement *start_scene = xSettings->FirstChildElement("SceneToLoadAtStart");
		TiXmlElement *gui_state = xSettings->FirstChildElement("GUIState");
		TiXmlElement *xTerrainNormal = xSettings->FirstChildElement("UseTerrainNormalOnDrop");
		TiXmlElement *xRayPickDist = xSettings->FirstChildElement("MousePickDistance");
		
		TiXmlElement *xscene_path = xSettings->FirstChildElement("ScenePath");
		TiXmlElement *xtemplates = xSettings->FirstChildElement("TemplateFiles");
		TiXmlElement *xobj_id = xSettings->FirstChildElement("ObjectID");
		TiXmlElement *xnetwork = xSettings->FirstChildElement("Network");
		

		TiXmlElement *xnum_threads = xSettings->FirstChildElement("NumRTCThreads");
		TiXmlElement *xupdate_freq = xSettings->FirstChildElement("UpdateFreq");

		if(xupdate_freq)
		{
			m_UpdateFreq = atoi(xupdate_freq->Attribute("value"));
		}

		if(xnum_threads)
			m_NumRTCThreads = atoi(xnum_threads->Attribute("value"));
		
		if(xnetwork)
		{
			if(xnetwork->Attribute("Mode"))
				m_Mode = xnetwork->Attribute("Mode");

			if(xnetwork->Attribute("ServerName"))
				m_ServerName = xnetwork->Attribute("ServerName");
			
			if(xnetwork->Attribute("ServerPort"))
				m_ServerPort = atoi(xnetwork->Attribute("ServerPort"));

			if(xnetwork->Attribute("ClientName"))
				m_ClientName = xnetwork->Attribute("ClientName");
			
			if(xnetwork->Attribute("ClientPort"))
				m_ClientPort = atoi(xnetwork->Attribute("ClientPort"));
		}



		/*if(gui_state && gui_state->Attribute("default"))
		{
			const std::string c_dir = MFCManager::Get().GetConfigurationDirPath();
			GASS::FilePath path;
			m_GUIState = gui_state->Attribute("default");
			path.SetPath(m_GUIState);
			m_GUIState = c_dir + path.GetPath();
		}*/
		if(start_scene && start_scene->Attribute("path"))
		{
			GASS::FilePath path;
			m_StartScene = start_scene->Attribute("path");
			path.SetPath(m_StartScene);
			m_StartScene = path.GetPath();
		}

		if(xTerrainNormal && xTerrainNormal->Attribute("value"))
		{
			std::string value = xTerrainNormal->Attribute("value");
			m_UseTerrainNormalOnDrop = false;
			if(Misc::ToLower(value) == "true")
			{
				m_UseTerrainNormalOnDrop = true;
			}
		}

		if(xRayPickDist && xRayPickDist->Attribute("value"))
		{
			int value = atoi(xRayPickDist->Attribute("value"));
			m_RayPickDist = value;
		}

		if(xscenes)
		{
			TiXmlElement *xItem = xscenes->FirstChildElement("Scene");
			while(xItem)
			{
				std::string name = xItem->Attribute("name");
				std::string path = xItem->Attribute("path");

				GASS::FilePath fp;
				fp.SetPath(path);
				path = fp.GetPath();
				m_SceneNames.push_back(name);
				m_ScenePaths.push_back(path);
				xItem = xItem->NextSiblingElement("Scene");
			}
		}
		if(xscene_path)
		{
			GASS::FilePath path;
			m_ScenePath  = xscene_path->Attribute("value");
			path.SetPath(m_ScenePath);
			m_ScenePath = path.GetPath();

			boost::filesystem::path boost_path(m_ScenePath); 
			if( boost::filesystem::exists(boost_path))  
			{
				boost::filesystem::directory_iterator end ;    
				for( boost::filesystem::directory_iterator iter(boost_path) ; iter != end ; ++iter )      
				{
					if (boost::filesystem::is_directory( *iter ) )      
					{   
						if(boost::filesystem::exists(boost::filesystem::path(iter->path().string() + "/scene.xml")))
						{

							std::string scene_name =iter->path().filename().generic_string();
							m_SceneNames.push_back(scene_name);
							m_ScenePaths.push_back(iter->path().string());
						}
					}
				}     
			}
		}

		if(xtemplates)
		{
			TiXmlElement *xItem = xtemplates->FirstChildElement("Template");
			while(xItem)
			{
				std::string path = xItem->Attribute("path");
				GASS::FilePath fp;
				fp.SetPath(path);
				path = fp.GetPath();
				m_Templates.push_back(path);
				xItem = xItem->NextSiblingElement("Template");
			}
		}

		if(xobj_id)
		{
			std::string value = xobj_id->Attribute("unique");
			if(Misc::ToLower(value) == "true")
				m_UseObjectID = true;
			else
				m_UseObjectID = false;

			m_IDSuffix = xobj_id->Attribute("id_suffix");
			m_IDPrefix = xobj_id->Attribute("id_prefix");
		}

		xmlDoc->Clear();
		// Delete our allocated document and return success ;)
		delete xmlDoc;
		return true;
	}

	void EditorApplication::UnloadScene()
	{
		SceneObjectPtr obj;
		MessagePtr selection_msg(new ObjectSelectedMessage(obj,0));
		//Clear selection!
		EditorManager::GetPtr()->GetMessageManager()->SendImmediate(selection_msg);
		EditorManager::GetPtr()->GetMessageManager()->Clear();
		//free scene
		//if(m_Scene)
		//	m_Scene->Unload();
		m_Scene.reset();
		//clear editor manager messages
		EditorManager::GetPtr()->GetMessageManager()->Clear();
	}

	void  EditorApplication::LoadScene(const std::string &scene_path)
	{
		UnloadScene();
		m_Scene = ScenePtr(new Scene());
		m_Scene->Create();
		m_Scene->Load(scene_path);
	}

	void EditorApplication::NewScene(const std::string &scene_path)
	{
		UnloadScene();
		m_Scene = ScenePtr(new Scene());
		m_Scene->Create();
		//m_Scene->AddSceneScene(scene);
		SaveScene(scene_path);
		LoadScene(scene_path);
	}

	void  EditorApplication::SaveScene(const std::string &scene_path)
	{
		if(m_Scene)
		{
			m_Scene->Save(scene_path);
		}
	}
}

	

