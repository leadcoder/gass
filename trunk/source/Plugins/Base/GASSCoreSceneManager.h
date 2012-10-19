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

#ifndef CORE_SCENE_MANAGER_H
#define CORE_SCENE_MANAGER_H

#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/Scheduling/GASSTaskGroups.h"

namespace GASS
{


	/**
		Scene manager that owns all sim components.  This scene manager can 
		also be used by plugins that only provid some new sim components but dont want to create
		a brand new scene manager.
	*/
	class CoreSceneManager : public Reflection<CoreSceneManager, BaseSceneManager>
	{
	public:
		CoreSceneManager();
		virtual ~CoreSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
	protected:
		void OnLoad(MessagePtr message);
		void OnLoadSceneObject(MessagePtr message);
	private:
		bool m_Init;
	};
	typedef boost::shared_ptr<CoreSceneManager> CoreSceneManagerPtr; 
	
	
}
#endif