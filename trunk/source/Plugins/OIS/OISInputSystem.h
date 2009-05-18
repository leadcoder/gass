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

#include <OIS/OIS.h>
#include "Sim/Common.h"
#include "Core/Utils/Log.h"
#include "Core/MessageSystem/MessageType.h"
#include "Sim/Systems/Input/IInputSystem.h"
#include "Sim/Systems/SimSystem.h"


namespace GASS
{


	class OISInputSystem :  public Reflection<OISInputSystem, SimSystem>, public IInputSystem, public OIS::MouseListener,public OIS::KeyListener,public OIS::JoyStickListener
	{
	public:
		OISInputSystem();
		virtual ~OISInputSystem();

		static void RegisterReflection();
		virtual void OnCreate();

		void Shutdown();
		//some helper functions to make certain things easier
		int inline KeyDown(int index)		{ return ( CurKey(index))&&(!OldKey(index)); }
		int inline KeyStillDown(int index)	{ return ( CurKey(index))&&( OldKey(index)); }
		int inline KeyUp(int index)		{ return (!CurKey(index))&&( OldKey(index)); }
		int inline KeyStillUp(int index)	{ return (!CurKey(index))&&(!OldKey(index)); }
		int CurKey(int index);

		int inline ButtonDown(int index)		{ return (CurButton(index)) && (!OldButton(index)); }
		int inline ButtonStillDown(int index)	{ return (CurButton(index)) && (OldButton(index)); }
		int inline ButtonUp(int index)		{ return (!CurButton(index)) && (OldButton(index)); }
		int inline ButtonStillUp(int index)	{ return (!CurButton(index)) && (!OldButton(index)); }
		int CurButton(int index);
		float GetCursorDeltaX() ;
		float GetCursorDeltaY() ;
		float GetScrollWheelDelta() ;

		int inline JoystickButtonDown(int device, int index)		{ return (CurJoystickButton(device, index)) && (!OldJoystickButton(device, index)); }
		int inline JoystickButtonStillDown(int device, int index)	{ return (CurJoystickButton(device, index)) && (OldJoystickButton(device, index)); }
		int inline JoystickButtonUp(int device, int index)		{ return (!CurJoystickButton(device, index)) && (OldJoystickButton(device, index)); }
		int inline JoystickButtonStillUp(int device, int index)	{ return (!CurJoystickButton(device, index)) && (!OldJoystickButton(device, index)); }
		int inline CurJoystickButton(int device, int index) 
		{ 
			if (device >= m_Joys.size())
				return 0;
			return m_JoyState[device].buttonDown(index);
		}
		float GetJoystickAxis(int device, int axis);
		int GetJoystickPOV(int device, int pov);

		// Deprecated:
		float GetGameController0Axis(int axis) { return GetJoystickAxis(0, axis); }
		float GetGameController1Axis(int axis) { return GetJoystickAxis(1, axis); }
		
		virtual void AddKeyListener(IKeyListener* key_listener);
		virtual void RemoveKeyListener(IKeyListener* key_listener);

		virtual void AddMouseListener(IMouseListener* mouse_listener);
		virtual void RemoveMouseListener(IMouseListener* mouse_listener);

		virtual void AddGameControllerListener(IGameControllerListener* );
		virtual void RemoveGameControllerListener(IGameControllerListener* );
	
		OIS::Mouse*    GetMouse(){return m_Mouse;}
		OIS::Keyboard* GetKeyboard(){return m_Keyboard;}


		virtual bool keyPressed( const OIS::KeyEvent &arg );
		virtual bool keyReleased( const OIS::KeyEvent &arg );

		virtual bool mouseMoved( const OIS::MouseEvent &arg );
		virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
		virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );


		virtual bool buttonPressed( const OIS::JoyStickEvent &arg, int button );
		virtual bool buttonReleased( const OIS::JoyStickEvent &arg, int button );

		virtual bool axisMoved( const OIS::JoyStickEvent &arg, int axis ) ;

		//Joystick Event, amd sliderID
		virtual bool sliderMoved( const OIS::JoyStickEvent &, int );
		//Joystick Event, amd povID
		virtual bool povMoved( const OIS::JoyStickEvent &, int );


		void OnlyUpdateWhenFocued(bool value){m_OnlyUpdateWhenFocued = value;}
		void SetActive(bool value){m_Active = value;}

		int	 GetJoystickIndex() { return m_JoystickDeviceCount; }
		void IncJoystickDeviceCount() { m_JoystickDeviceCount++; }

		void SetWindow(int window)
        {
            m_Window = window;
        }

		SystemType GetSystemType() {return "InputSystem";}
	private:
		float NormalizeMouse(float value);
	

		void OnInit(MessagePtr message);		
		void Update(double delta_time);

		bool GetExclusiveMode() const {return m_ExclusiveMode;}
		void SetExclusiveMode(bool value) {m_ExclusiveMode = value;}


		int inline OldKey(int index) { return (m_OldKeyBuffer[index]); }
		
		int OldButton(int index);
	
	
		
		int inline OldJoystickButton(int device, int index) 
		{ 
			if (device >= m_Joys.size())
				return 0;
			return m_OldJoyState[device].buttonDown(index);
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
		bool m_OnlyUpdateWhenFocued;
		bool m_Active;
		float m_MouseSpeed;
		int m_Window;
		bool m_ExclusiveMode;

	};
}
