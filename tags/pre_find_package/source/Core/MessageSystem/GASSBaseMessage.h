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

#ifndef GASS_BASE_MESSAGE_H
#define GASS_BASE_MESSAGE_H

#include "Core/Common.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Message
	*  @{
	*/

	/**
	Message implementation that can be used to send messages without any data 
	besides the default data needed to support the IMessage interface.
	This class can also serve as a convenience class that custom message implementations
	can inherit from
	*/
	class GASSCoreExport BaseMessage : public IMessage
	{
	public:
		BaseMessage(SenderID sender_id = -1, double delay= 0);
		virtual ~BaseMessage();

		void SetDeliverDelay(double delay);
		double  GetDeliverDelay() const;

		SenderID GetSenderID() const;
		MessageType GetType() const;

	private:
		double m_Delay;
		SenderID m_SenderID;
	};
	typedef SPTR<BaseMessage> BaseMessagePtr;
}
#endif 
