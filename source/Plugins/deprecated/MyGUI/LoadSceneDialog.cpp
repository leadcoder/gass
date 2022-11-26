#include "LoadSceneDialog.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSScene.h"

namespace GASS
{
	LoadSceneDialog::LoadSceneDialog(MyGUI::Widget* _parent)
	{
		const MyGUI::IntSize& size = MyGUI::RenderManager::getInstance().getViewSize();
		const int win_w = 200;
		const int win_h = 300;

		MyGUI::IntSize win_size(win_w, win_h);
		m_Window = MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("WindowCS", MyGUI::IntCoord(size.width/2 - win_w/2, size.height/2 - win_h/2, win_w, win_h), MyGUI::Align::Default, "Overlapped","LoadSceneWindow");
		m_Window->setCaption("Select Scene");
		m_Window->setMinSize(win_size);
		m_Window->setMaxSize(win_size);
		MyGUI::Canvas* canvas = m_Window->createWidget<MyGUI::Canvas>("Canvas", MyGUI::IntCoord(0, 0, m_Window->getClientCoord().width, m_Window->getClientCoord().height), MyGUI::Align::Default,"LoadSceneCanvas");
		m_SceneList = canvas->createWidget<MyGUI::ListBox>("ListBox",0,0,200,200,MyGUI::Align::Center,"LoadSceneList");
		std::vector<std::string> scenes = SimEngine::Get().GetSavedScenes();
		for(size_t i = 0; i< scenes.size();i++)
		{
			m_SceneList->addItem(scenes[i]);
		}
		m_LoadButton = canvas->createWidget<MyGUI::Button>("Button", MyGUI::IntCoord(0, 200, 100, 20), MyGUI::Align::Bottom,"LoadSceneButton");
		m_LoadButton->setTextAlign(MyGUI::Align::Center);
		m_LoadButton->setCaption("Load");
		m_LoadButton->eventMouseButtonClick+= MyGUI::newDelegate(this, &LoadSceneDialog::eventMouseButtonClick);

		m_CancelButton = canvas->createWidget<MyGUI::Button>("Button", MyGUI::IntCoord(100, 200, 100, 20), MyGUI::Align::Bottom,"CancelSceneButton");
		m_CancelButton->setTextAlign(MyGUI::Align::Center);
		m_CancelButton->setCaption("Cancel");
		m_CancelButton->eventMouseButtonClick+= MyGUI::newDelegate(this, &LoadSceneDialog::eventMouseButtonClick);
	}

	LoadSceneDialog::~LoadSceneDialog()
	{

	}
	
	void LoadSceneDialog::eventMouseButtonClick(MyGUI::Widget* _sender)
	{
		if (_sender == m_LoadButton)
		{
			const size_t index = m_SceneList->getIndexSelected();
			if(index > -1)
			{
				const std::string scene_name = m_SceneList->getItemNameAt(index);
				//Assume first scene is main scene
				//ScenePtr scene = SimEngine::Get().GetScenes().getNext();
				SimEngine::SceneIterator iter = SimEngine::Get().GetScenes();
				if(iter.hasMoreElements())
				{
					ScenePtr scene = iter.getNext();
					scene->Load(scene_name );
					m_Window->setVisible(false);
				}
			}
		}
		else if (_sender == m_CancelButton)
		{
			m_Window->setVisible(false);
		}
	}
} 
