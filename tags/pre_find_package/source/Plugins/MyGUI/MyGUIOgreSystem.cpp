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
#include "MyGUIOgreSystem.h"
#include <stdio.h>
#include "Sim/Messages/GASSScriptSystemMessages.h"
#include <MyGUI.h>
#include "MyGUI_LastHeader.h"
#include <MyGUI_OgrePlatform.h>
#include "StatisticInfo.h"
#include "MainMenu.h"

namespace GASS
{
	MyGUIOgreSystem::MyGUIOgreSystem() 
	{

	}

	MyGUIOgreSystem::~MyGUIOgreSystem()
	{

	}

	void MyGUIOgreSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("MyGUIOgreSystem",new GASS::Creator<MyGUIOgreSystem, ISystem>);
	}

	void MyGUIOgreSystem::Init()
	{
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(MyGUIOgreSystem::OnInputSystemLoaded,InputSystemLoadedEvent,0));
		MyGUISystem::Init();
	}
	
	void MyGUIOgreSystem::OnInputSystemLoaded(InputSystemLoadedEventPtr message)
	{
		InputSystemPtr input_system = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<IInputSystem>();
		input_system->AddKeyListener(this);
		input_system->AddMouseListener(this);

		MyGUI::OgrePlatform* m_Platform = new MyGUI::OgrePlatform();
		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		//Ogre::Camera* ocam = sm->getCameraIterator().getNext();
		Ogre::RenderTarget *target = NULL;
		if (Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator().hasMoreElements())
			target = Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator().getNext();
		m_Platform->initialise((Ogre::RenderWindow*)target, sm,"MyGUI");
		InitGUI();
	}

	/*bool MyGUIOgreSystem::MouseMoved(const MouseData &data)
	{
		if(mGUI)
		{
			MyGUI::InputManager::getInstance().injectMouseMove(data.XAbs,data.YAbs, data.ZAbs);
		}
		return true;
	}
	bool MyGUIOgreSystem::MousePressed(const MouseData &data,MouseButtonId id )
	{
		if(mGUI)
		{
			MyGUI::MouseButton button((MyGUI::MouseButton::Enum) id);
			MyGUI::InputManager::getInstance().injectMousePress(data.XAbs,data.YAbs,button);
		}
		//CEGUI::System::getSingleton().injectMouseButtonDown(ConvertOISButtonToCegui(id));
		return true;
	}
	bool MyGUIOgreSystem::MouseReleased(const MouseData &data,MouseButtonId id )
	{
		if(mGUI)
		{
			MyGUI::MouseButton button((MyGUI::MouseButton::Enum) id);
			MyGUI::InputManager::getInstance().injectMouseRelease(data.XAbs,data.YAbs,button);
		}
		//CEGUI::System::getSingleton().injectMouseButtonUp(ConvertOISButtonToCegui(id));
		return true;

	}

	bool MyGUIOgreSystem::KeyPressed(int key, unsigned int text)
	{

		// do event injection
		//CEGUI::System& cegui = CEGUI::System::getSingleton();

		// key down
		//cegui.injectKeyDown(key);

		// now character
		//cegui.injectChar(text);
		return true;
	}

	bool MyGUIOgreSystem::KeyReleased( int key, unsigned int text)
	{
		//CEGUI::System::getSingleton().injectKeyUp(key);
		return true;
	}*/

}