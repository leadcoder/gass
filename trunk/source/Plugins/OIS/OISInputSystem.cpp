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


#include "Plugins/OIS/OISInputSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Graphics/IGraphicsSystem.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Core/System/SystemFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include <boost/bind.hpp>

#include <OgreRenderWindow.h>


namespace GASS
{
	OISInputSystem::OISInputSystem() :
		m_Window(0),
		m_Inverted (false),
		m_Active (true),
		m_OnlyUpdateWhenFocued (true),
		m_MouseSpeed(20),
		m_ExclusiveMode(true)
	{
		m_KeyBuffer =  new char[256];
		m_OldKeyBuffer =  new char[256];
		memset(m_KeyBuffer,0,256);
		memset(m_OldKeyBuffer,0,256);
	}

	OISInputSystem::~OISInputSystem()
	{
		delete[] m_KeyBuffer;
		delete[] m_OldKeyBuffer;
	}

	void OISInputSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("OISInputSystem",new GASS::Creator<OISInputSystem, ISystem>);
		RegisterProperty<bool>("ExclusiveMode", &GASS::OISInputSystem::GetExclusiveMode, &GASS::OISInputSystem::SetExclusiveMode);
	}

	void OISInputSystem::OnCreate()
	{
		SimEngine::GetPtr()->GetRuntimeController()->Register(UPDATE_FUNC( OISInputSystem::Update));
		GetSimSystemManager()->RegisterForMessage(SimSystemManager::SYSTEM_MESSAGE_MAIN_WINDOW_CREATED, MESSAGE_FUNC( OISInputSystem::OnInit),1);
	}

	void OISInputSystem::OnInit(MessagePtr message)
	{
		m_Window = boost::any_cast<int>(message->GetData("MainHandle"));
		OIS::ParamList pl;
		size_t windowHnd = m_Window;
		std::ostringstream windowHndStr;
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

		if(!m_ExclusiveMode)
		{
#if defined OIS_WIN32_PLATFORM
			pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
			pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
			pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
			pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#elif defined OIS_LINUX_PLATFORM
			pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
			pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
			pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
			pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif
		}
		m_InputManager = OIS::InputManager::createInputSystem( pl );

		//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
		bool bufferedKeys = true;
		bool bufferedMouse = true;
		bool bufferedJoy = true;
		m_Keyboard = static_cast<OIS::Keyboard*>(m_InputManager->createInputObject( OIS::OISKeyboard, bufferedKeys ));
		m_Keyboard->setEventCallback(this);
		m_Mouse = static_cast<OIS::Mouse*>(m_InputManager->createInputObject( OIS::OISMouse, bufferedMouse ));
		m_Mouse->setEventCallback(this);

#ifdef WIN32
		if (m_InputManager->numJoySticks() > 0) {
			Log::Print("OISInputSystem: %d joysticks available",m_InputManager->numJoySticks());
			try {
				for (int i = 0; i < m_InputManager->numJoySticks(); i++) {
					OIS::JoyStick* joy = static_cast<OIS::JoyStick*>(m_InputManager->createInputObject( OIS::OISJoyStick, bufferedJoy ));
					joy->setEventCallback(this);
					Log::Print("OISInputSystem: Joystick ID #%d '%s' - %d axes, %d buttons, %d hats",joy->getID(), joy->vendor().c_str(), joy->axes(), joy->buttons(), joy->hats());
					m_Joys.push_back(joy);
					m_JoyState.push_back(joy->getJoyStickState());
					m_OldJoyState.push_back(joy->getJoyStickState());
				}
			} catch(std::exception& e) {
				Log::Warning("OISInputSystem: Exception caught while initializing joysticks: %s", e.what());
			} catch(...) {
				Log::Warning("OISInputSystem: Something caught while initializing joysticks");
			}
		} else {
			Log::Print("OISInputSystem: No joystick");
		}
		#else
		if (m_InputManager->numJoysticks() > 0) {
			Log::Print("OISInputSystem: %d joysticks available",m_InputManager->numJoysticks());
			try {
				for (int i = 0; i < m_InputManager->numJoysticks(); i++) {
					OIS::JoyStick* joy = static_cast<OIS::JoyStick*>(m_InputManager->createInputObject( OIS::OISJoyStick, bufferedJoy ));
					Log::Print("OISInputSystem: Joystick ID #%d '%s' - %d axes",joy->getID(), joy->vendor().c_str(), joy->getJoyStickState().mAxes.size());
					joy->capture();
					m_Joys.push_back(joy);
					m_JoyState.push_back(joy->getJoyStickState());
					m_OldJoyState.push_back(joy->getJoyStickState());
				}
			} catch(std::exception& e) {
				Log::Warning("OISInputSystem: Exception caught while initializing joysticks: %s", e.what());
			} catch(...) {
				Log::Warning("OISInputSystem: Something caught while initializing joysticks");
			}
		} else {
			Log::Print("OISInputSystem: No joystick");
		}
		#endif

		GraphicsSystemPtr gs = static_cast<SimSystemManager*>(GetOwner())->GetFirstSystem<IGraphicsSystem>();
		if(gs)
		{
			unsigned int width, height;
			int left, top;
			gs->GetMainWindowInfo(width, height, left, top);
			const OIS::MouseState &ms = m_Mouse->getMouseState();
			ms.width = width;
			ms.height = height;
		}
	}

	void OISInputSystem::Shutdown()
	{
		m_InputManager->destroyInputObject(m_Keyboard);
		m_InputManager->destroyInputObject(m_Mouse);
		for (int i = 0; i < m_Joys.size(); i++)
			m_InputManager->destroyInputObject(m_Joys[i]);
		m_Joys.erase(m_Joys.begin(),m_Joys.end());
		m_InputManager->destroyInputSystem(m_InputManager);
		m_Keyboard = 0;
		m_Mouse = 0;
		m_InputManager = 0;
	}

	int OISInputSystem::CurButton(int index)
	{
		return (m_MouseState.buttonDown((OIS::MouseButtonID)index));
	}

	int OISInputSystem::OldButton(int index)
	{
		return (m_OldMouseState.buttonDown((OIS::MouseButtonID)index));
	}

	void OISInputSystem::Update(double double_delta)
	{
		if(m_Window == 0)
			return;

		if(!m_Active) //check if we are out of focus
		{
			//feed console and gui?
			m_Keyboard->capture();
			memset(m_KeyBuffer,0,256);
			memset(m_OldKeyBuffer,0,256);
			return;
		}
		m_Keyboard->capture();
		memcpy(m_OldKeyBuffer,m_KeyBuffer,256);
		m_Keyboard->copyKeyStates(m_KeyBuffer);

		m_Mouse->capture();

		m_OldMouseState = m_MouseState;
		m_MouseState = m_Mouse->getMouseState();
		for (int i = 0; i < m_Joys.size(); i++) {
			m_Joys[i]->capture();
			m_OldJoyState[i] = m_JoyState[i];
			m_JoyState[i] = m_Joys[i]->getJoyStickState();
		}
	}

	float OISInputSystem::GetCursorDeltaX()
	{
		return NormalizeMouse(m_MouseState.X.rel);
	}

	float OISInputSystem::GetCursorDeltaY()
	{
		return NormalizeMouse(m_MouseState.Y.rel);
	}

	float OISInputSystem::GetScrollWheelDelta()
	{
		return m_MouseState.Z.rel;
	}


	int OISInputSystem::CurKey(int index)
	{
		return m_KeyBuffer[index];
	}

	float OISInputSystem::GetJoystickAxis(int device, int axis)
	{
		if (device >= m_Joys.size() || axis > m_JoyState[device].mAxes.size())
			return 0.0;
		else
			return ((float)m_JoyState[device].mAxes[axis].abs) / 32768.0;
	}

	int OISInputSystem::GetJoystickPOV(int device, int pov)
	{
		if (device >= m_Joys.size() || pov > 3)
			return 0;
		else
			return m_JoyState[device].mPOV[pov].direction;
	}

	void OISInputSystem::AddKeyListener(IKeyListener* key_listener)
	{
		m_KeyListeners.push_back(key_listener);

	}
	void OISInputSystem::RemoveKeyListener(IKeyListener* key_listener)
	{
		std::vector<IKeyListener*>::iterator iter = m_KeyListeners.begin();
		while(iter != m_KeyListeners.end())
		{
			IKeyListener* kl = *iter;
			if(kl == key_listener)
			{
				iter = m_KeyListeners.erase(iter);
			}
			else iter++;
		}
	}

	void OISInputSystem::AddMouseListener(IMouseListener* mouse_listener)
	{
		m_MouseListeners.push_back(mouse_listener);
	}

	void OISInputSystem::RemoveMouseListener(IMouseListener* mouse_listener)
	{
		std::vector<IMouseListener*>::iterator iter = m_MouseListeners.begin();
		while(iter != m_MouseListeners.end())
		{
			IMouseListener* ml = *iter;
			if(ml == mouse_listener)
			{
				iter = m_MouseListeners.erase(iter);
			}
			else iter++;
		}
	}

	void OISInputSystem::AddGameControllerListener(IGameControllerListener* gc_listener)
	{
		m_GameControllerListeners.push_back(gc_listener);
	}

	void OISInputSystem::RemoveGameControllerListener(IGameControllerListener* gc_listener)
	{
		std::vector<IGameControllerListener*>::iterator iter = m_GameControllerListeners.begin();
		while(iter != m_GameControllerListeners.end())
		{
			IGameControllerListener* ml = *iter;
			if(ml == gc_listener)
			{
				iter = m_GameControllerListeners.erase(iter);
			}
			else iter++;
		}
	}


	bool OISInputSystem::keyPressed( const OIS::KeyEvent &arg )
	{

		std::vector<IKeyListener*>::iterator iter = m_KeyListeners.begin();
		while(iter != m_KeyListeners.end())
		{
			IKeyListener* kl = *iter;
			kl->KeyPressed(arg.key,arg.text);
			iter++;
		}
		return true;
	}

	bool OISInputSystem::keyReleased( const OIS::KeyEvent &arg )
	{

		std::vector<IKeyListener*>::iterator iter = m_KeyListeners.begin();
		while(iter != m_KeyListeners.end())
		{
			IKeyListener* kl = *iter;
			kl->KeyReleased(arg.key,arg.text);
			iter++;
		}
		return true;
	}
	bool OISInputSystem::mouseMoved( const OIS::MouseEvent &arg )
	{
		std::vector<IMouseListener*>::iterator iter = m_MouseListeners.begin();
		while(iter != m_MouseListeners.end())
		{
			IMouseListener* ml = *iter;

			//Normalize values

			ml->MouseMoved(NormalizeMouse(arg.state.X.rel),NormalizeMouse(arg.state.Y.rel));
			iter++;
		}
		return true;
	}

	bool OISInputSystem::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		std::vector<IMouseListener*>::iterator iter = m_MouseListeners.begin();
		while(iter != m_MouseListeners.end())
		{
			IMouseListener* ml = *iter;
			ml->MousePressed(id);
			iter++;
		}
		return true;
	}

	bool OISInputSystem::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		std::vector<IMouseListener*>::iterator iter = m_MouseListeners.begin();
		while(iter != m_MouseListeners.end())
		{
			IMouseListener* ml = *iter;
			ml->MouseReleased(id);
			iter++;
		}
		return true;
	}

	float OISInputSystem::NormalizeMouse(float value)
	{
		float ret = 0;
		if (value > m_MouseSpeed)
			ret = 1;
		else if (value < -m_MouseSpeed)
			ret = -1;
		else
			ret = value / m_MouseSpeed;
		return ret;
	}


	bool OISInputSystem::buttonPressed( const OIS::JoyStickEvent &arg, int button )
	{
		std::vector<IGameControllerListener*>::iterator iter = m_GameControllerListeners.begin();
		while(iter != m_GameControllerListeners.end())
		{
			IGameControllerListener* ml = *iter;
			ml->ButtonPressed(arg.device->getID(),button);
			iter++;
		}
		return true;
	}
	bool OISInputSystem::buttonReleased( const OIS::JoyStickEvent &arg, int button )
	{
		std::vector<IGameControllerListener*>::iterator iter = m_GameControllerListeners.begin();
		while(iter != m_GameControllerListeners.end())
		{
			IGameControllerListener* ml = *iter;
			ml->ButtonReleased(arg.device->getID(),button);
			iter++;
		}
		return true;
	}

	bool OISInputSystem::axisMoved( const OIS::JoyStickEvent &arg, int axis )
	{
		std::vector<IGameControllerListener*>::iterator iter = m_GameControllerListeners.begin();
		float value = ((float)arg.state.mAxes[axis].abs) / 32768.0;

		while(iter != m_GameControllerListeners.end())
		{

			IGameControllerListener* ml = *iter;

			ml->AxisMoved(arg.device->getID(),axis, value);
			iter++;
		}
		return true;
	}

	//Joystick Event, amd sliderID
	bool OISInputSystem::sliderMoved( const OIS::JoyStickEvent &, int )
	{
		return true;
	}
		//Joystick Event, amd povID
	bool OISInputSystem::povMoved( const OIS::JoyStickEvent &, int )
	{
		return true;
	}
}

