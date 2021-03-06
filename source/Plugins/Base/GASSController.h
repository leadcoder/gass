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
#include "GASSControlSetting.h"
#include "GASSControlSettingsSystem.h"
#include "Sim/Interface/GASSIInputSystem.h"
#include "Sim//GASSSimSystemManager.h"

namespace GASS
{
	
	
	class Controller
	{
	public:
		Controller(const std::string &name,ControlSetting* owner) :
			m_Name(name),
			m_Owner(owner),
			m_Device(0),
			m_NonRepeating(true)
		{

		}
		virtual ~Controller(){}

		void FireInput(float value,ControllerType ctype)
		{
			SystemMessagePtr system_msg(new ControllSettingsMessage(m_Owner->m_Name, m_Name,value,ctype));
			m_Owner->m_Owner->GetSimSystemManager()->PostMessage(system_msg);
		}

		int m_Device;
		bool m_NonRepeating;
		ControlSetting* m_Owner;
		std::string m_Name;
	};

	class RemoteController : public Controller
	{
	public:
		RemoteController(const std::string &name,ControlSetting* owner):Controller(name,owner){}
		~RemoteController() override {}
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
		KeyTriggerController(const std::string &name,ControlSetting* owner) : TriggerController(name,owner), m_Key(0)
		{
			m_Owner->GetInputSystem()->AddKeyListener(this);
		}

		~KeyTriggerController() override
		{
			m_Owner->GetInputSystem()->RemoveKeyListener(this);
		}

		bool KeyPressed( int key, unsigned int /*text*/)  override
		{
			if(key == m_Key)
			{
				FireInput(1.0,CT_TRIGGER);
				
			}
			return true;
		}

		bool KeyReleased( int key, unsigned int /*text*/) override
		{
			if(key == m_Key)
			{
				FireInput(0.0,CT_TRIGGER);
				
			}
			return true;
		}		
		int m_Key;
	};

	class ButtonTriggerController : public TriggerController, public IMouseListener, public IGameControllerListener
	{
	public:
		ButtonTriggerController(const std::string &name,ControlSetting* owner) : TriggerController(name,owner), m_Button(0)
		{
			m_Owner->GetInputSystem()->AddMouseListener(this);
			m_Owner->GetInputSystem()->AddGameControllerListener(this);
		}

		~ButtonTriggerController() override
		{
			m_Owner->GetInputSystem()->RemoveMouseListener(this);
			m_Owner->GetInputSystem()->RemoveGameControllerListener(this);
		}

		bool MouseMoved(const MouseData &/*data*/) override
		{
			return true;
		}

		bool MousePressed(const MouseData &/*data*/, MouseButtonId id ) override
		{
			if(m_Button == id)
			{
				switch(m_Device)
				{
				case DEVICE_MOUSE:
					FireInput(1.0,CT_TRIGGER);
					break;
				}
			}
			return true;
		}
		bool MouseReleased(const MouseData &/*data*/,MouseButtonId id ) override
		{
			if(m_Button == id)
			{
				switch(m_Device)
				{
				case DEVICE_MOUSE:
					FireInput(0.0,CT_TRIGGER);
					break;
				}
			}
			return true;
		}

		bool ButtonPressed(int device, int button) override
		{
			if(m_Button == button && device == m_Device - DEVICE_GAME_CONTROLLER_0)
			{
				FireInput(1.0,CT_TRIGGER);
			}
			return true;	
		}

		bool ButtonReleased(int device,int button) override
		{
			if(m_Button == button && device == m_Device - DEVICE_GAME_CONTROLLER_0)
			{
				FireInput(0.0,CT_TRIGGER);
			}
			return true;	
		}

		bool AxisMoved(int ,int ,float ) override
		{
			return true;	
		}
		//Joystick Event, amd sliderID
		bool SliderMoved(int, int, float ) override
		{
			return true;	
		}
		//Joystick Event, amd povID
		bool PovMoved(int ,int , float ) override
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

		~KeyAxisController() override
		{
			m_Owner->GetInputSystem()->RemoveKeyListener(this);
		}

		bool KeyPressed(int key, unsigned int) override 
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
				FireInput(value,CT_AXIS);
				
			}
			return true;
		}

		bool KeyReleased( int key, unsigned int /*text*/) override
		{
			if(key == m_PosKey || key == m_NegKey)
			{

				FireInput(0.0,CT_AXIS);
				
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
		
		~AxisAxisController() override
		{
			m_Owner->GetInputSystem()->RemoveMouseListener(this);
			m_Owner->GetInputSystem()->RemoveGameControllerListener(this);
		}

		bool MouseMoved(const MouseData &data) override
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
				FireInput(value,CT_AXIS);
				
				break;
			}
			return true;

		}
		bool MousePressed(const MouseData&, MouseButtonId) override
		{
			return true;

		}
		bool MouseReleased(const MouseData&, MouseButtonId) override
		{
			return true;
		}

		bool ButtonPressed(int, int) override
		{
			return true;	
		}
		bool ButtonReleased(int, int) override
		{
			return true;	
		}
		bool AxisMoved(int device,int axis,float value) override
		{
			if(device == m_Device-DEVICE_GAME_CONTROLLER_0 && axis == m_Axis-INPUT_AXIS_0)
			{
				value = value*m_Invert;
				FireInput(value,CT_AXIS);
				
			}
			return true;
		}
		//Joystick Event, amd sliderID
		bool SliderMoved(int, int, float) override
		{
			return true;	
		}
		//Joystick Event, amd povID
		bool PovMoved(int, int, float) override
		{
			return true;	
		}

		int m_Axis;
		int m_Invert;
	};

	class ButtonAxisController : public AxisController
	{
	public:
		ButtonAxisController(const std::string &name,ControlSetting* owner) : AxisController(name,owner), m_PosKey(0), m_NegKey(0)
		{

		}
		int m_PosKey;
		int m_NegKey;
	};
}




