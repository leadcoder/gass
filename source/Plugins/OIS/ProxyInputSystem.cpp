/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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


#include "Plugins/OIS/ProxyInputSystem.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSystemFactory.h"

namespace GASS
{
	ProxyInputSystem::ProxyInputSystem(SimSystemManagerWeakPtr manager) : Reflection(manager), m_KeyActive(true),
		m_JoyActive(true),	
		m_MouseActive(true)
	{

	}

	ProxyInputSystem::~ProxyInputSystem()
	{

	}

	void ProxyInputSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register<ProxyInputSystem>("ProxyInputSystem");
		RegisterGetSet("EnableKey", &GASS::ProxyInputSystem::GetEnableKey, &GASS::ProxyInputSystem::SetEnableKey);
		RegisterGetSet("EnableMouse", &GASS::ProxyInputSystem::GetEnableMouse, &GASS::ProxyInputSystem::SetEnableMouse);
		RegisterGetSet("EnableJoystick", &GASS::ProxyInputSystem::GetEnableJoystick, &GASS::ProxyInputSystem::SetEnableJoystick);
	}

	void ProxyInputSystem::OnSystemInit()
	{
		SimEngine::Get().GetSimSystemManager()->PostMessage(SystemMessagePtr(new InputSystemLoadedEvent()));
	}

	//Not supported
	void ProxyInputSystem::ClipInputWindow(int /*left*/,int /*top*/,int /*right*/,int /*bottom*/)
	{

	}

	void ProxyInputSystem::OnSystemUpdate(double /*delta_time*/)
	{

	}

	void ProxyInputSystem::AddKeyListener(IKeyListener* key_listener)
	{
		m_KeyListeners.push_back(key_listener);

	}
	void ProxyInputSystem::RemoveKeyListener(IKeyListener* key_listener)
	{
		std::vector<IKeyListener*>::iterator iter = m_KeyListeners.begin();
		while(iter != m_KeyListeners.end())
		{
			IKeyListener* kl = *iter;
			if(kl == key_listener)
			{
				iter = m_KeyListeners.erase(iter);
			}
			else ++iter;
		}
	}

	void ProxyInputSystem::AddMouseListener(IMouseListener* mouse_listener)
	{
		m_MouseListeners.push_back(mouse_listener);
	}

	void ProxyInputSystem::RemoveMouseListener(IMouseListener* mouse_listener)
	{
		std::vector<IMouseListener*>::iterator iter = m_MouseListeners.begin();
		while(iter != m_MouseListeners.end())
		{
			IMouseListener* ml = *iter;
			if(ml == mouse_listener)
			{
				iter = m_MouseListeners.erase(iter);
			}
			else ++iter;
		}
	}

	void ProxyInputSystem::AddGameControllerListener(IGameControllerListener* gc_listener)
	{
		m_GameControllerListeners.push_back(gc_listener);
	}

	void ProxyInputSystem::RemoveGameControllerListener(IGameControllerListener* gc_listener)
	{
		std::vector<IGameControllerListener*>::iterator iter = m_GameControllerListeners.begin();
		while(iter != m_GameControllerListeners.end())
		{
			IGameControllerListener* ml = *iter;
			if(ml == gc_listener)
			{
				iter = m_GameControllerListeners.erase(iter);
			}
			else ++iter;
		}
	}

	void ProxyInputSystem::SetEnableKey(bool value)
	{
		m_KeyActive = value;
	}
	void ProxyInputSystem::SetEnableJoystick(bool value)
	{
		m_JoyActive = value;
	}
	void ProxyInputSystem::SetEnableMouse(bool value)
	{
		m_MouseActive = value;
	}

	bool ProxyInputSystem::GetEnableKey() const
	{
		return m_KeyActive;
	}

	bool ProxyInputSystem::GetEnableJoystick() const
	{
		return m_JoyActive;
	}

	bool ProxyInputSystem::GetEnableMouse() const
	{
		return m_MouseActive;
	}

	void ProxyInputSystem::InjectMouseMoved(const MouseData &data)
	{
		if(m_MouseActive)
		{
			std::vector<IMouseListener*>::iterator iter = m_MouseListeners.begin();
			while(iter != m_MouseListeners.end())
			{
				IMouseListener* ml = *iter;
				//Normalize values
				ml->MouseMoved(data);
				++iter;
			}
		}
	}

	void ProxyInputSystem::InjectMousePressed(const MouseData &data, MouseButtonId id )
	{
		if(m_MouseActive)
		{
			std::vector<IMouseListener*>::iterator iter = m_MouseListeners.begin();
			while(iter != m_MouseListeners.end())
			{
				IMouseListener* ml = *iter;
				ml->MousePressed(data,id);
				++iter;
			}
		}
	}

	void ProxyInputSystem::InjectMouseReleased(const MouseData &data, MouseButtonId id )
	{
		if(m_MouseActive)
		{
			std::vector<IMouseListener*>::iterator iter = m_MouseListeners.begin();
			while(iter != m_MouseListeners.end())
			{
				IMouseListener* ml = *iter;
				ml->MouseReleased(data,id);
				++iter;
			}
		}
	}

	void ProxyInputSystem::InjectKeyPressed( int key, unsigned int text)
	{
		if(m_KeyActive)
		{
			std::vector<IKeyListener*>::iterator iter = m_KeyListeners.begin();
			while(iter != m_KeyListeners.end())
			{
				IKeyListener* kl = *iter;
				kl->KeyPressed(key,text);
				++iter;
			}
		}
	}
	void ProxyInputSystem::InjectKeyReleased( int key, unsigned int text)
	{
		if(m_KeyActive)
		{
			std::vector<IKeyListener*>::iterator iter = m_KeyListeners.begin();
			while(iter != m_KeyListeners.end())
			{
				IKeyListener* kl = *iter;
				kl->KeyReleased(key,text);
				++iter;
			}
		}
	}
}

