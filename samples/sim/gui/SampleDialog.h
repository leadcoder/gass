#pragma once

//avoid warning spam from MYGUI
#   pragma warning (disable : 4100)
#   pragma warning (disable : 4127)
#   pragma warning (disable : 4512)
#   pragma warning (disable : 4267)
#   pragma warning (disable : 4702)

#include <MyGUI.h>
#include "Plugins/MyGUI/BaseLayout/BaseLayout.h"

namespace GASS
{
	class SampleDialog :	public wraps::BaseLayout
	{
	public:
		SampleDialog(MyGUI::Widget* _parent);
		virtual ~SampleDialog();
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

