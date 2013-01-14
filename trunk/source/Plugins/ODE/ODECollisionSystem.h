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


#include <ode/ode.h>
#include <map>
#include "Sim/Interface/GASSICollisionSystem.h"
#include "Sim/Interface/GASSICollisionSceneManager.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/GASSSimSystem.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Core/System/GASSBaseSystem.h"
#include "tbb/spin_mutex.h"

namespace GASS
{
	class ODECollisionSystem : public Reflection<ODECollisionSystem , SimSystem> 
	{
	public:
		ODECollisionSystem();
		virtual ~ODECollisionSystem();
		static void RegisterReflection();
		virtual void Init();
		//virtual void Update(double delta_time);
		virtual std::string GetSystemName() const {return "ODECollisionSystem";}
	private:
	};
	typedef boost::shared_ptr<ODECollisionSystem> ODECollisionSystemPtr;
}

