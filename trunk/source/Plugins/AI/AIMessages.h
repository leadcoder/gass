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

#ifndef AI_MESSAGES_H
#define AI_MESSAGES_H
#include "Sim/GASS.h"

namespace GASS
{
	class DoorMessage : public BaseMessage
	{
	public:
		DoorMessage(bool open, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay) , m_Open(open)
		  {

		  }
		  bool GetOpen() const {return m_Open;}
	private:
		bool m_Open;
	};
	typedef SPTR<DoorMessage> DoorMessagePtr;


	class HealthChangedMessage : public BaseMessage
	{
	public:
		HealthChangedMessage(double value, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay) , m_Health(value)
		  {

		  }
		  double GetHealth() const {return m_Health;}
	private:
		double m_Health;
	};
	typedef SPTR<HealthChangedMessage> HealthChangedMessagePtr;

	class StanceChangedMessage : public BaseMessage
	{
	public:
		StanceChangedMessage(int stance, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay) , m_Stance(stance)
		  {

		  }
		  int  GetStance() const {return m_Stance;}
	private:
		int m_Stance;
	};
	typedef SPTR<StanceChangedMessage> StanceChangedMessagePtr;
}
#endif
