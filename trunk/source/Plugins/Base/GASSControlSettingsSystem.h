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
/*# pragma warning (disable : 4530)
# pragma warning (disable : 4786)
# pragma warning (disable : 4244)
# pragma warning (disable : 4305)
# pragma warning (disable : 4251)
# pragma warning (disable : 4275)
# pragma warning( disable : 4290 )
# pragma warning( disable: 4661)
*/

#include "Sim/GASSCommon.h"
#include "Sim/Systems/GASSSimSystem.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Systems/Input/GASSIControlSettingsSystem.h"

#include <map>
#include <string>

namespace GASS
{
	class ControlSetting;
	class EnumLookup;
	struct IntConstant;

	class ControlSettingsSystem : public Reflection<ControlSettingsSystem, SimSystem>, public IControlSettingsSystem
	{
	public:
		ControlSettingsSystem();
		virtual ~ControlSettingsSystem();
		virtual void Load(const std::string &filename);
		//virtual void Update(double delta_time);
		virtual std::string GetSystemName() const {return "ControlSettingsSystem";}
		virtual std::string GetNameFromIndex(const std::string &settings, int index);
		virtual int GetIndexFromName(const std::string &settings, const std::string &name);
		virtual void Init();
		static void RegisterReflection();
	private:
		ControlSetting* GetControlSetting(const std::string &name) const;
		ControlSetting* NewRemoteControlSetting(const std::string &name);
		int GetDevice(std::string device) const;
		void Add(const std::string &name,ControlSetting* cs);
		typedef std::map<std::string,ControlSetting*> ControlSettingMap;
		ControlSettingMap m_ControlSettingMap;
		EnumLookup* m_InputStringTable;
	};
}
