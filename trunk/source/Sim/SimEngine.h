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
#include "Core/Utils/Singleton.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include <boost/shared_ptr.hpp>

namespace GASS
{
	class PluginManager;
	class SimSystemManager;
	class BaseObjectTemplateManager;
	class IRuntimeController;
	
	typedef boost::shared_ptr<PluginManager> PluginManagerPtr;
	typedef boost::shared_ptr<SimSystemManager> SystemManagerPtr;
	typedef boost::shared_ptr<BaseObjectTemplateManager> BaseObjectTemplateManagerPtr;
	typedef boost::shared_ptr<ControlSettingsManager> ControlSettingsManagerPtr;
	typedef boost::shared_ptr<IRuntimeController> RuntimeControllerPtr;

	/**\
	* \brief Base class for Sim.
	*/

	class GASSExport SimEngine  : public Singleton<SimEngine>
	{
	public:
		SimEngine();
		virtual ~SimEngine();
		static SimEngine* GetPtr();
		static SimEngine& Get();

		/**
		

		*/
		bool Init(const std::string &plugin_file = std::string("plugins.xml"), 
				  const std::string &system_file = std::string("systems.xml"));

		/**
		Main update for GASS.
		@remarks
			The application is responsible for calling this function each frame
			Then function will then update GASS at a speed set by m_SimulationUpdateInterval
			If the framerate is below m_SimulationUpdateInterval only one simulation step i executed
			otherwise time slicing is used.
			This function also prepare the scene for smooth rendering. This is achived by
			interpolation between the last two simulation steps, this leads to a small lag in rendering time
			This will of cause change if we change simulataion update interval
		@param delta is the frame time.
		*/
		void Update(float delta_time);
		
		bool Shutdown();

		SystemManagerPtr GetSystemManager() {return m_SystemManager;}
		BaseObjectTemplateManagerPtr GetSimObjectManager() {return m_SimObjectManager;}
		ControlSettingsManagerPtr GetControlSettingsManager() {return m_ControlSettingsManager;}
		RuntimeControllerPtr GetRuntimeController(){return m_RTC;}
	private:
		PluginManagerPtr m_PluginManager;
		SystemManagerPtr m_SystemManager;
		BaseObjectTemplateManagerPtr m_SimObjectManager;
		ControlSettingsManagerPtr m_ControlSettingsManager;
		RuntimeControllerPtr m_RTC;
	};
}

