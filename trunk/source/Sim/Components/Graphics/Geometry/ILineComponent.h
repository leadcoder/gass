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
#include "Core/Math/Vector.h"

namespace GASS
{
	struct LineData
	{
		Vec3 pos;
		Vec3 color;
	};
	class GASSExport ILineComponent
	{
	public:
		virtual ~ILineComponent(){}
		virtual void Add(const std::vector<LineData> &line_seg) = 0;
		virtual void Clear() = 0;
		
	};

	typedef boost::shared_ptr<ILineComponent> LineComponentPtr;
	typedef boost::weak_ptr<ILineComponent> LineComponentWeakPtr;
}

