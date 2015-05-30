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

#pragma once

#include "Sim/GASSCommon.h"
#include "Sim/GASSSimSystem.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"

#include <string>

namespace GASS
{
	class SimSystemManager;

	class SimulationSystem : public Reflection<SimulationSystem, SimSystem>
	{
	public:
		SimulationSystem();
		virtual ~SimulationSystem();
		static void RegisterReflection();
		virtual std::string GetSystemName() const {return "SimulationSystem";}
		virtual void Init();
		//virtual void Update(double delta, TaskNode2* caller);
	protected:
		//void OnScenarioEvent(SimEventPtr message);
		//bool m_Update;
	};
	typedef SPTR<SimulationSystem> SimulationSystemPtr;
}
