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

#ifndef SIM_SCENE_MANAGER_H
#define SIM_SCENE_MANAGER_H

#include <map>
#include "Core/MessageSystem/Message.h"
#include "Sim/Scenario/Scene/BaseSceneManager.h"


namespace GASS
{
	/**
		Scene manager that owns all sim components.  This scene manager can 
		also be used by plugins that only provid some new sim components but dont want to create
		a brand new scene manager.
	*/
	class SimSceneManager  : public Reflection<SimSceneManager, BaseSceneManager>
	{
	public:
		SimSceneManager();
		virtual ~SimSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		void Update(double delta_time);
	protected:
		void OnLoad(MessagePtr message);
		void OnUnload(MessagePtr message);
		void OnLoadSceneObject(MessagePtr message);
	private:
		void SetPrimaryThread(bool value);
		bool GetPrimaryThread() const;

		bool m_Init;
		double m_SimulationUpdateInterval;
		double m_TimeToProcess;
		int m_MaxSimSteps;
		bool m_Paused;
		bool m_PrimaryThread;
		
	};
}
#endif