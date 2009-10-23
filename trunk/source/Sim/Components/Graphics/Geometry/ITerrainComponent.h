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
#include "Sim/Components/Graphics/MeshData.h"


namespace GASS
{
	class GASSExport ITerrainComponent
	{
	public:
		virtual ~ITerrainComponent(){}
		virtual Float GetHeight(Float x, Float z) = 0;
		virtual bool CheckOnTerrain(Float x, Float z)= 0;
		virtual Float GetSizeZ() = 0;
		virtual Float GetSizeX() = 0;
		virtual unsigned int GetSamplesX()=0;
		virtual unsigned int GetSamplesZ()=0;
		virtual void  GetBounds(Vec3 &min,Vec3 &max) = 0;
		virtual void GetHeightAndNormal(Float x, Float z, Float &height,Vec3 &normal) = 0;
	private:
	};

	typedef boost::shared_ptr<ITerrainComponent> TerrainComponentPtr;
}

