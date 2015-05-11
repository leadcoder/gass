#ifndef GASS_MAIN_MENU_H
#define GASS_MAIN_MENU_H

#include <MyGUI.h>
#include "BaseLayout/BaseLayout.h"

namespace GASS
{
	class LoadSceneDialog;
	ATTRIBUTE_CLASS_LAYOUT(MainMenu, "GASSMainMenu.layout");
	
	class MainMenu :
		public wraps::BaseLayout
	{
	public:
		MainMenu(MyGUI::Widget* _parent = nullptr);
		virtual ~MainMenu();
		void Init();
		void notifyMenuCtrlAccept(MyGUI::MenuControl* _sender, MyGUI::MenuItem* _item);
		void notifyFileMenu(MyGUI::MenuControl* _sender, MyGUI::MenuItem* _item);
		void notifyResourceMenu(MyGUI::MenuControl* _sender, MyGUI::MenuItem* _item);
	private:
		MyGUI::MenuBar* m_MenuBar;
		LoadSceneDialog *m_LoadSceneDialog;

		MyGUI::MenuItemPtr m_ItemStartStopSim;
		MyGUI::MenuItemPtr m_ItemPauseSim;
	};
} 

#endif 
