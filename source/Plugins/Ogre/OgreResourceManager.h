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

#include "Sim/GASSResourceManager.h"
#include "Sim/GASSSimSystem.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Core/Utils/GASSFilePath.h"
#include "Core/MessageSystem/GASSMessageType.h"

namespace GASS
{
	class OgreResourceManager
	{
	public:
		OgreResourceManager();
		virtual ~OgreResourceManager();
		static void RegisterReflection();
		void Init();
		void RemoveResourceGroup(const std::string &resource_group);
		void AddResourceGroup(ResourceGroupPtr group, bool load);

		void AddResourceLocation(ResourceLocationPtr location);
		void RemoveResourceLocation(ResourceLocationPtr location);
	protected:
		bool m_Initialized;
	};
}
