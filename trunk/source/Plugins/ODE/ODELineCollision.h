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

#ifndef ODE_LINE_COLLISION
#define ODE_LINE_COLLISION

#include <ode/ode.h>
#include "Sim/Systems/Collision/ICollisionSystem.h"

namespace GASS
{
	class ODEPhysicsSceneManager;
	class ODELineCollision
	{
	public:
		ODELineCollision(CollisionRequest *request,CollisionResult *result, ODEPhysicsSceneManagerPtr ode_scene);
		virtual ~ODELineCollision();
		void Process();
	private:
		static void Callback(void *data, dGeomID o1, dGeomID o2);
		void ProcessCallback(dGeomID o1, dGeomID o2);
		dGeomID m_RayGeom;
		Vec3 m_RayDir;
		Vec3 m_RayStart;
		double m_RayLength;
		CollisionRequest* m_Request;
		CollisionResult* m_Result;
		ODEPhysicsSceneManagerWeakPtr m_SceneManager;
	};
	
}

#endif