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
#include "Core/Math/GASSSphere.h"
#include "Core/Math/GASSQuaternion.h"
#include "Sim/GASSGeometryFlags.h"

namespace GASS
{
	class SceneObject;
	typedef GASS_WEAK_PTR<SceneObject> SceneObjectWeakPtr;
	
	struct CollisionResult
	{
		Vec3 CollNormal;
		Vec3 CollPosition;
		double CollDist;
		bool Coll;
		SceneObjectWeakPtr CollSceneObject;
	};

	
	/**
		Collision interface
		
		Implementaion of this interface must be thread safe so that 
		interaction is can be done during RTC update if running GASS 
		in multi-threaded mode. 
	*/

	class ICollisionSceneManager
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(ICollisionSceneManager)
	public:
	
	/**
		Raycast check
		@param ray_start
		@param ray_dir
		@param flags ???
		@param result
		@param return_first_hit
	*/
		virtual void Raycast(const Vec3 &ray_start, const Vec3 &ray_dir, GeometryFlags flags, CollisionResult &result, bool return_first_hit = false) const = 0;
		virtual bool GetTerrainHeight(const Vec3& location, double& height, GeometryFlags flags) const = 0;
		virtual bool GetHeightAboveTerrain(const Vec3& location, double& height, GeometryFlags flags) const = 0;
		virtual bool GetHeightAboveSeaLevel(const Vec3& location, double& height) const = 0;
		virtual bool GetUpVector(const Vec3& location, GASS::Vec3& up_vec) const = 0;
		virtual bool GetOrientation(const Vec3& location, Quaternion& rot) const = 0;
		virtual bool GetLocationOnTerrain(const Vec3& location, GeometryFlags flags, Vec3& terrain_location) const = 0;
	};

	typedef GASS_SHARED_PTR<ICollisionSceneManager> CollisionSceneManagerPtr;
}
