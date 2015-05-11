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

#pragma once

#include "Sim/GASSCommon.h"
#ifdef WIN32
#include <OIS.h>
#else
#include <OIS/OIS.h>
#endif

#include "Core/Utils/GASSLogManager.h"
#include "Core/MessageSystem/GASSMessageType.h"
#include "Sim/Interface/GASSIInputSystem.h"
#include "Sim/GASSSimSystem.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"


namespace GASS
{

	class OISInputSystem :  public Reflection<OISInputSystem, SimSystem>, public IInputSystem,public IProxyInputSystem, public OIS::MouseListener,public OIS::KeyListener,public OIS::JoyStickListener
	{
	public:
		OISInputSystem();
		virtual ~OISInputSystem();
		static void RegisterReflection();
		virtual void OnCreate(SimSystemManagerPtr owner);
		virtual void Init();
		virtual std::string GetSystemName() const {return "OISInputSystem";}
		virtual void Update(double delta_time,TaskNode2* caller);
		
		//IInputSystem
		virtual void AddKeyListener(IKeyListener* key_listener);
		virtual void RemoveKeyListener(IKeyListener* key_listener);
		virtual void AddMouseListener(IMouseListener* mouse_listener);
		virtual void RemoveMouseListener(IMouseListener* mouse_listener);
		virtual void AddGameControllerListener(IGameControllerListener* );
		virtual void RemoveGameControllerListener(IGameControllerListener* );
		virtual void ClipInputWindow(int left,int top,int right,int bottom);
		virtual void SetEnableKey(bool value);
		virtual void SetEnableJoystick(bool value);
		virtual void SetEnableMouse(bool value);
		virtual bool GetEnableKey() const;
		virtual bool GetEnableJoystick() const;
		virtual bool GetEnableMouse() const;
		virtual void SetMainWindowHandle(void *main_win_handle);
	
		//IProxyInputSystem
		virtual void InjectMouseMoved(const MouseData &data);
		virtual void InjectMousePressed(const MouseData &data, MouseButtonId id );
		virtual void InjectMouseReleased(const MouseData &data, MouseButtonId id );
		virtual void InjectKeyPressed( int key, unsigned int text);
		virtual void InjectKeyReleased( int key, unsigned int text);	
		
		//OIS::KeyListener
		virtual bool keyPressed( const OIS::KeyEvent &arg );
		virtual bool keyReleased( const OIS::KeyEvent &arg );

		//OIS::MouseListener
		virtual bool mouseMoved( const OIS::MouseEvent &arg );
		virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
		virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

		//OIS::JoyStickListener
		virtual bool buttonPressed( const OIS::JoyStickEvent &arg, int button );
		virtual bool buttonReleased( const OIS::JoyStickEvent &arg, int button );
		virtual bool axisMoved( const OIS::JoyStickEvent &arg, int axis ) ;
		virtual bool sliderMoved( const OIS::JoyStickEvent &, int ); //Joystick Event, amd sliderID
		virtual bool povMoved( const OIS::JoyStickEvent &, int ); //Joystick Event, amd povID
	private:
		//ADD_PROPERTY(double,UpdateFrequency);
		ADD_PROPERTY(float,GameControllerAxisMinValue);
		ADD_PROPERTY(bool,OnlyProxy);

		int	 GetJoystickIndex() { return m_JoystickDeviceCount; }
		void IncJoystickDeviceCount() { m_JoystickDeviceCount++; }
		
		void Shutdown();
		OIS::Mouse*    GetMouse(){return m_Mouse;}
		OIS::Keyboard* GetKeyboard(){return m_Keyboard;}
		float NormalizeMouseDelta(float value);
		MouseButtonId ToGASS(OIS::MouseButtonID ois_id) const;
		MouseData ToGASS(const OIS::MouseEvent &arg);
		void OnInit(MainWindowCreatedEventPtr message);
		void OnViewportMovedOrResized(RenderWindowResizedEventPtr message);
		bool GetExclusiveMode() const {return m_ExclusiveMode;}
		void SetExclusiveMode(bool value) {m_ExclusiveMode = value;}
		int inline OldKey(int index) { return (m_OldKeyBuffer[index]); }
		int OldButton(int index);
		int inline OldJoystickButton(int device, int index)
		{
			if (device >= m_Joys.size())
				return 0;
			return m_OldJoyState[device].mButtons[index];
		}

		OIS::InputManager* m_InputManager;
		OIS::Mouse*    m_Mouse;
		OIS::Keyboard* m_Keyboard;
		std::vector<OIS::JoyStick*> m_Joys;
		std::vector<OIS::JoyStickState> m_JoyState;
		std::vector<OIS::JoyStickState> m_OldJoyState;

		char* m_KeyBuffer;
		char* m_OldKeyBuffer;

		OIS::MouseState m_MouseState;
		OIS::MouseState m_OldMouseState;

		std::vector<IKeyListener*> m_KeyListeners;
		std::vector<IMouseListener*> m_MouseListeners;
		std::vector<IGameControllerListener*> m_GameControllerListeners;

		int m_JoystickDeviceCount;
		bool m_Inverted;
		bool m_KeyActive;
		bool m_JoyActive;
		bool m_MouseActive;
		float m_MouseSpeed;
		void* m_Window;
		bool m_ExclusiveMode;
		int m_MouseWinOffsetX;
		int m_MouseWinOffsetY;
		int m_MouseWinWidth;
		int m_MouseWinHeight;
		
	};
}
