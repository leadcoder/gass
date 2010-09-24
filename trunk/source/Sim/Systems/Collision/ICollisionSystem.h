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

#include "Sim/Common.h"
#include "Core/System/ISystem.h"
#include "Core/Math/Sphere.h"
#include <string>

namespace GASS
{
	class ScenarioScene;
	class SceneObject;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;
	typedef boost::weak_ptr<ScenarioScene> ScenarioSceneWeakPtr;

	enum CollisionType
	{
		COL_SPHERE,
		COL_LINE
	};

	struct CollisionRequest
	{
		CollisionType Type;
		Sphere ColSphere;
		Vec3 LineStart;
		Vec3 LineEnd;
		ScenarioSceneWeakPtr Scene;
		bool ReturnFirstCollisionPoint;
		long int CollisionBits;
	};
	
	struct CollisionResult
	{
		Vec3 CollNormal;
		Vec3 CollPosition;
		double CollDist;
		bool Coll;
		SceneObjectWeakPtr CollSceneObject;
	};

	typedef unsigned int CollisionHandle;

	/**
		Collision interface
	*/

	class GASSExport ICollisionSystem
	{
	public:
		virtual ~ICollisionSystem(){}
		/**
			Request a new collision query, the handle returned is used when to check
			if the request is processed.
		*/
		virtual CollisionHandle Request(const CollisionRequest &request)= 0;
		
		/**
			Check is a collision request is processed. The function return true is the 
			request is processed and the result is placed in the CollisionResult argument.
		*/
		virtual bool Check(CollisionHandle handle, CollisionResult &result)= 0;

		/**
			This function will force a the collision system to process 
			the collision request and the collision result will be
			available immediately. 
			@remarks Calling this method can stall the caller if the collision system
			implementation is threaded and therefore is busy processing other requests
		*/
		virtual void Force(CollisionRequest &request, CollisionResult &result)= 0;
	protected:
	};

	typedef boost::shared_ptr<ICollisionSystem> CollisionSystemPtr;
}
