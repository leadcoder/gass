/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
*                                                                           *
* GASS is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* GASS is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with GASS. If not, see <http://www.gnu.org/licenses/>.              *
*****************************************************************************/

#include <stdio.h>
#include "Sim/Messages/GASSScriptSystemMessages.h"
#include "MyGUISystem.h"
#include <MyGUI.h>
#include "MyGUI_LastHeader.h"
#include <MyGUI_OgrePlatform.h>
#include "StatisticInfo.h"
#include "MainMenu.h"

namespace GASS
{
	MyGUISystem::MyGUISystem() : mGUI(NULL)
	{
	}

	MyGUISystem::~MyGUISystem()
	{

	}

	void MyGUISystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("MyGUISystem",new GASS::Creator<MyGUISystem, ISystem>);
	}

	void MyGUISystem::OnCreate(SystemManagerPtr owner)
	{
		SimSystem::OnCreate(owner);
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(MyGUISystem::OnLoadGUIScript,GUIScriptMessage,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(MyGUISystem::OnInputSystemLoaded,InputSystemLoadedMessage,0));
	}

	void MyGUISystem::Init()
	{
		SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);
	}

	void MyGUISystem::OnInputSystemLoaded(InputSystemLoadedMessagePtr message)
	{
		InputSystemPtr input_system = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IInputSystem>();
		input_system->AddKeyListener(this);
		input_system->AddMouseListener(this);
	}

	diagnostic::StatisticInfo* mInfo =NULL;

	void MyGUISystem::OnLoadGUIScript(GUIScriptMessagePtr message)
	{
		//CEGUI::System::getSingleton().executeScriptFile(message->GetFilename());
		MyGUI::OgrePlatform* mPlatform = new MyGUI::OgrePlatform();
		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		//Ogre::Camera* ocam = sm->getCameraIterator().getNext();
		Ogre::RenderTarget *target = NULL;
		if (Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator().hasMoreElements())
			target = Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator().getNext();
		mPlatform->initialise((Ogre::RenderWindow*)target, sm);
		mGUI = new MyGUI::Gui();
		mGUI->initialise("MyGUI_Core.xml");
		mInfo = new diagnostic::StatisticInfo();
		mInfo->setVisible(true);

		MainMenu* menu = new MainMenu(NULL);
		menu->Init();
		

		//load main menu from layout

		/*MyGUI::MenuCtrlPtr menu = mGUI->createWidget<MyGUI::MenuCtrl>("PopupMenu", MyGUI::IntCoord(200, 20, 150, 100), MyGUI::Align::Default, "Overlapped");

		MyGUI::MenuItemPtr item1 = menu->addItem("line1", MyGUI::MenuItemType::Popup);
		MyGUI::MenuItemPtr item2 = menu->addItem("line2", MyGUI::MenuItemType::Normal);
		MyGUI::MenuItemPtr item3 = menu->addItem("line3", MyGUI::MenuItemType::Popup);

		MyGUI::MenuCtrlPtr submenu1 = item1->createItemChild();
		MyGUI::MenuItemPtr subitem1 = submenu1->addItem("subline1", MyGUI::MenuItemType::Normal);
		MyGUI::MenuItemPtr subitem2 = submenu1->addItem("subline2", MyGUI::MenuItemType::Popup);

		MyGUI::MenuCtrlPtr submenu2 = item3->createItemChild();
		MyGUI::MenuItemPtr subitem21 = submenu2->addItem("subline21", MyGUI::MenuItemType::Normal);
		MyGUI::MenuItemPtr subitem22 = submenu2->addItem("subline22", MyGUI::MenuItemType::Normal);

		MyGUI::MenuCtrlPtr submenu21 = subitem2->createItemChild();
		MyGUI::MenuItemPtr subitem31 = submenu21->addItem("subline31", MyGUI::MenuItemType::Normal);

		menu->setVisible(true);*/
	}


	void MyGUISystem::Update(double delta_time)
	{
		if(mInfo )
		{
			mInfo->change("FPS", 10);
		mInfo->change("triangle", 10);
		mInfo->change("batch", 10);
		mInfo->change("batch gui", 10);
		mInfo->update();
	}
	}

	bool MyGUISystem::MouseMoved(const MouseData &data)
	{
		if(mGUI)
		{
			MyGUI::InputManager::getInstance().injectMouseMove(data.XAbs,data.YAbs, data.ZAbs);
		}
		return true;
	}
	bool MyGUISystem::MousePressed(const MouseData &data,MouseButtonId id )
	{
		if(mGUI)
		{
			MyGUI::MouseButton button((MyGUI::MouseButton::Enum) id);
			MyGUI::InputManager::getInstance().injectMousePress(data.XAbs,data.YAbs,button);
		}
		//CEGUI::System::getSingleton().injectMouseButtonDown(ConvertOISButtonToCegui(id));
		return true;
	}
	bool MyGUISystem::MouseReleased(const MouseData &data,MouseButtonId id )
	{
		if(mGUI)
		{
			MyGUI::MouseButton button((MyGUI::MouseButton::Enum) id);
			MyGUI::InputManager::getInstance().injectMouseRelease(data.XAbs,data.YAbs,button);
		}
		//CEGUI::System::getSingleton().injectMouseButtonUp(ConvertOISButtonToCegui(id));
		return true;

	}

	bool MyGUISystem::KeyPressed(int key, unsigned int text)
	{

		// do event injection
		//CEGUI::System& cegui = CEGUI::System::getSingleton();

		// key down
		//cegui.injectKeyDown(key);

		// now character
		//cegui.injectChar(text);
		return true;
	}

	bool MyGUISystem::KeyReleased( int key, unsigned int text)
	{
		//CEGUI::System::getSingleton().injectKeyUp(key);
		return true;
	}

}
