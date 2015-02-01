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


#include "tinyxml2.h"
#include <boost/filesystem.hpp>

namespace GASS
{
	EditorApplication::EditorApplication(const FilePath &log_folder) : m_Initilized(false), 
		m_Mode("StandAlone"),
		m_ServerName("GASS_SERVER"),
		m_ClientName("GASS_CLIENT"),
		m_ServerPort(2001),
		m_ClientPort(2002)
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

	void EditorApplication::Init(const FilePath &working_folder, const std::string &gass_configuration, void* main_win_handle, void *render_win_handle)
	{	
		const std::string config_path = working_folder.GetFullPath() + "../configuration/";
		//app settings
		LoadSettings(config_path + "EditorApplication.xml");
		SimEngine *se = SimEngine::GetPtr();

		const std::string gass_config_file = config_path + gass_configuration;
		//se->Init(gass_config_file,m_LogFolder);
		se->Init(gass_config_file);

		//load keyboard config!
		ControlSettingsSystemPtr css = se->GetSimSystemManager()->GetFirstSystemByClass<IControlSettingsSystem>();
		css->Load(config_path +  "GASSControlSettings.xml");
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

	bool  EditorApplication::LoadSettings(const std::string &filename)
	{
		tinyxml2::XMLDocument     *xmlDoc = new tinyxml2::XMLDocument();
		if (xmlDoc->LoadFile(filename.c_str()) != tinyxml2::XML_NO_ERROR)
		{
			//Fatal error, cannot load
			return false;
		}
		tinyxml2::XMLElement *xSettings = 0;
		xSettings = xmlDoc->FirstChildElement("EditorApplication");
		tinyxml2::XMLElement *start_scene = xSettings->FirstChildElement("SceneToLoadAtStart");
		tinyxml2::XMLElement *xnetwork = xSettings->FirstChildElement("Network");
		tinyxml2::XMLElement *xupdate_freq = xSettings->FirstChildElement("UpdateFreq");

		
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

}

	

