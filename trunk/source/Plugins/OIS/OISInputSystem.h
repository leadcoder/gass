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
#ifdef WIN32
#include <OIS.h>
#else
#include <OIS/OIS.h>
#endif
#include "Sim/Common.h"
#include "Core/Utils/Log.h"
#include "Core/MessageSystem/MessageType.h"
#include "Sim/Systems/Input/IInputSystem.h"
#include "Sim/Systems/SimSystem.h"
#include "Sim/Scheduling/ITaskListener.h"
#include "Sim/Systems/Messages/CoreSystemMessages.h"
#include "Sim/Systems/Messages/GraphicsSystemMessages.h"


namespace GASS
{

	class OISInputSystem :  public Reflection<OISInputSystem, SimSystem>, public IInputSystem, public OIS::MouseListener,public OIS::KeyListener,public OIS::JoyStickListener, public ITaskListener
	{
	public:
		OISInputSystem();
		virtual ~OISInputSystem();

		static void RegisterReflection();
		virtual void OnCreate();

		void Shutdown();

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

		SystemType GetSystemType() const {return "InputSystem";}

		//ITaskListener interface
		void Update(double delta_time);
		TaskGroup GetTaskGroup() const;
	private:
		ADD_ATTRIBUTE(double,UpdateFrequency);
		ADD_ATTRIBUTE(float,GameControllerAxisMinValue);

		float NormalizeMouse(float value);
		void OnInit(MainWindowCreatedNotifyMessagePtr message);


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
		bool m_OnlyUpdateWhenFocued;
		bool m_Active;
		float m_MouseSpeed;
		int m_Window;
		bool m_ExclusiveMode;
		
		double m_TimeSinceLastUpdate;
		

	};
}
