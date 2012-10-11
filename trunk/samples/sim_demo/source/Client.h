#include "SimApplication.h"

class SimClient : public SimApplication
{
public:
	SimClient(const std::string &config) : SimApplication(config), m_IsConnected(false)
	{

	}
	virtual ~SimClient(){}
	bool IsConnected() const {return m_IsConnected;}
	void OnServerResponse(GASS::MessagePtr message)
	{
		GASS::ServerResponseMessagePtr mess = boost::shared_dynamic_cast<GASS::ServerResponseMessage>(message);
		printf("Client got response from server:%s\n",mess->GetServerName().c_str());
		//Connect to server?
		//try to connect
		GASS::SimEngine::Get().GetSimSystemManager()->SendImmediate(GASS::MessagePtr(new GASS::ConnectToServerMessage(mess->GetServerName(),2005)));


		m_IsConnected = true;
	}

	void OnLoadScene(GASS::MessagePtr message)
	{
		GASS::StartSceanrioRequestMessagePtr mess = boost::shared_dynamic_cast<GASS::StartSceanrioRequestMessage>(message);
		printf("Client got scene request message:%s\n",mess->GetSceneName().c_str());
		//m_Scene->Load("../../../common/data/scenes/" + mess->GetSceneName());

		m_Scene = GASS::SimEngine::Get().LoadScene(m_SceneName);
		GASS::ScenePtr scene = GASS::ScenePtr(m_Scene);

		GASS::SceneObjectPtr free_obj = scene->LoadObjectFromTemplate("FreeCameraObject",scene->GetRootSceneObject());
		GASS::MessagePtr pos_msg(new GASS::PositionMessage(scene->GetStartPos()));
		if(free_obj)
		{
			free_obj->SendImmediate(pos_msg);
			GASS::MessagePtr camera_msg(new GASS::ChangeCameraMessage(free_obj,"ALL"));
			scene->PostMessage(camera_msg);
		}
	}

	bool Init()
	{
		m_Engine = new GASS::SimEngine();
		m_Engine->Init(GASS::FilePath("../Configuration/GASS.xml"));

		GASS::GraphicsSystemPtr gfx_sys = m_Engine->GetSimSystemManager()->GetFirstSystem<GASS::IGraphicsSystem>();
		gfx_sys->CreateViewport("MainViewport", "MainWindow", 0,0,1, 1);


		for(int i = 0; i <  m_Templates.size();i++)
		{
			m_Engine->GetSceneObjectTemplateManager()->Load(m_Templates[i]);
		}

		m_Engine->GetSimSystemManager()->RegisterForMessage(REG_TMESS(SimClient::OnServerResponse,GASS::ServerResponseMessage,0));
		m_Engine->GetSimSystemManager()->RegisterForMessage(REG_TMESS(SimClient::OnLoadScene,GASS::StartSceanrioRequestMessage,0));
		m_Engine->GetSimSystemManager()->SendImmediate(GASS::MessagePtr(new GASS::StartClientMessage("SimDemoClient",2006,2005)));

		GASS::ScenePtr scene (new GASS::Scene());
		m_Scene = scene;

		printf("\n\nWaiting for server");
		float update_time = 0;
		while(!IsConnected())
		{
			//update engine (need to process messages)
			m_Engine->Update();

			//send ping request
			m_Engine->GetSimSystemManager()->PostMessage(GASS::MessagePtr(new GASS::PingRequestMessage(2001)));
			update_time += 1.0;
			//			Sleep(1000);
			std::cout << ".";
			//			if(GetAsyncKeyState(VK_ESCAPE))
			//				return false;
		}
		return true;
	}
private:
	bool m_IsConnected;
};



