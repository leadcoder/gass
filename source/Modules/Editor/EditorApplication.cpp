#include "EditorApplication.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"
#include "Sim/GASS.h"
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{
	EditorApplication::EditorApplication(const FilePath &log_folder) : m_Initilized(false), 
		m_Mode("StandAlone"),
		m_ServerName("GASS_SERVER"),
		m_ClientName("GASS_CLIENT"),
		m_ServerPort(2001),
		m_ClientPort(2002),
		m_UpdateFreq(20)
	{
		new SimEngine(log_folder);
		//LogManager* log_man = new LogManager();
		//const std::string log_file = log_folder.GetFullPath() + "GASS.log";
		//log_man->createLog(log_file, true, true);
		//m_LogFolder = log_folder;
	}

	EditorApplication::~EditorApplication(void)
	{
		delete SimEngine::GetPtr();
	}

	void EditorApplication::Init(const FilePath &editor_config, const FilePath &gass_configuration, const FilePath &control_configuration, void* main_win_handle, void *render_win_handle)
	{	
		//app settings
		LoadSettings(editor_config.GetFullPath());
		SimEngine *se = SimEngine::GetPtr();
		
		//se->Init(gass_config_file,m_LogFolder);
		se->Init(gass_configuration);

		//load additional keyboard configuration
		ControlSettingsSystemPtr css = se->GetSimSystemManager()->GetFirstSystemByClass<IControlSettingsSystem>();
		css->Load(control_configuration.GetFullPath());
		//Start client or server?
		if(StringUtils::ToLower(m_Mode) == "server")
		{
			
			se->GetSimSystemManager()->PostMessage(SystemMessagePtr(new StartServerRequest(m_ServerName, m_ServerPort)));
		}
		else if (StringUtils::ToLower(m_Mode) == "client")
		{
			se->GetSimSystemManager()->SendImmediate(SystemMessagePtr(new StartClientRequest(m_ClientName, m_ClientPort,m_ServerPort)));
			SystemMessagePtr connect_message(new ConnectToServerRequest(m_ServerName, m_ServerPort,8888, 2));
			se->GetSimSystemManager()->PostMessage(connect_message);
		}

		//Create main render window, this will trigger
		const std::string render_win_name = "RenderWindow";
		GraphicsSystemPtr gfx_system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IGraphicsSystem>();
		RenderWindowPtr win = gfx_system->CreateRenderWindow(render_win_name,800,600,render_win_handle);
		win->CreateViewport("MainViewport", 0, 0, 1, 1);
	
		InputSystemPtr input_system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IInputSystem>();
		input_system->SetMainWindowHandle(main_win_handle);
		m_Initilized  = true;
	}

	bool EditorApplication::Update()
	{
		return SimEngine::Get().Update();
	}

	bool  EditorApplication::LoadSettings(const std::string &filename)
	{
		auto     *xml_doc = new tinyxml2::XMLDocument();
		if (xml_doc->LoadFile(filename.c_str()) != tinyxml2::XML_NO_ERROR)
		{
			//Fatal error, cannot load
			return false;
		}
		tinyxml2::XMLElement *x_settings = nullptr;
		x_settings = xml_doc->FirstChildElement("EditorApplication");
		tinyxml2::XMLElement *start_scene = x_settings->FirstChildElement("SceneToLoadAtStart");
		tinyxml2::XMLElement *xnetwork = x_settings->FirstChildElement("Network");
		tinyxml2::XMLElement *xupdate_freq = x_settings->FirstChildElement("UpdateFreq");

		
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
		xml_doc->Clear();
		// Delete our allocated document and return success ;)
		delete xml_doc;
		return true;
	}

}

	

