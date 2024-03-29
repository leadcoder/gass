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
#include <ode/ode.h>
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/GASSSimSystem.h"
#include "Sim/Interface/GASSIPhysicsSystem.h"

namespace GASS
{
	
	class ODEPhysicsSystem  : public Reflection<ODEPhysicsSystem, SimSystem>, public IPhysicsSystem
	{

	public:
		static void RegisterReflection();
		ODEPhysicsSystem(SimSystemManagerWeakPtr manager);
		~ODEPhysicsSystem() override;
		void OnSystemInit() override;
		void OnSystemShutdown() override;
		std::string GetSystemName() const override {return "ODEPhysicsSystem";}
	private:
		bool m_Init;
	};

	using ODEPhysicsSystemPtr = std::shared_ptr<ODEPhysicsSystem>;
}

