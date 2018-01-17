// CoreTest.cpp : Defines the entry point for the console application.
//
#include "SimApplication.h"
#include "Sim/Messages/GASSPlatformMessages.h"


class SimServer : public SimApplication
{
public:
	SimServer() : SimApplication()
	{
	}
	virtual ~SimServer()
	{

	}

	void OnClientConnected(GASS::ClientConnectedEventPtr message)
	{
		printf("Client connected to server:%s",message->GetClientName().c_str());

		//create new vehicle and send enter request
		GASS::ScenePtr scene(m_Scene);
		GASS::SceneObjectPtr object  = scene->LoadObjectFromTemplate("GTO",scene->GetRootSceneObject());
		GASS::Vec3 pos = scene->GetStartPos();
		static int clients_connected =0;
		clients_connected++;
		pos.x += clients_connected*5;
		if(object)
		{
			object->SendImmediateRequest(GASS::WorldPositionRequestPtr(new GASS::WorldPositionRequest(pos)));
			object->PostRequest(GASS::ClientRemoteMessagePtr(new GASS::ClientRemoteMessage(message->GetClientName(),"EnterVehicle","")));
		}
	}

	bool Init()
	{
		//same for client and server
		_CreateMainWindow();

		//do server stuff...

		//Add callback for client connections
		m_Engine->GetSimSystemManager()->RegisterForMessage(REG_TMESS(SimServer::OnClientConnected,GASS::ClientConnectedEvent,0));
		

		//request server start
		GASS_LOG(LINFO) << "ServerApplication::Init -- Start Loading Scene:" << m_SceneName;
		m_Engine->GetSimSystemManager()->SendImmediate(GASS::SystemMessagePtr(new GASS::StartServerRequest("SimDemoServer",2005)));
	
		_LoadScene(m_SceneName);
	
		//create server objects
		GASS::ScenePtr scene(m_Scene);
//		GASS::SceneObjectPtr object  = scene->LoadObjectFromTemplate("PXGTO",scene->GetRootSceneObject());
		GASS::SceneObjectPtr object  = scene->LoadObjectFromTemplate("GTO",scene->GetRootSceneObject());
		GASS::Vec3 pos = scene->GetStartPos();

		if(object)
			object->SendImmediateRequest(GASS::WorldPositionRequestPtr(new GASS::WorldPositionRequest(pos)));
		
		return true;
	}
};






