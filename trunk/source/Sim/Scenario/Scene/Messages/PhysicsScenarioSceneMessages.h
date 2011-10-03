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

#ifndef PHYSICS_SCENARIO_SCENE_MESSAGES_H
#define PHYSICS_SCENARIO_SCENE_MESSAGES_H

#include "Sim/Common.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Quaternion.h"
#include "Core/MessageSystem/BaseMessage.h"

namespace GASS
{
	/**
		Message used to activate/deactive physics
	*/
	class ActivatePhysicsMessage : public BaseMessage
	{
	public:
		/**
		Constructor
		@param activate Indicate mode (0 == inactive, 1==active)
		*/
		ActivatePhysicsMessage(int activate, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_Activate(activate){}
		  int Activate() const {return m_Activate;}
	private:
		int m_Activate;
	};
	typedef boost::shared_ptr<ActivatePhysicsMessage> ActivatePhysicsMessagePtr;
}

#endif