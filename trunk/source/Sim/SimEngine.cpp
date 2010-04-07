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

#include "Sim/SimEngine.h"
#include "Sim/Scheduling/TBBRuntimeController.h"
#include "Core/PluginSystem/PluginManager.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/System/ISystemManager.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"
#include "Core/Utils/Log.h"

//include to ensure interface export
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Components/Graphics/ICameraComponent.h"
#include "Sim/Components/Graphics/ILightComponent.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/ILineComponent.h"
#include "Sim/Systems/Collision/ICollisionSystem.h"
#include "Sim/Systems/SimSystemManager.h"

namespace GASS
{
	SimEngine::SimEngine()
	{
		//m_SimulationUpdateInterval = 1.0f/30.0f; //30Hz
		m_PluginManager = PluginManagerPtr(new PluginManager());
		m_SystemManager = SimSystemManagerPtr(new SimSystemManager());
		m_SimObjectManager = BaseComponentContainerTemplateManagerPtr(new BaseComponentContainerTemplateManager());
		m_ControlSettingsManager = ControlSettingsManagerPtr(new ControlSettingsManager());
		m_RTC = RuntimeControllerPtr(new TBBRuntimeController());
	}

	SimEngine::~SimEngine()
	{	

	}

	template<> SimEngine* Singleton<SimEngine>::m_Instance = 0;
	SimEngine* SimEngine::GetPtr(void)
	{
		assert(m_Instance);
		return m_Instance;
	}
	SimEngine& SimEngine::Get(void)
	{
		assert(m_Instance);
		return *m_Instance;
	}

	bool SimEngine::Init(const std::string &plugin_file, const std::string &system_file, const std::string &control_settings)
	{
	    Log::Print("SimEngine::Init -- Start");
		m_PluginManager->LoadFromFile(plugin_file);
		m_SystemManager->Load(system_file);
		m_ControlSettingsManager->Load(control_settings);
		//Initialize systems
		m_SystemManager->Init();

		m_RTC->Init();
		//Load some base game objects
		m_SimObjectManager->Load("scene_objects.xml");
		Log::Print("SimEngine::Init -- Done");
		return true;
	}

	void SimEngine::Update(double delta_time)
	{
		m_RTC->Update(delta_time);

		m_CurrentTime += delta_time;


		//TODO: this should not be done here
	
		m_ControlSettingsManager->Update(delta_time);

	}

	bool SimEngine::Shutdown()
	{
		return true;
	}

}
