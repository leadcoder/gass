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

#include "Sim/Common.h"
#include <map>
#include <string>
#include "Sim/Systems/Input/IInputSystem.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"

namespace GASS
{
	class Controller
	{
	public:
		Controller(const std::string &name,ControlSetting* owner) :
			m_Override(false), 
			m_Value(0),
			m_LastValue(0),
			m_Name(name),
			m_Owner(owner)
		{

		}
		virtual ~Controller(){}
		virtual float GetValue(){return 0;};
		virtual void SetValue(float value){m_Override = true; m_Value = value;}
		virtual void DisableOverride(){m_Override = false; m_Value = 0;}
		virtual void TryValue(float value){if(!m_Override) m_Value = value;}
		virtual void SetLastValue(float value){m_LastValue = value;}
		virtual bool HasChanged(){return (m_LastValue != GetValue());}
		int m_Device;
		bool m_Override;
		float m_Value;
		float m_LastValue;
		bool m_NonRepeating;
		ControlSetting* m_Owner;
		std::string m_Name;
	};

	class RemoteController : public Controller
	{
	public:
		RemoteController(const std::string &name,ControlSetting* owner):Controller(name,owner){}
		virtual ~RemoteController(){}
		float virtual GetValue(){return m_Value;};
		void virtual DisableOverride(){m_Override = false;}
	};


	class AxisController : public Controller
	{
		
	public:
		AxisController(const std::string &name,ControlSetting* owner) : Controller(name,owner)
		{

		}
		float virtual GetValue(){return 0;};
	};

	class TriggerController : public Controller
	{
	public:
		TriggerController(const std::string &name,ControlSetting* owner) : Controller(name,owner)
		{

		}
		float virtual GetValue(){return 0;};
	};

	class KeyTriggerController : public TriggerController, public IKeyListener
	{
	public:
		KeyTriggerController(const std::string &name,ControlSetting* owner) : TriggerController(name,owner)
		{
			m_Owner->GetInputSystem()->AddKeyListener(this);
		}

		~KeyTriggerController()
		{
			m_Owner->GetInputSystem()->RemoveKeyListener(this);
		}

		bool KeyPressed( int key, unsigned int text)
		{
			if(key == m_Key)
			{
				int id = (int) this;
				MessagePtr system_msg(new Message(ControlSetting::CONTROLLER_MESSAGE_NEW_INPUT,id));
				system_msg->SetData("Controller",m_Name);
				system_msg->SetData("Value",1.0f);
				m_Owner->GetMessageManager()->SendGlobalMessage(system_msg);
			}
			return true;
		}

		bool KeyReleased( int key, unsigned int text)
		{
			if(key == m_Key)
			{
				int id = (int) this;
				MessagePtr system_msg(new Message(ControlSetting::CONTROLLER_MESSAGE_NEW_INPUT,id));
				system_msg->SetData("Controller",m_Name);
				system_msg->SetData("Value",0.0f);
				m_Owner->GetMessageManager()->SendGlobalMessage(system_msg);
			}
			return true;
		}

		float virtual GetValue()
		{
			if(m_Override) return m_Value;
			int ret = 0;
			switch(m_Device)
			{
			case DEVICE_KEYBOARD:
				if(m_NonRepeating) ret = m_Owner->m_Input->KeyDown(m_Key);
				else ret = m_Owner->m_Input->KeyStillDown(m_Key);
				break;
			default:
				ret = 0;
			}
			if(m_Override) ret = m_Value;
			return (float) ret;
		};
		int m_Key;
	};

	class ButtonTriggerController : public TriggerController, public IMouseListener, public IGameControllerListener
	{
	public:
		ButtonTriggerController(const std::string &name,ControlSetting* owner) : TriggerController(name,owner)
		{
			m_Owner->GetInputSystem()->AddMouseListener(this);
			m_Owner->GetInputSystem()->AddGameControllerListener(this);
		}

