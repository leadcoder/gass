/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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
#include "Sim/Systems/Collision/ICollisionSystem.h"
#include "Core/system/BaseSystem.h"

namespace GASS
{
	class ODECollisionSystem : public Reflection<ODECollisionSystem , BaseSystem> , public ICollisionSystem
	{
	public:
		typedef std::map<CollisionHandle,CollisionRequest> RequestMap;
		typedef std::map<CollisionHandle,CollisionResult> ResultMap;
	public:
		ODECollisionSystem();
		virtual ~ODECollisionSystem();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual SystemType GetSystemType() {return "CollisionSystem";}
		virtual CollisionHandle Request(const CollisionRequest &request);
		virtual bool Check(CollisionHandle handle, CollisionResult &result);
	private:
		void Process();
		RequestMap m_RequestMap;
		ResultMap m_ResultMap;
		unsigned int m_HandleCount;
	};

	
}

