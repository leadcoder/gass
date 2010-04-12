// CoreTest.cpp : Defines the entry point for the console application.
//
#include "SimApplication.h"


class SimServer : public SimApplication
{
public:
	SimServer(const std::string config,const std::string &data_path ="") : SimApplication(config,data_path)
	{
	}
	virtual ~SimServer()
	{

	}
	
	void OnClientConnected(GASS::MessagePtr message)
	{
		GASS::ClientConnectedMessagePtr mess = boost::shared_dynamic_cast<GASS::ClientConnectedMessage>(message);
		printf("Client connected to server:%s",mess->GetClientName().c_str());
	}

	bool Init()
	{
		//std::string data_path = getenv("GASS_DATA_PATH");
		std::string data_path = "";
		

		m_Engine = new GASS::SimEngine();
		m_Engine->Init(m_Plugins,m_SystemConfig,m_ControlSettings);
		
		m_Engine->GetSimSystemManager()->SendImmediate(GASS::MessagePtr(new GASS::StartServerMessage("SimDemoServer",2001)));
		GASS::MessageFuncPtr callback(new GASS::MessageFunc<GASS::IMessage>(boost::bind( &SimServer::OnClientConnected, this, _1 ),this));
		m_Engine->GetSimSystemManager()->RegisterForMessage(typeid(GASS::ClientConnectedMessage),callback,0);

		
		for(int i = 0; i <  m_Templates.size();i++)
		{
			m_Engine->GetSimObjectManager()->Load(data_path + m_Templates[i]);
		}


		GASS::ScenarioPtr scenario (new GASS::Scenario());
		m_Scenario = scenario;
		m_Scenario->Load(data_path + m_ScenarioName);

		

		//if(m_Instances != "")
		//	scenario->GetScenarioScenes().at(0)->GetObjectManager()->LoadFromFile(m_Instances);
		for(int i = 0; i <  m_Objects.size();i++)
		{
			
			GASS::SceneObjectPtr object = m_Scenario->GetScenarioScenes().at(0)->GetObjectManager()->LoadFromTemplate(m_Objects[i]);
			
			GASS::Vec3 pos = m_Scenario->GetScenarioScenes().at(0)->GetStartPos();
			pos.x += 10*i;
			GASS::MessagePtr pos_msg(new GASS::PositionMessage(pos));
			if(object)
				object->SendImmediate(pos_msg);
		}
		return true;
	}
};






