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

#ifndef AI_SCENE_MANAGER_H
#define AI_SCENE_MANAGER_H

#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Sim/GASSBaseSceneManager.h"

namespace GASS
{
	class AISceneManager : public Reflection<AISceneManager, BaseSceneManager>
	{
	public:
		AISceneManager();
		virtual ~AISceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void SystemTick(double delta_time); 
		virtual void OnInit();
		virtual void OnShutdown();
		virtual bool GetSerialize() const {return false;}
	protected:
		ADD_PROPERTY(bool,Paused)
	};
	typedef SPTR<AISceneManager> AISceneManagerPtr; 

}
#endif