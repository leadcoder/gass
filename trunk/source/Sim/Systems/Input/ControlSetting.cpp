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

#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Systems/Input/IInputSystem.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/Controller.h"
#include "Core/MessageSystem/MessageManager.h"
namespace GASS
{

	ControlSetting::ControlSetting(IInputSystem* input)
	{
		m_Input = input;
		m_MM =  new MessageManager();
	}

	ControlSetting::~ControlSetting(void)
	{

	}

	void ControlSetting::Update(double delta_time)
	{
		m_MM->Update(delta_time);
	}

	float ControlSetting::Get(const std::string &input)
	{
	
		ControllerMap::iterator pos;

		pos = m_ControllerMap.find(input);

		if (pos != m_ControllerMap.end()) // not in map.
		{
			Controller* controller = m_ControllerMap[input];
			return controller->GetValue();
		}
		else
		{
			//if(m_Log) Log::Warning("Unknown object type: %s",name.c_str());
			return 0;
		}
	}

	void ControlSetting::AddController(Controller *controller, const std::string &name,int action)
	{
		m_ControllerMap[name] = controller;
		int index = m_ControllerMap.size();
		m_NameToIndex[name] = index;
		m_IndexToName[index] = name;
		controller->m_Owner = this;
	}


	Controller*  ControlSetting::GetController(const std::string &input)
	{
		ControllerMap::iterator pos;


		pos = m_ControllerMap.find(input);

		if (pos != m_ControllerMap.end()) // not in map.
		{
			Controller* ctrl = m_ControllerMap[input];
			return ctrl;
		}
		else
		{
			//if(m_Log) Log::Warning("Unknown object type: %s",name.c_str());
			return NULL;
		}
	}

	void ControlSetting::Set(const std::string &input,float value)
	{
		Controller* controller = GetController(input);
		if(controller) controller->SetValue(value);
	};

	void ControlSetting::TrySet(const std::string &input,float value)
	{
		Controller* controller = GetController(input);
		if(controller) controller->TryValue(value);
	};

	void ControlSetting::DisableOverride(const std::string &input)
	{
		Controller* controller = GetController(input);
		if(controller) 
		{
			controller->DisableOverride();
		}
	};
}
