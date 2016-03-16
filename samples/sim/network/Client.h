#include "SimApplication.h"

class SimClient : public SimApplication
{
public:
	SimClient() : SimApplication(), m_IsConnected(false)
	{

	}
	virtual ~SimClient(){}
	bool IsConnected() const {return m_IsConnected;}
	void OnServerResponse(GASS::ServerResponseEventPtr message)
	{
		printf("Client got response from server:%s\n",message->GetServerName().c_str());
		
		//Connect to server...
		GASS::SimEngine::Get().GetSimSystemManager()->SendImmediate(GASS::SystemMessagePtr(new GASS::ConnectToServerRequest(message->GetServerName(),m_ServerPort)));
		m_IsConnected = true;
	}

	void OnLoadSceneRequest(GASS::LoadSceneRequestPtr message)
	{
		printf("Client got scene request message:%s\n",message->GetSceneName().c_str());
		_LoadScene(message->GetSceneName());
		/*m_Scene = GASS::SimEngine::Get().CreateScene(mess->GetSceneName());
		GASS::ScenePtr scene = GASS::ScenePtr(m_Scene);
		scene->Load(mess->GetSceneName());

		GASS::LogManager::getSingleton().stream() << "SimApplication::Init -- Scene Loaded:" << m_SceneName;

		//create free camera and set start pos
		GASS::SceneObjectPtr free_obj = scene->LoadObjectFromTemplate("FreeCameraObject",scene->GetRootSceneObject());
		GASS::PositionRequestPtr pos_msg(new GASS::PositionRequest(scene->GetStartPos()));
		if(free_obj)
		{
			free_obj->SendImmediateRequest(pos_msg);
			GASS::SystemMessagePtr camera_msg(new GASS::ChangeCameraRequest(free_obj->GetFirstComponentByClass<GASS::ICameraComponent>()));
			m_Engine->GetSimSystemManager()->PostMessage(camera_msg);
		}*/
	}	

	bool Init()
	{
		_CreateMainWindow();

		//listen for server response
		m_Engine->GetSimSystemManager()->RegisterForMessage(REG_TMESS(SimClient::OnServerResponse,GASS::ServerResponseEvent,0));


		//listen when server change scenery
		m_Engine->GetSimSystemManager()->RegisterForMessage(REG_TMESS(SimClient::OnLoadSceneRequest,GASS::LoadSceneRequest,0));
		
		//Start client
		m_Engine->GetSimSystemManager()->SendImmediate(GASS::SystemMessagePtr(new GASS::StartClientRequest("SimDemoClient",m_ClientPort,m_ServerPort)));
		printf("\n\nWaiting for server to start");
		//float update_time = 0;
		while(!IsConnected())
		{
			//update engine (need to process messages)
			m_Engine->Update();
			//send ping request
			m_Engine->GetSimSystemManager()->PostMessage(GASS::SystemMessagePtr(new GASS::PingRequest(m_ServerPort)));
			Sleep(500);
			std::cout << ".";
		}
		return true;
	}
private:
	bool m_IsConnected;
};

