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

#pragma once
#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/MessageSystem/GASSMessageType.h"
#include "Sim/Interface/GASSIInputSystem.h"
#include "Sim/GASSSimSystem.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"


namespace GASS
{

	class ProxyInputSystem :  public Reflection<ProxyInputSystem, SimSystem>, public IInputSystem, public IProxyInputSystem
	{
	public:
		ProxyInputSystem();
		virtual ~ProxyInputSystem();
		static void RegisterReflection();
		virtual void OnCreate(SystemManagerPtr owner);
		virtual void Init();
		virtual std::string GetSystemName() const {return "ProxyInputSystem";}
		virtual void Update(double delta_time);
		
		//IInputSystem
		virtual void AddKeyListener(IKeyListener* key_listener);
		virtual void RemoveKeyListener(IKeyListener* key_listener);
		virtual void AddMouseListener(IMouseListener* mouse_listener);
		virtual void RemoveMouseListener(IMouseListener* mouse_listener);
		virtual void AddGameControllerListener(IGameControllerListener* );
		virtual void RemoveGameControllerListener(IGameControllerListener* );
		
		virtual void SetEnableKey(bool value);
		virtual void SetEnableJoystick(bool value);
		virtual void SetEnableMouse(bool value);
		virtual bool GetEnableKey() const;
		virtual bool GetEnableJoystick() const;
		virtual bool GetEnableMouse() const;
		virtual void ClipInputWindow(int left,int top,int right,int bottom);
		virtual void SetMainWindowHandle(void *main_win_handle){}
	
		virtual void InjectMouseMoved(const MouseData &data);
		virtual void InjectMousePressed(const MouseData &data, MouseButtonId id );
		virtual void InjectMouseReleased(const MouseData &data, MouseButtonId id );
		virtual void InjectKeyPressed( int key, unsigned int text);
		virtual void InjectKeyReleased( int key, unsigned int text);	
		
	private:
		std::vector<IKeyListener*> m_KeyListeners;
		std::vector<IMouseListener*> m_MouseListeners;
		std::vector<IGameControllerListener*> m_GameControllerListeners;

		bool m_Inverted;
		bool m_KeyActive;
		bool m_JoyActive;
		bool m_MouseActive;
		float m_MouseSpeed;
		bool m_ExclusiveMode;
	};
}
