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


#include "Plugins/OIS/OISInputSystem.h"
#include "Sim/GASSSimEngine.h"

#include "Sim/Interface/GASSIGraphicsSystem.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSException.h"

namespace GASS
{
	OISInputSystem::OISInputSystem() : m_Window(0),
		m_Inverted (false),
		m_KeyActive(true),
		m_JoyActive(true),
		m_MouseActive(true),
		m_OnlyProxy(false),
		m_MouseSpeed(20),
		m_ExclusiveMode(true),
		m_GameControllerAxisMinValue(0),
		m_MouseWinOffsetX(0),
		m_MouseWinOffsetY(0),
		m_InputManager(NULL)
	{
		m_UpdateGroup=UGID_PRE_SIM;
		m_KeyBuffer =  new char[256];
		m_OldKeyBuffer =  new char[256];
		memset(m_KeyBuffer,0,256);
		memset(m_OldKeyBuffer,0,256);
	}

	OISInputSystem::~OISInputSystem()
	{
		Shutdown();
		delete[] m_KeyBuffer;
		delete[] m_OldKeyBuffer;
	}

	void OISInputSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("OISInputSystem",new GASS::Creator<OISInputSystem, SimSystem>);
		RegisterProperty<bool>("ExclusiveMode", &GASS::OISInputSystem::GetExclusiveMode, &GASS::OISInputSystem::SetExclusiveMode);
		//RegisterProperty<double>("UpdateFrequency", &GASS::OISInputSystem::GetUpdateFrequency, &GASS::OISInputSystem::SetUpdateFrequency);
		RegisterProperty<float>("GameControllerAxisMinValue", &GASS::OISInputSystem::GetGameControllerAxisMinValue, &GASS::OISInputSystem::SetGameControllerAxisMinValue);
		RegisterProperty<bool>("EnableKey", &GASS::OISInputSystem::GetEnableKey, &GASS::OISInputSystem::SetEnableKey);
		RegisterProperty<bool>("EnableMouse", &GASS::OISInputSystem::GetEnableMouse, &GASS::OISInputSystem::SetEnableMouse);
		RegisterProperty<bool>("EnableJoystick", &GASS::OISInputSystem::GetEnableJoystick, &GASS::OISInputSystem::SetEnableJoystick);
		RegisterProperty<bool>("OnlyProxy", &GASS::OISInputSystem::GetOnlyProxy, &GASS::OISInputSystem::SetOnlyProxy);

	}

	void OISInputSystem::OnCreate(SimSystemManagerPtr owner)
	{
		SimSystem::OnCreate(owner);
		//GetSimSystemManager()->RegisterForMessage(REG_TMESS(OISInputSystem::OnInit,RenderWindowCreatedEvent,1));
	}

	void OISInputSystem::Init()
	{
		if(m_OnlyProxy)
		{
			//SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);
			SimEngine::Get().GetSimSystemManager()->PostMessage(SystemMessagePtr(new InputSystemLoadedEvent()));
		}
	}

	void OISInputSystem::SetMainWindowHandle(void *main_win_handle)
	{
		if(m_Window)
		{
			GASS_EXCEPT(Exception::ERR_FILE_NOT_FOUND,"Main window already present","OISInputSystem::SetMainWindowHandle");
		}
		if(!m_OnlyProxy)
		{
			//SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);
			m_Window = main_win_handle;
			OIS::ParamList pl;
			size_t windowHnd = (size_t) m_Window;
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
			bool bufferedJoy = false;
			m_Keyboard = static_cast<OIS::Keyboard*>(m_InputManager->createInputObject( OIS::OISKeyboard, bufferedKeys ));
			m_Keyboard->setEventCallback(this);
			m_Mouse = static_cast<OIS::Mouse*>(m_InputManager->createInputObject( OIS::OISMouse, bufferedMouse ));
			m_Mouse->setEventCallback(this);

#ifdef WIN32
			if (m_InputManager->getNumberOfDevices(OIS::OISJoyStick) > 0) {
				LogManager::getSingleton().stream() << "OISInputSystem:" <<m_InputManager->getNumberOfDevices(OIS::OISJoyStick) <<  "joysticks available";
				try {
					for (int i = 0; i < m_InputManager->getNumberOfDevices(OIS::OISJoyStick); i++)
					{
						OIS::JoyStick* joy = static_cast<OIS::JoyStick*>(m_InputManager->createInputObject(OIS::OISJoyStick, bufferedJoy ));
						joy->setEventCallback(this);
						joy->setBuffered(true);
						joy->capture();
						LogManager::getSingleton().stream() << "OISInputSystem: Joystick ID" << joy->getID() <<  " " << joy->vendor() << " " << joy->getNumberOfComponents(OIS::OIS_Axis) << " axes " <<  joy->getNumberOfComponents(OIS::OIS_Button) << " buttons " << joy->getNumberOfComponents(OIS::OIS_POV) << " hats";
						m_Joys.push_back(joy);
						//m_JoyState.push_back(joy->getJoyStickState());
						//m_OldJoyState.push_back(joy->getJoyStickState());
					}
				} catch(std::exception& e) {
					LogManager::getSingleton().stream() << "OISInputSystem: Exception caught while initializing joysticks:" <<  e.what();
				} catch(...) {
					LogManager::getSingleton().stream() << "OISInputSystem: Something caught while initializing joysticks";
				}
			} else {
				LogManager::getSingleton().stream() <<  "OISInputSystem: No joystick";
			}
#else
			if (m_InputManager->getNumberOfDevices(OIS::OISJoyStick) > 0) {
				LogManager::getSingleton().stream() << "OISInputSystem:" <<m_InputManager->getNumberOfDevices(OIS::OISJoyStick) <<  "joysticks available";
				try {
					for (int i = 0; i < m_InputManager->getNumberOfDevices(OIS::OISJoyStick); i++) {
						OIS::JoyStick* joy = static_cast<OIS::JoyStick*>(m_InputManager->createInputObject( OIS::OISJoyStick, bufferedJoy ));
						LogManager::getSingleton().stream() << "OISInputSystem: Joystick ID" << joy->getID() <<  " " << joy->vendor() << " " << joy->getNumberOfComponents(OIS::OIS_Axis) << " axes " <<  joy->getNumberOfComponents(OIS::OIS_Button) << " buttons " << joy->getNumberOfComponents(OIS::OIS_POV) << " hats";
						joy->capture();
						m_Joys.push_back(joy);
						m_JoyState.push_back(joy->getJoyStickState());
						m_OldJoyState.push_back(joy->getJoyStickState());
					}
				} catch(std::exception& e) {
					LogManager::getSingleton().stream() << "OISInputSystem: Exception caught while initializing joysticks:" <<  e.what();
				} catch(...) {
					LogManager::getSingleton().stream() <<  "OISInputSystem: No joystick";
				}
			} else {
				LogManager::getSingleton().stream() <<  "OISInputSystem: No joystick";
			}
#endif

			GraphicsSystemPtr gs = GetSimSystemManager()->GetFirstSystemByClass<IGraphicsSystem>();
			if(gs)
			{
				RenderWindowPtr window = gs->GetMainRenderWindow();
				const OIS::MouseState &ms = m_Mouse->getMouseState();
				ms.width = window->GetWidth();
				ms.height = window->GetHeight();
				m_MouseWinWidth = ms.width;
				m_MouseWinHeight = ms.height;
			}
			SimEngine::Get().GetSimSystemManager()->PostMessage(SystemMessagePtr(new InputSystemLoadedEvent()));
		}

	}

	void OISInputSystem::Shutdown()
	{
		if(m_InputManager)
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
	}

	void OISInputSystem::OnViewportMovedOrResized(RenderWindowResizedEventPtr message)
	{
		if(m_Mouse)
		{
			if(message->GetWindowName() == "MainWindow")
			{
				const OIS::MouseState &ms = m_Mouse->getMouseState();
				ms.width = message->GetWidth();
				ms.height = message->GetHeight();
			}
		}
	}

	void OISInputSystem::ClipInputWindow(int left,int top,int right,int bottom)
	{
		m_MouseWinOffsetX = left;
		m_MouseWinOffsetY = top;
		m_MouseWinWidth = right-left;
		m_MouseWinHeight = bottom-top;
	}

	void OISInputSystem::Update(double delta_time,TaskNode2* caller)
	{
		if(m_Window == 0)
			return;

		if(m_KeyActive)
		{
			m_Keyboard->capture();
			memcpy(m_OldKeyBuffer,m_KeyBuffer,256);
			m_Keyboard->copyKeyStates(m_KeyBuffer);
		}
		if(m_MouseActive)
		{
			m_Mouse->capture();

			m_OldMouseState = m_MouseState;
			m_MouseState = m_Mouse->getMouseState();
		}

		if(m_JoyActive)
		{
			for (int i = 0; i < m_Joys.size(); i++)
			{
				m_Joys[i]->capture();
				//m_OldJoyState[i] = m_JoyState[i];
				//m_JoyState[i] = m_Joys[i]->getJoyStickState();
			}
		}
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
			else ++iter;
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
			else ++iter;
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
			else ++iter;
		}
	}

	bool OISInputSystem::keyPressed( const OIS::KeyEvent &arg )
	{

		std::vector<IKeyListener*>::iterator iter = m_KeyListeners.begin();
		while(iter != m_KeyListeners.end())
		{
			IKeyListener* kl = *iter;
			kl->KeyPressed(arg.key,arg.text);
			++iter;
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
			++iter;
		}
		return true;
	}

	MouseData OISInputSystem::ToGASS(const OIS::MouseEvent &arg)
	{
		MouseData data;
		data.XRel = NormalizeMouseDelta(arg.state.X.rel);
		data.YRel = NormalizeMouseDelta(arg.state.Y.rel);
		data.ZRel = NormalizeMouseDelta(arg.state.Z.rel);

		data.XAbs = arg.state.X.abs - m_MouseWinOffsetX;
		data.YAbs = arg.state.Y.abs - m_MouseWinOffsetY;

		data.XAbsNorm = float(data.XAbs)/float(m_MouseWinWidth);
		data.YAbsNorm = float(data.YAbs)/float(m_MouseWinHeight);
		data.ZAbs = arg.state.Z.abs;
		return data;
	}

	bool OISInputSystem::mouseMoved( const OIS::MouseEvent &arg )
	{
		MouseData data = ToGASS(arg);

		//dont send if mouse outside window
		if(data.XAbsNorm < 0.0 || data.XAbsNorm > 1.0 || data.YAbsNorm < 0.0 || data.YAbsNorm > 1.0)
			return false;

		std::vector<IMouseListener*>::iterator iter = m_MouseListeners.begin();
		while(iter != m_MouseListeners.end())
		{
			IMouseListener* ml = *iter;
			//Normalize values
			ml->MouseMoved(data);
			++iter;
		}
		return true;
	}

	MouseButtonId OISInputSystem::ToGASS(OIS::MouseButtonID ois_id) const
	{
		MouseButtonId ret;
		switch(ois_id)
		{
		case OIS::MB_Left:
			ret = MBID_LEFT;
			break;
		case OIS::MB_Right:
			ret = MBID_RIGHT;
			break;
		case OIS::MB_Middle:
			ret = MBID_MIDDLE;
			break;
		case OIS::MB_Button3:
			ret = MBID_3;
			break;
		case OIS::MB_Button4:
			ret = MBID_4;
			break;
		case OIS::MB_Button5:
			ret = MBID_5;
			break;
		case OIS::MB_Button6:
			ret = MBID_6;
			break;
		case OIS::MB_Button7:
			ret = MBID_7;
			break;
		}
		return ret;
	}

	bool OISInputSystem::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		MouseData data = ToGASS(arg);

		//dont send if mouse outside window
		if(data.XAbsNorm < 0.0 || data.XAbsNorm > 1.0 || data.YAbsNorm < 0.0 || data.YAbsNorm > 1.0)
			return false;

		std::vector<IMouseListener*>::iterator iter = m_MouseListeners.begin();
		while(iter != m_MouseListeners.end())
		{
			IMouseListener* ml = *iter;
			ml->MousePressed(data,ToGASS(id));
			++iter;
		}
		return true;
	}



	bool OISInputSystem::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{

		MouseData data = ToGASS(arg);
		//dont send if mouse outside window
		if(data.XAbsNorm < 0.0 || data.XAbsNorm > 1.0 || data.YAbsNorm < 0.0 || data.YAbsNorm > 1.0)
			return false;

		std::vector<IMouseListener*>::iterator iter = m_MouseListeners.begin();
		while(iter != m_MouseListeners.end())
		{
			IMouseListener* ml = *iter;
			ml->MouseReleased(data,ToGASS(id));
			++iter;
		}
		return true;
	}

	float OISInputSystem::NormalizeMouseDelta(float value)
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
			++iter;
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
			++iter;
		}
		return true;
	}

	bool OISInputSystem::axisMoved( const OIS::JoyStickEvent &arg, int axis )
	{
		std::vector<IGameControllerListener*>::iterator iter = m_GameControllerListeners.begin();
		float value = ((float)arg.state.mAxes[axis].abs) / 32768.0;

		//Clamp axis value?
		if(fabs(value) < m_GameControllerAxisMinValue) //clamp
			value  = 0;


		while(iter != m_GameControllerListeners.end())
		{

			IGameControllerListener* ml = *iter;

			ml->AxisMoved(arg.device->getID(),axis, value);
			++iter;
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


	void OISInputSystem::SetEnableKey(bool value)
	{
		m_KeyActive = value;
	}
	void OISInputSystem::SetEnableJoystick(bool value)
	{
		m_JoyActive = value;
	}
	void OISInputSystem::SetEnableMouse(bool value)
	{
		m_MouseActive = value;
	}

	bool OISInputSystem::GetEnableKey() const
	{
		return m_KeyActive;
	}

	bool OISInputSystem::GetEnableJoystick() const
	{
		return m_JoyActive;
	}

	bool OISInputSystem::GetEnableMouse() const
	{
		return m_MouseActive;
	}

	void OISInputSystem::InjectMouseMoved(const MouseData &data)
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

	void OISInputSystem::InjectMousePressed(const MouseData &data, MouseButtonId id )
	{
		std::vector<IMouseListener*>::iterator iter = m_MouseListeners.begin();
		while(iter != m_MouseListeners.end())
		{
			IMouseListener* ml = *iter;
			ml->MousePressed(data,id);
			++iter;
		}
	}

	void OISInputSystem::InjectMouseReleased(const MouseData &data, MouseButtonId id )
	{
		std::vector<IMouseListener*>::iterator iter = m_MouseListeners.begin();
		while(iter != m_MouseListeners.end())
		{
			IMouseListener* ml = *iter;
			ml->MouseReleased(data,id);
			++iter;
		}
	}

	void OISInputSystem::InjectKeyPressed( int key, unsigned int text)
	{
		std::vector<IKeyListener*>::iterator iter = m_KeyListeners.begin();
		while(iter != m_KeyListeners.end())
		{
			IKeyListener* kl = *iter;
			kl->KeyPressed(key,text);
			++iter;
		}
	}
	void OISInputSystem::InjectKeyReleased( int key, unsigned int text)
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

