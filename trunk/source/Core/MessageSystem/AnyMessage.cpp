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
#include <boost/any.hpp>
#include "Core/Utils/Log.h"
#include "Core/MessageSystem/AnyMessage.h"

namespace GASS
{
	/*AnyMessage::AnyMessage(MessageType type) :
			m_TypeID(type),
			m_SenderID(-1),
			m_Delay(0)
	{

	}*/

	AnyMessage::AnyMessage(MessageType type, SenderID sender_id, double delay) :
			m_TypeID(type),
			m_SenderID(sender_id),
			m_Delay(delay)
	{

	}


	AnyMessage::~AnyMessage()
	{

	}

	boost::any AnyMessage::GetData(const std::string &data_name)
	{
		if(m_Data.end() == m_Data.find(data_name))
		{
			Log::Error("AnyMessage data %s doesnt exist",data_name.c_str());
			return boost::any();
		}
		return m_Data[data_name];
	}

	void AnyMessage::SetData(const std::string &data_name, boost::any data)
	{
		m_Data[data_name] = data;
	}

	SenderID AnyMessage::GetSenderID() const
	{
		return m_SenderID;
	}

	void AnyMessage::SetDeliverDelay(double delay)
	{
		m_Delay = delay;
	}

	MessageType AnyMessage::GetType() const
	{
		return m_TypeID;
	}

	double  AnyMessage::GetDeliverDelay() const
	{
		return m_Delay;
	}

}
