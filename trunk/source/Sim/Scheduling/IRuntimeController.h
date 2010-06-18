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

#ifndef I_RUNTIME_CONTROLLER_H
#define I_RUNTIME_CONTROLLER_H

#include "Sim/Common.h"
#include "Sim/Scheduling/ITaskListener.h"
#include <vector>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/bind.hpp>


namespace GASS
{
	class ITaskListener;
	typedef std::vector<ITaskListener*> TaskListenerVector;

	/**
		Interface for runtime controlling. A runtime controller is responsible 
		for tick all its registered clients. When a client want update calls from
		the sim system it register a update function through the Register function. 
		When the client is removed or disabled it's responsible to call the
		Unregister function before it's destroyed. This is important because the 
		RTC don’t have any information about the client except it's callback function.
		An natural way to solve this is that each client that want to get ticks from
		the RTC have to inherit from a RTC-client-interface and then the RTC could save
		a weak pointer to each client and automatically unregister clients when they 
		die. However current implementation will not force client to inherit from jet 
		another interface but this maybe change in the future.

		Further the client also have to provide a TaskGroup when during registration.
		Clients that belong to te same task group is grantid to get sequenze updates 
		and 
	*/

	class GASSExport IRuntimeController 
	{
	public:
		virtual ~IRuntimeController(){}
		/**
			Initilize the RTC
		*/
		virtual void Init() = 0;
		/**
			Update the RTC
			@param delta_time Time since last update
		*/
		virtual void Update(double delta_time) = 0;
		
		/**
			Register new update callback function
			@param callback Function to receive update calls, ie. MyClass::MyUpdate(double delta_time)
			@param group The task group this update callback should be executed from
			@remarks If you want to forced the update callback to be executed 
			from the main application thread you should use the "MAIN" task group
		*/
		virtual void Register(ITaskListener* listener) = 0;
		/**
			Unregister update callback function
			@param callback It's necessary to provide the function again because that’s
			the only identifier for the RTC.
		*/
		virtual void Unregister(ITaskListener* listener) = 0;

		/**
			Log RTC state
		*/
		virtual void Log() = 0;
	private:
	};
}
#endif
