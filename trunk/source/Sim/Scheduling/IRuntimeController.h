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
#include <vector>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/bind.hpp>


namespace GASS
{
	typedef boost::function<void(double delta_time)> UpdateFunc;
	#define	UPDATE_FUNC(X) boost::bind(&X, this, _1 )

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
			@param force_primary_thread Should this callback be forced to executed from the main application thread. 

		*/
		virtual void Register(UpdateFunc callback, bool force_primary_thread = true) = 0;
		/**
			Unregister update callback function
			@param callback It's necessary to provide the function again because that’s
			the only identifier for the RTC.
		*/
		virtual void Unregister(UpdateFunc callback) = 0;
	private:
	};
}