		~ButtonTriggerController()
		{
			m_Owner->GetInputSystem()->RemoveMouseListener(this);
			m_Owner->GetInputSystem()->RemoveGameControllerListener(this);
		}

		virtual bool MouseMoved(float x,float y)
		{
			return true;
		}

		virtual bool MousePressed( int id )
		{
			if(m_Button == id)
			{
				switch(m_Device)
				{
				case DEVICE_MOUSE:
					int id = (int) this;
					MessagePtr system_msg(new Message(ControlSetting::CONTROLLER_MESSAGE_NEW_INPUT,id));
					system_msg->SetData("Controller",m_Name);
					system_msg->SetData("Value",1.0f);
					m_Owner->GetMessageManager()->SendGlobalMessage(system_msg);
					break;
				}
			}
			return true;
		}
		virtual bool MouseReleased( int id )
		{
			if(m_Button == id)
			{
				switch(m_Device)
				{
				case DEVICE_MOUSE:
					int id = (int) this;
					MessagePtr system_msg(new Message(ControlSetting::CONTROLLER_MESSAGE_NEW_INPUT,id));
					system_msg->SetData("Controller",m_Name);
					system_msg->SetData("Value",0.0f);
					m_Owner->GetMessageManager()->SendGlobalMessage(system_msg);
					break;
				}
			}
			return true;

		}

		virtual bool ButtonPressed(int device, int button)
		{
			if(m_Button == button && device == m_Device - DEVICE_GAME_CONTROLLER_0)
			{
				int id = (int) this;
				MessagePtr system_msg(new Message(ControlSetting::CONTROLLER_MESSAGE_NEW_INPUT,id));
				system_msg->SetData("Controller",m_Name);
				system_msg->SetData("Value",1.0f);
				m_Owner->GetMessageManager()->SendGlobalMessage(system_msg);
			}
			return true;	
		}
		virtual bool ButtonReleased(int device,int button)
		{
			if(m_Button == button && device == m_Device - DEVICE_GAME_CONTROLLER_0)
			{
				int id = (int) this;
				MessagePtr system_msg(new Message(ControlSetting::CONTROLLER_MESSAGE_NEW_INPUT,id));
				system_msg->SetData("Controller",m_Name);
				system_msg->SetData("Value",0.0f);
				m_Owner->GetMessageManager()->SendGlobalMessage(system_msg);
			}
			return true;	
		}
		virtual bool AxisMoved(int device,int axis,float value)
		{
			return true;	
		}
		//Joystick Event, amd sliderID
		virtual bool SliderMoved(int device,int axis, float value)
		{
			return true;	
		}
		//Joystick Event, amd povID
		virtual bool PovMoved(int device,int axis, float value)
		{
			return true;	
		}

		float virtual GetValue()
		{
			if(m_Override) return m_Value;
			int ret = 0;
			switch(m_Device)
			{
			case DEVICE_MOUSE:
				if(m_NonRepeating) ret = m_Owner->m_Input->ButtonDown(m_Button);
				else ret = m_Owner->m_Input->ButtonStillDown(m_Button);
				break;
			case DEVICE_GAME_CONTROLLER_0:
			case DEVICE_GAME_CONTROLLER_1:
			case DEVICE_GAME_CONTROLLER_2:
			case DEVICE_GAME_CONTROLLER_3:
			case DEVICE_GAME_CONTROLLER_4:
				if(m_NonRepeating) ret = m_Owner->m_Input->JoystickButtonDown(m_Device-DEVICE_GAME_CONTROLLER_0, m_Button);
				else ret = m_Owner->m_Input->JoystickButtonStillDown(m_Device-DEVICE_GAME_CONTROLLER_0, m_Button);
				break;
			default:
				ret = 0;
			}
			return (float) ret;
		};
		int m_Button;
	};

