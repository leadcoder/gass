#pragma once

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

