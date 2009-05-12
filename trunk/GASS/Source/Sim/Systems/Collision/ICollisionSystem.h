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

#include "Sim/Common.h"
#include "Core/System/ISystem.h"
#include "Core/Math/Sphere.h"
#include <string>

namespace GASS
{
	class ScenarioScene;
	class SceneObject;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

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
		ScenarioScene* Scene;
		bool ReturnFirstCollisionPoint;
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

	class GASSExport ICollisionSystem
	{
	public:
		virtual ~ICollisionSystem(){}
		virtual CollisionHandle Request(const CollisionRequest &request)= 0;
		virtual bool Check(CollisionHandle handle, CollisionResult &result)= 0;
	protected:
	};

	typedef boost::shared_ptr<ICollisionSystem> CollisionSystemPtr;
}
