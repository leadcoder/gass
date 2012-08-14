#ifndef GASS_MAIN_MENU_H
#define GASS_MAIN_MENU_H

#include <MyGUI.h>
#include "BaseLayout/BaseLayout.h"

namespace GASS
{

	ATTRIBUTE_CLASS_LAYOUT(MainMenu, "GASSMainMenu.layout");
	
	class MainMenu :
		public wraps::BaseLayout
	{
	public:
		MainMenu(MyGUI::Widget* _parent = nullptr);
		virtual ~MainMenu();
		void Init();
	private:
		
	};
} 

#endif 
