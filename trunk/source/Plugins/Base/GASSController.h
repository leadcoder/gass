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
#include "GASSControlSetting.h"
#include <map>
#include <string>
#include "Sim/Systems/Input/GASSIInputSystem.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{
	
	
	class Controller
	{
	public:
		Controller(const std::string &name,ControlSetting* owner) :
			m_Name(name),
			m_Owner(owner)
		{

		}
		virtual ~Controller(){}
		int m_Device;
		bool m_NonRepeating;
		ControlSetting* m_Owner;
		std::string m_Name;
	};

	class RemoteController : public Controller
	{
	public:
		RemoteController(const std::string &name,ControlSetting* owner):Controller(name,owner){}
		virtual ~RemoteController(){}

	};


	class AxisController : public Controller
	{
		
	public:
		AxisController(const std::string &name,ControlSetting* owner) : Controller(name,owner)
		{

		}
		float virtual GetValue() const {return 0;};
	};

	class TriggerController : public Controller
	{
	public:
		TriggerController(const std::string &name,ControlSetting* owner) : Controller(name,owner)
		{

		}
		float virtual GetValue() const {return 0;};
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
				MessagePtr system_msg(new ControllerMessage(m_Name,1.0,CT_TRIGGER));
				m_Owner->GetMessageManager()->PostMessage(system_msg);
			}
			return true;
		}

		bool KeyReleased( int key, unsigned int text)
		{
			if(key == m_Key)
			{
				MessagePtr system_msg(new ControllerMessage(m_Name,0.0,CT_TRIGGER));
				m_Owner->GetMessageManager()->PostMessage(system_msg);
			}
			return true;
		}		
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

		virtual bool MouseMoved(const MouseData &data)
		{
			return true;
		}

		virtual bool MousePressed(const MouseData &data, MouseButtonId id )
		{
			if(m_Button == id)
			{
				switch(m_Device)
				{
				case DEVICE_MOUSE:
				
					MessagePtr system_msg(new ControllerMessage(m_Name,1.0,CT_TRIGGER));
					m_Owner->GetMessageManager()->PostMessage(system_msg);
					break;
				}
			}
			return true;
		}
		virtual bool MouseReleased(const MouseData &data,MouseButtonId id )
		{
			if(m_Button == id)
			{
				switch(m_Device)
				{
				case DEVICE_MOUSE:
				
					MessagePtr system_msg(new ControllerMessage(m_Name,0.0,CT_TRIGGER));
					m_Owner->GetMessageManager()->PostMessage(system_msg);
					break;
				}
			}
			return true;

		}

		virtual bool ButtonPressed(int device, int button)
		{
			if(m_Button == button && device == m_Device - DEVICE_GAME_CONTROLLER_0)
			{
				
				MessagePtr system_msg(new ControllerMessage(m_Name,1.0,CT_TRIGGER));
				m_Owner->GetMessageManager()->PostMessage(system_msg);
			}
			return true;	
		}
		virtual bool ButtonReleased(int device,int button)
		{
			if(m_Button == button && device == m_Device - DEVICE_GAME_CONTROLLER_0)
			{
				
				MessagePtr system_msg(new ControllerMessage(m_Name,0.0,CT_TRIGGER));
				m_Owner->GetMessageManager()->PostMessage(system_msg);
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
				
				MessagePtr system_msg(new ControllerMessage(m_Name,value,CT_AXIS));
				m_Owner->GetMessageManager()->PostMessage(system_msg);
			}
			return true;
		}

		bool KeyReleased( int key, unsigned int text)
		{
			if(key == m_PosKey || key == m_NegKey)
			{
			
				MessagePtr system_msg(new ControllerMessage(m_Name,0.0,CT_AXIS));
				m_Owner->GetMessageManager()->PostMessage(system_msg);
			}
			return true;
		}

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

		virtual bool MouseMoved(const MouseData &data)
		{
			float value = 0;
			switch(m_Device)
			{
			case DEVICE_MOUSE:
				switch(m_Axis)
				{
				case INPUT_AXIS_0:
					value  = m_Invert* data.XRel;
					break;
				case INPUT_AXIS_1:
					value  = m_Invert* data.YRel;
					break;
				case INPUT_AXIS_2:
					value  = m_Invert* data.ZRel;
					break;
				default:
					value  = 0;
				}
				
				MessagePtr system_msg(new ControllerMessage(m_Name,value,CT_AXIS));
				m_Owner->GetMessageManager()->PostMessage(system_msg);
				break;
			}
			return true;

		}
		virtual bool MousePressed(const MouseData &data, MouseButtonId id )
		{
			return true;

		}
		virtual bool MouseReleased(const MouseData &data, MouseButtonId id )
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
				value = value*m_Invert;
				MessagePtr system_msg(new ControllerMessage(m_Name,value,CT_AXIS));
				m_Owner->GetMessageManager()->PostMessage(system_msg);
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

		int m_Axis;
		int m_Invert;
	};

	class ButtonAxisController : public AxisController
	{
	public:
		ButtonAxisController(const std::string &name,ControlSetting* owner) : AxisController(name,owner)
		{

		}
		int m_PosKey;
		int m_NegKey;
	};
}




