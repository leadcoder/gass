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

#include "Sim/GASSCommon.h"

namespace GASS
{

	/**
		Network object interface that all network components should be derived from
		
		Note that interaction with this interface during RTC update is undefined 
		if running GASS in multi-threaded mode. Interaction with components should 
		instead be done through messages.
	*/
	
	class ISoundComponent : public Reflection<ISoundComponent, Component>
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(ISoundComponent)
	public:
		virtual bool GetPlay() const = 0;
		virtual void SetPlay(bool value) = 0;
		virtual void Stop() = 0;
		virtual float GetVolume() const = 0;
		virtual void SetVolume(float volume) = 0;
		virtual bool GetLoop() const = 0;
		virtual void SetLoop(bool loop) = 0;
		virtual void SetPitch(float pitch) = 0;
		virtual float GetPitch() const = 0;
		virtual float GetMinDistance() const = 0;
		virtual void SetMinDistance(float min_dist) = 0;
		virtual float GetMaxDistance() const = 0;
		virtual void SetMaxDistance(float max_dist) = 0;
		virtual void SetRolloff(float rolloff) = 0;
		virtual float GetRolloff() const = 0;
	};

	typedef GASS_WEAK_PTR<ISoundComponent> SoundComponentWeakPtr;
	typedef GASS_SHARED_PTR<ISoundComponent> SoundComponentPtr;
}
