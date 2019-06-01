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

#pragma once
#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSIInputSystem.h"
#include "Sim/GASSSimSystem.h"

namespace GASS
{

	class ProxyInputSystem :  public Reflection<ProxyInputSystem, SimSystem>, public IInputSystem, public IProxyInputSystem
	{
	public:
		ProxyInputSystem(SimSystemManagerWeakPtr manager);
		~ProxyInputSystem() override;
		static void RegisterReflection();
		void Init() override;
		std::string GetSystemName() const override {return "ProxyInputSystem";}
		void OnSystemUpdate(double delta_time) override;
		
		//IInputSystem
		void AddKeyListener(IKeyListener* key_listener) override;
		void RemoveKeyListener(IKeyListener* key_listener) override;
		void AddMouseListener(IMouseListener* mouse_listener) override;
		void RemoveMouseListener(IMouseListener* mouse_listener) override;
		void AddGameControllerListener(IGameControllerListener* ) override;
		void RemoveGameControllerListener(IGameControllerListener* ) override;
		
		void SetEnableKey(bool value) override;
		void SetEnableJoystick(bool value) override;
		void SetEnableMouse(bool value) override;
		bool GetEnableKey() const override;
		bool GetEnableJoystick() const override;
		bool GetEnableMouse() const override;
		void ClipInputWindow(int left,int top,int right,int bottom) override;
		void SetMainWindowHandle(void * /*main_win_handle*/) override{}
	
		void InjectMouseMoved(const MouseData &data) override;
		void InjectMousePressed(const MouseData &data, MouseButtonId id ) override;
		void InjectMouseReleased(const MouseData &data, MouseButtonId id ) override;
		void InjectKeyPressed( int key, unsigned int text) override;
		void InjectKeyReleased( int key, unsigned int text) override;	
		
	private:
		std::vector<IKeyListener*> m_KeyListeners;
		std::vector<IMouseListener*> m_MouseListeners;
		std::vector<IGameControllerListener*> m_GameControllerListeners;

		//bool m_Inverted;
		bool m_KeyActive;
		bool m_JoyActive;
		bool m_MouseActive;
		//float m_MouseSpeed;
		//bool m_ExclusiveMode;
	};
}
