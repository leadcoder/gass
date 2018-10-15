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

#ifndef PHYSICS_SCENE_MESSAGES_H
#define PHYSICS_SCENE_MESSAGES_H

#include "Sim/GASSCommon.h"

namespace GASS
{

	//*********************************************************
	// ALL MESSAGES IN THIS SECTION CAN BE POSTED BY USER
	//*********************************************************

	/**
		Message sent by physics scene manager after physics update
	*/
	class PostPhysicsSceneUpdateEvent : public SceneEventMessage
	{
	public:
		PostPhysicsSceneUpdateEvent(double delta_time, SenderID sender_id = -1, double delay= 0) :
		  SceneEventMessage(sender_id , delay), m_DeltaTime(delta_time) {}
		  double GetDeltaTime() const {return m_DeltaTime;}
	private:
		double m_DeltaTime;
	};
	typedef GASS_SHARED_PTR<PostPhysicsSceneUpdateEvent> PostPhysicsSceneUpdateEventPtr;
}

#endif