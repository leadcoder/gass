// CoreTest.cpp : Defines the entry point for the console application.
//
#include "SimApplication.h"


class SimServer : public SimApplication
{
public:
	SimServer(const std::string config) : SimApplication(config)
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
		m_Engine = new GASS::SimEngine();
		m_Engine->Init(m_Plugins,m_SystemConfig,m_ControlSettings);
		
		m_Engine->GetSimSystemManager()->SendImmediate(GASS::MessagePtr(new GASS::StartServerMessage("SimDemoServer",2005)));
		GASS::MessageFuncPtr callback(new GASS::MessageFunc<GASS::IMessage>(boost::bind( &SimServer::OnClientConnected, this, _1 ),this));
		m_Engine->GetSimSystemManager()->RegisterForMessage(typeid(GASS::ClientConnectedMessage),callback,0);

		for(int i = 0; i <  m_Templates.size();i++)
		{
			m_Engine->GetSimObjectManager()->Load(m_Templates[i]);
		}

		GASS::ScenarioPtr scenario (new GASS::Scenario());
		m_Scenario = scenario;
		if(m_Scenario->Load(m_ScenarioName))
		{
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
		}
		else
		{
			GASS::Log::Error("Failed to load scenario %s", m_ScenarioName.c_str());
			return false;
		}
		return true;
	}
};






