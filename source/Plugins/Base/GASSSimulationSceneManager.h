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
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/Interface/GASSIMissionSceneManager.h"

namespace GASS
{
	/**
		Scene manager that owns all game components.
	*/
	class SimulationSceneManager : public Reflection<SimulationSceneManager, BaseSceneManager>, public IMissionSceneManager
	{
	public:
		SimulationSceneManager(SceneWeakPtr scene);
		static void RegisterReflection();
		void OnPostConstruction() override;
		void OnSceneCreated() override;
		void OnSceneShutdown() override;
		bool GetSerialize() const override {return false;}
	protected:
	};
	using SimulationSceneManagerPtr = std::shared_ptr<SimulationSceneManager>; 
}
