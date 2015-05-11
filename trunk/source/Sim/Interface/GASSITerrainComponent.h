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
#include "Sim/GASSGraphicsMesh.h"
#include "Core/Math//GASSAABox.h"


namespace GASS
{
	/**
		Interface that all terrain components should be derived from.
		This can for instanace be used by a physics engine to get information
		how to create a collision heightfield or by other components that 
		new terrain height information in a fast way.

		Note that interaction with this interface during RTC update is undefined 
		if running GASS in multi-threaded mode. Interaction with components should 
		instead be done through messages.
	*/
	class IHeightmapTerrainComponent
	{
	public:
		virtual ~IHeightmapTerrainComponent(){}

		
		/**
		Get terrain height at sample point x,z
		*/
		virtual Float GetHeightAtSample(int x, int z) const = 0;

		/**
		Get terrain height at world position x,z (assume y up direction)
		*/
		virtual Float GetHeightAtWorldLocation(Float x, Float z) const = 0;
		//virtual void GetHeightAndNormal(Float x, Float z, Float &height,Vec3 &normal) const = 0;

		/**
		Get terrain width samples
		*/
		virtual unsigned int GetNumSamplesW() const =0;

		/**
		Get terrain height samples
		*/
		virtual unsigned int GetNumSamplesH() const =0;

		/**
		Get pointer to terrain data height field, if not available return NULL
		*/
		//virtual float* GetHeightData() const = 0;


		/**
		Get terrain bounds in world coordinates
		*/
		virtual AABox GetBoundingBox() const = 0;

	private:
	};
	typedef SPTR<IHeightmapTerrainComponent> HeightmapTerrainComponentPtr;
}

