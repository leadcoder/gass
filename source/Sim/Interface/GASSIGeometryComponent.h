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
#include "Sim/GASSGeometryFlags.h"
#include "Core/Math/GASSAABox.h"
#include "Core/Math/GASSSphere.h"


namespace GASS
{
	/**
		Basic geometry interface that all geometry components (mesh, terrain, lines etc) 
		should be derived from.
		This can for instance be used by a physics engine to get information
		about geometry size. 
		Note that interaction with this interface during RTC update is undefined 
		if running GASS in multi-threaded mode. Interaction with components should 
		instead be done through messages.
	*/

	class IGeometryComponent 
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(IGeometryComponent)
	public:
		/**
			Should return axis aligned bounding box in world coordinates
		*/
		virtual AABox GetBoundingBox()const = 0;

		/**
			Should return bounding sphere in world coordinates
		*/
		virtual Sphere GetBoundingSphere()const = 0;

		/**
			Should return the geometry flags
		*/
		virtual GeometryFlags GetGeometryFlags() const = 0;
		
		/**
			Set object geometry flags
		*/
		virtual void SetGeometryFlags(GeometryFlags flags) = 0;

		/**
			Enable/disable collision 
		*/
		virtual void SetCollision(bool value) = 0;

		/**
			Get collision status
		*/
		virtual bool GetCollision() const = 0;
	};
	typedef GASS_SHARED_PTR<IGeometryComponent> GeometryComponentPtr;
}
