#pragma once

#include "Core/Common.h"

//avoid warning spam from MYGUI
#   pragma warning (disable : 4100)
#   pragma warning (disable : 4127)
#   pragma warning (disable : 4512)
#   pragma warning (disable : 4267)
#   pragma warning (disable : 4702)

#include <MyGUI.h>
#include "BaseLayout/BaseLayout.h"

namespace GASS
{
	class LoadSceneDialog :	public wraps::BaseLayout
	{
	public:
		LoadSceneDialog(MyGUI::Widget* _parent);
		virtual ~LoadSceneDialog();
		void eventMouseButtonClick(MyGUI::Widget* _sender);
		void setVisible(bool value){m_Window->setVisible(value);}
	private:
		MyGUI::MenuBar* m_MenuBar;
		MyGUI::Button* m_LoadButton;
		MyGUI::Button* m_CancelButton;
		MyGUI::ListBox* m_SceneList;
		MyGUI::Window* m_Window;
	};
}

