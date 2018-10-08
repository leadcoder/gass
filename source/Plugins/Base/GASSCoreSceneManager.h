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

#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneManager.h"


namespace GASS
{
	/**
		Scene manager that owns sim components.  This scene manager can 
		also be used by plugins that only provide some new sim components but dont want to create
		a new scene manager.
	*/
	class CoreSceneManager : public Reflection<CoreSceneManager, BaseSceneManager>
	{
	public:
		CoreSceneManager();
		virtual ~CoreSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void OnInit();
		virtual void OnShutdown();
		virtual void OnPostSystemUpdate(double delta_time);
		virtual void OnPreSystemUpdate(double delta_time);
		virtual bool GetSerialize() const {return false;}
	protected:
	};
	typedef GASS_SHARED_PTR<CoreSceneManager> CoreSceneManagerPtr; 
}
