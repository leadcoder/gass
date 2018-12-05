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

#ifndef GASS_MESSAGETYPE_H
#define GASS_MESSAGETYPE_H


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
		Class holding the message subscriber registration
	*/
	class MessageReg
	{
	public:
		MessageFuncPtr m_Callback;
		int m_Priority = 0;
	};
	typedef GASS_SHARED_PTR<MessageReg> MessageRegPtr;
	typedef std::list<MessageRegPtr> MessageRegList;
	
	
	/**
		Class holding all suscribers of a certain message type
	*/
	class MessageTypeListeners
	{
	public:
		MessageType m_TypeID;
		MessageRegList m_MessageRegistrations;
	};
	typedef GASS_SHARED_PTR<MessageTypeListeners> MessageTypeListenerPtr;

}
#endif 
