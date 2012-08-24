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

#include "Sim/GASSSimEngine.h"
#include "Sim/Scheduling/GASSTBBRuntimeController.h"

#include "Sim/Utils/GASSProfiler.h"
#include "Sim/Utils/GASSProfileRuntimeHandler.h"

#include "Core/PluginSystem/GASSPluginManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/System/GASSISystemManager.h"
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSLogManager.h"
//include to ensure interface export
#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Sim/Components/Graphics/GASSICameraComponent.h"
#include "Sim/Components/Graphics/GASSILightComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSITerrainComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSIMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSIShape.h"
#include "Sim/Components/Physics/GASSIPhysicsGeometryComponent.h"
#include "Sim/Components/Network/GASSINetworkComponent.h"
#include "Sim/Components/Common/GASSIWaypointListComponent.h"
#include "Sim/Systems/Collision/GASSICollisionSystem.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Systems/Input/GASSIInputSystem.h"
#include "Sim/Systems/Messages/GASSCoreSystemMessages.h"
#include "Sim/Scene/GASSSceneObject.h"


namespace GASS
{
	SimEngine::SimEngine(): m_CurrentTime(0)
	{
		//m_SimulationUpdateInterval = 1.0f/30.0f; //30Hz
		m_PluginManager = PluginManagerPtr(new PluginManager());
		m_SystemManager = SimSystemManagerPtr(new SimSystemManager());
		m_SceneObjectTemplateManager = BaseComponentContainerTemplateManagerPtr(new BaseComponentContainerTemplateManager());
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

	void SimEngine::Init(const std::string &plugin_file, const std::string &system_file, const std::string &control_settings, int num_rtc_threads)
	{
		// Create log manager
		if(LogManager::getSingletonPtr() == 0)
		{
			LogManager* log_man = new LogManager();
			log_man->createLog("GASS.log", true, true);
		}
	    LogManager::getSingleton().stream() << "SimEngine Initialization Started";
		m_PluginManager->LoadFromFile(plugin_file);
		
		m_SystemManager->Load(system_file);
		
		m_ControlSettingsManager->Load(control_settings);
		
		//Initialize systems
		m_SystemManager->Init();

		m_RTC->Init(num_rtc_threads);

		//intilize profiler
		ProfileSample::m_OutputHandler = new ProfileRuntimeHandler();
		ProfileSample::ResetAll();

		LogManager::getSingleton().stream() << "SimEngine Initialization Completed";
	}

	void SimEngine::Update(double delta_time)
	{
		//ProfileSample::ResetAll();
		{
		PROFILE("SimEngine::Update")
		//m_RTC->Update(delta_time);
		//update systems
		GetSimSystemManager()->Update(delta_time);
		m_CurrentTime += delta_time;
		}
#ifdef PROFILER
		ProfileSample::Output();
#endif
	}


	


	SceneWeakPtr SimEngine::LoadScene(const FilePath &path)
	{
		ScenePtr scene = ScenePtr(new Scene());
		scene->Create();
		scene->Load(path);
		m_Scenes.push_back(scene);
		return scene;
	}

	void SimEngine::UnloadScene(SceneWeakPtr scene)
	{

	}

	SceneObjectPtr SimEngine::CreateObjectFromTemplate(const std::string &template_name) const
	{
		ComponentContainerPtr cc  = m_SceneObjectTemplateManager->CreateFromTemplate(template_name);
		SceneObjectPtr so = boost::shared_static_cast<SceneObject>(cc);
		return so;
	}

	bool SimEngine::Shutdown()
	{
		return true;
	}

	SimEngine::SceneIterator SimEngine::GetScenes()
	{
		return SceneIterator(m_Scenes.begin(),m_Scenes.end());
	}

	SimEngine::ConstSceneIterator SimEngine::GetScenes() const
	{
		return ConstSceneIterator(m_Scenes.begin(),m_Scenes.end());
	}

}
