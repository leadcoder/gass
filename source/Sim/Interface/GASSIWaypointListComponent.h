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

namespace GASS
{
	class IWaypointListComponent
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(IWaypointListComponent)
	public:
		virtual std::vector<Vec3> GetWaypoints(bool relative_position = true) const = 0;
		virtual float GetRadius() const = 0;
		virtual void SetRadius(float radius) = 0;
		virtual std::string GetWaypointTemplate() const = 0;
		virtual void SetWaypointTemplate(const std::string& name) = 0;
		virtual void SetClosed(bool value) = 0;
		virtual bool GetClosed() const = 0;
		virtual void SetAutoRotateWaypoints(bool value) = 0;
		virtual bool GetAutoRotateWaypoints() const = 0;
		virtual void SetShowPathLine(bool value) = 0;
		virtual bool GetShowPathLine() const = 0;
		virtual int GetSplineSteps() const = 0;
		virtual void SetSplineSteps(int steps) = 0;
		virtual bool GetEnableSpline()const = 0;
		virtual void SetEnableSpline(bool value) = 0;
	};
	using WaypointListComponentPtr = std::shared_ptr<IWaypointListComponent>;
}
