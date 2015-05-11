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
#include "Sim/GASSGeometryFlags.h"
#include "Core/Math/GASSAABox.h"
#include "Core/Math/GASSSphere.h"


namespace GASS
{
	/**
		Basic geometry interface that all geometry components (mesh, terrain, lines etc) 
		should be derived from.
		This can for instanace be used by a physics engine to get information
		about geomtry size. 
		Note that interaction with this interface during RTC update is undefined 
		if running GASS in multi-threaded mode. Interaction with components should 
		instead be done through messages.
	*/

	class IGeometryComponent 
	{
	public:
		virtual ~IGeometryComponent(){}

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
	protected:
	};
	typedef SPTR<IGeometryComponent> GeometryComponentPtr;
}
