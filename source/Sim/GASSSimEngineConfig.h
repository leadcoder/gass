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
#include "Sim/GASSResourceManager.h"
#include "Sim/GASSSimSystemManagerConfig.h"
#include "Core/Utils/GASSFilePath.h"

namespace GASS
{
	enum class InputOptions
	{
		OSG,
		OIS
	};

	enum class PhysicsOptions
	{
		PHYSX,
		ODE,
		NONE
	};

	enum class SoundOptions
	{
		OPENAL,
		NONE
	};

	enum class NetworkOptions
	{
		RAKNET,
		NONE
	};

	class GASSExport SimEngineConfig
	{
	public:
		static SimEngineConfig LoadFromfile(const FilePath& filename);
		static SimEngineConfig Create(PhysicsOptions physics= PhysicsOptions::NONE,
			SoundOptions sound = SoundOptions::NONE,
			NetworkOptions network = NetworkOptions::NONE,
			InputOptions input = InputOptions::OSG);
		std::string DataPath = "%GASS_DATA_HOME%";
		std::string LogFolder;
		std::vector<std::string> Plugins;
		bool AddObjectIDToName = true;
		std::string ObjectIDPrefix{ "_" };
		std::string ObjectIDSufix;
		ResourceManagerConfig ResourceConfig;
		SimSystemManagerConfig SimSystemManager;
	};
}

