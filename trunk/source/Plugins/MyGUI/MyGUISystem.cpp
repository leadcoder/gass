/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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

#include "MyGUISystem.h"
#include <stdio.h>
#include "Sim/Messages/GASSScriptSystemMessages.h"
#include <MyGUI.h>
#include "MyGUI_LastHeader.h"
#include "MyGUI_RTTLayer.h"
#include "StatisticInfo.h"
#include "MainMenu.h"

namespace GASS
{
	MyGUISystem::MyGUISystem() : mGUI(NULL),
		m_Initialized(false)
	{

	}

	MyGUISystem::~MyGUISystem()
	{

	}

	void MyGUISystem::RegisterReflection()
	{

	}

	void MyGUISystem::Init()
	{
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(MyGUISystem::OnLoadGUIScript,GUIScriptRequest,0));
	}

	void MyGUISystem::InitGUI()
	{
		mGUI = new MyGUI::Gui();

		mGUI->initialise("MyGUI_Core.xml");
		
		MyGUI::FactoryManager::getInstance().registerFactory<MyGUI::RTTLayer>("Layer");
		MyGUI::ResourceManager::getInstance().load("RTTResources.xml");

		MyGUI::ResourceManager::getInstance().load("MyGUI_BlackOrangeTheme.xml");
	//	mInfo = new diagnostic::StatisticInfo();
	//	mInfo->setVisible(true);
		MainMenu* menu = new MainMenu(NULL);
		menu->Init();
		GetSimSystemManager()->SendImmediate(GUILoadedEventPtr(new GUILoadedEvent(mGUI)));
		//SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);
		m_Initialized = true;
	}
	
	void MyGUISystem::OnLoadGUIScript(GUIScriptRequestPtr message)
	{
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

	void MyGUISystem::Update(double delta_time,TaskNode2* caller)
	{
		/*if(mInfo )
		{
			mInfo->change("FPS", 10);
			mInfo->change("triangle", 10);
			mInfo->change("batch", 10);
			mInfo->change("batch gui", 10);
			mInfo->update();
		}*/
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
		return true;
	}

	bool MyGUISystem::MouseReleased(const MouseData &data,MouseButtonId id )
	{
		if(mGUI)
		{
			MyGUI::MouseButton button((MyGUI::MouseButton::Enum) id);
			MyGUI::InputManager::getInstance().injectMouseRelease(data.XAbs,data.YAbs,button);
		}
		return true;

	}

	bool MyGUISystem::KeyPressed(int key, unsigned int text)
	{
		//TODO inject input?
		return true;
	}

	bool MyGUISystem::KeyReleased( int key, unsigned int text)
	{
		return true;
	}

}
