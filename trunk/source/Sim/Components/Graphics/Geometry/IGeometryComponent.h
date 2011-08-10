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
#include "Core/Math/AABox.h"
#include "Core/Math/Sphere.h"
#include "Sim/Components/Graphics/GeometryCategory.h"

namespace GASS
{
	/**
		Basic geometry interface that all geometry components (mesh, terrain, lines etc) 
		should be derived from.
		This can for instanace be used by a physics engine to get information
		about geomtry size. 
		Note that all runtime interaction should be done through 
		messages if running multi-threaded.
	*/

	class GASSExport IGeometryComponent 
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
			Should return the geometry category, for instance
			a terrain geometry should return GT_TERRAIN
		*/
		virtual GeometryCategory GetGeometryCategory() const = 0;
	protected:
	};
	typedef boost::shared_ptr<IGeometryComponent> GeometryComponentPtr;
}
