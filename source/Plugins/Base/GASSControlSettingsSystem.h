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
#include "Sim/GASSSimSystem.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"

namespace GASS
{
	class ControlSetting;
	class EnumLookup;
	struct IntConstant;

	class ControlSettingsSystem : public Reflection<ControlSettingsSystem, SimSystem>, public IControlSettingsSystem
	{
	public:
		static void RegisterReflection();

		ControlSettingsSystem(SimSystemManagerWeakPtr manager);
		~ControlSettingsSystem() override;
		void OnSystemInit() override;
		void OnSystemShutdown() override;
		void Load(const std::string &filename) override;
		std::string GetSystemName() const override {return "ControlSettingsSystem";}
		std::string GetNameFromIndex(const std::string &settings, int index) override;
		int GetIndexFromName(const std::string &settings, const std::string &name) override;
	private:
		void Free();
		ControlSetting* GetControlSetting(const std::string &name) const;
		ControlSetting* NewRemoteControlSetting(const std::string &name);
		void Add(const std::string &name,ControlSetting* cs);
		typedef std::map<std::string,ControlSetting*> ControlSettingMap;
		ControlSettingMap m_ControlSettingMap;
		EnumLookup* m_InputStringTable;
	};
}
