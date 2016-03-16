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
#include "Sim/Interface/GASSIRenderWindow.h"
#include <string>

namespace GASS
{
	/**
		Interface projection scene managers should be derived from.
		A projection scene manager support transformations from local scene space to 
		different coordinate systems
	*/
	class IProjectionSceneManager
	{
	public:
		virtual ~IProjectionSceneManager(){}
		virtual void WGS84ToScene(double lat,double lon, double &x,double &y) = 0;
		virtual void SceneToWGS84(double x,double y, double &lat,double &lon) = 0;
		virtual std::string GetProjection() const = 0;
		virtual void SetProjection(const std::string &projection) = 0;
	protected:
	};
	typedef GASS_SHARED_PTR<IProjectionSceneManager> ProjectionSceneManagerPtr;
}
