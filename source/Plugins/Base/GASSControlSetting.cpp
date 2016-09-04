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

#include "GASSControlSetting.h"
#include "GASSControlSettingsSystem.h"
#include "GASSController.h"
#include "Sim/Interface/GASSIInputSystem.h"

namespace GASS
{

	ControlSetting::ControlSetting(const std::string &name, ControlSettingsSystem* owner, IInputSystem* input) : m_Name(name),
		m_Owner(owner),
		m_Input(input)
	{
		
	}

	ControlSetting::~ControlSetting(void)
	{
		ControllerMap::const_iterator iter = m_ControllerMap.begin();
		while(iter != m_ControllerMap.end())
		{
			delete iter->second;
			iter++;
		}
		m_ControllerMap.clear();
	}

	void ControlSetting::AddController(Controller *controller, const std::string &name,int)
	{
		m_ControllerMap[name] = controller;
		const int index = static_cast<int>(m_ControllerMap.size());
		m_NameToIndex[name] = index;
		m_IndexToName[index] = name;
		controller->m_Owner = this;
	}

	Controller*  ControlSetting::GetController(const std::string &input) const
	{
		ControllerMap::const_iterator pos;
		pos = m_ControllerMap.find(input);

		if (pos != m_ControllerMap.end()) // not in map.
		{
			Controller* ctrl = (*pos).second;
			return ctrl;
		}
		else
		{
			return NULL;
		}
	}
}
