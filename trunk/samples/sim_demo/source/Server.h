// CoreTest.cpp : Defines the entry point for the console application.
//
#include "SimApplication.h"


class SimServer : public SimApplication
{
public:
	SimServer() : SimApplication()
	{
	}
	virtual ~SimServer()
	{

	}

	void OnClientConnected(GASS::MessagePtr message)
	{
		GASS::ClientConnectedEventPtr mess = DYNAMIC_PTR_CAST<GASS::ClientConnectedEvent>(message);
		printf("Client connected to server:%s",mess->GetClientName().c_str());
	}

	bool Init()
	{
		//same for client and server
		_CreateMainWindow();

		//server stuff

		//Add callback for client connections
		GASS::MessageFuncPtr callback(new GASS::MessageFunc<GASS::IMessage>(boost::bind( &SimServer::OnClientConnected, this, _1 ),shared_from_this()));
		m_Engine->GetSimSystemManager()->RegisterForMessage(typeid(GASS::ClientConnectedEvent),callback,0);

		//request server start
		GASS::LogManager::getSingleton().stream() << "ServerApplication::Init -- Start Loading Scene:" <<  m_SceneName;
		m_Engine->GetSimSystemManager()->SendImmediate(GASS::SystemMessagePtr(new GASS::StartServerRequest("SimDemoServer",2005)));
	
		_LoadScene(m_SceneName);
	
		//create server objects
		GASS::ScenePtr scene(m_Scene);
		GASS::SceneObjectPtr object  = scene->LoadObjectFromTemplate("JimTank",scene->GetRootSceneObject());
		GASS::Vec3 pos = scene->GetStartPos();

		if(object)
			object->SendImmediateRequest(GASS::WorldPositionRequestPtr(new GASS::WorldPositionRequest(pos)));

		/*for(int i = 0; i <  m_ServerObjects.size();i++)
		{
			GASS::SceneObjectPtr object = scene->LoadObjectFromTemplate(m_ServerObjects[i],scene->GetRootSceneObject());
			GASS::Vec3 pos = scene->GetStartPos();
			pos.x += 10*i;
			GASS::MessagePtr pos_msg(new GASS::PositionRequest(pos));
			if(object)
				object->SendImmediate(pos_msg);
		}*/
		return true;
	}
};