	class KeyAxisController : public AxisController , public IKeyListener
	{
	public:
		KeyAxisController(const std::string &name,ControlSetting* owner) : AxisController(name,owner)
		{
			m_Owner->GetInputSystem()->AddKeyListener(this);
		}

		~KeyAxisController()
		{
			m_Owner->GetInputSystem()->RemoveKeyListener(this);
		}
		bool KeyPressed(int key, unsigned int text)
		{
			if(key == m_PosKey || key == m_NegKey)
			{
				float value = 0;
				if(key == m_PosKey)
				{
					value = 1.0f;

				}
				else
				{
					value = -1.0f;
				}
				int id = (int) this;
				MessagePtr system_msg(new Message(ControlSetting::CONTROLLER_MESSAGE_NEW_INPUT,id));
				system_msg->SetData("Controller",m_Name);
				system_msg->SetData("Value",value);
				m_Owner->GetMessageManager()->SendGlobalMessage(system_msg);
			}
			return true;
		}

		bool KeyReleased( int key, unsigned int text)
		{
			if(key == m_PosKey || key == m_NegKey)
			{
				int id = (int) this;
				MessagePtr system_msg(new Message(ControlSetting::CONTROLLER_MESSAGE_NEW_INPUT,id));
				system_msg->SetData("Controller",m_Name);
				system_msg->SetData("Value",0.0f);
				m_Owner->GetMessageManager()->SendGlobalMessage(system_msg);
			}
			return true;
		}


		float virtual GetValue()
		{
			if(m_Override) return m_Value;
			int ret = 0;
			switch(m_Device)
			{	
			case DEVICE_KEYBOARD:
				if(m_Owner->m_Input->CurKey(m_PosKey)) ret = 1;
				if(m_Owner->m_Input->CurKey(m_NegKey)) ret = -1;
				break;
			default:
				ret = 0;
			}
			return (float) ret;
		};
		int m_PosKey;
		int m_NegKey;
	};

	class AxisAxisController : public AxisController , public IMouseListener, public IGameControllerListener
	{
	public:
		AxisAxisController(const std::string &name,ControlSetting* owner) : AxisController(name,owner)
		{
			m_Owner->GetInputSystem()->AddMouseListener(this);
			m_Owner->GetInputSystem()->AddGameControllerListener(this);
		}
		
		~AxisAxisController()
		{
			m_Owner->GetInputSystem()->RemoveMouseListener(this);
			m_Owner->GetInputSystem()->RemoveGameControllerListener(this);
		}

		virtual bool MouseMoved(float x,float y)
		{
			float value = 0;
			switch(m_Device)
			{
			case DEVICE_MOUSE:
				switch(m_Axis)
				{
				case INPUT_AXIS_0:
					value  = m_Invert* x;
					break;
				case INPUT_AXIS_1:
					value  = m_Invert* y;
					break;
				case INPUT_AXIS_2:
					//ret = m_Invert* z;
					break;
				default:
					value  = 0;
				}
				int id = (int) this;
				MessagePtr system_msg(new Message(ControlSetting::CONTROLLER_MESSAGE_NEW_INPUT,id));
				system_msg->SetData("Controller",m_Name);
				system_msg->SetData("Value",value);
				m_Owner->GetMessageManager()->SendGlobalMessage(system_msg);
				break;
			}
			return true;

		}
		virtual bool MousePressed( int id )
		{
			return true;

		}
		virtual bool MouseReleased( int id )
		{
			return true;

		}


