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

		GASS::GraphicsSystemPtr gfx_sys = m_Engine->GetSimSystemManager()->GetFirstSystem<GASS::IGraphicsSystem>();
		gfx_sys->CreateViewport("MainViewport", "MainWindow", 0,0,1, 1);


		m_Engine->GetSimSystemManager()->SendImmediate(GASS::MessagePtr(new GASS::StartServerMessage("SimDemoServer",2005)));
		GASS::MessageFuncPtr callback(new GASS::MessageFunc<GASS::IMessage>(boost::bind( &SimServer::OnClientConnected, this, _1 ),shared_from_this()));
		m_Engine->GetSimSystemManager()->RegisterForMessage(typeid(GASS::ClientConnectedMessage),callback,0);

		for(int i = 0; i <  m_Templates.size();i++)
		{
			m_Engine->GetSceneObjectTemplateManager()->Load(m_Templates[i]);
		}

		GASS::ScenePtr scene (new GASS::Scene());
		m_Scene = scene;
		m_Scene->Load(m_SceneName);
		//create free camera and set start pos
		GASS::SceneObjectPtr free_obj = m_Scene->LoadObjectFromTemplate("FreeCameraObject",m_Scene->GetRootSceneObject());
		GASS::MessagePtr pos_msg(new GASS::PositionMessage(m_Scene->GetStartPos()));
		if(free_obj)
		{
			free_obj->SendImmediate(pos_msg);
			GASS::MessagePtr camera_msg(new GASS::ChangeCameraMessage(free_obj,"ALL"));
			m_Scene->PostMessage(camera_msg);
		}

		for(int i = 0; i <  m_Objects.size();i++)
		{
			GASS::SceneObjectPtr object = m_Scene->LoadObjectFromTemplate(m_Objects[i],m_Scene->GetRootSceneObject());

			GASS::Vec3 pos = m_Scene->GetStartPos();
			pos.x += 10*i;
			GASS::MessagePtr pos_msg(new GASS::PositionMessage(pos));
			if(object)
				object->SendImmediate(pos_msg);
		}
		return true;
	}
};






