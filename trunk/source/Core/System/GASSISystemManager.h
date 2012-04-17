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

#ifndef GASS_I_SYSTEMMANAGER_H
#define GASS_I_SYSTEMMANAGER_H

#include "Core/Common.h"
#include "Core/System/GASSISystem.h"

namespace GASS
{

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup System
	*  @{
	*/

	/**
		Interface for a system manager, a system manager 
		is the owner of all systems and responsible for 
		intialize and updating it's system 
	*/
	class GASSCoreExport ISystemManager
	{
	public:
		virtual ~ISystemManager(){}
		virtual void Init() = 0;
		virtual void Update(float delta_time) = 0;
	private:
	};
	typedef boost::shared_ptr<ISystemManager> SystemManagerPtr;
	typedef boost::weak_ptr<ISystemManager> SystemManagerWeakPtr;
}
#endif // #ifndef ISYSTEMMANAGER_HH
