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

#ifndef ISYSTEM_HH
#define ISYSTEM_HH

#include "Core/Reflection/GASSBaseReflectionObject.h"
#include <string>

namespace GASS
{
	class ISystemManager;
	typedef boost::shared_ptr<ISystemManager> SystemManagerPtr;
	typedef std::string SystemType;
	
	class ISystemListener
	{

	public:
		virtual void SystemTick(double delta_time) = 0;
	};
	typedef boost::shared_ptr<ISystemListener> SystemListenerPtr;


	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup System
	*  @{
	*/


	/**
		Interface for arbitrary system. 
		Many applications can be divided in to different 
		systems each with a specific task. For instance, 
		a game engine usually has a graphic, sound, 
		physics and input system. 
		By deriving your system from this interface 
		you enable the system manager to load your system
		and make it accessible to the rest of your 
		application.

	*/

	class GASSCoreExport ISystem
	{
	public:
		virtual ~ISystem(){}
		virtual std::string GetSystemName() const = 0;
		virtual void OnCreate(SystemManagerPtr owner)= 0;
		virtual void Init() = 0;
		virtual void Register(SystemListenerPtr listener) = 0;
		virtual void Unregister(SystemListenerPtr listener)= 0;
	};
	typedef boost::shared_ptr<ISystem> SystemPtr;
	typedef boost::weak_ptr<ISystem> SystemWeakPtr;
}
#endif // #ifndef ISYSTEM_HH
