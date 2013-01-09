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
		GASS::ClientConnectedEventPtr mess = boost::shared_dynamic_cast<GASS::ClientConnectedEvent>(message);
		printf("Client connected to server:%s",mess->GetClientName().c_str());
	}

	bool Init()
	{
		m_Engine = new GASS::SimEngine();
		m_Engine->Init(GASS::FilePath("../Configuration/GASS.xml"));

		GASS::GraphicsSystemPtr gfx_sys = m_Engine->GetSimSystemManager()->GetFirstSystem<GASS::IGraphicsSystem>();
		gfx_sys->CreateViewport("MainViewport", "MainWindow", 0,0,1, 1);

		m_Engine->GetSimSystemManager()->SendImmediate(GASS::SystemMessagePtr(new GASS::StartServerRequest("SimDemoServer",2005)));
		GASS::MessageFuncPtr callback(new GASS::MessageFunc<GASS::IMessage>(boost::bind( &SimServer::OnClientConnected, this, _1 ),shared_from_this()));
		m_Engine->GetSimSystemManager()->RegisterForMessage(typeid(GASS::ClientConnectedEvent),callback,0);

		for(int i = 0; i <  m_Templates.size();i++)
		{
			m_Engine->GetSceneObjectTemplateManager()->Load(m_Templates[i]);
		}

		m_Scene = m_Engine->LoadScene(m_SceneName);
		GASS::ScenePtr scene = GASS::ScenePtr(m_Scene);
		//create free camera and set start pos
		GASS::SceneObjectPtr free_obj = scene->LoadObjectFromTemplate("FreeCameraObject",scene->GetRootSceneObject());
		GASS::MessagePtr pos_msg(new GASS::PositionMessage(scene->GetStartPos()));
		if(free_obj)
		{
			free_obj->SendImmediate(pos_msg);
			GASS::SceneMessagePtr camera_msg(new GASS::ChangeCameraRequest(free_obj,"ALL"));
			scene->PostMessage(camera_msg);
		}

		for(int i = 0; i <  m_Objects.size();i++)
		{
			GASS::SceneObjectPtr object = scene->LoadObjectFromTemplate(m_Objects[i],scene->GetRootSceneObject());

			GASS::Vec3 pos = scene->GetStartPos();
			pos.x += 10*i;
			GASS::MessagePtr pos_msg(new GASS::PositionMessage(pos));
			if(object)
				object->SendImmediate(pos_msg);
		}
		return true;
	}
};






