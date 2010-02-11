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
#include "Core/System/BaseSystem.h"
#include <string>

namespace GASS
{
	class SimSystemManager;
	typedef boost::shared_ptr<SimSystemManager> SimSystemManagerPtr;
	class GASSExport SimSystem : public BaseSystem
	{
	public:
		SimSystem();
		virtual ~SimSystem();
		static void RegisterReflection();
		/**
		Convenience function to get SimSystemManager without casting from ISystemManager
		*/
		SimSystemManagerPtr GetSimSystemManager() const;
	protected:
	};
}
