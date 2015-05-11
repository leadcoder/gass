/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/MessageSystem/GASSBaseMessage.h"

namespace GASS
{

	//*********************************************************
	// ALL MESSAGES IN THIS SECTION CAN BE POSTED BY USER
	//*********************************************************

	/**
		Message used to activate/deactivate physics
	*/
	class ActivatePhysicsRequest : public SceneRequestMessage
	{
	public:
		/**
		Constructor
		@param activate Indicate mode (0 == inactive, 1==active)
		*/
		ActivatePhysicsRequest(int activate, SenderID sender_id = -1, double delay= 0) :
		  SceneRequestMessage(sender_id , delay) ,
			  m_Activate(activate){}
		  int GetActivate() const {return m_Activate;}
	private:
		int m_Activate;
	};
	typedef SPTR<ActivatePhysicsRequest> ActivatePhysicsRequestPtr;

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
	typedef SPTR<PostPhysicsSceneUpdateEvent> PostPhysicsSceneUpdateEventPtr;
}

#endif