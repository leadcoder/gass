#include "MainMenu.h"
#include "LoadSceneDialog.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"

namespace GASS
{
	MainMenu::MainMenu(MyGUI::Widget* _parent)
	{
		initialiseByAttributes(this, _parent);
	}

	void MainMenu::Init()
	{
		assignWidget(m_MenuBar, "MainMenu");
		MyGUI::MenuItem* file = m_MenuBar->findItemById("File");
		file->getItemChild()->eventMenuCtrlAccept += MyGUI::newDelegate(this, &MainMenu::notifyFileMenu);

		MyGUI::MenuItemPtr mi = m_MenuBar->createWidget<MyGUI::MenuItem>("MenuBarButton", 0, 0, 60, 20,  MyGUI::Align::Default);
		//MyGUI::MenuItemPtr mi = m_MenuBar->createWidget<MyGUI::MenuItem>("MenuBarButton");
		MyGUI::PopupMenuPtr p = mi->createWidget<MyGUI::PopupMenu>(MyGUI::WidgetStyle::Popup, "PopupMenu",MyGUI::IntCoord(0,0,88,68),MyGUI::Align::Default, "Popup");
		mi->setItemType(MyGUI::MenuItemType::Popup);
		mi->setCaption(("Simulation"));
		p->setPopupAccept(true);
		
		//m_ItemStartStopSim = p->addItem("StartSim", MyGUI::MenuItemType::Normal,"Command_StartStopSim");
		m_ItemPauseSim = p->addItem("PauseSim", MyGUI::MenuItemType::Normal,"Command_PauseSim");
		//p->addItem(("get new Vehicl2e"), MyGUI::MenuItemType::Normal);

/*		MyGUI::MenuItem* item = m_MenuBar->addItem("Simulation");
		MyGUI::MenuControl* control = item->createItemChild();
		control->addItem("sIMmENU", MyGUI::MenuItemType::Popup);
		control->setPopupAccept(true);
//		control->eventMenuCtrlAccept += MyGUI::newDelegate(this, &MainMenuControl::notifyWidgetsSelect);
		MyGUI::MenuControl* child = control->createItemChildAt(control->getItemCount() - 1);
		//child->eventMenuCtrlAccept += MyGUI::newDelegate(this, &MainMenuControl::notifyWidgetsSelect);
		child->setPopupAccept(true);
		child->addItem("Pause", MyGUI::MenuItemType::Normal);
	*/	//control->setItemName("Pause");


		MyGUI::MenuItem* res = m_MenuBar->findItemById("Resources");
		res->getItemChild()->eventMenuCtrlAccept += MyGUI::newDelegate(this, &MainMenu::notifyResourceMenu);

		m_MenuBar->eventMenuCtrlAccept += MyGUI::newDelegate(this, &MainMenu::notifyMenuCtrlAccept);
		m_MenuBar->setVisible(true);
		m_LoadSceneDialog = new LoadSceneDialog(NULL);
		m_LoadSceneDialog->setVisible(false);
	}

	MainMenu::~MainMenu()
	{

	}

	void MainMenu::notifyFileMenu(MyGUI::MenuControl* _sender, MyGUI::MenuItem* _item)
	{
		//MyGUI::Widget* widget = *_item->getItemData<MyGUI::Widget*>();
	}

	void MainMenu::notifyResourceMenu(MyGUI::MenuControl* _sender, MyGUI::MenuItem* _item)
	{
		//MyGUI::Widget* widget = *_item->getItemData<MyGUI::Widget*>();
	}

	void MainMenu::notifyMenuCtrlAccept(MyGUI::MenuControl* _sender, MyGUI::MenuItem* _item)
	{
		const std::string& item_id = _item->getItemId();
		if(item_id == "Command_Quit")
		{
			exit(0);
		}

		else if(item_id == "Command_FileLoad")
		{
			m_LoadSceneDialog->setVisible(true);
		}
		else if(item_id == "Command_ReloadAllResources")
		{
			SystemMessagePtr message(new ReloadMaterial());
			SimEngine::Get().GetSimSystemManager()->PostMessage(message);
		}
		else if(item_id == "Command_StartSim")
		{
			SimulationState sim_state = SimEngine::Get().GetRunTimeController()->GetSimulationState();
			//m_ItemStartSim->setStateCheck(true);
			if(sim_state == SS_RUNNING)
			{
				SimEngine::Get().GetRunTimeController()->StopSimulation();
				m_ItemStartStopSim->setItemName("StartSim");
			}
			else if(sim_state == SS_STOPPED)
			{
				SimEngine::Get().GetRunTimeController()->StartSimulation();
				m_ItemStartStopSim->setItemName("StopSim");
			}
		}
		else if(item_id == "Command_PauseSim")
		{
			SimulationState sim_state = SimEngine::Get().GetRunTimeController()->GetSimulationState();
			if(sim_state == SS_RUNNING)
			{
				SimEngine::Get().GetRunTimeController()->SetSimulationPaused(true);
				m_ItemPauseSim->setItemName("ResumeSim");
			}
			else if(sim_state == SS_PAUSED)
			{
				SimEngine::Get().GetRunTimeController()->SetSimulationPaused(true);
				m_ItemPauseSim->setItemName("PauseSim");
			}
		}
		/*MyGUI::UString* data = _item->getItemData<MyGUI::UString>(false);
		if (data != nullptr)
			CommandManager::getInstance().setCommandData(*data);

		const std::string& command = _item->getItemId();
		if (MyGUI::utility::startWith(command, "Command_"))
			CommandManager::getInstance().executeCommand(command);*/
	}
	
} 
