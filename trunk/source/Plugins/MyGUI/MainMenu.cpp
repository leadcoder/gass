#include "MainMenu.h"

namespace GASS
{


	MainMenu::MainMenu(MyGUI::Widget* _parent)
	{
		initialiseByAttributes(this, _parent);
		
		
	}

	void MainMenu::Init()
	{
		MyGUI::Widget* m_File;
		assignWidget(m_File, "File");
		MyGUI::Widget* q_widget = m_File->findWidget("Quit");
	}

	MainMenu::~MainMenu()
	{
	}
} 
