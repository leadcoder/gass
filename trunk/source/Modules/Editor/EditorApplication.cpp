#include "EditorApplication.h"
#include "Modules/Editor/EditorSystem.h"
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
#include "Modules/Editor/ToolSystem/GoToPositionTool.h"
#include "Modules/Editor/ToolSystem/EditPositionTool.h"


#include "tinyxml.h"
#include <boost/filesystem.hpp>

namespace GASS
{
	EditorApplication::EditorApplication() : m_Initilized(false), 
		m_Mode("StandAlone"),
		m_ServerName("GASS_SERVER"),
		m_ClientName("GASS_CLIENT"),
		m_ServerPort(2001),
		m_ClientPort(2002)
	{
		new SimEngine();
		LogManager* log_man = new LogManager();
		log_man->createLog("GASS.log", true, true);
	}

	EditorApplication::~EditorApplication(void)
	{
		delete SimEngine::GetPtr();
	}

	void EditorApplication::Init(const FilePath &working_folder, const FilePath &appdata_folder_path, const FilePath &mydocuments_folder_path, const std::string &render_system, void* main_win_handle, void *render_win_handle)
	{	
		const std::string config_path = working_folder.GetFullPath() + "../configuration/";
		//app settings
		LoadSettings(config_path + "EditorApplication.xml");
		SimEngine *se = SimEngine::GetPtr();

		se->SetScenePath(FilePath(se->GetScenePath().GetFullPath() + render_system + "/" ));
		
		const std::string gass_config_file = config_path + "GASS" + render_system + ".xml";
		se->Init(gass_config_file);

		//load keyboard config!
		ControlSettingsSystemPtr css = se->GetSimSystemManager()->GetFirstSystemByClass<IControlSettingsSystem>();
		if(css)
		{
			css->Load(config_path +  "GASSControlSettings.xml");
		}

//		se->GetSimSystemManager()->RegisterForMessage(REG_TMESS(EditorApplication::OnSceneCreated,PostSceneCreateEvent,0));
		EditorSystemPtr es = se->GetSimSystemManager()->GetFirstSystemByClass<EditorSystem>();
		if(!es)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get EditorSystem", "EditorApplication::Init");
		es->SetPaths(working_folder,appdata_folder_path,mydocuments_folder_path);

		//Start client or server?
		if(Misc::ToLower(m_Mode) == "server")
		{
			SystemMessagePtr server_mess(new StartServerRequest(m_ServerName, m_ServerPort));
			se->GetSimSystemManager()->PostMessage(server_mess);
		}
		else if (Misc::ToLower(m_Mode) == "client")
		{
			se->GetSimSystemManager()->SendImmediate(SystemMessagePtr(new StartClientRequest(m_ClientName, m_ClientPort,m_ServerPort)));
			SystemMessagePtr connect_message(new ConnectToServerRequest(m_ServerName, m_ServerPort,8888, 2));
			se->GetSimSystemManager()->PostMessage(connect_message);
		}

		//Create main render window, this will trigger
		const std::string render_win_name = "RenderWindow";
		GASS::GraphicsSystemPtr gfx_system = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::IGraphicsSystem>();
		RenderWindowPtr win = gfx_system->CreateRenderWindow(render_win_name,800,600,render_win_handle);
		win->CreateViewport("MainViewport", 0, 0, 1, 1);
	
		GASS::InputSystemPtr input_system = GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<GASS::IInputSystem>();
		input_system->SetMainWindowHandle(main_win_handle);
		m_Initilized  = true;
	}

	void EditorApplication::Update()
	{
		SimEngine::Get().Update();
	}

	/*void EditorApplication::OnLoadScene(PostSceneCreateEventPtr message)
	{
		ScenePtr scene = message->GetScene();
		//load top camera
		Vec3 vel(0,0,0);
		Vec3 pos = scene->GetStartPos();
		Quaternion rot(scene->GetStartRot());
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
			SystemMessagePtr camera_msg(new ChangeCameraRequest(free_obj->GetFirstComponentByClass<ICameraComponent>()));
			SimEngine::Get().GetSimSystemManager()->PostMessage(camera_msg);
		}

		SceneObjectPtr top_obj = scene->LoadObjectFromTemplate("TopCameraObject",scene->GetRootSceneObject());
		if(top_obj)
			top_obj->SendImmediate(pos_msg);
	}*/

	bool  EditorApplication::LoadSettings(const std::string &filename)
	{
		TiXmlDocument     *xmlDoc = new TiXmlDocument(filename.c_str());
		if (!xmlDoc->LoadFile())
		{
			//Fatal error, cannot load
			return false;
		}
		TiXmlElement *xSettings = 0;
		xSettings = xmlDoc->FirstChildElement("EditorApplication");
		TiXmlElement *start_scene = xSettings->FirstChildElement("SceneToLoadAtStart");
		TiXmlElement *gui_state = xSettings->FirstChildElement("GUIState");
		TiXmlElement *xnetwork = xSettings->FirstChildElement("Network");
		TiXmlElement *xexternal_update = xSettings->FirstChildElement("ExternalUpdate");
		TiXmlElement *xupdate_freq = xSettings->FirstChildElement("UpdateFreq");

		if(xupdate_freq)
		{
			m_UpdateFreq = atoi(xupdate_freq->Attribute("value"));
		}

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
		if(start_scene && start_scene->Attribute("value"))
		{
			m_StartScene = start_scene->Attribute("value");
		}


		xmlDoc->Clear();
		// Delete our allocated document and return success ;)
		delete xmlDoc;
		return true;
	}
	

/*	void EditorApplication::NewScene()
	{
		m_Scene = ScenePtr(SimEngine::Get().NewScene());
	}*/

	/*void EditorApplication::SaveScene(const FilePath &path)
	{
		SimEngine::Get().SaveScene(path);
	}*/

}

	

