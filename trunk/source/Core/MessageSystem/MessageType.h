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

#ifndef MESSAGETYPE_HH
#define MESSAGETYPE_HH


#include <list>
#include "Core/MessageSystem/IMessage.h"

namespace GASS
{
	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Message
	*  @{
	*/

	/**
		Class holding the message suscriber registration
	*/
	class MessageReg
	{
	public:
		MessageReg(){}
		virtual ~MessageReg(){}
		MessageFuncPtr m_Callback;
		int m_Priority;
	};
	typedef boost::shared_ptr<MessageReg> MessageRegPtr;
	typedef std::list<MessageRegPtr> MessageRegList;
	
	
	/**
		Class holding all suscribers of a certain message type
	*/
	class MessageTypeListeners
	{
	public:
		MessageTypeListeners(){}
		virtual ~MessageTypeListeners()
		{
		}

		MessageType m_TypeID;
		MessageRegList m_MessageRegistrations;
	};
	typedef boost::shared_ptr<MessageTypeListeners> MessageTypeListenersPtr;

}
#endif 