		virtual bool ButtonPressed(int device, int button)
		{
			
			return true;	
		}
		virtual bool ButtonReleased(int device,int button)
		{
			return true;	
		}
		virtual bool AxisMoved(int device,int axis,float value)
		{
			if(device == m_Device-DEVICE_GAME_CONTROLLER_0 && axis == m_Axis-INPUT_AXIS_0)
			{
				int id = (int) this;
				MessagePtr system_msg(new Message(ControlSetting::CONTROLLER_MESSAGE_NEW_INPUT,id));
				system_msg->SetData("Controller",m_Name);
				value = value*m_Invert;
				system_msg->SetData("Value",value);
				m_Owner->GetMessageManager()->SendGlobalMessage(system_msg);
			}
			return true;
		}
		//Joystick Event, amd sliderID
		virtual bool SliderMoved(int device,int axis, float value)
		{
			return true;	
		}
		//Joystick Event, amd povID
		virtual bool PovMoved(int device,int axis, float value)
		{
			return true;	
		}

		float virtual GetValue()
		{
			if(m_Override) return m_Value;
			float ret = 0;
			switch(m_Device)
			{
			case DEVICE_MOUSE:
				switch(m_Axis)
				{
				case INPUT_AXIS_0:
					ret = m_Invert* m_Owner->m_Input->GetCursorDeltaX();
					break;
				case INPUT_AXIS_1:
					ret = m_Invert* m_Owner->m_Input->GetCursorDeltaY();
					break;
				case INPUT_AXIS_2:
					ret = m_Invert* m_Owner->m_Input->GetScrollWheelDelta();
					break;
				default:
					ret = 0;
				}
				break;
			case DEVICE_GAME_CONTROLLER_0:
			case DEVICE_GAME_CONTROLLER_1:
			case DEVICE_GAME_CONTROLLER_2:
			case DEVICE_GAME_CONTROLLER_3:
			case DEVICE_GAME_CONTROLLER_4:
				switch(m_Axis)
				{
				case INPUT_AXIS_0:
					ret = m_Invert*m_Owner->m_Input->GetJoystickAxis(m_Device-DEVICE_GAME_CONTROLLER_0,0);
					break;
				case INPUT_AXIS_1:
					ret = m_Invert*m_Owner->m_Input->GetJoystickAxis(m_Device-DEVICE_GAME_CONTROLLER_0,1);
					break;
				case INPUT_AXIS_2:
					ret = m_Invert*m_Owner->m_Input->GetJoystickAxis(m_Device-DEVICE_GAME_CONTROLLER_0,2);
					break;
				case INPUT_AXIS_3:
					ret = m_Invert*m_Owner->m_Input->GetJoystickAxis(m_Device-DEVICE_GAME_CONTROLLER_0,3);
					break;
				case INPUT_AXIS_4:
					ret = m_Invert*m_Owner->m_Input->GetJoystickAxis(m_Device-DEVICE_GAME_CONTROLLER_0,4);
					break;
				case INPUT_AXIS_5:
					ret = m_Invert*m_Owner->m_Input->GetJoystickAxis(m_Device-DEVICE_GAME_CONTROLLER_0,5);
					break;
				case INPUT_AXIS_6:
					ret = m_Invert*m_Owner->m_Input->GetJoystickAxis(m_Device-DEVICE_GAME_CONTROLLER_0,6);
					break;
				case INPUT_AXIS_7:
					ret = m_Invert*m_Owner->m_Input->GetJoystickAxis(m_Device-DEVICE_GAME_CONTROLLER_0,7);
					break;

				case INPUT_POV_0_X:
					if (m_Owner->m_Input->GetJoystickPOV(m_Device-DEVICE_GAME_CONTROLLER_0,0) & POV_EAST)
						ret = 1;
					else if (m_Owner->m_Input->GetJoystickPOV(m_Device-DEVICE_GAME_CONTROLLER_0,0) & POV_WEST)
						ret = -1;
					ret *= m_Invert;
					break;
				case INPUT_POV_0_Y:
					if (m_Owner->m_Input->GetJoystickPOV(m_Device-DEVICE_GAME_CONTROLLER_0,0) & POV_SOUTH)
						ret = 1;
					else if (m_Owner->m_Input->GetJoystickPOV(m_Device-DEVICE_GAME_CONTROLLER_0,0) & POV_NORTH)
						ret = -1;
					ret *= m_Invert;
					break;
				case INPUT_POV_1_X:
					if (m_Owner->m_Input->GetJoystickPOV(m_Device-DEVICE_GAME_CONTROLLER_0,1) & POV_EAST)
						ret = 1;
					else if (m_Owner->m_Input->GetJoystickPOV(m_Device-DEVICE_GAME_CONTROLLER_0,1) & POV_WEST)
						ret = -1;
					ret *= m_Invert;
					break;
				case INPUT_POV_1_Y:
					if (m_Owner->m_Input->GetJoystickPOV(m_Device-DEVICE_GAME_CONTROLLER_0,1) & POV_SOUTH)
						ret = 1;
					else if (m_Owner->m_Input->GetJoystickPOV(m_Device-DEVICE_GAME_CONTROLLER_0,1) & POV_NORTH)
						ret = -1;
					ret *= m_Invert;
					break;
				case INPUT_POV_2_X:
					if (m_Owner->m_Input->GetJoystickPOV(m_Device-DEVICE_GAME_CONTROLLER_0,2) & POV_EAST)
						ret = 1;
					else if (m_Owner->m_Input->GetJoystickPOV(m_Device-DEVICE_GAME_CONTROLLER_0,2) & POV_WEST)
						ret = -1;
					ret *= m_Invert;
					break;
				case INPUT_POV_2_Y:
					if (m_Owner->m_Input->GetJoystickPOV(m_Device-DEVICE_GAME_CONTROLLER_0,2) & POV_SOUTH)
						ret = 1;
					else if (m_Owner->m_Input->GetJoystickPOV(m_Device-DEVICE_GAME_CONTROLLER_0,2) & POV_NORTH)
						ret = -1;
					ret *= m_Invert;
					break;
				case INPUT_POV_3_X:
					if (m_Owner->m_Input->GetJoystickPOV(m_Device-DEVICE_GAME_CONTROLLER_0,3) & POV_EAST)
						ret = 1;
					else if (m_Owner->m_Input->GetJoystickPOV(m_Device-DEVICE_GAME_CONTROLLER_0,3) & POV_WEST)
						ret = -1;
					ret *= m_Invert;
					break;
				case INPUT_POV_3_Y:
					if (m_Owner->m_Input->GetJoystickPOV(m_Device-DEVICE_GAME_CONTROLLER_0,3) & POV_SOUTH)
						ret = 1;
					else if (m_Owner->m_Input->GetJoystickPOV(m_Device-DEVICE_GAME_CONTROLLER_0,3) & POV_NORTH)
						ret = -1;
					ret *= m_Invert;
					break;

				default:
					ret = 0;
				}
				break;
			default:
				ret = 0;
			}
			return (float) ret;
		}
		int m_Axis;
		int m_Invert;
	};

	class ButtonAxisController : public AxisController
	{
	public:
		ButtonAxisController(const std::string &name,ControlSetting* owner) : AxisController(name,owner)
		{

		}
		float virtual GetValue()
		{
			if(m_Override) return m_Value;
			int ret = 0;
			switch(m_Device)
			{	
			case DEVICE_GAME_CONTROLLER_0:
			case DEVICE_GAME_CONTROLLER_1:
			case DEVICE_GAME_CONTROLLER_2:
			case DEVICE_GAME_CONTROLLER_3:
			case DEVICE_GAME_CONTROLLER_4:
				if (m_Owner->m_Input->CurJoystickButton(m_Device-DEVICE_GAME_CONTROLLER_0, m_PosKey)) ret = 1;
				if (m_Owner->m_Input->CurJoystickButton(m_Device-DEVICE_GAME_CONTROLLER_0, m_NegKey)) ret = -1;
				break;
			default:
				ret = 0;
			}
			return (float) ret;
		};

		int m_PosKey;
		int m_NegKey;
	};
}




