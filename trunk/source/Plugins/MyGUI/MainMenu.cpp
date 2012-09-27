#include "MainMenu.h"

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

		m_MenuBar->eventMenuCtrlAccept += MyGUI::newDelegate(this, &MainMenu::notifyMenuCtrlAccept);
	}

	MainMenu::~MainMenu()
	{
	}

	void MainMenu::notifyFileMenu(MyGUI::MenuControl* _sender, MyGUI::MenuItem* _item)
	{
		//MyGUI::Widget* widget = *_item->getItemData<MyGUI::Widget*>();
	}

	void MainMenu::notifyMenuCtrlAccept(MyGUI::MenuControl* _sender, MyGUI::MenuItem* _item)
	{
		const std::string& item_id = _item->getItemId();
		if(item_id == "Quit")
		{
			exit(0);
		}
		/*MyGUI::UString* data = _item->getItemData<MyGUI::UString>(false);
		if (data != nullptr)
			CommandManager::getInstance().setCommandData(*data);

		const std::string& command = _item->getItemId();
		if (MyGUI::utility::startWith(command, "Command_"))
			CommandManager::getInstance().executeCommand(command);*/
	}
} 